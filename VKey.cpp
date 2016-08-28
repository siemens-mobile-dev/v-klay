#include "StdAfx.h"
#include "vkey.h"
#include "V_KLay.h"


_TCHAR secLay[]=_T("KeysLocation%s");
_TCHAR keyKey[]=_T("Key%s");

VKey::VKey(void)
{
	pSymCtrl=NULL;
	pSymCodeCtrl=NULL;
	SymbolsCount=0;
}

VKey::~VKey(void)
{
}

BOOL VKey::Init(LPCTSTR lpstrLayoutName, LPCTSTR lpstrKeyName, CEdit* pSymEditCtrl, CEdit* pSymCodeEditCtrl)
{
	pSymCtrl=NULL;
	pSymCodeCtrl=NULL;
	SymbolsCount=0;
	if(pSymEditCtrl==NULL || pSymCodeEditCtrl==NULL)
		return FALSE;
	pSymCtrl=pSymEditCtrl;
	pSymCodeCtrl=pSymCodeEditCtrl;
	SetWindowLongPtr(pSymCtrl->GetSafeHwnd(), GWL_USERDATA, (LONG)(LONG_PTR)this);
	SetWindowLongPtr(pSymCodeCtrl->GetSafeHwnd(), GWL_USERDATA, (LONG)(LONG_PTR)this);
	AddrOfRecord.RemoveAll();
	strKeyName=lpstrKeyName;
	strLayoutName=lpstrLayoutName;
	return TRUE;
}

BOOL VKey::LoadOptions(void)
{
	if(!Options.IsValid)
	{
		pSymCodeCtrl->SetReadOnly(TRUE);
		pSymCtrl->SetReadOnly(TRUE);
		return FALSE;
	}
	CString sec,key;
	key.Format(keyKey, strKeyName);
	sec.Format(secLay, strLayoutName);
	INT64 t;
	CStringList seclist;
	VGetPrivateProfileSectionNames(&seclist, Options.strINIFileName);
	AddrOfRecord.RemoveAll();
	for(POSITION pos=seclist.GetHeadPosition(); pos;)
	{
		CString &seclistItem=seclist.GetNext(pos);
		if(_tcsncmp(seclistItem, sec, sec.GetLength())!=0)
			continue;
		t=VGetPrivateProfileHex(seclistItem, key, -1, Options.strINIFileName);
		if(t!=-1)
		{
			AddrOfRecord.Add(t + Options.FlashAddrCorrection);
		}
	}
	if(AddrOfRecord.GetSize()!=0)
	{
		pSymCodeCtrl->SetReadOnly(FALSE);
		pSymCtrl->SetReadOnly(FALSE);
	}
	else
	{
		pSymCodeCtrl->SetReadOnly(TRUE);
		pSymCtrl->SetReadOnly(TRUE);
	}
	return TRUE;
}

BOOL VKey::LoadFlash()
{
	if(!Options.IsValid)
	{
		AfxMessageBox(MSTR("For reading layout of key you must load the INI file first!"));
		return FALSE;
	}
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For reading layout of key you must select the device first!"));
		return FALSE;
	}
	BOOL ret=TRUE;
	INT64 t;
	pSymCodeCtrl->SetWindowText(NULL);
	pSymCtrl->SetWindowText(NULL);
	TRY
	{
		if(AddrOfRecord.GetSize())
		{
			INT64 addr=0;
			SymbolsCount=0;
			if(!pDevice->Read(AddrOfRecord[0], Options.SymbolsCountSize, &SymbolsCount))
				AfxThrowUserException();
			if(SymbolsCount > SYMBOLS_MAX_COUNT)
			{
				CString str;
				str.Format(_T("VKey: The count of symbols (%i) on key \"%s\" is greater than program can handle (%i).\nList of symbols will be truncated!"),
					(int)SymbolsCount, strKeyName, SYMBOLS_MAX_COUNT);
				AfxMessageBox(str);
				SymbolsCount = SYMBOLS_MAX_COUNT;
			}
			if(!pDevice->Read(AddrOfRecord[0]+Options.SymbolsCountSize, 4, &addr))
				AfxThrowUserException();
			t= addr - Options.BaseSoftAddr + Options.BaseFlashAddr;
			if(!pDevice->Read(t, SymbolsCount*2, Symbols))
				AfxThrowUserException();
			Symbols[SymbolsCount]=0;
		}
	}
	CATCH(CUserException, e)
	{
		ret=FALSE;
	}
	AND_CATCH_ALL(e)
	{
		e->ReportError();
		ret=FALSE;
	}
	END_CATCH_ALL;
	if(ret)
	{
		UpdateSymEdit(FALSE);
		UpdateSymCodeEdit(FALSE);
	}
	return ret;
}

