#pragma once

#include "VOptions.h"

#define SYMBOLS_MAX_COUNT	1024

class VKey
{
public:
	VKey(void);
	~VKey(void);
	CEdit *pSymCtrl;
	CEdit *pSymCodeCtrl;
	BOOL Init(LPCTSTR lpstrLayoutName, LPCTSTR lpstrKeyName, CEdit* pSymEditCtrl, CEdit* pSymCodeEditCtrl);
	CInt64Array	AddrOfRecord;
	WCHAR Symbols[SYMBOLS_MAX_COUNT+1];
	DWORD SymbolsCount;
	CString strLayoutName;
	CString strKeyName;
	BOOL LoadOptions(void);
	BOOL LoadFlash();
	BOOL SaveFlash(INT64 &BytesUsed);
	BOOL UpdateSymEdit(BOOL bSaveAndValidate=FALSE);
	BOOL UpdateSymCodeEdit(BOOL bSaveAndValidate=FALSE);
	BOOL LoadDocument(LPCTSTR lpstrDocFileName);
	BOOL SaveDocument(LPCTSTR lpstrDocFileName);
	INT64 GetStoreAddress(INT64 &BytesUsed);
	BOOL WriteSymInStoreAddress(INT64 StoreAddr);
};
