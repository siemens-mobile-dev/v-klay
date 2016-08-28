// V_KLay.cpp : Defines the class behaviors for the application.
//

#define IS_MAIN
#include "stdafx.h"
#include "V_KLay.h"
#include "KLayDlg.h"
#include ".\v_klay.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CMD_STRING	0
#define CMD_DWORD	1
#define CMD_BOOL	2
#define CMD_INT64	3
typedef struct CmdOption_
{
	LPCTSTR Name;
	int Type;
	void* pStorage;
	BOOL* pIsExist;
} CmdOption;


//	Options
///////////////////////////////////////////////////////////////////////////

CString o_strLocale;
CString o_strLanguage;
int o_PatcherFontSize=8;
BOOL o_MainWndMaximize=-1;

CString o_strPatchDir;
CString o_strLayoutDir;
CString o_strFlasherDir;
CString o_strFlashPartDir;

BOOL o_bDisableWarningsMsg=FALSE;
BOOL o_bDisableRTFSaveWarningsMsg=FALSE;
int o_AutoignitionType=AUTOIGN_AUTO;
BOOL o_bCommSimple=FALSE;			//disable use OVERLAPPED mode and other advanced things of COM port?
BOOL o_bIsRepairPatchCanSkip=FALSE; //is do not abort operation on 'Cancel' in 'Save Repair Patch As...' dialog?
BOOL o_bIsPatchIgnogeNoOld=FALSE; //is ingore all errors about old data not exist?
CString o_CommentGroupString;
_TCHAR defaultCommentGroupString[]=_T(";");
BOOL o_bIsRepairPatchTextSimple=FALSE;
BOOL o_bCommRTS=TRUE;
BOOL o_bCommDTR=TRUE;
BOOL o_bDoNotInsertLocaleInfoInPatch=FALSE;
BOOL o_bIsEnableBootcoreWritting=FALSE;
//BOOL o_bIsHideWarningForNoWriteNoReadArea=FALSE;
INT64 o_PatcherWrapAddr = 0;		// if address of block in text of patch is greater than this value, this value will be subtracted from address.
BOOL o_bIsPatcherSwapPasteKeys = FALSE;
BOOL o_bIsDisableFubuAutosave = FALSE;

BOOL o_cmdQuit=FALSE;
DWORD o_cmdWorkWith=0;
CString o_cmdPhone;
CString o_cmdPhonePortName;
DWORD o_cmdPhoneBaud=-1;
CString o_cmdPartFName;
INT64 o_cmdPartFrom=-1;
BOOL o_cmdDoWrite=FALSE;
BOOL o_cmdDoRead=FALSE;
INT64 o_cmdFrom=-1;
CString o_cmdFileName;

BOOL o_cmdExist_bDisableWarningsMsg=FALSE;
BOOL o_cmdExist_WorkWith=FALSE;
BOOL o_cmdExist_Phone=FALSE;
BOOL o_cmdExist_PhonePortName=FALSE;
BOOL o_cmdExist_PhoneBaud=FALSE;
BOOL o_cmdExist_PartFName=FALSE;
BOOL o_cmdExist_PartFrom=FALSE;
BOOL o_cmdExist_From=FALSE;

CmdOption CmdOptionsList[]={
	{_T("qu"), CMD_BOOL, &o_cmdQuit, NULL},
	{_T("ww"), CMD_DWORD, &o_cmdWorkWith, &o_cmdExist_WorkWith},
	{_T("p"), CMD_STRING, &o_cmdPhone, &o_cmdExist_Phone},
	{_T("pcom"), CMD_STRING, &o_cmdPhonePortName, &o_cmdExist_PhonePortName},
	{_T("pbaud"), CMD_DWORD, &o_cmdPhoneBaud, &o_cmdExist_PhoneBaud},
	{_T("ff"), CMD_STRING, &o_cmdPartFName, &o_cmdExist_PartFName},
	{_T("fffrom"), CMD_INT64, &o_cmdPartFrom, &o_cmdExist_PartFrom},
	{_T("warn"), CMD_BOOL, &o_bDisableWarningsMsg, &o_cmdExist_bDisableWarningsMsg},
	{_T("wr"), CMD_BOOL, &o_cmdDoWrite, NULL},
	{_T("rd"), CMD_BOOL, &o_cmdDoRead, NULL},
	{_T("fnfrom"), CMD_INT64, &o_cmdFrom, &o_cmdExist_From},
};

#define CMD_OPTIONS_COUNT (sizeof(CmdOptionsList)/sizeof(CmdOption))

_TCHAR* ExtToolArgs[]={
	_T("$0"),
	_T("$p"),
	_T("$f"),
	_T("$k"),
};

_TCHAR keyDisWarn[]=_T("bDisableWarningsMsg");
_TCHAR keyDisRTFSaveWarn[]=_T("bDisableRTFSaveWarningsMsg");
_TCHAR keyLocale[]=_T("strLocale");
_TCHAR keyPatcherFontSize[]=_T("PatcherFontSize");
_TCHAR keyLang[]=_T("Language");
_TCHAR keyAIgnType[]=_T("AutoIgnitionType");
_TCHAR keyCommSimple[]=_T("IsCommPortSimpleMode");
_TCHAR keyIsRepairPatchCanSkip[]=_T("IsSavingOfRepairPatchCanBeSkippped");
_TCHAR keyIsPatchIgnoreNoOld[]=_T("IsPatchIgnoreNoOld");
_TCHAR keyPatchDir[]=_T("PatchCurDir");
_TCHAR keyLayoutDir[]=_T("LayoutCurDir");
_TCHAR keyFlasherDir[]=_T("FlasherCurDir");
_TCHAR keyFlashPartDir[]=_T("FlashPartCurDir");

_TCHAR secMainWnd[]=_T("MainWindow");
_TCHAR keyIsMaximized[]=_T("IsMaximized");
_TCHAR keyXPos[]=_T("XPos");
_TCHAR keyYPos[]=_T("YPos");
_TCHAR keyXSize[]=_T("XSize");
_TCHAR keyYSize[]=_T("YSize");

_TCHAR secOptions[]=_T("Options");
_TCHAR keyCurDevTyp[]=_T("CurDeviceType");
_TCHAR keyCurPhone[]=_T("CurPhoneName");
_TCHAR keyCommentGroupString[]=_T("CommentGroupString");
_TCHAR keyIsRepairPatchTextSimple[]=_T("IsRepairPatchTextSimple");
_TCHAR keyCommRTS[]=_T("CommRTSState");
_TCHAR keyCommDTR[]=_T("CommDTRState");
_TCHAR keyDoNotInsertLocaleInfoInPatch[]=_T("DoNotInsertLocaleInfoInPatch");
_TCHAR keyIsEnableBootcoreWritting[]=_T("EnableBootcoreWritting");
_TCHAR keyPatcherWrapAddr[]=_T("PatcherWrapAddr");
_TCHAR keyPatcherSwapPasteKeys[]=_T("IsPatcherSwapPasteKeys");
_TCHAR keyDisableFubuAutosave[]=_T("DisableFubuAutosave");

_TCHAR keyToolsRunMethod[]=_T("ToolsRunMethod");
_TCHAR keyToolExtLayPath[]=_T("ToolExtKeybLayEditorPath");
_TCHAR keyToolExtPicPath[]=_T("ToolExtPhonePicturesEditorPath");
_TCHAR keyToolExtFntPath[]=_T("ToolExtPhoneFontEditorPath");
_TCHAR keyToolExt1Path[]=_T("ToolExt1Path");
_TCHAR keyToolExt2Path[]=_T("ToolExt2Path");
_TCHAR keyToolExt3Path[]=_T("ToolExt3Path");
_TCHAR keyToolExtLayArg[]=_T("ToolExtKeybLayEditorArg");
_TCHAR keyToolExtPicArg[]=_T("ToolExtPhonePicturesEditorArg");
_TCHAR keyToolExtFntArg[]=_T("ToolExtPhoneFontEditorArg");
_TCHAR keyToolExt1Arg[]=_T("ToolExt1Arg");
_TCHAR keyToolExt2Arg[]=_T("ToolExt2Arg");
_TCHAR keyToolExt3Arg[]=_T("ToolExt3Arg");
_TCHAR keyTmpPath[]=_T("TmpPath");

