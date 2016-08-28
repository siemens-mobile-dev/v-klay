#pragma once

typedef CArray<INT64, INT64> CInt64Array;

//Options for keyboard layout from .INI file for phone firmwares.
class VOptions
{
public:
	VOptions(void);
	~VOptions(void);
	BOOL Load(LPCTSTR lpIniFilePath=NULL);
	BOOL IsValid;
	CInt64Array StoreAddrOfBlock;
	CInt64Array StoreSizeOfBlock;
	INT64 INIPartialFlashAddress;
	INT64 INIPartialFlashSize;
	INT64 BaseSoftAddr;
	INT64 BaseFlashAddr;
	INT64 FlashAddrCorrection;
	INT64 SoftAddrCorrection;
	CString strINIFileName;
	int SymbolsCountSize;
};
