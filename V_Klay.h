// V_KLay.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "VOptions.h"
#include "vkey.h"
#include "VDevice.h"
#include "ProgressDlg.h"
#include <afxpriv.h> // for WM_SETMESSAGESTRING and WM_IDLEUPDATECMDUI
#include "strings.h"
#include "EXECryptor\SDK\MSVC\EXECryptor.h"

#define TIMEOUT_INFINITY							-1
typedef unsigned __int64 VTIMER;

extern CString o_strLocale;
extern int o_PatcherFontSize;
extern CString o_strLanguage;
extern BOOL o_MainWndMaximize;

extern CString o_strPatchDir;
extern CString o_strLayoutDir;
extern CString o_strFlasherDir;
extern CString o_strFlashPartDir;

extern BOOL o_bDisableWarningsMsg;
extern BOOL o_bDisableRTFSaveWarningsMsg;
extern int o_AutoignitionType;
#define AUTOIGN_AUTO		0
#define AUTOIGN_SIMPLE		1
#define AUTOIGN_ORIGINAL	2
#define AUTOIGN_NONE		-1
extern BOOL o_bCommSimple;
extern BOOL o_bIsRepairPatchCanSkip;
extern BOOL o_bIsPatchIgnogeNoOld;
extern CString o_CommentGroupString;
extern BOOL o_bIsRepairPatchTextSimple;
extern BOOL o_bCommRTS;
extern BOOL o_bCommDTR;
extern BOOL o_bDoNotInsertLocaleInfoInPatch;
extern BOOL o_bIsEnableBootcoreWritting;
//extern BOOL o_bIsHideWarningForNoWriteNoReadArea;
extern INT64 o_PatcherWrapAddr;				// if address of block in text of patch is greater than this value, this value will be subtracted from address.
extern BOOL o_bIsPatcherSwapPasteKeys;
extern BOOL o_bIsDisableFubuAutosave;

extern BOOL o_cmdQuit;
extern DWORD o_cmdWorkWith;
extern CString o_cmdPhone;
extern CString o_cmdPhonePortName;
extern DWORD o_cmdPhoneBaud;
extern CString o_cmdPartFName;
extern INT64 o_cmdPartFrom;
extern BOOL o_cmdDoWrite;
extern BOOL o_cmdDoRead;
extern INT64 o_cmdFrom;
extern CString o_cmdFileName;

extern BOOL o_cmdExist_bDisableWarningsMsg;
extern BOOL o_cmdExist_WorkWith;
extern BOOL o_cmdExist_Phone;
extern BOOL o_cmdExist_PhonePortName;
extern BOOL o_cmdExist_PhoneBaud;
extern BOOL o_cmdExist_PartFName;
extern BOOL o_cmdExist_PartFrom;
extern BOOL o_cmdExist_From;

extern _TCHAR secMainWnd[];
extern _TCHAR keyIsMaximized[];
extern _TCHAR keyXPos[];
extern _TCHAR keyYPos[];
extern _TCHAR keyXSize[];
extern _TCHAR keyYSize[];

extern CString vkpfilter;
extern CString vkpfiltersave;
extern CString vklaydocfilter;
extern CString exefilter;
extern CString openDialogTitle;
extern CString saveDialogTitle;

extern _TCHAR keyToolsRunMethod[];
extern _TCHAR keyToolExtLayPath[];
extern _TCHAR keyToolExtPicPath[];
extern _TCHAR keyToolExtFntPath[];
extern _TCHAR keyToolExt1Path[];
extern _TCHAR keyToolExt2Path[];
extern _TCHAR keyToolExt3Path[];
extern _TCHAR keyToolExtLayArg[];
extern _TCHAR keyToolExtPicArg[];
extern _TCHAR keyToolExtFntArg[];
extern _TCHAR keyToolExt1Arg[];
extern _TCHAR keyToolExt2Arg[];
extern _TCHAR keyToolExt3Arg[];
extern _TCHAR keyTmpPath[];
extern _TCHAR defaultTmpPathRelativeApp[];

extern _TCHAR addr64format[];
extern _TCHAR addrformat[];
extern _TCHAR dataformat[];
extern _TCHAR int64format[];
extern _TCHAR uint64format[];
extern _TCHAR intformat[];
extern _TCHAR uintformat[];
extern _TCHAR hex4digformat[];
extern _TCHAR hex8diglongformat[];
extern _TCHAR hex16digi64format[];


extern CString flashfilter;
extern _TCHAR secOptions[];
extern _TCHAR keyCurDevTyp[];
extern _TCHAR keyCurPhone[];


extern CString strToolsToolTipFormat;
extern _TCHAR strFNTimeFormat[];

extern _TCHAR* ExtToolArgs[];




#define VM_IS_CAN_CLOSE				WM_USER+1
#define VM_OPTIONS_CHANGED			WM_USER+2
#define VM_IDLE						WM_USER+3
#define VM_INITIAL_UPDATE			WM_USER+4
#define VM_DEVICE_CHANGED			WM_USER+5
#define VM_GET_PATCH_MENU			WM_USER+6
#define VM_APP_OPTIONS_CHANGED		WM_USER+7

#define SHIFTED 0x8000

typedef struct OLECALLBACK_MENU_DATA_tag
{
	WORD seltype;
	LPOLEOBJECT poleobj;
    CHARRANGE* pchrg;
	HMENU* phmenu;
} OLECALLBACK_MENU_DATA;