_TCHAR defaultTmpPathRelativeApp[]=_T("temp\\");


CString exefilter;

CString strToolsToolTipFormat;
_TCHAR strFNTimeFormat[]=_T("%Y-%m-%d_%H-%M-%S");

///////////////////////////////////////////////////////////////////////////

// return nuber of bytes copied to buffer/of size of needed buffer
int V_RegQueryValueExW_MB2WC(DWORD type, IN LPCSTR lpMB, OUT LPWSTR lpWC, IN int cbWC)
{
	cbWC /= sizeof(_TCHAR);
	int len = 0;
	if(type == REG_SZ || type == REG_EXPAND_SZ)
	{
		len = MultiByteToWideChar(CP_ACP, 0, lpMB, -1, lpWC, cbWC);
	}
	else if(type == REG_MULTI_SZ)
	{
		int curlen;
		while(lpMB[0] != 0)
		{
			curlen = MultiByteToWideChar(CP_ACP, 0, lpMB, -1, lpWC, cbWC);
			len += curlen;
			for(; lpMB[0] != 0; lpMB++);
			lpMB++;
		}
		len++;
	}
	return len * sizeof(_TCHAR);
}

LONG __stdcall V_RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	LPSTR stra = (LPSTR)lpValueName;
	if((ULONG_PTR)(lpValueName) > 0xffff)
	{
		int na = WideCharToMultiByte(CP_ACP, 0, lpValueName, -1, NULL, 0, NULL, NULL) + 16;
		stra = (LPSTR)_alloca(na + 1);
		WideCharToMultiByte(CP_ACP, 0, lpValueName, -1, stra, na, NULL, NULL);
	}
	DWORD type;
	DWORD databuflen = 0;
	if(lpcbData != NULL)
		databuflen = *lpcbData;
	LONG ret = RegQueryValueExA(hKey, stra, lpReserved, &type, lpData, lpcbData);
	if(lpType != NULL)
		*lpType = type;
	if(ret == ERROR_SUCCESS && lpcbData != NULL
		&& (type == REG_SZ || type == REG_EXPAND_SZ || type == REG_MULTI_SZ))
	{
		if(lpData == NULL)
		{
			*lpcbData += 16;
			LPSTR vala = (LPSTR)_alloca(*lpcbData);
			ret = RegQueryValueExA(hKey, stra, lpReserved, &type, (LPBYTE)vala, lpcbData);
			if(ret == ERROR_SUCCESS)
				*lpcbData = V_RegQueryValueExW_MB2WC(type, vala, NULL, 0);
		}
		else
		{
			LPSTR vala = (LPSTR)_alloca(*lpcbData + 1);
			memcpy(vala, lpData, *lpcbData);
			*lpcbData = V_RegQueryValueExW_MB2WC(type, vala, (LPWSTR)lpData, databuflen);
			if(*lpcbData == 0 && databuflen != 0)
				ret = ERROR_MORE_DATA;
		}
	}
	return ret;
}

// override
extern "C" FARPROC Unicows_RegQueryValueExW = (FARPROC)&V_RegQueryValueExW;









typedef BOOL (WINAPI CANCELIO)(HANDLE);

BOOL VCancelIo(IN HANDLE hFile)
{
	HINSTANCE hKernel32 = LoadLibrary(_T("KERNEL32.DLL"));
	if(hKernel32 == NULL)
		return FALSE;
	CANCELIO* lpfnCancelIo = (CANCELIO*) GetProcAddress(hKernel32, "CancelIo");
	if(lpfnCancelIo == NULL)
		return FALSE;
	BOOL ret = lpfnCancelIo(hFile);
	FreeLibrary(hKernel32);
	return ret;
}

UINT64 GetAppVersion()
{
	LPBYTE ver=(LPBYTE)LockResource(LoadResource(NULL, FindResource(NULL,
								MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION)));
	return ((UINT64)(*((DWORD*)(ver + 0x30))) << 32) | ((UINT64)(*((DWORD*)(ver + 0x34))) << 0);
}

CString GetFileNameFromPath(LPCTSTR strPath)
{
	LPTSTR fa=_tcsrchr(strPath, _TCHAR('\\'));
	CString str;
	if(fa!=NULL)
		str=fa+1;
	else
		str=strPath;
	return str;
}

BOOL SndMsg(LPCTSTR sndFileName, int beepNumIfSndFailed /*= 1*/)
{
	if(sndFileName != NULL)
	{
		HANDLE sf = CreateFile(sndFileName, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, NULL);
		if(sf != INVALID_HANDLE_VALUE)
			CloseHandle(sf);
		else
			sndFileName = NULL;
		if(sndFileName != NULL)
			if(!PlaySound(sndFileName, NULL, SND_ASYNC | SND_NODEFAULT | SND_NOWAIT))
				sndFileName = NULL;
	}
	if(sndFileName == NULL)
	{
		MessageBeep(-1);
		for(int i = 1; i < beepNumIfSndFailed; i++)
		{
			Sleep(500);
			MessageBeep(-1);
		}
	}
	return TRUE;
}

CString AddPathToFileNameAndExpandEnvStr(LPCTSTR fileName, LPCTSTR fileNameForGetPath)
{
	CString str;
	if(fileName == NULL)
		return str;
	if(_tcslen(fileName) == 0)
		return str;
	ExpandEnvironmentStrings(fileName, str.GetBuffer(4 * 1024), 4 * 1024);
	str.ReleaseBuffer();
	if(GetFileDir(str).IsEmpty())
	{
		CString tmp;
		if(fileNameForGetPath != NULL)
		{
			ExpandEnvironmentStrings(GetFileDir(fileNameForGetPath), tmp.GetBuffer(4 * 1024), 4 * 1024);
			tmp.ReleaseBuffer();
		}
		str = tmp + str;
	}
	return str;
}

TCHAR strFlasherDefFName[]=_T("_From_%2.2X.bin");

CString GetDefaultFlashFileName(INT64 fromAddr, LPCTSTR strExtra /*= NULL*/)
{
	CString strDocFileName, strDevName;
	strDevName = _T("Mem");
	if(pDevice != NULL)
	{
		strDocFileName = pDevice->GetName();
		if(!strDocFileName.IsEmpty())
			strDevName = strDocFileName;
	}
	strDocFileName.Format(strFlasherDefFName, fromAddr >> 16);
	if(strExtra != NULL)
	{
		strDevName += '_';
		strDevName += strExtra;
	}
	strDevName.Replace(_T(' '), _T('_'));
	strDocFileName = strDevName + '_' + COleDateTime::GetCurrentTime().Format(strFNTimeFormat) + strDocFileName;
	return strDocFileName;
}


#if _MFC_VER < 0x0700
#ifndef EM_SETTEXTEX
#define EM_SETTEXTEX			(WM_USER + 97)
#endif
/* Flags for the SETEXTEX data structure */
#ifndef ST_DEFAULT
#define ST_DEFAULT		0
#define ST_KEEPUNDO		1
#define ST_SELECTION	2
#endif
/* EM_SETTEXTEX info; this struct is passed in the wparam of the message */
#ifndef SETTEXTEX
typedef struct _settextex
{
	DWORD	flags;			/* flags (see the ST_XXX defines			*/
	UINT	codepage;		/* code page for translation (CP_ACP for sys default;
						       1200 for Unicode, -1 for control default	*/
} SETTEXTEX;
#endif
#endif


BYTE *StreamBuf=NULL;
DWORD StreamBufLen=0;
DWORD StreamBufPos=0;
CFile *StreamFile=NULL;
BOOL IsStreamFile=FALSE;