INT64 VKey::GetStoreAddress(INT64 &BytesUsed)
{
	INT64 pos;
	INT64 BlSize, Used=BytesUsed;
	int i;
	for(i=0, pos=0; i<Options.StoreSizeOfBlock.GetSize(); i++)
	{
		BlSize=Options.StoreSizeOfBlock[i];
		if(Used<BlSize)
		{
			if(Used+SymbolsCount*2<=BlSize)
			{
				pos+=Used+SymbolsCount*2;
				break;
			}
			Used=BlSize;
		}
		pos+=BlSize;
		Used-=BlSize;
	}
	if(i==Options.StoreSizeOfBlock.GetSize())
		return -1;
	BytesUsed=pos;
	return Options.StoreAddrOfBlock[i]+Used;
}

BOOL VKey::WriteSymInStoreAddress(INT64 StoreAddr)
{
	if(pDevice==NULL)
		return FALSE;
	return pDevice->Write(StoreAddr, SymbolsCount*2, Symbols);
}

BOOL VKey::SaveFlash(INT64 &BytesUsed)
{
	if(!Options.IsValid)
	{
		AfxMessageBox(MSTR("For writing layout of key you must load the INI file first!"));
		return FALSE;
	}
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For writing layout of key you must select the device first!"));
		return FALSE;
	}
	BOOL ret=TRUE;
	TRY
	{
		if(AddrOfRecord.GetSize()!=0)
		{
			INT64 BlAddr=GetStoreAddress(BytesUsed);
			if(BlAddr==-1)
			{
				AfxMessageBox(MSTR("This keyboard layout does not fit in Flash."));
				AfxThrowUserException();
			}

			WriteSymInStoreAddress(BlAddr);
			INT64 pos = BlAddr - Options.BaseFlashAddr + Options.BaseSoftAddr;
			for(int i=0; i<AddrOfRecord.GetSize(); i++)
			{
				if(!pDevice->Write(AddrOfRecord[i], Options.SymbolsCountSize, &SymbolsCount))
					AfxThrowUserException();
				if(!pDevice->Write(AddrOfRecord[i]+Options.SymbolsCountSize, 4, &pos))
					AfxThrowUserException();
			}
		}
	}
	CATCH(CUserException, e)
	{
		ret=FALSE;
	}
	AND_CATCH_ALL(e)
	{
		e->ReportError();
		ret=FALSE;
	}
	END_CATCH_ALL;
	return ret;
}

BOOL VKey::UpdateSymEdit(BOOL bSaveAndValidate/*=FALSE*/)
{
	static BOOL IsEditUpdatingNow=FALSE;
	if(IsEditUpdatingNow)
		return TRUE;
	IsEditUpdatingNow=TRUE;
	CString str;
	DWORD i;
#ifndef _UNICODE
	char *pch=new char[MB_CUR_MAX];
#endif
	if(!bSaveAndValidate)
	{
		for(i=0; i<SymbolsCount; i++)
#ifdef _UNICODE
			str+= (WCHAR)(Symbols[i]==0 ? 0xfffe : Symbols[i]);
#else
		{
			memset(pch, 0, MB_CUR_MAX);
			wctomb(pch, Symbols[i]==0 ? 0x20 : Symbols[i]);
			str+=*pch;
		}
#endif
		pSymCtrl->SetWindowText(str);
	}
	else
	{
		pSymCtrl->GetWindowText(str);
		SymbolsCount=str.GetLength();
		if(SymbolsCount > SYMBOLS_MAX_COUNT)
			SymbolsCount = SYMBOLS_MAX_COUNT;
#ifdef _UNICODE
		for(i=0; i<SymbolsCount; i++)
			Symbols[i]= str.GetAt(i)==0xfffe ? 0x0000 : str.GetAt(i);
#else
		mbstowcs(Symbols, str, SymbolsCount);
#endif
		UpdateSymCodeEdit(FALSE);
	}
	IsEditUpdatingNow=FALSE;
#ifndef _UNICODE
	delete pch;
#endif
	return TRUE;
}

_TCHAR keysymformat[]=_T("0x%4.4x, ");

BOOL VKey::UpdateSymCodeEdit(BOOL bSaveAndValidate/*=FALSE*/)
{
	static BOOL IsEditUpdatingNow=FALSE;
	if(IsEditUpdatingNow)
		return TRUE;
	IsEditUpdatingNow=TRUE;
	CString str, strn;
	DWORD i;
	if(!bSaveAndValidate)
	{
		for(i=0; i<SymbolsCount; i++)
		{
			strn.Format(keysymformat, Symbols[i]);
			str+=strn;
		}
		i=str.GetLength();
		if(i>=2)
		{
			str.GetBuffer(i);
			str.ReleaseBuffer(i-2);
		}
		pSymCodeCtrl->SetWindowText(str);
	}
	else
	{
		pSymCodeCtrl->GetWindowText(str);
		str.TrimLeft();
		str.TrimRight();
		SymbolsCount=0;
		if(!str.IsEmpty())
		{
			LPTSTR lpstop, lpstr=(LPTSTR)((LPCTSTR)str);
			while(*lpstr)
			{
				Symbols[SymbolsCount++]=(WCHAR)_tcstol(lpstr, &lpstop, 0);
				for(;*lpstop && !v_istspace(*lpstop); lpstop++);
				if(lpstop==lpstr)
				{
					lpstr++;
					continue;
				}
				lpstr=lpstop;
			}
		}
		UpdateSymEdit(FALSE);
	}
	IsEditUpdatingNow=FALSE;
	return TRUE;
}