// CKLayApp:
// See V_KLay.cpp for the implementation of this class
//

class CKLayApp : public CWinApp
{
public:
	CString m_AppPath;
	CString m_LoadersPath;
	CString m_LangPath;
	CString m_INIPath;

	CKLayApp();
	~CKLayApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual BOOL IsIdleMessage( MSG* pMsg );
//	CCommandLineInfo m_cmdInfo;

// Implementation

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnIdle(LONG lCount);
	CString GetCmdLineFileName(void);
	BOOL LoadOptions(void);
	BOOL SaveOptions(void);
protected:
//#if _MFC_VER < 0x0700
//	HINSTANCE m_richEditLibrary;
//#endif;
public:
	CString GetLangFileInfo(LPCTSTR LangFName, LPCTSTR strInfoName);
	BOOL ShowHelp(void);
//	virtual int ExitInstance();
	virtual int ExitInstance();
};

extern CKLayApp theApp;
extern VOptions Options;
extern int cklaydlg_oldphonelistitem;

extern DWORD VGetPrivateProfileSection(LPCTSTR lpSection, CStringList *strKeysList, LPCTSTR lpFileName);
extern DWORD VGetPrivateProfileSectionNames(CStringList *strSectionsList, LPCTSTR lpFileName);
extern CString VGetPrivateProfileString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
extern INT64 VGetPrivateProfileHex(LPCTSTR lpSection, LPCTSTR lpKey, INT64 nDefault, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
inline INT64 VGetPrivateProfileInt(LPCTSTR lpSection, LPCTSTR lpKey, INT64 nDefault, LPCTSTR lpFileName, BOOL *IsValueExist=NULL) {return VGetPrivateProfileHex(lpSection, lpKey, nDefault, lpFileName, IsValueExist);}
extern BOOL VGetPrivateProfileData(LPCTSTR lpSection, LPCTSTR lpKey, BYTE *&pData, DWORD &Length, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
extern CString VGetParameterFromList(const CString& strList, int& curPos);
int VStrAToBCD(char* pStrA, void* pbcdNumBuf, int bcdNumBufLen);
VTIMER SetTimerEventAfter(DWORD durMillisec);
BOOL IsTimerEvent(VTIMER vTimerEvent);
INT64 VStrToInt(LPCTSTR str);
INT64 VStrToSignedHex(LPCTSTR str, LPTSTR* end=NULL);
CString VSignedHexToStr(INT64 signedHex, LPCTSTR format=NULL);



extern void ShowPatchOutOfFlashMessage(UINT strID, DWORD PatchAddr, DWORD PatchSize);
extern VDevice *pDevice;
extern CProgressDlg *pProgress;
extern UINT64 GetAppVersion();
extern CString GetFileNameFromPath(LPCTSTR strPath);
extern void AFXAPI DDX_TextRich2(CDataExchange* pDX, int nIDC, CString& value);
extern CString IdentStr(LPCTSTR lpstrForIdent, int spacesCount=2, int trailingSpacesCount=0);
extern DWORD getdata(LPCTSTR& b, BYTE* data, BYTE* bytesInGroup=NULL, DWORD groupsNum=0, DWORD* groupsOutNum=NULL);
extern int CheckFileNameString(LPCTSTR strFN, LPCTSTR additionalReplace=NULL);
extern CString GetFileName(LPCTSTR filePath);
extern CString GetFileTitle(LPCTSTR filePath);
extern CString GetFileExt(LPCTSTR filePath);
extern CString GetFileDir(LPCTSTR filePath);
extern CString GetFileDriveRootPath(LPCTSTR filePath);
extern void IncludeTrailingSlash(CString &path);
extern BOOL SetForeground(CWnd *wnd);
extern DWORD RichStreamIn(CRichEditCtrl *pRichCtrl, LPCTSTR data, DWORD dataLen, int nFormat);
extern DWORD GetRichStreamOutSize(CRichEditCtrl *pRichCtrl, int nFormat);
extern DWORD RichStreamOut(CRichEditCtrl *pRichCtrl, LPCTSTR dataBuf, DWORD dataBufLen, int nFormat);
extern DWORD RichStreamOutTextPartial(CRichEditCtrl *pRichCtrl, CString& strText, DWORD minCharPos, DWORD maxCharPos);
extern DWORD RichStreamOutText(CRichEditCtrl *pRichCtrl, CString& strText);
extern BOOL v_istspace(_TCHAR ch);
extern INT64 GetAddrFromFileName(LPCTSTR filePathOrName);
extern DWORD ParseEscapeString(LPTSTR lpszStr, LPTSTR lpszDest=NULL, LPTSTR *lpEnd=NULL, BOOL bDestNoUnicode=FALSE);
extern BOOL VCancelIo(IN HANDLE hFile);
extern BOOL SndMsg(LPCTSTR sndFileName, int beepNumIfSndFailed = 1);
extern CString AddPathToFileNameAndExpandEnvStr(LPCTSTR fileName, LPCTSTR fileNameForGetPath);
extern CString GetDefaultFlashFileName(INT64 fromAddr, LPCTSTR strExtra = NULL);


#define _countof(array)		(sizeof(array)/sizeof(array[0]))

