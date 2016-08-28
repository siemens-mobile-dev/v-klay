
#include "stdafx.h"
#include "V_Utilites.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif









//////////////////////////////////////////////////////////////////////////
// Sets the time duration after what the event has occur
VTIMER SetTimerEventAfter(DWORD durMillisec)
{
	return ((((VTIMER)GetTickCount()) << 32) | durMillisec);
}

// Check, if timer event is occur?
BOOL IsTimerEvent(VTIMER vTimerEvent)
{
	if(((DWORD*)(&vTimerEvent))[0]==INFINITE)
		return FALSE;
	if(vTimerEvent==0)
		return TRUE;
	DWORD cur=GetTickCount();
	cur= (cur>=((DWORD*)(&vTimerEvent))[1] ?
		(cur-((DWORD*)(&vTimerEvent))[1]) : ((0xffffffffUL-((DWORD*)(&vTimerEvent))[1])+1+cur) );
	return cur >= ((DWORD*)(&vTimerEvent))[0];
}


BOOL IsDoubleEqual(double d1, double d2, double delta/*=0.00000001*/)
{
	return fabs(d2-d1)<delta;
}


BOOL SetWindowNewText(CWnd *pWndCtrl, LPCTSTR lpszNew)
{
	if(pWndCtrl==NULL)
		return FALSE;
	int nNewLen = lstrlen(lpszNew);
	TCHAR szOld[256];
	// fast check to see if text really changes (reduces flash in controls)
	if (nNewLen > _countof(szOld) ||
		pWndCtrl->GetWindowText(szOld, _countof(szOld)) != nNewLen ||
		lstrcmp(szOld, lpszNew) != 0)
	{
		pWndCtrl->SetWindowText(lpszNew);
		return TRUE;
	}
	return FALSE;
}




//	INI files handling
/////////////////////////////////////////////////////////////////////////////////////////////////////////
_TCHAR *iniBuffer=NULL;
size_t iniBufferLen=0;
size_t iniBufferPos=0;
CString iniCurFileName;

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
	CString fn= lpFileName!=NULL ? lpFileName : _T("WIN.INI");
	if(iniCurFileName.IsEmpty() || iniCurFileName!=fn)
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

DWORD getdata(LPCTSTR& b, BYTE* data)
{
	DWORD len, n, count=0;
	_TCHAR numbuf[3]={0,0,0};
	_TCHAR *end;
	while(*b)
	{
		if(!_istxdigit(b[0]))
		{
			if(b[0]==_T(',') && (_istxdigit(b[1]) || b[1]==_T('\"')))
			{
				b++;
				continue;
			}
			else if(b[0]==_T('\"'))
			{
				len=ParseEscapeString((LPTSTR)(b+1), (LPTSTR)data, (LPTSTR*)&b, TRUE);
				data+=len;
				count+=len;
				if(b[0]==_T('\"'))
					b++;
				continue;
			}
			break;
		}
		if(b[0]==_T('0'))
		{
			if(b[1]==_T('b'))
			{
				n=_tcstoul(b+=2, &end, 2);
				len=(DWORD)(end-b);
				if(len==0)
					return -1;
				len= len/8 + (len%8 == 0 ? 0 : 1);
				memcpy(data, &n, len);
				data+=len;
				count+=len;
				b=end;
				continue;
			}
			else if(b[1]==_T('x'))
			{
				n=_tcstoul(b+=2, &end, 16);
				len=(DWORD)(end-b);
				if(len==0)
					return -1;
				len= len/2 + (len%2 == 0 ? 0 : 1);
				memcpy(data, &n, len);
				data+=len;
				count+=len;
				b=end;
				continue;
			}
		}
		numbuf[0]=*(b++);
		if(!_istxdigit(*b))
			return -1;
		numbuf[1]=*(b++);
		n=_tcstoul(numbuf, &end, 16);
		if(end-numbuf != 2)
			return -1;
		*(data++)=(BYTE)n;
		count++;
	}
	return count;
}