#ifdef _UNICODE
#define _KEYCHAR _TCHAR
#else
#define _KEYCHAR WORD
#endif
_TCHAR sdkeyid[]=_T("\r\nKey");
_TCHAR sdformat[]=_T("%s%s=");
BOOL VKey::LoadDocument(LPCTSTR lpstrDocFileName)
{
	BOOL ret=TRUE;
	SymbolsCount=0;
	pSymCodeCtrl->SetWindowText(NULL);
	pSymCtrl->SetWindowText(NULL);
	TRY
	{
		CFile f(lpstrDocFileName, CFile::modeRead | CFile::typeBinary);
#ifdef _UNICODE
		CString str;
		str.Format(sdformat, strKeyName, strLayoutName);
		str=CString(sdkeyid)+str;
		int eqmatchcount=str.GetLength();
#else
		CString str1;
		str1.Format(sdformat, strKeyName, strLayoutName);
		str1=CString(sdkeyid)+str1;
		_KEYCHAR str[1024];
		int eqmatchcount=0;
		for(; eqmatchcount<str1.GetLength(); eqmatchcount++)
			str[eqmatchcount]=(_KEYCHAR)str1[eqmatchcount];
#endif
		DWORD pos, fsize=(DWORD)f.GetLength();
		_KEYCHAR ch;
		int eqcount;
		for(eqcount=0, pos=0; pos<fsize;)
		{
			f.Read(&ch, sizeof(_KEYCHAR));
			pos+=sizeof(_KEYCHAR);
			if(ch==str[eqcount])
			{
				if(++eqcount==eqmatchcount)
				{
					break;
				}
			}
			else
			{
				eqcount=0;
				if(ch==str[eqcount])
					++eqcount;
			}
		}
		if(pos<fsize)
		{
			eqcount=0; eqmatchcount=sizeof(sdkeyid)/sizeof(_KEYCHAR)-1;
			for(; pos<fsize; )
			{
				f.Read(&ch, sizeof(_KEYCHAR));
				pos+=sizeof(_KEYCHAR);
				if(ch==sdkeyid[eqcount])
				{
					if(++eqcount==eqmatchcount)
						break;
				}
				else
				{
					if(eqcount)
					{
						if(SymbolsCount+eqcount>=SYMBOLS_MAX_COUNT)
							eqcount=SYMBOLS_MAX_COUNT-SymbolsCount;
						memcpy(Symbols+SymbolsCount, sdkeyid, eqcount*sizeof(_KEYCHAR));
						SymbolsCount+=eqcount;
						eqcount=0;
						if(SymbolsCount>=SYMBOLS_MAX_COUNT)
							break;
					}
					if(ch==sdkeyid[eqcount])
					{
						++eqcount;
					}
					else
					{
						Symbols[SymbolsCount++]=ch;
						if(SymbolsCount>=SYMBOLS_MAX_COUNT)
							break;
					}
				}
			}
			if(pos>=fsize && eqcount)
			{
				if(SymbolsCount+eqcount>=SYMBOLS_MAX_COUNT)
					eqcount=SYMBOLS_MAX_COUNT-SymbolsCount;
				memcpy(Symbols+SymbolsCount, sdkeyid, eqcount*sizeof(_KEYCHAR));
				SymbolsCount+=eqcount;
			}
		}
		f.Close();
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		ret=FALSE;
	}
	END_CATCH_ALL;
	UpdateSymEdit(FALSE);
	UpdateSymCodeEdit(FALSE);
	return ret;
}

BOOL VKey::SaveDocument(LPCTSTR lpstrDocFileName)
{
	TRY
	{
		CFile f(lpstrDocFileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary);
		f.Seek(0, CFile::end);
#ifdef _UNICODE
		CString str;
		str.Format(sdformat, strKeyName, strLayoutName);
		str=CString(sdkeyid)+str;
		f.Write(str, str.GetLength()*sizeof(_KEYCHAR));
#else
		CString str1;
		str1.Format(sdformat, strKeyName, strLayoutName);
		str1=CString(sdkeyid)+str1;
		_KEYCHAR str[1024];
		int eqmatchcount=0;
		for(; eqmatchcount<str1.GetLength(); eqmatchcount++)
			str[eqmatchcount]=(_KEYCHAR)str1[eqmatchcount];
		f.Write(str, eqmatchcount*sizeof(_KEYCHAR));
#endif
		f.Write(Symbols, SymbolsCount*sizeof(_KEYCHAR));
		f.Close();
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}