DWORD CALLBACK EditStreamInCallback(
  DWORD_PTR dwCookie, // application-defined value
  LPBYTE pbBuff,      // data buffer
  LONG cb,            // number of bytes to read or write
  LONG *pcb           // number of bytes transferred
)
{
	TRY
	{
		if(IsStreamFile)
		{
			*pcb=StreamFile->Read(pbBuff, cb);
		}
		else
		{
			*pcb=min(StreamBufLen-StreamBufPos, ((DWORD)cb));
			memcpy(pbBuff, StreamBuf+StreamBufPos, *pcb);
			StreamBufPos+=*pcb;
		}
	}
	CATCH_ALL(e)
	{
		return 1;
	}
	END_CATCH_ALL
	return 0;
}

// if dataLen==-1, then data = a CFile*
DWORD RichStreamIn(CRichEditCtrl *pRichCtrl, LPCTSTR data, DWORD dataLen, int nFormat)
{
	if(dataLen==-1)
	{
		IsStreamFile=TRUE;
		StreamFile=(CFile*)data;
	}
	else
	{
		IsStreamFile=FALSE;
		StreamBuf=(BYTE*)data;
		StreamBufPos=0;
		StreamBufLen=dataLen;
	}
	EDITSTREAM es;
	es.dwCookie=NULL;
	es.dwError=-1;
	es.pfnCallback=EditStreamInCallback;
	return pRichCtrl->StreamIn(nFormat, es);
}

DWORD CALLBACK EditStreamOutCallback(
  DWORD_PTR dwCookie, // application-defined value
  LPBYTE pbBuff,      // data buffer
  LONG cb,            // number of bytes to read or write
  LONG *pcb           // number of bytes transferred
)
{
	TRY
	{
		if(IsStreamFile)
		{
			StreamFile->Write(pbBuff, cb);
			*pcb=cb;
		}
		else
		{
			*pcb=min(StreamBufLen-StreamBufPos, ((DWORD)cb));
			memcpy(StreamBuf+StreamBufPos, pbBuff, *pcb);
			StreamBufPos+=*pcb;
		}
	}
	CATCH_ALL(e)
	{
		return 1;
	}
	END_CATCH_ALL
	return 0;
}

// if dataBufLen==-1, then dataBuf = a CFile*
DWORD RichStreamOut(CRichEditCtrl *pRichCtrl, LPCTSTR dataBuf, DWORD dataBufLen, int nFormat)
{
	if(dataBufLen==-1)
	{
		IsStreamFile=TRUE;
		StreamFile=(CFile*)dataBuf;
	}
	else
	{
		IsStreamFile=FALSE;
		StreamBuf=(BYTE*)dataBuf;
		StreamBufPos=0;
		StreamBufLen=dataBufLen;
	}
	EDITSTREAM es;
	es.dwCookie=NULL;
	es.dwError=-1;
	es.pfnCallback=EditStreamOutCallback;
	return pRichCtrl->StreamOut(nFormat, es);
}

//return the number of bytes in text (not includign terminating zero char).
DWORD RichStreamOutText(CRichEditCtrl *pRichCtrl, CString& strText)
{
	int nFormat=SF_TEXT;
#ifdef _UNICODE
	nFormat|=SF_UNICODE;
#endif
	DWORD buflen=GetRichStreamOutSize(pRichCtrl, nFormat);
	RichStreamOut(pRichCtrl, (strText.GetBuffer((int)((buflen/sizeof(TCHAR)+16)))), buflen, nFormat);
	strText.ReleaseBuffer(buflen/sizeof(TCHAR));
	return buflen;
}


DWORD StreamPartialMin=0;
DWORD StreamPartialMax=0;
DWORD StreamPartialPos=0;

DWORD CALLBACK EditStreamOutTextPartialCallback(
  DWORD_PTR dwCookie, // application-defined value
  LPBYTE pbBuff,      // data buffer
  LONG cb,            // number of bytes to read or write
  LONG *pcb           // number of bytes transferred
)
{
	TRY
	{
		if(StreamPartialPos >= StreamPartialMax)
			return 1;
		if(StreamPartialPos+cb >= StreamPartialMin)
		{
			int ro=0, rs;
			if(StreamPartialPos < StreamPartialMin)
				ro=StreamPartialMin - StreamPartialPos;
			rs=min(StreamPartialMax-StreamPartialMin-StreamBufPos, ((DWORD)(cb-ro)));
			memcpy(StreamBuf+StreamBufPos, pbBuff+ro, rs);
			StreamBufPos+=rs;
		}
		*pcb=cb;
		StreamPartialPos+=cb;
	}
	CATCH_ALL(e)
	{
		return 1;
	}
	END_CATCH_ALL
	return 0;
}

DWORD RichStreamOutTextPartial(CRichEditCtrl *pRichCtrl, CString& strText, DWORD minCharPos, DWORD maxCharPos)
{
	if(maxCharPos<=minCharPos)
		return 0;
	int nFormat=SF_TEXT;
#ifdef _UNICODE
	nFormat|=SF_UNICODE;
#endif
	StreamBuf=(BYTE*)strText.GetBuffer(maxCharPos-minCharPos+16);
	StreamBufPos=0;
	StreamPartialMin=minCharPos*sizeof(TCHAR);
	StreamPartialMax=maxCharPos*sizeof(TCHAR);
	StreamPartialPos=0;
	EDITSTREAM es;
	es.dwCookie=NULL;
	es.dwError=-1;
	es.pfnCallback=EditStreamOutTextPartialCallback;
	pRichCtrl->StreamOut(nFormat, es);
	strText.ReleaseBuffer(StreamBufPos/sizeof(_TCHAR));
	return StreamBufPos;
}


DWORD CALLBACK EditStreamOutGetSizeCallback(
  DWORD_PTR dwCookie, // application-defined value
  LPBYTE pbBuff,      // data buffer
  LONG cb,            // number of bytes to read or write
  LONG *pcb           // number of bytes transferred
)
{
	*pcb=cb;
	return 0;
}

DWORD GetRichStreamOutSize(CRichEditCtrl *pRichCtrl, int nFormat)
{
	EDITSTREAM es;
	es.dwCookie=NULL;
	es.dwError=-1;
	es.pfnCallback=EditStreamOutGetSizeCallback;
	return pRichCtrl->StreamOut(nFormat, es);
}

void AFXAPI DDX_TextRich2(CDataExchange* pDX, int nIDC, CString& value)
{
	CRichEditCtrl* re=(CRichEditCtrl*)pDX->m_pDlgWnd->GetDlgItem(nIDC);
	if(pDX->m_bSaveAndValidate)
	{
		RichStreamOutText(re, value);
	}
	else
	{
		SETTEXTEX st;
#ifdef _UNICODE
		st.codepage=1200;
#else
		st.codepage=CP_ACP;
#endif
		st.flags=ST_DEFAULT;
		re->SendMessage(EM_SETTEXTEX, (WPARAM)&st, (LPARAM)((LPCTSTR)value));
	}
}


//	INI files handling
/////////////////////////////////////////////////////////////////////////////////////////////////////////
_TCHAR *iniBuffer=NULL;
size_t iniBufferLen=0;
size_t iniBufferPos=0;
CString iniCurFileName;
CFileStatus iniCurFileStatus;

void iniDestroy(void)
{
	if(iniBuffer!=NULL)
	{
		delete iniBuffer;
		iniBuffer=NULL;
	}
	iniCurFileName.Empty();
	iniBufferLen=0;
	iniBufferPos=0;
}

