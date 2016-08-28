#include "StdAfx.h"
#include "voptions.h"
#include "v_klay.h"
#include "KLayDlg.h"

VOptions::VOptions(void)
{
	IsValid=FALSE;
	INIPartialFlashAddress=-1;
	INIPartialFlashSize=-1;
	FlashAddrCorrection=0;
	SoftAddrCorrection=0;
	SymbolsCountSize=2;
}

VOptions::~VOptions(void)
{
}

_TCHAR secAddr[]=_T("KeysAdressing");
_TCHAR keyPfa[]=_T("PartialFlashAddress");
_TCHAR keyPfs[]=_T("PartialFlashSize");
_TCHAR keySac[]=_T("SoftAddrCorrection");
_TCHAR keyFac[]=_T("FlashAddrCorrection");
_TCHAR keyBsa[]=_T("MapBaseSoftAddr");
_TCHAR keyBfa[]=_T("MapBaseFlashAddr");
_TCHAR keyScs[]=_T("SymCountSize");

_TCHAR secStorBl[]=_T("KeysStorageBlock%4.4i");
_TCHAR keySbb[]=_T("Begin");
_TCHAR keySbs[]=_T("Size");

BOOL VOptions::Load(LPCTSTR lpIniFilePath)
{
	if(lpIniFilePath==NULL)
	{
		CFileDialog fdlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, NULL, AfxGetMainWnd());
		fdlg.m_ofn.lpstrTitle=openDialogTitle;
		if(fdlg.DoModal()!=IDOK)
			return FALSE;
		strINIFileName=fdlg.GetPathName();
	}
	else
		strINIFileName=lpIniFilePath;

	CKLayDlg *dlg=(CKLayDlg*)AfxGetMainWnd();
	IsValid=FALSE;
	SoftAddrCorrection=VGetPrivateProfileHex(secAddr, keySac, 0, strINIFileName);
	FlashAddrCorrection=VGetPrivateProfileHex(secAddr, keyFac, 0, strINIFileName);
	BaseSoftAddr=VGetPrivateProfileHex(secAddr, keyBsa, -1, strINIFileName);
	BaseFlashAddr=VGetPrivateProfileHex(secAddr, keyBfa, -1, strINIFileName);
	INIPartialFlashAddress=VGetPrivateProfileHex(secAddr, keyPfa, -1, strINIFileName);
	INIPartialFlashSize=VGetPrivateProfileHex(secAddr, keyPfs, -1, strINIFileName);
	if(BaseSoftAddr==-1 || BaseFlashAddr==-1 || INIPartialFlashAddress==-1 || INIPartialFlashSize==-1)
		return FALSE;
	BaseSoftAddr=BaseSoftAddr+SoftAddrCorrection;
	BaseFlashAddr=BaseFlashAddr+FlashAddrCorrection;

	SymbolsCountSize=(int)VGetPrivateProfileHex(secAddr, keyScs, 2, strINIFileName);
	if(SymbolsCountSize > sizeof(DWORD))
		return FALSE;

	int i;
	CString str;
	StoreAddrOfBlock.RemoveAll();
	StoreSizeOfBlock.RemoveAll();
	INT64 t;
	for(i=0; ; i++)
	{
		str.Format(secStorBl, i);
		t=VGetPrivateProfileHex(str, keySbb, -1, strINIFileName);
		if(t==-1)
			break;
		StoreAddrOfBlock.Add(t+FlashAddrCorrection);
        t=VGetPrivateProfileHex(str, keySbs, 0, strINIFileName);
		if(t==0)
			return FALSE;
		StoreSizeOfBlock.Add(t);
	}
	if(i==0)
		return FALSE;
	IsValid=TRUE;
	if(dlg!=NULL)
		for(int i=0; i<VKL_PAGES_COUNT; i++)
			if(dlg->pPage[i])
				dlg->pPage[i]->SendMessage(VM_OPTIONS_CHANGED, i, (LPARAM)this);
	return TRUE;
}