//	Put in out CString next line from text.
//	Returns FALSE if Current position is on end of text.
BOOL StrGetNextLine(LPCTSTR lpstrSource, DWORD* pCurPosInSource /*= NULL*/, CString* pstrOutLine /*= NULL*/)
{
	DWORD curpos = 0;
	if(pCurPosInSource != NULL)
		curpos = *pCurPosInSource;
	if(pCurPosInSource != NULL)
		pstrOutLine->Empty();
	if(lpstrSource == NULL)
		return FALSE;
	if(lpstrSource[curpos] == _T('\0'))
		return FALSE;
	DWORD i;
	for(i = curpos; lpstrSource[i] != _T('\0') && lpstrSource[i] != _T('\n') && lpstrSource[i] != _T('\r'); i++);
	if(pstrOutLine != NULL)
		pstrOutLine->SetString(lpstrSource + curpos, i - curpos);
	if(lpstrSource[i] != _T('\0'))
	{
		if((lpstrSource[i] == _T('\n') && lpstrSource[i + 1] == _T('\r')) ||
		   (lpstrSource[i] == _T('\r') && lpstrSource[i + 1] == _T('\n')) )
			i++;
		i++;
	}
	curpos = i;
	if(pCurPosInSource != NULL)
		*pCurPosInSource = curpos;
	return TRUE;
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
	for(i=0; *s!=_T('\0') && *s!=_T('\"'); s++, i++)
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

UINT VGetPrivateProfileHex(LPCTSTR lpSection,  LPCTSTR lpKey, INT nDefault,
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
	return _tcstol(str, NULL, 0);
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

CString VGetCurrentDirectory()
{
	CString strCurDir;
	DWORD cdlen = GetCurrentDirectory(MAX_PATH, strCurDir.GetBuffer(MAX_PATH));
	if(cdlen == 0)
		strCurDir.ReleaseBuffer(0);
	else if(cdlen > MAX_PATH)
	{
		cdlen += 16;
		strCurDir.ReleaseBuffer(0);
		DWORD cdlennew = GetCurrentDirectory(cdlen, strCurDir.GetBuffer(cdlen));
		if(cdlennew == 0 || cdlennew > cdlen)
			strCurDir.ReleaseBuffer(0);
		else
			strCurDir.ReleaseBuffer(cdlennew);
	}
	else
        strCurDir.ReleaseBuffer(cdlen);
	return strCurDir;
}

BOOL VSetCurrentDirectory(LPCTSTR lpstrNewCurDir)
{
	CString str = lpstrNewCurDir;
	str.Trim();
	if(str.IsEmpty())
		return FALSE;
	return SetCurrentDirectory(str);
}

CString VGetFullPathName(LPCTSTR lpstrFile)
{
	CString strFullPath;
	LPSTR filePart;
	DWORD len = GetFullPathName(lpstrFile, MAX_PATH, strFullPath.GetBuffer(MAX_PATH), &filePart);
	if(len == 0)
		strFullPath.ReleaseBuffer(0);
	else if(len > MAX_PATH)
	{
		len += 16;
		strFullPath.ReleaseBuffer(0);
		DWORD lennew = GetFullPathName(lpstrFile, len, strFullPath.GetBuffer(len), &filePart);
		if(lennew == 0 || lennew > len)
			strFullPath.ReleaseBuffer(0);
		else
			strFullPath.ReleaseBuffer(lennew);
	}
	else
        strFullPath.ReleaseBuffer(len);
	return strFullPath;
}

//	return -1 if no address in file name
int GetAddrFromFileName(LPCTSTR filePathOrName)
{
	int a=-1;
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
	if(str-fname >= 4)
		if(*(str-4)==_TCHAR('_') && (*(str-3)==_TCHAR('-') || *(str-3)==_TCHAR('+'))
			&& _istxdigit(*(str-1)) && _istxdigit(*(str-1)))
			a=_tcstol(str-3, NULL, 16)*0x10000;
	if(a==-1 && str-fname >= 3)
		if(*(str-3)==_TCHAR('_') && _istxdigit(*(str-2)) && _istxdigit(*(str-1)))
			a=_tcstol(str-2, NULL, 16)*0x10000;
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

UINT64 GetAppVersion()
{
	UINT64 ver = -1;
	HRSRC vres = FindResource(NULL,	MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if(vres != NULL)
	{
		HGLOBAL vglb = LoadResource(NULL, vres);
		if(vglb != NULL)
		{
			LPBYTE vmem = (LPBYTE)LockResource(vglb);
			if(vmem != NULL)
			{
                ver = ((UINT64)(*((DWORD*)(vmem + 0x30))) << 32) | ((UINT64)(*((DWORD*)(vmem + 0x34))) << 0);
			}
		}
	}
	return ver;
}

CString GetAppFilePath(HMODULE hModule /*= NULL*/)
{
	CString filePath;
	DWORD len = GetModuleFileName(hModule == NULL ? AfxGetInstanceHandle() : hModule,
		filePath.GetBuffer(MAX_PATH + 1024), MAX_PATH + 1024);
	if(len == 0 || len == MAX_PATH + 1024)
		filePath.Empty();
	else
		filePath.ReleaseBuffer(len);
	return filePath;
}

CString GetVersionInfoString(LPCTSTR valueName, LPCTSTR pstrFilePath /*= NULL*/)
{
	CString filePath = pstrFilePath == NULL ? GetAppFilePath() : pstrFilePath;

	BYTE *pVerInfoValue = NULL;
	UINT VerInfoValueLen = 0;
	if(!filePath.IsEmpty())
	{
		DWORD h;
		DWORD visize = GetFileVersionInfoSize(filePath, &h);
		if(visize != 0)
		{
			BYTE * vibuf = new BYTE[visize];
			if(vibuf != NULL)
			{
				if(GetFileVersionInfo(filePath, h, visize, vibuf))
				{
					// Structure used to store enumerated languages and code pages.
					struct LANGANDCODEPAGE
					{
					  WORD wLanguage;
					  WORD wCodePage;
					} *lpTranslate;
					UINT vislen;
					BYTE *pvis;
					// Read the list of languages and code pages.
					if(VerQueryValue(vibuf, _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &vislen))
					{
						if(vislen != 0)
						{
							CString str;
							str.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),
									lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, valueName);
							if(VerQueryValue(vibuf, (LPTSTR)(LPCTSTR)str, (LPVOID*)&pvis, &vislen))
							{
								if(vislen != 0)
								{
									VerInfoValueLen = vislen;
									pVerInfoValue = new BYTE[VerInfoValueLen];
									if(pVerInfoValue != NULL)
										memcpy(pVerInfoValue, pvis, VerInfoValueLen);
								}
							}
						}
					}
				}
				delete vibuf;
			}
		}
	}

	CString valueString;
	if(pVerInfoValue != NULL)
	{
		valueString.SetString((char*)pVerInfoValue, VerInfoValueLen);
		delete pVerInfoValue;
		pVerInfoValue = NULL;
		VerInfoValueLen = 0;
	}
	return valueString;
}

CString GetVersionInfoFileVersion(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("FileVersion"), pstrFilePath);
}

CString GetVersionInfoFileDescription(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("FileDescription"), pstrFilePath);
}