BOOL iniOpenFile(LPCTSTR lpFileName)
{
	BOOL reRead = FALSE;
	CString fn= lpFileName!=NULL ? lpFileName : _T("WIN.INI");
	if(iniCurFileName.IsEmpty() || iniCurFileName!=fn)
	{
		reRead = TRUE;
	}
	else
	{
		CFileStatus fs;
		CFile::GetStatus(iniCurFileName, fs);
		if(fs.m_ctime != iniCurFileStatus.m_ctime
			|| fs.m_mtime != iniCurFileStatus.m_mtime
			|| fs.m_size != iniCurFileStatus.m_size)
			reRead = TRUE;
	}

	if(reRead)
	{
		iniDestroy();
		CFile f;
		if(!f.Open(fn, CFile::modeRead | CFile::typeBinary))
			return FALSE;
		iniCurFileName=fn;
		iniBufferLen=(size_t)f.GetLength();
		iniBuffer=new _TCHAR[iniBufferLen+16];
#ifdef UNICODE
		char *fbuf=new char[iniBufferLen+16];
		f.Read(fbuf, (UINT)iniBufferLen);
		fbuf[iniBufferLen]=0;
		MultiByteToWideChar(1251, 0, fbuf, -1, iniBuffer, (int)iniBufferLen+16);
		delete fbuf;
#else
		f.Read(iniBuffer, (UINT)iniBufferLen);
#endif
		iniBuffer[iniBufferLen]=0;
		f.GetStatus(iniCurFileStatus);
		f.Close();
	}
	iniBufferPos=0;
	return TRUE;
}

void iniCloseFile(void)
{
	iniBufferPos=0;
}

BOOL iniReadString(CString &str)
{
	if(iniBuffer==NULL || iniBufferPos >= iniBufferLen)
		return FALSE;
	size_t len=_tcscspn(iniBuffer+iniBufferPos, _T("\r\n"));
	if(len==-1)
		len=iniBufferLen-iniBufferPos;
	str.SetString(iniBuffer+iniBufferPos, (int)len);
	iniBufferPos+=len;
	if(iniBufferPos < iniBufferLen)
	{
		iniBufferPos++;
		if(iniBuffer[iniBufferPos]==_TCHAR('\r') || iniBuffer[iniBufferPos]==_TCHAR('\n'))
			iniBufferPos++;
	}
	return TRUE;
}

