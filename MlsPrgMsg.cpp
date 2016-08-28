// MlsPrgMsg.cpp: implementation of the CMlsPrgMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MlsPrgMsg.h"
#include <wincrypt.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMlsPrgMsg::CMlsPrgMsg()
{
	Init_CRC32_Table();
	m_lpFileBuffer=NULL;
	m_nFileSize=0;
//	m_wCurLang=INVALID_LANGID;									//V/
	m_wCurLang=MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT);	//V/  the class must work even if lang id is not set.
	m_msgStore.RemoveAll();
}

CMlsPrgMsg::~CMlsPrgMsg()
{

}

LPCTSTR CMlsPrgMsg::GetMultiLangStr(LPCTSTR lpcszStr)
{
	if (m_wCurLang==INVALID_LANGID) return lpcszStr;

	UINT64 key=GetStringKey(lpcszStr);
//	UINT c32=Get_CRC(lpcszStr);
	CLangMsg::CPair *pCurVal=m_msgStore.PLookup(key);
	if(pCurVal==NULL)
		return lpcszStr; // string is not found, so function will return the same as got
	return (LPCTSTR)pCurVal->value;
}

void CMlsPrgMsg::SetCurrentLang(WORD wLangId)
{
	m_wCurLang=wLangId;
}

BOOL CMlsPrgMsg::ParsePlainFile(LPCTSTR lpcszFilename, int encoding/* = 0*/)
{
	CFile *hSource=NULL;
	LPBYTE lpDump=NULL;
	LONG nFileSize=0;
	BOOL b=FALSE;
    // Open source file.
	TRY
	{
		hSource = new CFile(lpcszFilename, CFile::modeRead|CFile::typeBinary); 
		nFileSize=(LONG)hSource->GetLength();
		lpDump=new BYTE[nFileSize+MAX_PATH];
		hSource->Read(lpDump, nFileSize);
		hSource->Close();
		b=ParseMemoryDump(lpDump, nFileSize, encoding);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
	if(hSource!=NULL)
		delete hSource;
	if(lpDump!=NULL)
		delete lpDump;
	return b;
}

BOOL CMlsPrgMsg::ParseEncFile(LPCTSTR lpcszFilename,LPCTSTR lpcszPassword)
{
	CFile *hSource      = NULL;
    BOOL eof = FALSE;
	INT nDecryptedBytes = 0;

    HCRYPTPROV hProv   = 0;
    HCRYPTKEY hKey     = 0;
    HCRYPTHASH hHash   = 0;

    PBYTE pbKeyBlob = NULL;
    PBYTE pbBuffer = NULL;
    DWORD dwBlockLen;
    DWORD dwBufferLen;
    DWORD dwCount;

    BOOL status = FALSE;

	PBYTE szPassword=NULL;
	INT len;

	len=(INT)_tcslen(lpcszPassword);
	szPassword=new BYTE[len+2];
	memset(szPassword,0,len+2);

#ifndef UNICODE
	memcpy(szPassword,lpcszPassword,len);
#else
	wcstombs((char *)szPassword,lpcszPassword,len);
#endif


	TRY
	{
		// Open source file.
		hSource = new CFile(lpcszFilename, CFile::modeRead|CFile::typeBinary);
		m_nFileSize=(long)hSource->GetLength();
		m_lpFileBuffer= new BYTE[m_nFileSize+MAX_PATH];
		memset(m_lpFileBuffer, 0 , m_nFileSize+MAX_PATH);

		// Get handle to the default provider.
		if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, 0 ))
		{
			if(!CryptAcquireContext(&hProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET ))
			{
				//        printf("Error %x during CryptAcquireContext!\n", GetLastError());
				AfxThrowUserException();
			};
			//        printf("Error %x during CryptAcquireContext!\n", GetLastError());
			AfxThrowUserException();
		};

		// Decrypt the file with a session key derived from a password.

		// Create a hash object.
		if(!CryptCreateHash(hProv, HASH_ALGORITHM, 0, 0, &hHash)) 
		{
	//	    printf("Error %x during CryptCreateHash!\n", GetLastError());
			AfxThrowUserException();
		}

		// Hash in the password data.
		if(!CryptHashData(hHash, szPassword, len, 0)) 
		{
	//	    printf("Error %x during CryptHashData!\n", GetLastError());
			AfxThrowUserException();
		}

		// Derive a session key from the hash object.
		if(!CryptDeriveKey(hProv, ENCRYPT_ALGORITHM, hHash, 0, &hKey)) 
		{
	//	    printf("Error %x during CryptDeriveKey!\n", GetLastError());
			AfxThrowUserException();
		}

		// Destroy the hash object.
		CryptDestroyHash(hHash);
		hHash = 0;

		dwBlockLen = 1000;
		dwBufferLen = dwBlockLen;

		pbBuffer = new BYTE[dwBufferLen];

		do 
		{
			// Read up to 'dwBlockLen' bytes from source file.
			dwCount = hSource->Read(pbBuffer, dwBlockLen);
			if (hSource->GetPosition() < m_nFileSize) 
				eof=FALSE;
			else
				eof=TRUE;

			// Decrypt data
			if(!CryptDecrypt(hKey, 0, eof, 0, pbBuffer, &dwCount)) 
			{
	//            printf("Error %x during CryptDecrypt!\n", GetLastError());
				AfxThrowUserException();
			};

			// Write data to the destination memory block
			memcpy((LPBYTE)(m_lpFileBuffer+nDecryptedBytes),pbBuffer,dwCount);
			nDecryptedBytes+=dwCount;

		} while(!eof);

		status = TRUE;

		ParseMemoryDump(m_lpFileBuffer,m_nFileSize);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		m_nFileSize=0;
	}
	END_CATCH_ALL;
    // Close file.
    if(hSource!=NULL)
		delete hSource;

    // Free memory.
    if(pbKeyBlob!=NULL)
		delete pbKeyBlob;
    if(pbBuffer!=NULL)
		delete pbBuffer;
	if(szPassword!=NULL)
		delete szPassword;

    // Destroy session key.
    if(hKey) CryptDestroyKey(hKey);

    // Destroy hash object.
    if(hHash) CryptDestroyHash(hHash);

    // Release provider handle.
    if(hProv) CryptReleaseContext(hProv, 0);

	DeleteMemoryDump();

	return status;
}

