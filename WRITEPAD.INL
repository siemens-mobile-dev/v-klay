// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: writepad.inl
//
//  PURPOSE: Inline Functions
//
//  FUNCTIONS:
//
//  COMMENTS:
//


//
// ANSI/UNICODE conversions
//

extern int _temp;

__inline LPWSTR AtoWHelper(LPWSTR pw, LPCSTR pa, int nChars)
{
    pw[0] = 0;
    MultiByteToWideChar(CP_ACP,
                        0,
                        pa,
                        -1,
                        pw,
                        nChars);
    return pw;
}


#define AtoW(pa) (\
    ((LPCSTR)pa == NULL) ? NULL : (\
        _temp = lstrlen(pa) + 1,\
        AtoWHelper((LPWSTR)alloca(_temp * 2), pa, _temp)\
    )\
)


__inline LPSTR WtoAHelper(LPSTR pa, LPCWSTR pw, int nBytes)
{
    pa[0] = 0;
    WideCharToMultiByte(CP_ACP,
                        0,
                        pw,
                        -1,
                        pa,
                        nBytes,
                        NULL,
                        NULL);
    return pa;
}


#define WtoA(pw) (\
    ((LPCWSTR)pw == NULL) ? NULL : (\
        _temp = ((int)wcslen(pw) + 1) * 2,\
        WtoAHelper((LPSTR)alloca(_temp), pw, _temp)\
    )\
)
