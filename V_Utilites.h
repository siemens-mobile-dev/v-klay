
#pragma once

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// version.lib must be added in linker options as Addidional Depencies (Config -> Linker -> Input -> Addidional Depencies)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// determine number of elements in an array (not bytes)
#define _countof(array) (sizeof(array)/sizeof(array[0]))

// determine number of characters in the string (not bytes)
#define _strlenof(str) (sizeof(str)/sizeof(str[0])-1)

#define FLOAT_INVALID						((float)-3.1e+38)
#define IS_FLOAT_VALID(floatnum)			((floatnum) >= -3.09999e+38)
#define F2I(floatnum)						((floatnum) + 0.5)

#define INT64_INVALID						((__int64)(((unsigned __int64)1) << 63))
#define IS_INT64_VALID(int64num)			(((unsigned __int64)(int64num)) != ((unsigned __int64)INT64_INVALID))

#define SHIFTED 0x8000


typedef unsigned __int64 VTIMER;
extern VTIMER SetTimerEventAfter(DWORD durMillisec);	// Sets the time duration after what the event has occur
extern BOOL IsTimerEvent(VTIMER vTimerEvent);			// Check, if timer event is occur?
extern BOOL IsDoubleEqual(double d1, double d2, double delta=0.00000001);
extern BOOL SetWindowNewText(CWnd *pWndCtrl, LPCTSTR lpszNew);


DWORD ParseEscapeString(LPTSTR lpszStr, LPTSTR lpszDest=NULL, LPTSTR *lpEnd=NULL, BOOL bDestNoUnicode=FALSE);
DWORD VGetPrivateProfileSection(LPCTSTR lpSection, CStringList *strKeysList, LPCTSTR lpFileName);
DWORD VGetPrivateProfileSectionNames(CStringList *strSectionsList, LPCTSTR lpFileName);
CString VGetPrivateProfileString(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpDefault, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
UINT VGetPrivateProfileHex(LPCTSTR lpSection,  LPCTSTR lpKey, INT nDefault, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
BOOL VGetPrivateProfileData(LPCTSTR lpSection, LPCTSTR lpKey, BYTE *&pData, DWORD &Length, LPCTSTR lpFileName, BOOL *IsValueExist=NULL);
CString IdentStr(LPCTSTR lpstrForIdent, int leadingSpacesCount=2, int trailingSpacesCount=0);
int CheckFileNameString(LPCTSTR strFN, LPCTSTR additionalReplace=NULL);
CString GetFileName(LPCTSTR filePath);
CString GetFileTitle(LPCTSTR filePath);
CString GetFileExt(LPCTSTR filePath);
CString GetFileDir(LPCTSTR filePath);
CString GetFileDriveRootPath(LPCTSTR filePath);
void IncludeTrailingSlash(CString &path);
BOOL SetForeground(CWnd *wnd);
UINT64 GetAppVersion();
CString GetAppFilePath(HMODULE hModule = NULL);
CString GetVersionInfoString(LPCTSTR valueName, LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoFileVersion(LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoFileDescription(LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoCompanyName(LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoComments(LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoLegalCopyright(LPCTSTR pstrFilePath = NULL);
CString GetVersionInfoProductName(LPCTSTR pstrFilePath = NULL);
BOOL BrowseFolder(CString& strOutAndStartFolder, BOOL bCreateStartFolderIfNotExist = FALSE, LPCTSTR pstrBowseDialogTitle = NULL, HWND hwndBrowseDialogOwner = NULL);
BOOL StrGetNextLine(LPCTSTR lpstrSource, DWORD* pCurPosInSource = NULL, CString* pstrOutLine = NULL);
CString VGetCurrentDirectory();
BOOL VSetCurrentDirectory(LPCTSTR lpstrNewCurDir);
CString VGetFullPathName(LPCTSTR lpstrFile); //Retrieves the full path and file name of the specified file. Merges the name of the current drive and directory with the specified file name to determine the full path and file name of the specified file.