BOOL CMlsPrgMsg::ParseMemoryDump(LPBYTE lpDump, int nDumpSize, int encoding/* = 0*/)
{
	if (lpDump==NULL) return FALSE;

	char szLen[10];
	char szCrc[10];
	char szLangId[10];
	char *p;

	int i=0,nStartStrPos=0,nEndStrPos,len,j;
	UINT32 crc,lnt;
	UINT64 key;
	BOOL bGotEOL=FALSE;

	sprintf(szLangId, "L%04x", m_wCurLang);

	while (i<nDumpSize)
	{
		if (!bGotEOL)
		{
			if ((*(lpDump+i)==0x0D) && (*(lpDump+i+1)==0x0A))
			{
				nEndStrPos=i;
				bGotEOL=TRUE;
				i++;
			}
		}
		else
		{
			// 'p' will be a pointer to decrypted string
			// 'len' will be a size of decrypted string

			len=nEndStrPos-nStartStrPos;
			p=(char *)lpDump+nStartStrPos;
			*(lpDump+nEndStrPos)=0;

			bGotEOL=FALSE;
			nStartStrPos=i;

			// A new language string begins with "#define " string

			if ((len>15) && (strncmp(p,"#define ",8)==0))
			{
				// looking for valid language identifier...

//V/ read all strings, because in one file it is strings for only one language//	if (strncmp(p+8,szLangId,5)==0)
				{
					// Got new string with known language identifier.
					// Now we will find a key of the string

					j=(int)strlen("#define ")+4;
					while ((*(p+j)!=' ') && (j<len)) j++;
					if ((*(p+j)==' ') && (j<len))
					{
						// The 32-bit key of the string is written in hexdecimical format

						memset(szLen,0,sizeof(szLen));
						strncpy(szLen,p+j-11,3);
						sscanf(szLen,"%x",&lnt);

						memset(szCrc,0,sizeof(szCrc));
						strncpy(szCrc,p+j-8,8);
						sscanf(szCrc,"%x",&crc);

						key=lnt;
						key=key<<32;
						key+=crc;

						// The key of the string has been obtained.
						// Now, search for a beginning of the actual string constant
						// and its length

						while ((*(p+j)!='"') && (j<len)) j++;
						if (*(p+j)=='"')
						{
							j=j+1; // j - beginning of the string constant from p

							while ((*(p+len)!='"') && (len>0)) len--;

							if (*(p+len)=='"')
							{
								// replace the latest quotes with 0 to
								// fix the end of the string constant

								*(p+len)=0; len--;
								if (len>j)
								{
									// Put the string constant with key
									// in the corresponding multimap
#ifndef UNICODE
									EscapeString(p+j);
									m_msgStore.SetAt(key, CString(p+j));
#else
									len = (int)strlen(p + j);
									WCHAR *w = NULL;
									w = new WCHAR[len + 16];
									w[0] = 0;
									if(encoding == 1)
										MultiByteToWideChar(CP_UTF8, 0, p + j, -1, w, len + 1);
									else
										mbstowcs(w, p + j, len);
									w[len] = 0;
									EscapeString(w);
									m_msgStore.SetAt(key, CString(w));
									delete w;
#endif
								}
							}
						}
					}
				}
			}
		}
		i++;
	}

	return TRUE;
}

void CMlsPrgMsg::DeleteMemoryDump()
{
	if(m_lpFileBuffer==NULL)
		return;
	delete m_lpFileBuffer;
	m_lpFileBuffer=NULL;
	m_nFileSize=0;
}

// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
UINT32 CMlsPrgMsg::Reflect(UINT32 ref, char ch)
{// Used only by Init_CRC32_Table()

	UINT32 value=0;

	// Swap bit 0 for bit 7
	// bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

// Call this function only once to initialize the CRC table.
void CMlsPrgMsg::Init_CRC32_Table()
{
	// This is the official polynomial used by CRC-32 
	// in PKZip, WinZip and Ethernet. 
	UINT32 ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i]=Reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
		crc32_table[i] = Reflect(crc32_table[i], 32);
	}
}

UINT32 CMlsPrgMsg::Get_CRC(LPCSTR text)
{
	UINT32  ulCRC(0xffffffff);
	unsigned char* buffer;
	int len = (int)strlen(text);
	buffer = (unsigned char*)text;
	while(len--)
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++];
	return ulCRC ^ 0xffffffff;
}

UINT32 CMlsPrgMsg::Get_CRC(LPCWSTR text)
{
	UINT32  ulCRC(0xffffffff);
	unsigned char* buffer;
	int len = (int)wcslen(text);
	buffer = (unsigned char*)text;
	while(len--)
	{
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer];
		buffer+=2;
	};
	return ulCRC ^ 0xffffffff;
}

UINT64 CMlsPrgMsg::GetStringKey(LPCSTR lpszStr)
{
	UINT64 key;
	UINT32 crc32=Get_CRC(lpszStr);
	UINT32 len=(UINT32)strlen(lpszStr);
	key=len;
	key=key<<32;
	key+=crc32;
	return key;
}

UINT64 CMlsPrgMsg::GetStringKey(LPCWSTR lpszStr)
{
	UINT64 key;
	UINT32 crc32=Get_CRC(lpszStr);
	UINT32 len=(UINT32)wcslen(lpszStr);
	key=len;
	key=key<<32;
	key+=crc32;
	return key;
}

// This function converts a string that may contains escape 
// symbols (beginning with backslash '\' character).
//
// The single parameter is a pointer to the converted string.
// Since a result is always less or equal than original string,
// therefore no reason to use an additional buffer.
//
// The result will appear instead original string at the same buffer.

DWORD CMlsPrgMsg::EscapeString(LPTSTR lpszStr)
{
	if(lpszStr==NULL)
		return 0;
	int len=(int)_tcslen(lpszStr);
	if(len==0)
		return 0;

	LPCTSTR s=lpszStr;
	LPTSTR t, d=lpszStr;
	TCHAR sz[10];
#ifndef UNICODE
	char *mbchar=new char[MB_CUR_MAX];
#endif

	for(; *s!=_T('\0'); s++, d++)
	{
		if (*s!=_T('\\'))
		{
			*d=*s;
			continue;  // non-escape symbol
		}

		// Got escape sequence
		if(s[1]==_T('\0'))
			break;
		s++;
		memset(sz,0,sizeof(sz));
		memcpy(sz, s, (len-(s-lpszStr) < 8 ? len-(s-lpszStr) : 8)*sizeof(_TCHAR));

		// ASCII character in octal notation?
		if(sz[0] >= _T('0') && sz[0] <= _T('7'))
		{
			sz[3]=_T('\0');
			*d=(_TCHAR)_tcstoul(sz, &t, 8);
			s+=t-sz-1;
			continue;
		}

		switch(sz[0])
		{
			// ASCII character in hexadecimal notation?
		case _T('x'):
			sz[3]=_T('\0');
			*d=(_TCHAR)_tcstoul(sz+1, &t, 16);
			if(t-(sz+1) == 2)
				s+=2;
			else
				*d=_T('x');
			break;

			// Unicode character using hexadecimal representation (exactly four digits)?
		case _T('u'):
			sz[5]=_T('\0');
#ifdef UNICODE
			*d=(_TCHAR)_tcstoul(sz+1, &t, 16);
#else
			*d=_TCHAR('?');
			if(mbchar!=NULL)
				if(wctomb(mbchar, (wchar_t)_tcstoul(sz+1, &t, 16))!=-1)
					*d=mbchar[0];
#endif
			if(t-(sz+1) == 4)
				s+=4;
			else
				*d=_T('u');
			break;

		case _T('a'):
			*d=_T('\a');
			break;
		case _T('b'):
			*d=_T('\b');
			break;
		case _T('t'):
			*d=_T('\t');
			break;
		case _T('r'):
			*d=_T('\r');
			break;
		case _T('v'):
			*d=_T('\v');
			break;
		case _T('f'):
			*d=_T('\f');
			break;
		case _T('n'):
			*d=_T('\n');
			break;
		case _T('e'):
			*d=_T('\x1B');
			break;
			// Microsoft Specific —>
			// If a backslash precedes a character that does not appear in Table 1.4,
			// the compiler handles the undefined character as the character itself. 
			// For example, \x is treated as an x.
			// END Microsoft Specific
		default:
			*d=*s;
			break;
		};
	}

#ifndef UNICODE
	if(mbchar!=NULL)
		delete mbchar;
#endif
	*d=_T('\0');
	return (DWORD)(d-lpszStr);
}
