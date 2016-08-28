// MlsPrgMsg.h: interface for the CMlsPrgMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MLSPRGMSG_H__BF101CB7_2CD8_4B71_BB77_AD3D3A9FC7C0__INCLUDED_)
#define AFX_MLSPRGMSG_H__BF101CB7_2CD8_4B71_BB77_AD3D3A9FC7C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef INVALID_LANGID
#define INVALID_LANGID 0x0000
#endif

#define HASH_ALGORITHM CALG_MD5
#define ENCRYPT_ALGORITHM CALG_RC4
#define ENCRYPT_BLOCK_SIZE 1

#ifndef UNICODE
typedef CMap<UINT64, UINT64, CString, CString&> CLangMsg;
#else
typedef CMap<UINT64, UINT64, CString, CString&> CLangMsg;
#endif

class CMlsPrgMsg  
{
public:
	CMlsPrgMsg();
	virtual ~CMlsPrgMsg();

	LPCTSTR GetMultiLangStr(LPCTSTR lpcszStr);

	void SetCurrentLang(WORD wLangId);
	WORD GetCurrentLang() { return m_wCurLang; };

	BOOL ParsePlainFile(LPCTSTR lpcszFile, int encoding = 0);
	BOOL ParseEncFile(LPCTSTR lpcszFilename,LPCTSTR lpcszPassword);
	BOOL ParseMemoryDump(LPBYTE lpDump, int nDumpSize, int encoding = 0);
	void DeleteMemoryDump();

	// for CRC32
	UINT32 Get_CRC(LPCSTR text);
	UINT32 Get_CRC(LPCWSTR text);

	UINT64 GetStringKey(LPCSTR lpszStr);
	UINT64 GetStringKey(LPCWSTR lpszStr);

protected:
	WORD m_wCurLang;
	CLangMsg m_msgStore;

	LPBYTE	m_lpFileBuffer;
	long	m_nFileSize;

	// for CRC32
	void Init_CRC32_Table();
	UINT32 crc32_table[256]; // Lookup table array
	UINT32 Reflect(UINT32 ref, char ch); // Reflects CRC bits in the lookup table

	DWORD EscapeString(LPTSTR lpszStr);
};

#endif // !defined(AFX_MLSPRGMSG_H__BF101CB7_2CD8_4B71_BB77_AD3D3A9FC7C0__INCLUDED_)