CString GetVersionInfoCompanyName(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("CompanyName"), pstrFilePath);
}

CString GetVersionInfoComments(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("Comments"), pstrFilePath);
}

CString GetVersionInfoLegalCopyright(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("LegalCopyright"), pstrFilePath);
}

CString GetVersionInfoProductName(LPCTSTR pstrFilePath /*= NULL*/)
{
	return GetVersionInfoString(_T("ProductName"), pstrFilePath);
}


int CALLBACK BrowseFolder_BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg==BFFM_INITIALIZED)
		::SendMessage(hwnd, BFFM_SETSELECTION, FALSE, lpData);
	return 0;
}


BOOL BrowseFolder(CString& strOutAndStartFolder,
					 BOOL bCreateStartFolderIfNotExist /*= FALSE*/,
					 LPCTSTR pstrBowseDialogTitle /*= NULL*/,
					 HWND hwndBrowseDialogOwner /*= NULL*/)
{
	BOOL isOk = FALSE;
	LPITEMIDLIST pidl, curpidl = NULL;
	CString name, path = strOutAndStartFolder;
	if(!path.IsEmpty() && bCreateStartFolderIfNotExist)
		CreateDirectory(path, NULL);
	IShellFolder *pshf;
	if(SHGetDesktopFolder(&pshf) == NOERROR)
	{
		LPOLESTR ppath;
#ifdef UNICODE
		ppath = (LPOLESTR)(LPCTSTR)path;
#else
		ppath = new WCHAR[path.GetLength()+16];
		MultiByteToWideChar(CP_ACP, 0, path, -1, ppath, path.GetLength()+16);
#endif
		if(pshf->ParseDisplayName(hwndBrowseDialogOwner, NULL, ppath, NULL, &curpidl, NULL) != S_OK)
			curpidl = NULL;
#ifndef UNICODE
		delete ppath;
#endif
		pshf->Release();
	}
	IMalloc *im;
	if(SHGetMalloc(&im) != NOERROR)
		im = NULL;
	BROWSEINFO bi;
	bi.hwndOwner = hwndBrowseDialogOwner;
	bi.pidlRoot = NULL; //curpidl;
	bi.pszDisplayName = name.GetBuffer(MAX_PATH + 1024);
	bi.lpszTitle = pstrBowseDialogTitle != NULL ? pstrBowseDialogTitle : _T("Select folder...");
	bi.lpfn = BrowseFolder_BrowseCallbackProc;
	bi.lParam = (LPARAM)curpidl;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	pidl = SHBrowseForFolder(&bi);
	name.ReleaseBuffer();
	if(im != NULL && curpidl != NULL)
		im->Free(curpidl);
	if(pidl != NULL)
	{
		if(SHGetPathFromIDList(pidl, path.GetBuffer(MAX_PATH + 1024)))
		{
			path.ReleaseBuffer();
			IncludeTrailingSlash(path);
			strOutAndStartFolder = path;
			isOk = TRUE;
		}
		if(im != NULL)
			im->Free(pidl);
	}
	if(im != NULL)
		im->Release();
	return isOk;
}