BOOL iniFindSection(CString &sec)
{
	CString str;
	int beg, i;
	while(iniReadString(str))
	{
		str.Trim();
		if(str.IsEmpty())
			continue;
		if(str[0]!=_TCHAR('['))
			continue;
		i=str.Find(_TCHAR(']'));
		if(i==-1)
			continue;
		for(beg=1; _istspace(str[beg]); beg++);
		for(i--; _istspace(str[i]); i--);
		if(beg>i)
			beg=1, i=0;
		else
			i-=beg-1;
		if(!sec.IsEmpty())
		{
			if(i!=sec.GetLength())
				continue;
			if(_tcsncicmp(((LPCTSTR)str)+beg, sec, i)==0)
				return TRUE;
		}
		else
		{
			sec=str.Mid(beg, i);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL iniFindKeyAndReadValue(CString &key, CString &value, BOOL *IsRachedEndOfSection=NULL)
{
	CString str;
	int i, vali;
	value.Empty();
	if(IsRachedEndOfSection!=NULL)
		*IsRachedEndOfSection=TRUE;
	while(iniReadString(str))
	{
		str.Trim();
		if(str.IsEmpty())
			continue;
		if(str[0]==_TCHAR('['))
			break;
		if(str[0]==_TCHAR(';') || str[0]==_TCHAR('#'))		//skip comments
			continue;
		i=str.Find(_TCHAR('='));
		if(i==-1)
			continue;
		for(vali=i+1; _istspace(str[vali]); vali++);
		for(; i>0; i--)
			if(!_istspace(str[i-1]))
				break;
		if(!key.IsEmpty())
		{
			if(i!=key.GetLength())
				continue;
			if(_tcsncicmp(str, key, i)!=0)
				continue;
		}
		else
		{
			key=str.Left(i);
		}
		value=str.Right(str.GetLength()-vali);
		if(IsRachedEndOfSection!=NULL)
			*IsRachedEndOfSection=FALSE;
		break;
	}
	return !value.IsEmpty();
}

// This function converts a string that may contains escape 
// symbols (beginning with backslash '\' character).
//
// The single parameter is a pointer to the converted string.
// Since a result is always less or equal than original string,
// therefore no reason to use an additional buffer.
//
// The result will appear instead original string at the same buffer if dest is NULL.
// return the number of bytes in the out string.
DWORD ParseEscapeString(LPTSTR lpszStr, LPTSTR lpszDest/*=NULL*/, LPTSTR *lpEnd/*=NULL*/, BOOL bDestNoUnicode/*=FALSE*/)
{
	if(lpEnd!=NULL)
		*lpEnd=lpszStr;
	if(lpszStr==NULL)
		return 0;
	int len=(int)_tcslen(lpszStr);
	if(len==0)
		return 0;

	LPCTSTR s=lpszStr;
	LPTSTR t, dest= lpszDest==NULL ? lpszStr : lpszDest;
	int i;
	TCHAR sz[10];
	TCHAR ch;
	char *mbchar=new char[MB_CUR_MAX];
#ifdef UNICODE
	char mbch;
#endif;
	for(i=0; *s!=_T('\0') && *s!=_T('\"') && *s!=_T('\''); s++, i++)
	{
		if (*s!=_T('\\'))
		{
			ch=*s;			// non-escape symbol
		}
		else
		{
			// Got escape sequence
			if(s[1]==_T('\0'))
				break;
			s++;
			memset(sz, 0, sizeof(sz));
			memcpy(sz, s, (len-(s-lpszStr) < 8 ? len-(s-lpszStr) : 8)*sizeof(_TCHAR));

			// ASCII character in octal notation?
			if(sz[0] >= _T('0') && sz[0] <= _T('7'))
			{
				sz[3]=_T('\0');
				ch=(_TCHAR)_tcstoul(sz, &t, 8);
				s+=t-sz-1;
			}
			else
			{
				switch(sz[0])
				{
					// ASCII character in hexadecimal notation?
				case _T('x'):
					sz[3]=_T('\0');
					ch=(_TCHAR)_tcstoul(sz+1, &t, 16);
					if(t-(sz+1) == 2)
						s+=2;
					else
						ch=_T('x');
					break;

					// Unicode character using hexadecimal representation (exactly four digits)?
				case _T('u'):
					sz[5]=_T('\0');
#ifdef UNICODE
					ch=(_TCHAR)_tcstoul(sz+1, &t, 16);
#else
					ch=_TCHAR('?');
					if(mbchar!=NULL)
						if(wctomb(mbchar, (wchar_t)_tcstoul(sz+1, &t, 16))!=-1)
							ch=mbchar[0];
#endif
					if(t-(sz+1) == 4)
						s+=4;
					else
						ch=_T('u');
					break;

				case _T('a'):
					ch=_T('\a');
					break;
				case _T('b'):
					ch=_T('\b');
					break;
				case _T('t'):
					ch=_T('\t');
					break;
				case _T('r'):
					ch=_T('\r');
					break;
				case _T('v'):
					ch=_T('\v');
					break;
				case _T('f'):
					ch=_T('\f');
					break;
				case _T('n'):
					ch=_T('\n');
					break;
				case _T('e'):
					ch=_T('\x1B');
					break;
					// Microsoft Specific —>
					// If a backslash precedes a character that does not appear in Table 1.4,
					// the compiler handles the undefined character as the character itself. 
					// For example, \x is treated as an x.
					// END Microsoft Specific
				default:
					ch=*s;
					break;
				};
			}
		}
#ifdef UNICODE
		if(bDestNoUnicode)
		{
			mbch=(char)ch;
			if(mbchar!=NULL)
				if(wctomb(mbchar, ch)!=-1)
					mbch=mbchar[0];
			((char*)dest)[i]=mbch;
		}
		else
#endif
		{
			dest[i]=ch;
		}
	}
	if(mbchar!=NULL)
		delete mbchar;
#ifdef UNICODE
	if(bDestNoUnicode)
	{
		((char*)dest)[i]=0;
	}
	else
#endif
	{
		dest[i]=_T('\0');
	}
	if(lpEnd!=NULL)
		*lpEnd=(LPTSTR)s;
	return (DWORD)i;
}



DWORD VGetPrivateProfileSection(LPCTSTR lpSection, CStringList *strKeysList, LPCTSTR lpFileName)
{
	DWORD count=0;
	TRY
	{
		if(strKeysList!=NULL)
			strKeysList->RemoveAll();
		if(lpSection!=NULL)
		{
			CString key, value, sec=lpSection;
			sec.Trim();
			if(!sec.IsEmpty())
			{
				if(iniOpenFile(lpFileName))
				{
					if(iniFindSection(sec))
					{
						BOOL IsEnd=FALSE;
						while(1)
						{
							key.Empty();
							iniFindKeyAndReadValue(key, value, &IsEnd);
							if(IsEnd)
								break;
							count++;
							if(strKeysList!=NULL)
								strKeysList->AddTail(key+_TCHAR('=')+value);
						}
					}
					iniCloseFile();
				}
			}
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL;
	return count;
}

DWORD VGetPrivateProfileSectionNames(CStringList *strSectionsList, LPCTSTR lpFileName)
{
	DWORD count=0;
	TRY
	{
		if(strSectionsList!=NULL)
			strSectionsList->RemoveAll();
		if(iniOpenFile(lpFileName))
		{
			for(CString sec; iniFindSection(sec); sec.Empty())
			{
				count++;
				if(strSectionsList!=NULL)
					strSectionsList->AddTail(sec);
			}
			iniCloseFile();
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL;
	return count;
}

CString VGetPrivateProfileString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault,
								 LPCTSTR lpFileName, BOOL *IsValueExist/*=NULL*/)
{
	CString strRes;
	BOOL isValExist=FALSE;
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	TRY
	{
		if(lpSection!=NULL && lpKey!=NULL)
		{
			CString sec=lpSection, key=lpKey;
			sec.Trim();
			key.Trim();
			if(!sec.IsEmpty() && !key.IsEmpty())
			{
				if(iniOpenFile(lpFileName))
				{
					if(iniFindSection(sec))
						if(!iniFindKeyAndReadValue(key, strRes))
							strRes.Empty();
					iniCloseFile();
				}
			}
		}
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL;
	if(strRes.IsEmpty())
	{
		if(lpDefault!=NULL)
			strRes=lpDefault;
	}
	else
	{
		isValExist=TRUE;
	}
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	strRes.Trim();
	return strRes;
}

INT64 VGetPrivateProfileHex(LPCTSTR lpSection,  LPCTSTR lpKey, INT64 nDefault,
						   LPCTSTR lpFileName, BOOL *IsValueExist/*=NULL*/)
{
	BOOL isValExist=FALSE;
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	CString str;
	str=VGetPrivateProfileString(lpSection, lpKey, NULL, lpFileName, &isValExist);
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	if(!isValExist)
		return nDefault;
	return VStrToInt(str);
}

BOOL VGetPrivateProfileData(LPCTSTR lpSection, LPCTSTR lpKey, BYTE *&pData, DWORD &Length,
							LPCTSTR lpFileName, BOOL *IsValueExist/*=NULL*/)
{
	BOOL isValExist=FALSE;
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	pData=NULL;
	Length=0;
	CString str;
	str=VGetPrivateProfileString(lpSection, lpKey, NULL, lpFileName, &isValExist);
	if(IsValueExist!=NULL)
		*IsValueExist=isValExist;
	if(isValExist)
	{
//str=_T("0s\\7\\cC\\u041FTesting\\0\\xffÏðîâåðêà\\r\\05");
		if(str.GetLength()<2)
			return FALSE;
		if(str[0]==_T('0') && (str[1]==_T('s') || str[1]==_T('S')))
		{

			Length=ParseEscapeString((LPTSTR)((LPCTSTR)str)+2);
			if(Length!=0)
			{
				pData=new BYTE[Length+16];
#ifdef UNICODE
				WideCharToMultiByte(1251, 0, ((LPCTSTR)str)+2, Length, (LPSTR)pData, Length+16, NULL, NULL);
#else
				memcpy(pData, ((LPCTSTR)str)+2, Length);
#endif
			}
		}
		else
		{
			LPCTSTR pstr=str;
			pData=new BYTE[str.GetLength()+16];
			Length=getdata(pstr, (BYTE*)pData);
			if(Length==-1)
			{
				delete pData;
				pData=NULL;
				Length=0;
				return FALSE;
			}
		}
	}
	return TRUE;
}

CString VGetParameterFromList(const CString& strList, int& curPos)
{
	CString str;
	int i;
	for(;_istspace(strList[curPos]) && curPos < strList.GetLength(); curPos++);
	if(curPos < strList.GetLength())
	{
		if(strList[curPos]==_T('\"'))
		{
			i=strList.Find(_T('\"'), curPos+1);
			if(i==-1)
				i=strList.GetLength();
			str=strList.Mid(curPos+1, i-(curPos+1));
			curPos=i;
			for(; strList[curPos]!=_T(',') && strList[curPos]!=_T(':')/* && strList[curPos]!=_T('.')*/
				&& curPos < strList.GetLength(); curPos++);
		}
		else
		{
			i=curPos;
			for(; strList[curPos]!=_T(',') && strList[curPos]!=_T(':')/* && strList[curPos]!=_T('.')*/
				&& curPos < strList.GetLength(); curPos++);
			str=strList.Mid(i, curPos-i);
		}
		str.Trim();
		if(curPos < strList.GetLength())
			curPos++;
	}
	return str;
}

//	end of INI files handling.

///////////////////////////////////////////////////////////////////////////////////




void ShowPatchOutOfFlashMessage(UINT strID, DWORD PatchAddr, DWORD PatchSize)
{
	CString str, format;
	format.LoadString(strID);
	str.Format(format, PatchAddr, PatchSize);
	AfxMessageBox(str);
}

CString IdentStr(LPCTSTR lpstrForIdent, int leadingSpacesCount/*=2*/, int trailingSpacesCount/*=0*/)
{
	CString str(_TCHAR(' '), leadingSpacesCount);
	CString strtr(_TCHAR(' '), trailingSpacesCount);
	str+=lpstrForIdent;
	str+=strtr;
	return str;
}

int CheckFileNameString(LPCTSTR strFN, LPCTSTR additionalReplace/*=NULL*/)
{
	int errpos=-1, i, n=(int)_tcslen(strFN);
	int iadd, nadd=(int)_tcslen(additionalReplace);
	TCHAR ch;
	BOOL replace;
	for(i=0; i<n; i++)
	{
		ch=strFN[i];
		for(replace=FALSE, iadd=0; iadd<nadd; iadd++)
		{
			if(ch==additionalReplace[iadd])
			{
				replace=TRUE;
				break;
			}
		}
		if(replace ||
			ch<_T(' ') ||
			ch==_T('\"') ||
			ch==_T('*')	||
			ch==_T('/') ||
			ch==_T(':') ||
			ch==_T('<') ||
			ch==_T('>') ||
			ch==_T('\?') ||
			ch==_T('\\') ||
			ch==_T('|') ||
			ch==127
			)
		{
			((LPTSTR)strFN)[i]=_T('_');
			if(errpos==-1)
				errpos=i-1;
		}
	}
	return errpos;
}

CString GetFileName(LPCTSTR filePath)
{
	CString name=filePath;
	int i=name.ReverseFind(_TCHAR('\\'));
	if(i!=-1)
		name=name.Right(name.GetLength()-i-1);
	return name;
}

CString GetFileTitle(LPCTSTR filePath)
{
	CString name=GetFileName(filePath);
	int i=name.ReverseFind(_TCHAR('.'));
	if(i!=-1)
		name=name.Left(i);
	return name;
}

CString GetFileExt(LPCTSTR filePath)
{
	CString name=GetFileName(filePath);
	int i=name.ReverseFind(_TCHAR('.'));
	if(i!=-1)
		name=name.Right(name.GetLength()-i-1);
	else
		name.Empty();
	return name;
}

CString GetFileDir(LPCTSTR filePath)
{
	CString name=filePath;
	int i=name.ReverseFind(_TCHAR('\\'));
	if(i!=-1)
		name=name.Left(i+1);
	else
		name.Empty();
	return name;
}

CString GetFileDriveRootPath(LPCTSTR filePath)
{
	CString name=filePath;
	int i=name.Find(_T(":\\"));
	if(i!=-1)
		name=name.Left(i+2);
	else
		name.Empty();
	return name;
}

void IncludeTrailingSlash(CString &path)
{
	int len=path.GetLength();
	if(len==0)
		return;
	if(path[len-1]!=_T('\\'))
		path+=_T('\\');
}

//	return -1 if no address in file name
INT64 GetAddrFromFileName(LPCTSTR filePathOrName)
{
	INT64 a=-1;
	size_t n=_tcsclen(filePathOrName);
	LPCTSTR str=_tcsrchr(filePathOrName, _TCHAR('.'));
	if(str==NULL)
		str=filePathOrName+n;
	LPCTSTR fname=_tcsrchr(filePathOrName, _TCHAR('\\'));
	if(fname==NULL)
		fname=filePathOrName;
	else
		fname++;
	if(fname>=str)
		return -1;
	if(str-fname >= 6)
		if(*(str-6)==_TCHAR('_') && (*(str-5)==_TCHAR('-') || *(str-5)==_TCHAR('+'))
			&& _istxdigit(*(str-4)) && _istxdigit(*(str-3)) && _istxdigit(*(str-2)) && _istxdigit(*(str-1)))
			a=VStrToSignedHex(str-5)*0x10000;
	if(a==-1 && str-fname >= 5)
		if(*(str-5)==_TCHAR('_') && _istxdigit(*(str-4)) && _istxdigit(*(str-3)) && _istxdigit(*(str-2)) && _istxdigit(*(str-1)))
			a=VStrToSignedHex(str-4)*0x10000;
	if(a==-1 && str-fname >= 4)
		if(*(str-4)==_TCHAR('_') && (*(str-3)==_TCHAR('-') || *(str-3)==_TCHAR('+'))
			&& _istxdigit(*(str-2)) && _istxdigit(*(str-1)))
			a=VStrToSignedHex(str-3)*0x10000;
	if(a==-1 && str-fname >= 3)
		if(*(str-3)==_TCHAR('_') && _istxdigit(*(str-2)) && _istxdigit(*(str-1)))
			a=VStrToSignedHex(str-2)*0x10000;
	return a;
}

BOOL SetForeground(CWnd *wnd)
{
	if(wnd==NULL)
		return FALSE;
	DWORD ID;
	CWnd *pfg = CWnd::GetForegroundWindow();
	if(pfg!=NULL)
	{
		ID=GetWindowThreadProcessId(pfg->GetSafeHwnd(), NULL);
		AttachThreadInput(GetCurrentThreadId(), ID, TRUE);
	}
	wnd->SetForegroundWindow();
	wnd->SetActiveWindow();
	wnd->SetForegroundWindow();
	if(pfg!=NULL)
		AttachThreadInput(GetCurrentThreadId(), ID, FALSE);
	return pfg!=NULL;
}

BOOL v_istspace(_TCHAR ch)
{
	int ch2=ch;
#ifdef UNICODE
	ch2&=0xffff;
#else
	ch2&=0xff;
#endif
	return _istspace(ch2);
}

int VStrAToBCD(char* pStrA, void* pbcdNumBuf, int bcdNumBufLen)
{
	int p=0;
	if(pStrA!=NULL && pbcdNumBuf!=NULL && bcdNumBufLen!=0)
	{
		memset(pbcdNumBuf, 0, bcdNumBufLen);
		int i=0;
		for(; isspace(pStrA[i]) && pStrA[i]; i++);
		if(pStrA[i])
		{
			int beg=i;
			for(; isdigit(pStrA[i]) && pStrA[i]; i++);
			if(i!=beg)
			{
				bcdNumBufLen*=2;
				for(p=0, i--; i>=beg && p<bcdNumBufLen; i--, p++)
					((BYTE*)pbcdNumBuf)[p/2]|=((BYTE)((pStrA[i]-char('0'))&0x0f))<<((p%2)*4);
			}
		}
	}
	return p;
}


// Sets the time duration after what the event has occur
VTIMER SetTimerEventAfter(DWORD durMillisec)
{
	return ((((VTIMER)GetTickCount()) << 32) | durMillisec);
}

// Check, if timer event is occur?
BOOL IsTimerEvent(VTIMER vTimerEvent)
{
	if(((DWORD*)(&vTimerEvent))[0]==TIMEOUT_INFINITY)
		return FALSE;
	DWORD cur=GetTickCount();
	cur= (cur>=((DWORD*)(&vTimerEvent))[1] ?
		(cur-((DWORD*)(&vTimerEvent))[1]) : ((0xffffffffUL-((DWORD*)(&vTimerEvent))[1])+1+cur) );
	return cur >= ((DWORD*)(&vTimerEvent))[0];
}

// Convert string with 64 bit decimal, hex or octal integer (signed) in format [spaces][sign][digits][any_string] to INT64
INT64 VStrToInt(LPCTSTR str)
{
	INT64 val=0;
	_stscanf(str, _T("%I64i"), &val);
	return val;
}

// Convert string with 64 bit signed hexadecimal number in format [spaces][sign]["0x"|"0X"][hex_digits][any_string] to INT64
INT64 VStrToSignedHex(LPCTSTR str, LPTSTR* end/*=NULL*/)
{
	INT64 val=0;
	int ret=_stscanf(str, _T("%I64x"), &val);
	if(end!=NULL)
	{
		if(ret!=1)
		{
			*end=(LPTSTR)str;
		}
		else
		{
			int i=0;
			for(; _istspace(str[i]); i++);
			if(str[i]!=0)
			{
				if(str[i]==_T('-') || str[i]==_T('+'))
					i++;
				if(str[i]==_T('0'))
				{
					i++;
					if(str[i]==_T('x') || str[i]==_T('X'))
						i++;
				}
				if(str[i]!=0)
				{
					for(; _istxdigit(str[i]); i++);
				}
			}
			*end=(LPTSTR)str+i;
		}
	}
	return val;
}

// Convert 64 bit signed hexadecimal number in INT64 format to string in specified format
CString VSignedHexToStr(INT64 signedHex, LPCTSTR format/*=NULL*/)
{
	CString str;
	if(format==NULL)
		format=_T("0x%16.16I64X");
	if(signedHex >= 0)
	{
		str.Format(format, signedHex);
	}
	else
	{
		str.Format(format, -signedHex);
		for(int i=0; i<str.GetLength() && _istspace(str[i]); i++);
		if(i!=str.GetLength())
			str.Insert(i, _T('-'));
	}
	return str;
}














//////////////////////////////////////////////////////////////////////////


// CKLayApp

BEGIN_MESSAGE_MAP(CKLayApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CKLayApp construction

CKLayApp::CKLayApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#if _MFC_VER < 0x0700
	m_richEditLibrary=NULL;
#endif
}

CKLayApp::~CKLayApp()
{
#if _MFC_VER < 0x0700
	if(m_richEditLibrary==NULL)
	{
		FreeLibrary(m_richEditLibrary);
		m_richEditLibrary=NULL;
	}
#endif
}


// The one and only CKLayApp object

CKLayApp theApp;
VOptions Options;
VDevice *pDevice=NULL;
CProgressDlg *pProgress=NULL;
CMlsPrgMsg MyMsg;


// CKLayApp initialization

BOOL CKLayApp::InitInstance()
{
	SetRegistryKey(_T("Vi-Soft"));
	free((void*)m_pszProfileName);
	m_pszProfileName=_tcsdup(_T("V_KLay2"));

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	//Get program path to m_AppPath
	CString apname;
	::GetModuleFileName(AfxGetInstanceHandle( ), apname.GetBuffer(1024), 1024);
	apname.ReleaseBuffer();
	LPTSTR cmdline=(LPTSTR)(LPCTSTR)apname;
	if(cmdline[0]==_TCHAR('\"'))
		cmdline++;
	LPTSTR pathend=_tcsrchr(cmdline, _TCHAR('\\'));
	if(pathend!=NULL)
	{
		CString str(cmdline, (int)(pathend-cmdline+1));
		m_AppPath=str;
	}
	else
		m_AppPath=_T(".\\");

	m_LoadersPath=m_AppPath+_T("data\\Loaders\\");
	m_LangPath=m_AppPath+_T("data\\Lang\\");
	m_INIPath=m_AppPath+_T("data\\KeybLay\\");


	//Set Language
	o_strLanguage = GetProfileString(secOptions, keyLang, _T("English"));
	CString langfname = m_LangPath + o_strLanguage + _T(".h");
	CString stmp, str = GetLangFileInfo(langfname, _T("LOCALE"));
	int enc = 0;
	stmp = str;
	stmp.Trim();
	stmp.MakeLower();
	if(stmp == _T("utf8"))
	{
		enc = 1;
	}
	else
	{
		enc = 0;
		_tsetlocale(LC_ALL, str);
		_setmbcp(_MB_CP_LOCALE);
	}
	MyMsg.SetCurrentLang(MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT));
	MyMsg.ParsePlainFile(langfname, enc);
	if(enc == 0)
	{
		_tsetlocale(LC_ALL, _T(""));
		_setmbcp(_MB_CP_LOCALE);
	}
//
	str=CString(MSTR("All Files"))+_T(" (*.*)|*.*||");
	flashfilter=CString(MSTR("Binary Flash Files"))+_T(" (*.bin; *.fls)|*.bin; *.fls|")+str;
	vkpfilter=CString(MSTR("V_KLay General Patch Files"))+_T(" (*.vkp)|*.vkp|");
	vkpfiltersave=vkpfilter+CString(MSTR("V_KLay Patch Files in RTF"))+_T(" (*.vkp)|*.vkp|");
	CString str1=CString(MSTR("Cracker Files"))+_T(" (*.crk)|*.crk|")+str;
	vkpfilter+=str1;
	vkpfiltersave+=str1;
	vklaydocfilter=CString(MSTR("V_KLay Keys Files"))+_T(" (*.vkl)|*.vkl|")+str;
	exefilter=CString(MSTR("Programs"))+_T(" (*.exe, *.bat)|*.exe;*.bat|") + str;
	openDialogTitle=MSTR("Open");
	saveDialogTitle=MSTR("Save As");
//

	BOOL bOk;
//#if _MFC_VER >=0x0700
	bOk=AfxInitRichEdit2();
//#else
//	if(m_richEditLibrary==NULL)
//		m_richEditLibrary = ::LoadLibrary("RICHED20.DLL");
//	bOk=(m_richEditLibrary!=NULL);
//#endif
	if(!bOk)
		AfxMessageBox(_T("Error while initialize RichEdit!"));

//parsing command line
//	AfxMessageBox(m_lpCmdLine);
	LPCTSTR lpcmd=m_lpCmdLine;
	LPTSTR lpval;
	int i, opt;
	BOOL paropen;
	while(*lpcmd)
	{
		for(; isspace(*lpcmd); lpcmd++);
		if(*lpcmd!='/')
			break;
		if(*lpcmd==0)
			break;
		lpcmd++;
		for(i=0; lpcmd[i]!='=' && lpcmd[i]!=0; i++);
		if(lpcmd[i]==0)
		{
			lpcmd+=i;
			break;
		}
		_tcsncpy(apname.GetBuffer(i+16), lpcmd, i);
		apname.ReleaseBuffer(i);
		lpcmd+=i+1;
		for(opt=0; opt<CMD_OPTIONS_COUNT; opt++)
			if(apname.CompareNoCase(CmdOptionsList[opt].Name)==0)
				break;
		paropen=FALSE;
		lpval=apname.GetBuffer(_MAX_PATH+1);
		for(i=0; *lpcmd!=0; lpcmd++)
		{
			if(!paropen && *lpcmd==' ')
				break;
			else if(*lpcmd=='\"')
				paropen=!paropen;
			else if(i<_MAX_PATH)
				lpval[i++]=*lpcmd;
		}
		apname.ReleaseBuffer(i);
		if(opt<CMD_OPTIONS_COUNT)
		{
			if(CmdOptionsList[opt].pIsExist)
				*CmdOptionsList[opt].pIsExist=TRUE;
			if(CmdOptionsList[opt].pStorage)
			{
				apname.TrimLeft();
				apname.TrimRight();
				switch(CmdOptionsList[opt].Type)
				{
				case CMD_STRING:
					*((CString*)(CmdOptionsList[opt].pStorage))=apname;
					break;
				case CMD_DWORD:
					*((DWORD*)(CmdOptionsList[opt].pStorage))=(DWORD)VStrToInt(apname);
					break;
				case CMD_BOOL:
					*((BOOL*)(CmdOptionsList[opt].pStorage))=(DWORD)VStrToInt(apname);
					break;
				case CMD_INT64:
					*((INT64*)(CmdOptionsList[opt].pStorage))=(INT64)VStrToInt(apname);
					break;
				}
			}
		}
	}

	o_cmdFileName=lpcmd;
	o_cmdFileName.TrimLeft();
	o_cmdFileName.TrimRight();
	if(!o_cmdFileName.IsEmpty())
	{
		i=o_cmdFileName.GetLength()-1;
		if(o_cmdFileName[0]=='\"')
			o_cmdFileName.SetAt(0, ' ');
		if(o_cmdFileName[i]=='\"')
			o_cmdFileName.SetAt(i, ' ');
		o_cmdFileName.TrimLeft();
		o_cmdFileName.TrimRight();
	}

	if(o_cmdExist_bDisableWarningsMsg)
		o_bDisableWarningsMsg=!o_bDisableWarningsMsg;

//	// Parse command line for standard shell commands, DDE, file open
//	ParseCommandLine(m_cmdInfo);

	LoadOptions();

	CKLayDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CKLayApp::IsIdleMessage( MSG* pMsg )
{
	if (!CWinApp::IsIdleMessage( pMsg ) || 
		pMsg->message == WM_TIMER) 
		return FALSE;
	else
		return TRUE;
}

BOOL CKLayApp::OnIdle(LONG lCount)
{
	CKLayDlg *dlg=(CKLayDlg*)AfxGetMainWnd();
	if(dlg!=NULL)
		for(int i=0; i<VKL_PAGES_COUNT; i++)
			if(dlg->pPage[i])
				dlg->pPage[i]->SendMessage(VM_IDLE, i, (LPARAM)lCount);
	return CWinApp::OnIdle(lCount);
}

CString CKLayApp::GetCmdLineFileName(void)
{
	return o_cmdFileName;
}


BOOL CKLayApp::LoadOptions(void)
{
	if(!o_cmdExist_bDisableWarningsMsg)
		o_bDisableWarningsMsg=GetProfileInt(secOptions, keyDisWarn, o_bDisableWarningsMsg);
	o_bDisableRTFSaveWarningsMsg=GetProfileInt(secOptions, keyDisRTFSaveWarn, o_bDisableRTFSaveWarningsMsg);
	o_strLocale=GetProfileString(secOptions, keyLocale, o_strLocale);
	o_PatcherFontSize=GetProfileInt(secOptions, keyPatcherFontSize, o_PatcherFontSize);
	o_AutoignitionType=GetProfileInt(secOptions, keyAIgnType, o_AutoignitionType);
	o_bCommSimple=GetProfileInt(secOptions, keyCommSimple, o_bCommSimple);
	o_bCommRTS=GetProfileInt(secOptions, keyCommRTS, o_bCommRTS);
	o_bCommDTR=GetProfileInt(secOptions, keyCommDTR, o_bCommDTR);
	o_bIsRepairPatchCanSkip=GetProfileInt(secOptions, keyIsRepairPatchCanSkip, o_bIsRepairPatchCanSkip);
	o_bIsPatchIgnogeNoOld=GetProfileInt(secOptions, keyIsPatchIgnoreNoOld, o_bIsPatchIgnogeNoOld);
	o_CommentGroupString=GetProfileString(secOptions, keyCommentGroupString, defaultCommentGroupString);
	WriteProfileString(secOptions, keyCommentGroupString, defaultCommentGroupString);
	o_bIsRepairPatchTextSimple=GetProfileInt(secOptions, keyIsRepairPatchTextSimple, o_bIsRepairPatchTextSimple);
	o_bDoNotInsertLocaleInfoInPatch=GetProfileInt(secOptions, keyDoNotInsertLocaleInfoInPatch, o_bDoNotInsertLocaleInfoInPatch);
	WriteProfileInt(secOptions, keyDoNotInsertLocaleInfoInPatch, o_bDoNotInsertLocaleInfoInPatch);
	o_bIsEnableBootcoreWritting=GetProfileInt(secOptions, keyIsEnableBootcoreWritting, o_bIsEnableBootcoreWritting);
	o_PatcherWrapAddr = (INT64)GetProfileInt(secOptions, keyPatcherWrapAddr, (int)o_PatcherWrapAddr);
	WriteProfileInt(secOptions, keyPatcherWrapAddr, (int)o_PatcherWrapAddr);
	o_bIsPatcherSwapPasteKeys = GetProfileInt(secOptions, keyPatcherSwapPasteKeys, o_bIsPatcherSwapPasteKeys);
	o_bIsDisableFubuAutosave = GetProfileInt(secOptions, keyDisableFubuAutosave, o_bIsDisableFubuAutosave);
	WriteProfileInt(secOptions, keyDisableFubuAutosave, o_bIsDisableFubuAutosave);

	o_strPatchDir=GetProfileString(secOptions, keyPatchDir, o_strPatchDir);
	o_strLayoutDir=GetProfileString(secOptions, keyLayoutDir, o_strLayoutDir);
	o_strFlasherDir=GetProfileString(secOptions, keyFlasherDir, o_strFlasherDir);
	o_strFlashPartDir=GetProfileString(secOptions, keyFlashPartDir, o_strFlashPartDir);
	return TRUE;
}

BOOL CKLayApp::SaveOptions(void)
{
	if(this==NULL)
		return FALSE;
	WriteProfileInt(secOptions, keyDisWarn, o_bDisableWarningsMsg);
	WriteProfileInt(secOptions, keyDisRTFSaveWarn, o_bDisableRTFSaveWarningsMsg);
	WriteProfileString(secOptions, keyLocale, o_strLocale);
	WriteProfileInt(secOptions, keyPatcherFontSize, o_PatcherFontSize);
	WriteProfileString(secOptions, keyLang, o_strLanguage);
	WriteProfileInt(secOptions, keyAIgnType, o_AutoignitionType);
	WriteProfileInt(secOptions, keyCommSimple, o_bCommSimple);
	WriteProfileInt(secOptions, keyCommRTS, o_bCommRTS);
	WriteProfileInt(secOptions, keyCommDTR, o_bCommDTR);
	WriteProfileInt(secOptions, keyIsRepairPatchCanSkip, o_bIsRepairPatchCanSkip);
	WriteProfileInt(secOptions, keyIsPatchIgnoreNoOld, o_bIsPatchIgnogeNoOld);
	WriteProfileInt(secOptions, keyIsRepairPatchTextSimple, o_bIsRepairPatchTextSimple);
	WriteProfileInt(secOptions, keyIsEnableBootcoreWritting, o_bIsEnableBootcoreWritting);
	WriteProfileInt(secOptions, keyPatcherSwapPasteKeys, o_bIsPatcherSwapPasteKeys);

	CWnd *w=(CWnd*)AfxGetMainWnd();
	if(w!=NULL)
	{
		if(!w->IsIconic())
			WriteProfileInt(secMainWnd, keyIsMaximized, w->IsZoomed());
		WINDOWPLACEMENT wp;
		wp.length=sizeof(WINDOWPLACEMENT);
		w->GetWindowPlacement(&wp);
		WriteProfileInt(secMainWnd, keyXPos, wp.rcNormalPosition.left);
		WriteProfileInt(secMainWnd, keyYPos, wp.rcNormalPosition.top);
		WriteProfileInt(secMainWnd, keyXSize, wp.rcNormalPosition.right-wp.rcNormalPosition.left);
		WriteProfileInt(secMainWnd, keyYSize, wp.rcNormalPosition.bottom-wp.rcNormalPosition.top);
	}

	WriteProfileString(secOptions, keyPatchDir, o_strPatchDir);
	WriteProfileString(secOptions, keyLayoutDir, o_strLayoutDir);
	WriteProfileString(secOptions, keyFlasherDir, o_strFlasherDir);
	WriteProfileString(secOptions, keyFlashPartDir, o_strFlashPartDir);

	return TRUE;
}

CString CKLayApp::GetLangFileInfo(LPCTSTR LangFName, LPCTSTR strInfoName)
{
	CFile *f=NULL;
	char* hdr=NULL;
	CString str;
	CString key;
	key.Format(_T(" \"VLP_%s:"), strInfoName);
	TRY
	{
		f=new CFile(LangFName, CFile::modeRead);
		DWORD flen=(DWORD)f->GetLength();
		char* hdr=new char[flen+1];
		f->Read(hdr, flen);
		f->Close();
		hdr[flen]=0;
#ifdef UNICODE
		LPTSTR pustr=str.GetBuffer((flen+1)*2);
		mbstowcs(pustr, hdr, flen);
		pustr[(flen+1)*2-1]=0;
		str.ReleaseBuffer();
#else
		str=hdr;
#endif
		delete hdr;
		hdr=NULL;

		CString line;
		BOOL IsKeyFound=FALSE;
		int i, j;
		while(!str.IsEmpty())
		{
			str.TrimLeft(_T(" \t\r\n"));
			i=str.FindOneOf(_T("\r\n"));
			if(i==-1)
				i=str.GetLength();
			line=str.Left(i);
			str=str.Right(str.GetLength()-i);
			if(!line.IsEmpty())
			{
				if(IsKeyFound)
				{
					if(line.Find(_T("#define "))==0)
					{
						i=line.Find(_T('\"'));
						j=line.ReverseFind(_T('\"'));
						if(i!=-1 && j!=-1)
						{
							str=line.Mid(i+1, j-(i+1));
							break;
						}
					}
					IsKeyFound=FALSE;
				}
				if(!IsKeyFound && line.GetLength()>=3)
				{
					if(line[0]==_T('/') && line[1]==_T('/') && line[2]==_T(' '))
					{
			            i=line.Find(key);
						if(i!=-1)
							IsKeyFound=TRUE;
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		str.Empty();
	}
	END_CATCH_ALL
	if(f!=NULL)
		delete f;
	if(hdr!=NULL)
		delete hdr;
	return str;
}

BOOL CKLayApp::ShowHelp(void)
{
	return ((__int64)ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), _T("open"),
		m_AppPath+_T("v_klay_help.chm"), NULL, m_AppPath, SW_SHOWNORMAL) > 32);
}

int CKLayApp::ExitInstance()
{
	iniDestroy();
	return CWinApp::ExitInstance();
}
