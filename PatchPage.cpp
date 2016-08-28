// PatchPage.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "PatchPage.h"
#include "KLayDlg.h"
#include ".\patchpage.h"



/////////////////////////////////////////////////////////
DWORD patchCurLine;
DWORD patchCurCol;
__int64 patchCurAddrCorrection;
DWORD patchFlags;
BOOL IsMultilineComment;
CRichEditCtrl *patchRichEditCtrl=NULL;

// Register the FINDMSGSTRING message
UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

///////////////////////////////////////
_TCHAR patchDirective[]=_T("#pragma");

_TCHAR patchDirActDisable[]=_T("disable");
_TCHAR patchDirActEnable[]=_T("enable");
_TCHAR patchDirActSet[]=_T("set");
_TCHAR patchDirActQuery[]=_T("query");

_TCHAR patchDirParWarnOld[]=_T("warn_no_old_on_apply");
_TCHAR patchDirParWarnOldNew[]=_T("warn_if_new_exist_on_apply");
_TCHAR patchDirParUndo[]=_T("undo");
_TCHAR patchDirParWarnNewNotFound[]=_T("warn_if_old_exist_on_undo");
_TCHAR patchDirParOldEqualFF[]=_T("old_equal_ff");


_TCHAR patchRun[]=_T("#run");

_TCHAR patchRunTypeID[]=_T("on");
_TCHAR patchRunTypeOnApply[]=_T("apply");
_TCHAR patchRunTypeOnUndo[]=_T("undo");

_TCHAR patchRunMemset[]=_T("memset");
_TCHAR patchRunMemcpy[]=_T("memcpy");
_TCHAR patchRunMemreplace[]=_T("memreplace");
_TCHAR patchRunMemwritefromfile[]=_T("memwritefromfile");
_TCHAR patchRunMemreadtofile[]=_T("memreadtofile");

VPatchBlockFuncMemset pbfMemset;
VPatchBlockFuncMemcpy pbfMemcpy;
VPatchBlockFuncMemreplace pbfMemrep;
VPatchBlockFuncMemwritefromfile pbfMemwritefromfile;
VPatchBlockFuncMemreadtofile pbfMemreadtofile;

VPatchBlock *pPatchBlockFunctions[]={&pbfMemset, &pbfMemcpy, &pbfMemrep, &pbfMemwritefromfile, &pbfMemreadtofile};
#define VPB_FUNC_NUM	(sizeof(pPatchBlockFunctions)/sizeof(VPatchBlock*))

CString msgApplyDescr;
CString msgUndoDescr;
CString msgRepairIn;

_TCHAR appendHeaderFormat[]=_T("\n\n;______________________________________________________________________\n;\n; %s\n;______________________________________________________________________\n;\n\n");
_TCHAR separatorPatchText[]=_T("______________________________________________________________________");

#define _tsizeof(str) ((sizeof(str)-1)/sizeof(_TCHAR))

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void HilightPatchLine(DWORD line=-1)
{
	if(line==-1)
		line=patchCurLine;
	if(patchRichEditCtrl!=NULL && line!=0)
	{
		CHARRANGE cr;
		cr.cpMin=patchRichEditCtrl->LineIndex(line-1);
		cr.cpMax=patchRichEditCtrl->LineIndex(line);
		if(cr.cpMax!=-1 && cr.cpMax!=0 && cr.cpMax>cr.cpMin)
			cr.cpMax--;
		patchRichEditCtrl->SetSel(cr);
	}
}

void throwUser(LPCTSTR msgStr, DWORD line, BOOL bEnableErrorMessages/*=TRUE*/)
{
	if(bEnableErrorMessages)
	{
		CString str, msg;
		str.Format(_T("%i"), line);
		LPCTSTR rglpsz[1];
		rglpsz[0] = str;
		AfxFormatStrings(msg, msgStr, rglpsz, 1);
		if(line!=0)
			HilightPatchLine(line);
		AfxMessageBox(msg, MB_OK|MB_ICONSTOP);
	}
	AfxThrowUserException();
}

//////////////////////////////////////////////////////////////////////////
DWORD skipnewline(LPCTSTR &b)
{
	if(*b!=_TCHAR('\n') && *b!=_TCHAR('\r'))
		return 0;
	DWORD len=1;
	b++;
	if(*(b-1)==_TCHAR('\n'))
	{
		if(*b==_TCHAR('\r'))
		{
			b++;
			len++;
		}
	}
	else
	{
		if(*b==_TCHAR('\n'))
		{
			b++;
			len++;
		}
	}
	return len;
}

//////////////////////////////////////////////////////////////////////////
//DWORD skipconcatenating(LPCTSTR &b, BOOL bEnableErrorMessages=TRUE)
//{
//	if(*b!=_T('\\'))
//		return 0;
//	DWORD len;
//	b++;
//	if(!(len=skipnewline(b)))
//		throwUser(MSTR("Line %1:\nAfter concatenating symbol must be a new line."), patchCurLine, bEnableErrorMessages);
//	len++;
//	patchCurLine++;
//	return TRUE;
//}

//////////////////////////////////////////////////////////////////////////
DWORD skipdataspaces(LPCTSTR &b, BOOL bEnableErrorMessages=TRUE)
{
	DWORD n, len;
	for(len=0;;)
	{
		for(; *b && (*b==_TCHAR('\t') || *b==_TCHAR(' ')); b++, len++);
		//if(!(n=skipconcatenating(b, bEnableErrorMessages)))
			break;
		len+=n;
	}
	return len;
}

//////////////////////////////////////////////////////////////////////////
DWORD skipspaces(LPCTSTR &b)
{
	DWORD len=0;
	for(; *b && (*b==_TCHAR('\t') || *b==_TCHAR(' ')); b++, len++);
	return len;
}

//////////////////////////////////////////////////////////////////////////
// symbol is end of line with patch data
// (patch data - this is a symbols that a not inside the comments)
BOOL isendofdataline(LPCTSTR &b)
{
	BOOL isend= *b==_TCHAR('\n') || *b==_TCHAR('\r') || *b==_TCHAR(';') || *b==0;
	if(!isend)
	{
		isend= b[0]==_T('/') && b[1]==_T('*');
		if(isend)
			IsMultilineComment=TRUE;
		else
			isend= b[0]==_T('/') && b[1]==_T('/');
	}
	return isend;
}


//////////////////////////////////////////////////////////////////////////
DWORD findendofdataline(LPCTSTR b, BOOL bEnableErrorMessages=TRUE, BOOL bNotChangeLineCounter=FALSE)
{
	if(!*b)
		return 0;
	DWORD n, len, oldPatchCurLine=patchCurLine;
	for(len=0;;)
	{
		n=(DWORD)_tcscspn(b, _T("\r\n"));
		len+=n;
		b+=n;
		if(n==0 || *b==0)
			break;
		b--;
		//if(!(n=skipconcatenating(b, bEnableErrorMessages)))
		{
			b++;
			break;
		}
		len+=n;
	}
	if(bNotChangeLineCounter)
		patchCurLine=oldPatchCurLine;
	return len;
}

//////////////////////////////////////////////////////////////////////////
void findendofline(LPCTSTR &b)
{
	b+=_tcscspn(b, _T("\r\n"));
	if(!*b)
		return;
	skipnewline(b);
}

//////////////////////////////////////////////////////////////////////////

BYTE getdata_itab[] = {3, 5, 8, 10, 13, 15, 17, 20};
UINT64 getdata_iumax[] = {0xff, 0xffff, 0xffffff, 0xffffffff, 0xffffffffff, 0xffffffffffff, 0xffffffffffffff, 0xffffffffffffffff};
INT64  getdata_ismin[] = {-128, -32768, -8388608, -(int)2147483648, -549755813888, -140737488355328, -36028797018963968, -(__int64)9223372036854775808};
INT64  getdata_ismax[] = {+127, +32767, +8388607, +2147483647, +549755813887, +140737488355327, +36028797018963967, +9223372036854775807};

DWORD getdata(LPCTSTR& b, BYTE* data, BYTE* bytesInGroup/*=NULL*/, DWORD groupsNum/*=0*/, DWORD* groupsOutNum/*=NULL*/)
{
	DWORD len, n, count=0;
	int i;
	_TCHAR numbuf[3]={0,0,0};
	_TCHAR *end;
	DWORD groupIdx=0;
	if(groupsOutNum)
	{
		*groupsOutNum=0;
		if(bytesInGroup && groupsNum)
			memset(bytesInGroup, 0, min(groupsNum, sizeof(DWORD)));
	}
	DWORD groupBegCount=count;
	while(*b)
	{
		if(!_istxdigit(b[0]))
		{
			if(b[0]==_T(',') && (_istxdigit(b[1]) || b[1]==_T('\"') || b[1]==_T('\'')))
			{
				b++;
				if(groupsOutNum && groupIdx<groupsNum)
				{
					if(count-groupBegCount > 255)
						return -1;
					bytesInGroup[groupIdx]=(BYTE)(count-groupBegCount);
				}
				groupIdx++;
				groupBegCount=count;
				continue;
			}
			else if(b[0]==_T('\"'))
			{
				len=ParseEscapeString((LPTSTR)(b+1), (LPTSTR)data, (LPTSTR*)&b, TRUE);
				data+=len;
				count+=len;
				if(b[0] != _T('\"'))
					return -1;
				b++;
				continue;
			}
			else if(b[0]==_T('\''))
			{
#ifdef UNICODE
				len = ParseEscapeString((LPTSTR)(b+1), (LPTSTR)data, (LPTSTR*)&b, FALSE);
#else
				len = ParseEscapeString((LPTSTR)(b+1), (LPTSTR)data, (LPTSTR*)&b, TRUE);
				if(len != 0)
				{
					for(i = len-1; i >= 0; i--)
					{
						data[i*2+0] = data[i];
						data[i*2+1] = 0;
					}
				}
#endif
				len *= 2;
				data+=len;
				count+=len;
				if(b[0] !=_T('\''))
					return -1;
				b++;
				continue;
			}
			break;
		}
		if(b[0]==_T('0'))
		{
			len=0;
			if(b[1]==_T('n'))
			{
				n=_tcstoul(b+=2, &end, 2);
				len=(DWORD)(end-b);
				if(len==0)
					return -1;
				len= len/8 + (len%8 == 0 ? 0 : 1);
				memcpy(data, &n, len);
			}
			else if(b[1]==_T('x'))
			{
				n=_tcstoul(b+=2, &end, 16);
				len=(DWORD)(end-b);
				if(len==0)
					return -1;
				len= len/2 + (len%2 == 0 ? 0 : 1);
				memcpy(data, &n, len);
			}
			else if(b[1]==_T('i'))
			{
				//if(groupsOutNum || bytesInGroup==NULL || groupsNum==0 || groupIdx >= groupsNum)
				//	return -1;
				//if(groupsNum==1)
				//	len=((DWORD*)bytesInGroup)[0];
				//else
				//	len=bytesInGroup[groupIdx];
				//if(len <= (count-groupBegCount))
				//	return -1;
				//len=len-(count-groupBegCount);
				//__int64 num64=0;
				//n=_stscanf(b+=2, _T("%I64d"), &num64);
				//if(n!=1 || n==EOF)
				//	return -1;
				//if(*b==_T('+') || *b==_T('-'))
				//	b++;
				//for(end=(_TCHAR*)b; _istdigit(*end); end++);
				//memset(data, num64 < 0 ? -1 : 0, len);
				//n=min(len, sizeof(__int64));
				//memcpy(data, &num64, n);
				//if(n<sizeof(__int64))
				//	if(((BYTE*)&num64)[n] != (num64 < 0 ? 0xff : 0x00))
				//		return -1;
				__int64 num64 = 0;
				n = _stscanf(b+=2, _T("%I64d"), &num64);
				if(n!=1 || n==EOF)
					return -1;
				BOOL bsig = FALSE;
				if(*b == _T('+') || *b == _T('-'))
				{
					bsig = TRUE;
					b++;
				}
				for(end = (_TCHAR*)b; _istdigit(*end); end++);
				len = (DWORD)(end - b);
				if(len == 0)
					return -1;
				if(len < getdata_itab[0])
				{
					len = 1;
				}
				else
				{
					for(i = 0; i < _countof(getdata_itab); i++)
						if(getdata_itab[i] == len)
							break;
					if(i == _countof(getdata_itab))
					{
						AfxMessageBox(_T("Wrong number of digits in integer (0i), must be: 3 (for BYTE), 5 (for WORD), 8 (for 3 BYTES), 10 (for DWORD), 13 (for 5 BYTES), 15 (for 6 BYTES),  17 (for 7 BYTES), 20 (for 8 BYTES). Use leading zeroes for match amount of digits."),
							MB_OK | MB_ICONSTOP);
						return -1;
					}
					len = i + 1;
					if(bsig)
					{
						if(num64 > getdata_ismax[i] || num64 < getdata_ismin[i])
						{
							CString str;
							str.Format(_T("Signed integer value (0i%I64i) is out of range (from %I64i to %I64i)."),
								num64, getdata_ismin[i], getdata_ismax[i]);
							AfxMessageBox(str, MB_OK | MB_ICONSTOP);
							return -1;
						}
					}
					else
					{
						if(((UINT64)num64) > getdata_iumax[i])
						{
							CString str;
							str.Format(_T("Unsigned integer value (0i%I64u) is out of range (from %I64u to %I64u)."),
								(UINT64)num64, (UINT64)0, getdata_iumax[i]);
							AfxMessageBox(str, MB_OK | MB_ICONSTOP);
							return -1;
						}
					}
				}
				memcpy(data, &num64, len);
			}

			if(len)
			{
				data+=len;
				count+=len;
				b=end;
				if(*b!=_T(',') && !_istspace(*b) && *b!=0)
					return -1;
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
	if(groupsOutNum && groupIdx<groupsNum)
	{
		if(count-groupBegCount > 255)
		{
			if(groupIdx==0 && groupsNum>=sizeof(DWORD))
			{
				((DWORD*)bytesInGroup)[0]=(DWORD)(count-groupBegCount);
			}
			else
				return -1;
		}
		else
			bytesInGroup[groupIdx]=(BYTE)(count-groupBegCount);
	}
	if(groupsOutNum)
		*groupsOutNum=groupIdx+1;
	return count;
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


VPatchBlock::VPatchBlock(void)
{
	m_NewData=NULL;
	m_OldData=NULL;
	m_PhoneData=NULL;
	m_Flags=0;
	m_Size=0;
	m_NewSize=0;
	m_Addr=0;
	m_Stat=0;
	m_TextLine=-1;
}

VPatchBlock::~VPatchBlock(void)
{
	Clear();
}

BOOL VPatchBlock::Clear(void)
{
	if(m_NewData!=NULL)
	{
		delete m_NewData;
		m_NewData=NULL;
	}
	if(m_OldData!=NULL)
	{
		delete m_OldData;
		m_OldData=NULL;
	}
	if(m_PhoneData!=NULL)
	{
		delete m_PhoneData;
		m_PhoneData=NULL;
	}
	m_Flags=0;
	m_Size=0;
	m_NewSize=0;
	m_Addr=0;
	m_Stat=0;
	m_TextLine=-1;
	return TRUE;
}

VPatchBlock& VPatchBlock::operator=(const VPatchBlock& src)
{
	Clear();
	m_Addr=src.m_Addr;
	m_Size=src.m_Size;
	m_NewSize=src.m_NewSize;
	m_Flags=src.m_Flags;
	m_Stat=src.m_Stat;
	m_TextLine=src.m_TextLine;
	if(src.m_NewData!=NULL)
	{
		m_NewData=new BYTE[m_Size];
		if(m_NewData!=NULL)
			memcpy(m_NewData, src.m_NewData, m_Size);
	}
	if(src.m_OldData!=NULL)
	{
		m_OldData=new BYTE[m_Size];
		if(m_OldData!=NULL)
			memcpy(m_OldData, src.m_OldData, m_Size);
	}
	if(src.m_PhoneData!=NULL)
	{
		m_PhoneData=new BYTE[m_Size];
		if(m_PhoneData!=NULL)
			memcpy(m_PhoneData, src.m_PhoneData, m_Size);
	}
	return *this;
}

BOOL VPatchBlock::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	Clear();
	m_TextLine=patchCurLine;
	skipspaces(strLine);
	if(isendofdataline(strLine))
		return FALSE;
	if(!_istxdigit(*strLine))
		throwUser(MSTR("Line %1:\nAddress in format \"_HEX_NUMBER_: \" is expected (after number and before ':' - nothing, after ':' must be at least 1 space)."),
			m_TextLine, bEnableErrorMessages);

	_TCHAR *end;
	DWORD len;
	__int64 tr=0;
	int ret=_stscanf(strLine, _T("%I64x"), &tr);
	if(ret==1 && ret!=EOF && tr > ((DWORD)(-1)))
	{
		CString num, str, tmp;
		num.Format(_T("0x%X"), ((DWORD)-1));
		str=MSTR("Line %1:\nAddress overflow (must be less or equal to %s).");
		int pos=str.Find(_T("%1"));
		if(pos!=-1)
			str.SetAt(pos, 0x01);
		tmp.Format(str, num);
		if(pos!=-1)
		{
			str=_T("_1");
			str.SetAt(0,0x01);
			pos=tmp.Find(str);
			if(pos!=-1)
				tmp.SetAt(pos, _T('%'));
		}
		throwUser(tmp,
			m_TextLine, bEnableErrorMessages);
	}
	DWORD num=_tcstoul(strLine, &end, 16);
	len=(DWORD)(end-strLine);
    strLine=end;
	if(len==0 || strLine[0]!=_TCHAR(':')/* || !v_istspace(strLine[1])*/)
		throwUser(MSTR("Line %1:\nAddress in format \"_HEX_NUMBER_: \" is expected (after number and before ':' - nothing, after ':' must be at least 1 space)."),
			m_TextLine, bEnableErrorMessages);
	strLine++;

	skipdataspaces(strLine, bEnableErrorMessages);
	if(!_istxdigit(*strLine) && *strLine != _T('\"') && *strLine != _T('\''))
		throwUser(MSTR("Line %1:\nAfter address must be an old or new data."),
			m_TextLine, bEnableErrorMessages);

	m_Addr=(DWORD)(((__int64)num)+patchCurAddrCorrection);
	if(m_Addr > (DWORD)o_PatcherWrapAddr)
		m_Addr -= (DWORD)o_PatcherWrapAddr;
	len=findendofdataline(strLine, bEnableErrorMessages, TRUE)+8;
	BYTE *buf=NULL;
	BYTE *bytesInGroup=NULL;

	TRY
	{
		DWORD groupsNum, groupsRealNum=0;
		for(groupsNum=4, num=0; num<len; num++)
			if(strLine[num]==_T(','))
				groupsNum++;
		bytesInGroup=new BYTE[groupsNum+16];

		buf=new BYTE[len * 2 + 16];
		DWORD sizeold=getdata(strLine, buf, bytesInGroup, groupsNum, &groupsRealNum);
		DWORD sizenew=0;
		if(sizeold==-1)
			throwUser(MSTR("Line %1:\nThe number of symbols in old(new) data filed must be even."),
				m_TextLine, bEnableErrorMessages);
		skipdataspaces(strLine, bEnableErrorMessages);
		if(_istxdigit(*strLine) || *strLine == _T('\"') || *strLine == _T('\''))
		{
			sizenew=getdata(strLine, buf+sizeold, bytesInGroup, groupsRealNum, NULL);
			if(sizenew==-1)
				throwUser(MSTR("Line %1:\nThe number of symbols in the new data field must be even."),
					m_TextLine, bEnableErrorMessages);
			skipdataspaces(strLine, bEnableErrorMessages);
			if(!isendofdataline(strLine))
				throwUser(MSTR("Line %1:\nBad symbols after data."),
					m_TextLine, bEnableErrorMessages);
		}
		else if(!isendofdataline(strLine))
			throwUser(MSTR("Line %1:\nBad symbols in place, were there must be new data or nothing."),
				m_TextLine, bEnableErrorMessages);

		if(sizenew==0)
		{
			sizenew=sizeold;
			sizeold=0;
		}
		else if(sizenew > sizeold)
			throwUser(MSTR("Line %1:\nThe size of the old data must be greater or equal to the size of new data."),
				m_TextLine, bEnableErrorMessages);

		m_Size=max(sizenew,sizeold);
		m_NewSize=sizenew;
		m_NewData=new BYTE[m_Size];
		memcpy(m_NewData, buf+sizeold, sizenew);
		if(sizeold!=0)
		{
			memcpy(m_NewData+sizenew, buf+sizenew, m_Size-sizenew);
			m_OldData=new BYTE[m_Size];
			memcpy(m_OldData, buf, sizeold);
		}
		else if(patchFlags&VPB_FLAG_ENA_OLD_EQUAL_FF)
		{
			m_OldData=new BYTE[m_Size];
			memset(m_OldData, -1, m_Size);
		}
		else
		{
			m_OldData=NULL;
		}
		delete buf;

		m_Flags=patchFlags;
	}
	CATCH_ALL(e)
	{
		if(buf!=NULL)
			delete buf;
		buf=NULL;

		if(bytesInGroup!=NULL)
			delete bytesInGroup;
		bytesInGroup=NULL;

		THROW_LAST();
	}
	END_CATCH_ALL;

	return TRUE;
}

// Calculate new data dynamically
BOOL VPatchBlock::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	return TRUE;
}

BYTE* VPatchBlock::GetDataPtrFromDataCode(UINT dataCode)
{
	switch(dataCode)
	{
	case VPB_MAKETXT_OLD_DATA:
		return m_OldData;
	case VPB_MAKETXT_NEW_DATA:
		return m_NewData;
	case VPB_MAKETXT_PHONE_DATA:
		return m_PhoneData;
	}
	return NULL;
}

#define VPB_MAKETXT_BLOCK_LEN	16
_TCHAR strVPBMakeTextLine[]=_T("                                ");
// Convert data in block to the text string of patch
CString VPatchBlock::MakeTextLine(
	UINT dataOld/*=VPB_MAKETXT_OLD_DATA*/, UINT dataNew/*=VPB_MAKETXT_NEW_DATA*/, UINT dataComments/*=VPB_MAKETXT_NO_DATA*/,
	UINT MakeType/*=VPB_TYPE_ANY*/, UINT dataCmp1/*=VPB_MAKETXT_NO_DATA*/, UINT dataCmp2/*=VPB_MAKETXT_NO_DATA*/,
	BOOL bNoDataIdent/*=FALSE*/)
{
	BYTE *olddat, *newdat, *commdat, *cmp1dat, *cmp2dat;
	olddat=GetDataPtrFromDataCode(dataOld);
	newdat=GetDataPtrFromDataCode(dataNew);
	commdat=GetDataPtrFromDataCode(dataComments);
	cmp1dat=GetDataPtrFromDataCode(dataCmp1);
	cmp2dat=GetDataPtrFromDataCode(dataCmp2);
	CString line;
	if((olddat==NULL && newdat==NULL)
		|| m_Size==0 || newdat==NULL)
		return line;
	if(cmp1dat==NULL || cmp2dat==NULL)
		MakeType=VPB_TYPE_ANY;

	CString str, strNew, strOld, strComm;
	DWORD i, b, s, addr, ident;
	for(b=0, s=0; b<m_Size; b+=s)
	{
		if(MakeType==VPB_TYPE_EQUAL)
		{
			for(; cmp1dat[b]!=cmp2dat[b] && b<m_Size; b++);
			if(b>=m_Size)
				break;
			addr=(m_Addr+b)&~(VPB_MAKETXT_BLOCK_LEN-1);
			for(s=1; b+s<m_Size && addr==((m_Addr+b+s)&~(VPB_MAKETXT_BLOCK_LEN-1)); s++)
				if(cmp1dat[b+s]!=cmp2dat[b+s])
					break;
		}
		else if(MakeType==VPB_TYPE_DIFFERENT)
		{
			for(; cmp1dat[b]==cmp2dat[b] && b<m_Size; b++);
			if(b>=m_Size)
				break;
			addr=(m_Addr+b)&~(VPB_MAKETXT_BLOCK_LEN-1);
			for(s=1; b+s<m_Size && addr==((m_Addr+b+s)&~(VPB_MAKETXT_BLOCK_LEN-1)); s++)
				if(cmp1dat[b+s]==cmp2dat[b+s])
					break;
		}
		else
		{
			addr=(m_Addr+b)&~(VPB_MAKETXT_BLOCK_LEN-1);
			s=min(m_Size-b, (addr+VPB_MAKETXT_BLOCK_LEN)-(m_Addr+b));
		}

		ident=(m_Addr+b-addr)*2;
		str.Format(_T("%6.6X: "), m_Addr+b);
		line+=str;
		if(bNoDataIdent)
		{
			strOld.Empty();
			strNew.Empty();
			strComm.Empty();
		}
		else
		{
			strOld=strVPBMakeTextLine;
			strNew=strVPBMakeTextLine;
			strComm=strVPBMakeTextLine;
		}
		for(i=0; i<s; i++, ident+=2)
		{
			if(olddat!=NULL)
			{
				str.Format(_T("%2.2X"), olddat[i+b]);
				if(bNoDataIdent)
					strOld+=str;
				else
					strOld.CopyChars(((LPTSTR)(LPCTSTR)strOld)+ident, str, str.GetLength());
			}
			if(newdat!=NULL)
			{
				str.Format(_T("%2.2X"), newdat[i+b]);
				if(bNoDataIdent)
					strNew+=str;
				else
					strNew.CopyChars(((LPTSTR)(LPCTSTR)strNew)+ident, str, str.GetLength());
			}
			if(commdat!=NULL)
			{
				str.Format(_T("%2.2X"), commdat[i+b]);
				if(bNoDataIdent)
					strComm+=str;
				else
					strComm.CopyChars(((LPTSTR)(LPCTSTR)strComm)+ident, str, str.GetLength());
			}
		}
		line+=strOld;
		line+=_TCHAR(' ');
		line+=strNew;
		if(commdat!=NULL)
		{
			line+=_T(" ;");
			line+=strComm;
		}
		line+=_T("\r\n");
	}
//line+=_T("\r\n;**********************************************************************\r\n");
	return line;
}
//////////////////////////////////////////////////////////////////////////



VPatchBlockFunc::VPatchBlockFunc(void)
{
	m_FuncName=NULL;
}

BOOL VPatchBlockFunc::Clear(void)
{
	return VPatchBlock::Clear();
}

VPatchBlockFunc& VPatchBlockFunc::operator=(const VPatchBlockFunc& src)
{
	Clear();
	VPatchBlock::operator=(src);
	m_FuncName=src.m_FuncName;
	return *this;
}

BOOL VPatchBlockFunc::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	Clear();
	if(m_FuncName==NULL)
		return FALSE;
	m_TextLine=patchCurLine;
	skipspaces(strLine);
	if(isendofdataline(strLine))
		return FALSE;
	if(_tcsnicmp(strLine, m_FuncName, _tcslen(m_FuncName))!=0)
		return FALSE;
	strLine+=_tcslen(m_FuncName);
	skipspaces(strLine);
	if(*strLine!=_T('('))
		return FALSE;
	m_Flags=patchFlags;
	return TRUE;
}

// Calculate new data dynamically
BOOL VPatchBlockFunc::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	m_NewData=NULL;
	m_NewSize=0;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////



VPatchBlockFuncMemset::VPatchBlockFuncMemset(void)
{
	m_FuncName=patchRunMemset;
	m_ValueData=NULL;
	m_ValueSize=0;
}

BOOL VPatchBlockFuncMemset::Clear(void)
{
	VPatchBlockFunc::Clear();
	if(m_ValueData!=NULL)
	{
		delete m_ValueData;
		m_ValueData=NULL;
	}
	m_ValueSize=0;
	return TRUE;
}

VPatchBlockFuncMemset& VPatchBlockFuncMemset::operator=(const VPatchBlockFuncMemset& src)
{
	Clear();
	VPatchBlockFunc::operator=(src);
	m_ValueSize=src.m_ValueSize;
	if(src.m_ValueData!=NULL)
	{
		m_ValueData=new BYTE[m_ValueSize];
		if(m_ValueData!=NULL)
			memcpy(m_ValueData, src.m_ValueData, m_ValueSize);
	}
	return *this;
}

BOOL VPatchBlockFuncMemset::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	if(!VPatchBlockFunc::ParseTextLine(strLine, bEnableErrorMessages))
		return FALSE;
	return TRUE;
}

// Calculate new data dynamically
BOOL VPatchBlockFuncMemset::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	if(!VPatchBlockFunc::Run(PatchBlocks, PatchStream, pPatchDevice, pCurPhoneData))
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////



VPatchBlockFuncMemcpy::VPatchBlockFuncMemcpy()
{
	m_FuncName=patchRunMemcpy;
	m_AddrFrom=0;
}

BOOL VPatchBlockFuncMemcpy::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	if(!VPatchBlockFunc::ParseTextLine(strLine, bEnableErrorMessages))
		return FALSE;
	return TRUE;
}

// Calculate new data dynamically
BOOL VPatchBlockFuncMemcpy::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	if(!VPatchBlockFunc::Run(PatchBlocks, PatchStream, pPatchDevice, pCurPhoneData))
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////



VPatchBlockFuncMemreplace::VPatchBlockFuncMemreplace(void)
{
	m_FuncName=patchRunMemreplace;
	m_FindData=NULL;
	m_ReplaceData=NULL;
	m_FindReplaceSize=0;
	m_MaskByte=-1;
}

BOOL VPatchBlockFuncMemreplace::Clear(void)
{
	VPatchBlockFunc::Clear();
	if(m_FindData!=NULL)
	{
		delete m_FindData;
		m_FindData=NULL;
	}
	if(m_ReplaceData!=NULL)
	{
		delete m_ReplaceData;
		m_ReplaceData=NULL;
	}
	m_FindReplaceSize=0;
	m_MaskByte=-1;
	return TRUE;
}

VPatchBlockFuncMemreplace& VPatchBlockFuncMemreplace::operator=(const VPatchBlockFuncMemreplace& src)
{
	Clear();
	VPatchBlockFunc::operator=(src);
	m_FindReplaceSize=src.m_FindReplaceSize;
	m_MaskByte=src.m_MaskByte;
	if(src.m_FindData!=NULL)
	{
		m_FindData=new BYTE[m_FindReplaceSize];
		if(m_FindData!=NULL)
			memcpy(m_FindData, src.m_FindData, m_FindReplaceSize);
	}
	if(src.m_ReplaceData!=NULL)
	{
		m_ReplaceData=new BYTE[m_FindReplaceSize];
		if(m_ReplaceData!=NULL)
			memcpy(m_ReplaceData, src.m_ReplaceData, m_FindReplaceSize);
	}
	return *this;
}

BOOL VPatchBlockFuncMemreplace::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	if(!VPatchBlockFunc::ParseTextLine(strLine, bEnableErrorMessages))
		return FALSE;
	return TRUE;
}

// Calculate new data dynamically
BOOL VPatchBlockFuncMemreplace::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	if(!VPatchBlockFunc::Run(PatchBlocks, PatchStream, pPatchDevice, pCurPhoneData))
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////




VPatchBlockFuncMemwritefromfile::VPatchBlockFuncMemwritefromfile()
{
	m_FuncName=patchRunMemwritefromfile;
	m_FileOffset=0;
}

BOOL VPatchBlockFuncMemwritefromfile::ParseTextLine(LPCTSTR &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	if(!VPatchBlockFunc::ParseTextLine(strLine, bEnableErrorMessages))
		return FALSE;
	return ParseFuncArguments(strLine, bEnableErrorMessages);
}

BOOL VPatchBlockFuncMemwritefromfile::ParseFuncArguments(LPCTSTR  &strLine, BOOL bEnableErrorMessages/*=TRUE*/)
{
	return TRUE;
}
// Calculate new data dynamically
BOOL VPatchBlockFuncMemwritefromfile::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	if(!VPatchBlockFunc::Run(PatchBlocks, PatchStream, pPatchDevice, pCurPhoneData))
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////



VPatchBlockFuncMemreadtofile::VPatchBlockFuncMemreadtofile()
{
	m_FuncName=patchRunMemreadtofile;
}

// Calculate new data dynamically
BOOL VPatchBlockFuncMemreadtofile::Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData)
{
	if(!VPatchBlockFunc::Run(PatchBlocks, PatchStream, pPatchDevice, pCurPhoneData))
		return FALSE;
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////














CString vkpfilter;
CString vkpfiltersave;
// CPatchPage dialog

IMPLEMENT_DYNAMIC(CPatchPage, CDialog)
CPatchPage::CPatchPage(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchPage::IDD, pParent)
	, m_PatchData(_T(""))
{
	m_DynSize.AddControl(IDC_PATCH_SAVE_AS, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_SAVE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_RELOAD, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_LOAD, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_APPEND, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_NEW, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_CUR_LINE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_LINE_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_CUR_COL, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_COLUMN_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_DATA_FORMAT_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PATCH_DATA, EL(0), EL(0), EL(100), EL(100));

	m_IsDataInClipboard=FALSE;
	m_IsDocNew=TRUE;
	m_IsDocRichText=FALSE;
	m_pToolTipCtrl=NULL;
	m_pRichEditDoc=NULL;
	m_pFRDlg=NULL;

	Create(CPatchPage::IDD, pParent);

	CRect r;
	((CTabCtrl*)pParent)->InsertItem(((CTabCtrl*)pParent)->GetItemCount(), MSTR("General Patch"));
	((CTabCtrl*)pParent)->GetClientRect(&r);
	((CTabCtrl*)pParent)->AdjustRect(FALSE, &r);
	r.left-=2;
	MoveWindow(&r);

}

CPatchPage::~CPatchPage()
{
	if(m_pToolTipCtrl!=NULL)
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
}


void CPatchPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATCH_DATA, m_PatchDataCtrl);
	DDX_TextRich2(pDX, IDC_PATCH_DATA, m_PatchData);
}


BEGIN_MESSAGE_MAP(CPatchPage, CDialog)
	ON_BN_CLICKED(IDC_PATCH_LOAD, OnBnClickedPatchLoad)
	ON_BN_CLICKED(IDC_PATCH_SAVE_AS, OnBnClickedPatchSaveAs)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(VM_IS_CAN_CLOSE, OnIsCanClose)
	ON_MESSAGE(VM_IDLE, OnIdle)
	ON_MESSAGE(VM_APP_OPTIONS_CHANGED, OnAppOptionsChanged)
	ON_MESSAGE(VM_INITIAL_UPDATE, OnInitialUpdate)
	ON_MESSAGE(VM_GET_PATCH_MENU, OnGetPatchMenu)
	ON_BN_CLICKED(ID_PATCH_UNDO, OnBnClickedPatchUndo)
	ON_BN_CLICKED(ID_PATCH_APPLY, OnBnClickedPatchApply)
	ON_BN_CLICKED(IDC_PATCH_NEW, OnBnClickedPatchNew)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PATCH_DATA_PASTETEXT, OnPatchDataPastetext)
	ON_COMMAND(ID_PATCH_DATA_PASTE, OnPatchDataPaste)
	ON_COMMAND(ID_PATCH_DATA_CUT, OnPatchDataCut)
	ON_COMMAND(ID_PATCH_DATA_COPY, OnPatchDataCopy)
	ON_COMMAND(ID_PATCH_DATA_UNDO, OnPatchDataUndo)
	ON_COMMAND(ID_PATCH_DATA_REDO, OnPatchDataRedo)
	ON_COMMAND(ID_PATCH_DATA_SELECT_ALL, OnPatchDataSelectAll)
	ON_COMMAND(ID_PATCH_DATA_DELETE, OnPatchDataDelete)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PATCH_APPEND, OnBnClickedPatchAppend)
	ON_NOTIFY(EN_LINK, IDC_PATCH_DATA, OnEnLinkPatchData)
	ON_COMMAND(ID_PATCH_DATA_FONT, OnPatchDataFont)
	ON_WM_DESTROY()
	ON_NOTIFY(EN_SELCHANGE, IDC_PATCH_DATA, OnEnSelchangePatchData)
	ON_NOTIFY(EN_MSGFILTER, IDC_PATCH_DATA, OnEnMsgfilterPatchData)
	ON_COMMAND(ID_PATCH_DATA_BACKCOLOR, OnPatchDataBackcolor)
	ON_COMMAND(ID_PATCH_DATA_FIND, OnPatchDataFind)
	ON_COMMAND(ID_PATCH_DATA_REPLACE, OnPatchDataReplace)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace) 
	ON_BN_CLICKED(IDC_PATCH_SAVE, OnBnClickedPatchSave)
	ON_EN_CHANGE(IDC_PATCH_DATA, OnEnChangePatchData)
	ON_BN_CLICKED(ID_PATCH_APPLY_TEST, OnBnClickedPatchApplyTest)
	ON_BN_CLICKED(ID_PATCH_UNDO_TEST, OnBnClickedPatchUndoTest)
	ON_BN_CLICKED(IDC_PATCH_RELOAD, OnBnClickedPatchReload)
	ON_COMMAND(ID_PATCH_DATA_COMMENT, OnPatchDataComment)
	ON_COMMAND(ID_PATCH_DATA_UNCOMMENT, OnPatchDataUncomment)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPatchPage message handlers
void CPatchPage::OnContextMenu(CWnd*, CPoint point)
{
/*	// CG: This block was added by the Pop-up Menu component
	{
		CRect rect;
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CWnd *wnd=GetFocus();
			if(wnd==NULL)
				wnd=this;
			wnd->GetClientRect(rect);
			wnd->ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}
		m_PatchDataCtrl.GetClientRect(rect);
		m_PatchDataCtrl.ClientToScreen(rect);
		if(rect.PtInRect(point))
		{
			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_PATCH_MENU));
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			pPopup->EnableMenuItem(ID_PATCH_DATA_UNDO, MF_BYCOMMAND | 
				(m_PatchDataCtrl.CanUndo() ? MF_ENABLED : MF_GRAYED));
#if _MFC_VER >=0x0700
			pPopup->EnableMenuItem(ID_PATCH_DATA_REDO, MF_BYCOMMAND | 
				(m_PatchDataCtrl.CanRedo() ? MF_ENABLED : MF_GRAYED));
#else
			pPopup->EnableMenuItem(ID_PATCH_DATA_REDO, MF_BYCOMMAND | 
				(m_PatchDataCtrl.SendMessage(EM_CANREDO, 0, 0) ? MF_ENABLED : MF_GRAYED));
#endif
			long beg, end;
			m_PatchDataCtrl.GetSel(beg,end);
			pPopup->EnableMenuItem(ID_PATCH_DATA_CUT, MF_BYCOMMAND | 
				(beg!=end ? MF_ENABLED : MF_GRAYED));
			pPopup->EnableMenuItem(ID_PATCH_DATA_COPY, MF_BYCOMMAND | 
				(beg!=end ? MF_ENABLED : MF_GRAYED));
			pPopup->EnableMenuItem(ID_PATCH_DATA_PASTE, MF_BYCOMMAND | 
				(m_PatchDataCtrl.CanPaste() ? MF_ENABLED : MF_GRAYED));
			pPopup->EnableMenuItem(ID_PATCH_DATA_DELETE, MF_BYCOMMAND | 
				(beg!=end ? MF_ENABLED : MF_GRAYED));

			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}*/
}

afx_msg LRESULT CPatchPage::OnGetPatchMenu(WPARAM wUnused, LPARAM pOleCallbackMenuDataStruct)
{
	OLECALLBACK_MENU_DATA *pm= (OLECALLBACK_MENU_DATA*)pOleCallbackMenuDataStruct;
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_PATCH_MENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	ModifyMenuString(pPopup, ID_PATCH_DATA_UNDO, MSTR("&Undo"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_REDO, MSTR("&Redo"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_CUT, MSTR("Cu&t"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_COPY, MSTR("&Copy"));
	CString strP = MSTR("&Paste");
	CString strPT = MSTR("Paste &Text");
	if(o_bIsPatcherSwapPasteKeys)
	{
		ModifyMenuString(pPopup, ID_PATCH_DATA_PASTE, strPT);
		ModifyMenuString(pPopup, ID_PATCH_DATA_PASTETEXT, strP);
	}
	else
	{
		ModifyMenuString(pPopup, ID_PATCH_DATA_PASTE, strP);
		ModifyMenuString(pPopup, ID_PATCH_DATA_PASTETEXT, strPT);
	}
	ModifyMenuString(pPopup, ID_PATCH_DATA_DELETE, MSTR("&Delete"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_SELECT_ALL, MSTR("Select &All"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_FONT, CString(MSTR("&Font"))+_T("..."));
	ModifyMenuString(pPopup, ID_PATCH_DATA_BACKCOLOR, CString(MSTR("&Back Color"))+_T("..."));
	ModifyMenuString(pPopup, ID_PATCH_DATA_FIND, CString(MSTR("Fi&nd"))+_T("..."));
	ModifyMenuString(pPopup, ID_PATCH_DATA_REPLACE, CString(MSTR("Rep&lace"))+_T("..."));
	ModifyMenuString(pPopup, ID_PATCH_DATA_COMMENT, MSTR("C&omment"));
	ModifyMenuString(pPopup, ID_PATCH_DATA_UNCOMMENT, MSTR("Unco&mment"));

	pPopup->EnableMenuItem(ID_PATCH_DATA_UNDO, MF_BYCOMMAND | 
		(m_PatchDataCtrl.CanUndo() ? MF_ENABLED : MF_GRAYED));
#if _MFC_VER >=0x0700
	pPopup->EnableMenuItem(ID_PATCH_DATA_REDO, MF_BYCOMMAND | 
		(m_PatchDataCtrl.CanRedo() ? MF_ENABLED : MF_GRAYED));
#else
	pPopup->EnableMenuItem(ID_PATCH_DATA_REDO, MF_BYCOMMAND | 
		(m_PatchDataCtrl.SendMessage(EM_CANREDO, 0, 0) ? MF_ENABLED : MF_GRAYED));
#endif
	long beg, end;
	m_PatchDataCtrl.GetSel(beg,end);
	pPopup->EnableMenuItem(ID_PATCH_DATA_CUT, MF_BYCOMMAND | 
		(beg!=end ? MF_ENABLED : MF_GRAYED));
	pPopup->EnableMenuItem(ID_PATCH_DATA_COPY, MF_BYCOMMAND | 
		(beg!=end ? MF_ENABLED : MF_GRAYED));
	pPopup->EnableMenuItem(ID_PATCH_DATA_PASTE, MF_BYCOMMAND | 
		(m_PatchDataCtrl.CanPaste() ? MF_ENABLED : MF_GRAYED));
	BOOL cbistext=IsClipboardFormatAvailable(CF_TEXT);
#ifdef _UNICODE
	cbistext=cbistext || IsClipboardFormatAvailable(CF_UNICODETEXT);
#endif
	pPopup->EnableMenuItem(ID_PATCH_DATA_PASTETEXT, MF_BYCOMMAND | 
		(m_PatchDataCtrl.CanPaste() && cbistext ? MF_ENABLED : MF_GRAYED));
	pPopup->EnableMenuItem(ID_PATCH_DATA_DELETE, MF_BYCOMMAND | 
		(beg!=end ? MF_ENABLED : MF_GRAYED));
	*pm->phmenu=pPopup->GetSafeHmenu();
	menu.Detach();
	return TRUE;
}

void CPatchPage::OnCancel()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

char strLocaleID[]=";locale=";

BOOL CPatchPage::OpenDocument(LPCTSTR strDocFileName, BOOL bAppend/*=FALSE*/)
{
	if(!bAppend)
		((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, strDocFileName, FALSE);
	CFile *f=NULL;
	BYTE *buff=NULL;
	BOOL bOk=TRUE;
	TRY
	{
		BOOL bConvertErr=FALSE;
		int nFormat=0;
#ifdef _UNICODE
		nFormat|=SF_UNICODE;
#endif
		f= new CFile(strDocFileName, CFile::modeRead|CFile::typeBinary);
		DWORD fsize=(DWORD)f->GetLength();
		char typedet[10];
		typedet[f->Read(typedet, 9)]=0;
		f->Seek(0, CFile::begin);
		BOOL isRich=IsRichText((LPCTSTR)typedet);
		if(!isRich)
		{
			buff=new BYTE[fsize];
			f->Read(buff, fsize);
			char *strloc=NULL;
			char *stroldloc=NULL;
			if(fsize)
			{
				DWORD i;
				for(i=fsize-1; i!=-1 && isspace(buff[i]); i--);
				for(;i!=-1 && buff[i]!='\r' && buff[i]!='\n'; i--);
				if(i!=-1)
				{
					i++;
					if(fsize-i >= sizeof(strLocaleID)-1)
					{
						if(memcmp(strLocaleID, buff+i, sizeof(strLocaleID)-1)==0)
						{
							DWORD idstart=i;
							i+=sizeof(strLocaleID)-1;
							strloc=new char[fsize-i+1];
							for(DWORD j=0; i<fsize && buff[i]!='\r' && buff[i]!='\n'; i++, j++)
								strloc[j]=buff[i];
							strloc[j]=0;
							for(i=idstart-2, j=0; idstart>=2 && j<2; i-=2, j++) 
							{
								if(buff[i]!='\r' || buff[i+1]!='\n')
									break;
								idstart=i;
							}
							fsize-=fsize-idstart;
						}
					}
				}	
			}
			stroldloc=_strdup(setlocale(LC_ALL, NULL));
			if(strloc)
			{
				setlocale(LC_ALL, strloc);
				delete strloc;
			}
			else
			{
				_tsetlocale(LC_ALL, o_strLocale); 
			}
			_setmbcp(_MB_CP_LOCALE);

			TCHAR *tstr=m_PatchData.GetBuffer(fsize+16);
			memset(tstr, 0, (fsize+16)*sizeof(TCHAR));
#ifdef _UNICODE
			DWORD c;
			DWORD mbi, wi;
			for(mbi=0, wi=0, c=-1; mbi<fsize && c; wi++, mbi+=c)
			{
				c=mbtowc(tstr+wi, (const char*)(buff+mbi), fsize-mbi);
				if(c==0)
				{
					tstr[wi]=0;
				}
				else if(c==-1)
				{
					bConvertErr=TRUE;
					tstr[wi]=buff[mbi];
					c=1;
				}
			}
#else
			memcpy(tstr, buff, fsize);
#endif
			if(stroldloc)
			{
				setlocale(LC_ALL, stroldloc);
				_setmbcp(_MB_CP_LOCALE);
				free(stroldloc);
			}
			m_PatchData.ReleaseBuffer();
		}

		if(bAppend)
		{
			CString str;
			str.Format(appendHeaderFormat, GetFileName(strDocFileName));
			m_PatchDataCtrl.SetSel(-1, -1);
			m_PatchDataCtrl.SetSelectionCharFormat(m_RichEditDefaultCharFormat);
			RichStreamIn(&m_PatchDataCtrl, str, str.GetLength()*sizeof(TCHAR), nFormat | SF_TEXT | SFF_SELECTION);
			if(isRich)
				RichStreamIn(&m_PatchDataCtrl, (LPCTSTR)f, -1, SF_RTF | SFF_SELECTION);
			else
				RichStreamIn(&m_PatchDataCtrl, m_PatchData, m_PatchData.GetLength()*sizeof(TCHAR),
					nFormat | SF_TEXT | SFF_SELECTION);
			if(isRich)
				m_IsDocRichText=TRUE;
		}
		else
		{
			ReInsertClipboardData();
			m_PatchDataCtrl.SetWindowText(NULL);
			m_PatchDataCtrl.SetDefaultCharFormat(m_RichEditDefaultCharFormat);
			m_strDocFileName=strDocFileName;
			m_IsDocNew=FALSE;
			if(isRich)
				RichStreamIn(&m_PatchDataCtrl, (LPCTSTR)f, -1, SF_RTF);
			else
				RichStreamIn(&m_PatchDataCtrl, m_PatchData, m_PatchData.GetLength()*sizeof(TCHAR),
					nFormat | SF_TEXT);
			m_IsDocRichText=isRich;
			if(!m_IsDocRichText)
			{
				CHARFORMAT2 cf;
				memcpy(&cf, &m_RichEditDefaultCharFormat, sizeof(cf));
				cf.dwMask=CFM_FACE|CFM_SIZE|CFM_CHARSET;
				m_PatchDataCtrl.SetDefaultCharFormat(cf);
			}
		}

		if(bAppend)
			m_PatchDataCtrl.SetModify(TRUE);
		if(bConvertErr && !o_bDisableWarningsMsg)
			AfxMessageBox(MSTR("Warning!\nText of the patch has unknown characters!"), MB_OK|MB_ICONINFORMATION);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		bOk=FALSE;
	}
	END_CATCH_ALL;
	if(f!=NULL)
	{
		f->Close();
		delete f;
	}
	if(buff!=NULL)
		delete buff;
	if(!bAppend)
		m_PatchDataCtrl.SetModify(FALSE);
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strDocFileName, m_PatchDataCtrl.GetModify());
	return bOk;
}

void CPatchPage::OnBnClickedPatchLoad()
{
	if(!SaveModifiedDocument())
		return;
	CFileDialog fdlg(TRUE, NULL, GetFileName(m_strDocFileName), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, vkpfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=openDialogTitle;
	fdlg.m_ofn.lpstrInitialDir=GetCurPatchDir();
	if(fdlg.DoModal()!=IDOK)
		return;
	SaveCurPatchDir(fdlg.GetPathName());
	OpenDocument(fdlg.GetPathName());
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchReload()
{
	if(m_IsDocNew)
	{
		AfxMessageBox(MSTR("Current file was never be saved before. There is nothing to reopen!"));
		return;
	}
	if(m_PatchDataCtrl.GetModify())
		if(AfxMessageBox(
		   MSTR("Are you sure to reopen the current file?\nAll changes will be lost!"),
		   MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;
	OpenDocument(m_strDocFileName);
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchAppend()
{
	CFileDialog fdlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_EXPLORER,
		vkpfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=openDialogTitle;
	CString dir=GetFileDir(m_strDocFileName);
	if(dir.IsEmpty())
		dir=GetCurPatchDir();
	fdlg.m_ofn.lpstrInitialDir=dir;
	fdlg.m_ofn.nMaxFile = max(992 * 1024, _MAX_PATH);			// in win98 (1024*1024) bytes is not working !!!!!
	fdlg.m_ofn.lpstrFile=new TCHAR[fdlg.m_ofn.nMaxFile];
	if(fdlg.m_ofn.lpstrFile == NULL)
	{
		AfxMessageBox(_T("Not enough memory"));
		return;
	}
	fdlg.m_ofn.lpstrFile[0]=0;
	if(fdlg.DoModal()==IDOK)
	{
		CString str;
		POSITION pos;
		int n;
		for(n=0, pos=fdlg.GetStartPosition(); pos!=NULL; fdlg.GetNextPathName(pos), n++);
		CProgressDlg progress(MSTR("Loading patches..."), this);
		progress.SetUpper(n);
		progress.SetStep(1);
		for(pos=fdlg.GetStartPosition(); pos!=NULL && !progress.CheckCancelButton();)
		{
			str=fdlg.GetNextPathName(pos);
			progress.SetStatus(GetFileName(str));
			if(!OpenDocument(str, TRUE))
				break;
			progress.StepIt();
		}
		SaveCurPatchDir(str);
	}
	delete fdlg.m_ofn.lpstrFile;
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchSaveAs()
{
	SaveDocument();
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchSave()
{
	SaveDocument(FALSE);
	m_PatchDataCtrl.SetFocus();
}

// Convert string to data for writing in text patch file. Return the address of buffer with data - DELETE it after all.
BOOL CPatchPage::ConvertStringToDataForPatchFile(LPCTSTR pstrPatch, BYTE** pDataBuff, UINT64* pDataLen, BOOL* pbIsConvertErr/*=NULL*/)
{
	if(pstrPatch==NULL)
		return FALSE;
	if(pDataBuff!=NULL)
		*pDataBuff=NULL;
	if(pDataLen!=NULL)
		*pDataLen=0;
	if(pbIsConvertErr!=NULL)
		*pbIsConvertErr=FALSE;

	DWORD fsize;
	char *strloccpy, *strloc=new char[o_strLocale.GetLength()+16];
	DWORD i;
	BYTE *buff;
	BOOL bConvertErr=FALSE;

	char *stroldloc=_strdup(setlocale(LC_ALL, NULL));
	_tsetlocale(LC_ALL, o_strLocale); 
	_setmbcp(_MB_CP_LOCALE);

	DWORD len=(DWORD)_tcslen(pstrPatch);
#ifdef _UNICODE
	LPCTSTR str=pstrPatch;
	int cmbs;
	buff=new BYTE[MB_CUR_MAX*1024];
	for(fsize=0, i=0; i<len; i++, fsize+=cmbs)
	{
		cmbs=wctomb((char*)buff, str[i]);
		if(cmbs==-1)
			cmbs=1;
	}
	delete buff;
	buff=new BYTE[fsize+1024];
	DWORD mi;
	for(i=0, mi=0; i<len; i++, mi+=cmbs)
	{
		cmbs=wctomb((char*)(buff+mi), str[i]);
		if(cmbs==-1)
		{
			bConvertErr=TRUE;
			buff[mi]='?';
			cmbs=1;
		}
	}
	buff[mi]=0;
	fsize=mi;
	wcstombs(strloc, o_strLocale, o_strLocale.GetLength()+16);
#else
	fsize=len;
	buff=new BYTE[fsize+1024*2];
	memcpy(buff, pstrPatch, fsize);
	strcpy(strloc, o_strLocale);
#endif
	if(strloc[0]==0)
		strloccpy=setlocale(LC_ALL, NULL);
	else
		strloccpy=strloc;
	if(!o_bDoNotInsertLocaleInfoInPatch)
	{
		buff[fsize++]='\r';
		buff[fsize++]='\n';
		buff[fsize++]='\r';
		buff[fsize++]='\n';
		memcpy(buff+fsize, strLocaleID, sizeof(strLocaleID)-1);
		fsize+=sizeof(strLocaleID)-1;
		i=(DWORD)strlen(strloccpy);
		memcpy(buff+fsize, strloccpy, i);
		fsize+=i;
	}
	delete strloc;

	if(stroldloc)
	{
		setlocale(LC_ALL, stroldloc);
		_setmbcp(_MB_CP_LOCALE);
		free(stroldloc);
	}
	if(pDataBuff!=NULL)
		*pDataBuff=buff;
	else
		delete buff;
	if(pDataLen!=NULL)
		*pDataLen=fsize;
	if(pbIsConvertErr!=NULL)
		*pbIsConvertErr=bConvertErr;
	return TRUE;
}


BOOL CPatchPage::SaveDocument(BOOL bSaveAs/*=TRUE*/, LPCTSTR toFileWithNoQuestions/*=NULL*/)
{
	BOOL isRich=m_IsDocRichText;
	BOOL isRichObj=FALSE;
	if(!isRich)
	{
		IRichEditOle* ire=m_PatchDataCtrl.GetIRichEditOle();
		if(ire!=NULL)
		{
			if(ire->GetObjectCount()!=0)
				isRich=isRichObj=TRUE;
			ire->Release();
		}
	}
	if(!isRich)
	{
		CHARRANGE oldcr;
		m_PatchDataCtrl.GetSel(oldcr);
		int fvline=m_PatchDataCtrl.GetFirstVisibleLine();
		m_PatchDataCtrl.SetSel(0, -1);
		CHARFORMAT2 cf;
		DWORD dwMask=m_PatchDataCtrl.GetSelectionCharFormat(cf);
		m_PatchDataCtrl.SetSel(oldcr);
		m_PatchDataCtrl.LineScroll(fvline-m_PatchDataCtrl.GetFirstVisibleLine());
		DWORD dwCheck=CFM_ANIMATION/*|CFM_CHARSET|CFM_FACE|CFM_LCID*/|CFM_KERNING|CFM_REVAUTHOR
			|CFM_SPACING|CFM_STYLE
			|CFM_COLOR|CFM_BACKCOLOR|CFM_SIZE|CFM_WEIGHT|CFM_UNDERLINETYPE|CFM_OFFSET
			|CFM_ALLCAPS|CFM_BOLD|CFM_DISABLED|CFM_EMBOSS|CFM_HIDDEN|CFM_IMPRINT|CFM_ITALIC
			|CFM_OUTLINE|CFE_PROTECTED|CFE_REVISED|CFE_SHADOW|CFE_SMALLCAPS|CFE_STRIKEOUT|CFE_SUBSCRIPT
			|CFE_SUPERSCRIPT|CFE_UNDERLINE/*|CFM_LINK*/;
		if((dwMask&dwCheck)!=dwCheck)
			isRich=TRUE;
	}

	LPCTSTR pstrfile;
	if(toFileWithNoQuestions==NULL)
	{
		if(bSaveAs || m_IsDocNew || m_strDocFileName.IsEmpty())
		{
			CFileDialog fdlg(FALSE, _T("vkp"), GetFileName(m_strDocFileName), OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
				vkpfiltersave, AfxGetMainWnd());
			fdlg.m_ofn.lpstrTitle=saveDialogTitle;
			fdlg.m_ofn.lpstrInitialDir=GetCurPatchDir();
			fdlg.m_ofn.nFilterIndex=isRich ? 2 : 1;
			if(fdlg.DoModal()!=IDOK)
				return FALSE;
			SaveCurPatchDir(fdlg.GetPathName());
			isRich= fdlg.m_ofn.nFilterIndex==2;
			m_strDocFileName=fdlg.GetPathName();
		}

		if(!o_bDisableRTFSaveWarningsMsg && !m_IsDocRichText && isRich)
		{
			if(!isRichObj)
			{
				UINT ret=AfxMessageBox(
					MSTR("Current document will be saved as rich text document.\n\nYES - to save as rich text document.\nNO - to save as plain text document.\nCANCEL - cancel operation."),
					MB_YESNOCANCEL, MB_ICONQUESTION);
				if(ret==IDYES)
					isRich=TRUE;
				else if(ret==IDNO)
					isRich=FALSE;
				else
					return FALSE;
			}
		}

		m_IsDocRichText=isRich;
		pstrfile=m_strDocFileName;
	}
	else
	{
		pstrfile=toFileWithNoQuestions;
	}

	CString strtemp=CString(pstrfile)+_T(".tmp");
	TRY{CFile::Remove(strtemp);}CATCH_ALL(e){}END_CATCH_ALL;

	CFile *f=NULL;
	BYTE *buff=NULL;
	BOOL ret=FALSE;
	BOOL bConvertErr=FALSE;
	TRY
	{
		f= new CFile(strtemp, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		if(isRich)
		{
			RichStreamOut(&m_PatchDataCtrl, (LPCTSTR)f, -1, SF_RTF);
		}
		else
		{
			RichStreamOutText(&m_PatchDataCtrl, m_PatchData);
			UINT64 fsize;
			ConvertStringToDataForPatchFile(m_PatchData, &buff, &fsize, &bConvertErr);
			f->Write(buff, (UINT)fsize);
		}
		ret=TRUE;
	}
	CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
	if(f!=NULL)
	{
		f->Close();
		delete f;
	}
	if(buff!=NULL)
		delete buff;
	if(!ret)
	{
		TRY{CFile::Remove(strtemp);}CATCH_ALL(e){}END_CATCH_ALL;
		return ret;
	}

	TRY{CFile::Remove(pstrfile);}CATCH_ALL(e){}END_CATCH_ALL;
	TRY
	{
		CFile::Rename(strtemp, pstrfile);
		if(toFileWithNoQuestions==NULL)
			m_IsDocNew=FALSE;
	}
	CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;

	if(toFileWithNoQuestions==NULL)
	{
		if(bConvertErr)
		{
			if(!o_bDisableWarningsMsg)
				AfxMessageBox(MSTR("Text of patch have a bad symbols in comments.\nBad symbols is replaced by \"?\".\nTry to use other codepage (see \"Options\" -> \"Codepage of patch text\")"),
					MB_OK|MB_ICONINFORMATION);
		}
		else
			m_PatchDataCtrl.SetModify(FALSE);

		((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strDocFileName, m_PatchDataCtrl.GetModify());
	}
	return TRUE;
}

BOOL CPatchPage::DoPatchLogging(BOOL bOnApply/*=TRUE*/)
{
	CKLayApp *app=(CKLayApp*)AfxGetApp();
	if(app==NULL)
		return FALSE;
	CString lfname=app->m_AppPath+_T("log.exe");
	CFileStatus fs;
	if(!CFile::GetStatus(lfname, fs))
	{
		lfname=app->m_AppPath+_T("log.bat");
		if(!CFile::GetStatus(lfname, fs))
			return FALSE;
	}

	CString root=app->m_AppPath+_T("log\\");
	CreateDirectory(root, NULL);
	CString par, fpath=m_strDocFileName;
	if(fpath.IsEmpty())
		fpath=GetCurPatchDir()+GetDefaultFileName();
	CString fname=GetFileName(fpath);
	par.Format(_T("%s \"%s\" \"%s\" \"%s\""), bOnApply ? _T("/a") : _T("/u"), pDevice->GetUniqueName(), fname, fpath);
	SaveDocument(TRUE, root+_T("log.vkp"));
	return (__int64)ShellExecute(GetSafeHwnd(), NULL, lfname, par, root, SW_SHOWMINNOACTIVE) > 32;
}

// Do a applying patch
BOOL CPatchPage::DoPatchApply(CString &PatchData, VDevice *pPatchDevice, BOOL bYesToAll/*=FALSE*/, BOOL bTestMode/*=FALSE*/)
{
	m_strLastRepairPatchFileName.Empty();
	if(pPatchDevice==NULL)
	{
		AfxMessageBox(MSTR("For apply patch you must select the device first!"));
		return FALSE;
	}

	if(!o_bDisableWarningsMsg && !bYesToAll && !bTestMode)
		if(AfxMessageBox(MSTR("Apply the current patch?"), MB_YESNO)==IDNO)
			return FALSE;
	TRY
	{
		VPatchBlocks PatchBlocks;
		VPatchStream PatchStream;
		if(!PatchDataGet(PatchData, PatchBlocks))
			AfxThrowUserException();
		CProgressDlg Progress(0, this);
		UINT TRMask;
		Progress.ShowPerformanceInfo();
		if(!PatchDataConvert(PatchBlocks, PatchStream, TRUE, pPatchDevice, &TRMask, bYesToAll, bTestMode))
			AfxThrowUserException();
		PatchBlocks.RemoveAll();
		if(!bTestMode && (TRMask&VPB_PATCH_SAFE_TO_WRITE) && !(TRMask&(VPB_PATCH_ALREADY_EXIST|VPB_PATCH_EMPTY)))
			if(!PatchDataWrite(PatchStream, pPatchDevice))
				AfxThrowUserException();
		if(!pPatchDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();

		CString str;
		if(TRMask&VPB_PATCH_EMPTY)
			str=MSTR("Patch is empty. There is nothing to apply!");
		else if(TRMask&VPB_PATCH_ALREADY_EXIST)
			str=MSTR("Patch already exist in the phone. There is nothing to apply!");
		else if(bTestMode)
			str=MSTR("Patch can be applied successfully!");
		else
		{
			str=MSTR("Patch has been applied successfully!");
			if(pPatchDevice==pDevice)
				DoPatchLogging(TRUE);
		}
		if(!m_strLastRepairPatchFileName.IsEmpty() || (!(!bTestMode && o_bDisableWarningsMsg) && !bYesToAll))
		{
			str+=Progress.MakePerformanceReport();
			if(!m_strLastRepairPatchFileName.IsEmpty())
			{
				str+=     _T("\n\n");
				str+=     msgRepairIn;
				str+=     _T("\n");
				str+=     m_strLastRepairPatchFileName;
			}
			AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
		}
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		else
			m_PatchDataCtrl.SetFocus();
		pPatchDevice->Abort();
		pPatchDevice->Close();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// Do a applying patch
BOOL CPatchPage::DoPatchUndo(CString &PatchData, VDevice *pPatchDevice, BOOL bYesToAll/*=FALSE*/, BOOL bTestMode/*=FALSE*/)
{
	if(pPatchDevice==NULL)
	{
		AfxMessageBox(MSTR("For undo patch you must select the device first!"));
		return FALSE;
	}

	if(!o_bDisableWarningsMsg && !bYesToAll && !bTestMode)
		if(AfxMessageBox(MSTR("Undo the current patch?"), MB_YESNO)==IDNO)
			return FALSE;
	TRY
	{
		VPatchBlocks PatchBlocks;
		VPatchStream PatchStream;
		if(!PatchDataGet(PatchData, PatchBlocks))
			AfxThrowUserException();
		CProgressDlg Progress(0, this);
		UINT TRMask;
		Progress.ShowPerformanceInfo();
		if(!PatchDataConvert(PatchBlocks, PatchStream, FALSE, pPatchDevice, &TRMask, bYesToAll, bTestMode))
			AfxThrowUserException();
		PatchBlocks.RemoveAll();
		if(!bTestMode && (TRMask&VPB_PATCH_SAFE_TO_WRITE) && !(TRMask&(VPB_PATCH_ALREADY_EXIST|VPB_PATCH_EMPTY)))
			if(!PatchDataWrite(PatchStream, pPatchDevice))
				AfxThrowUserException();
		if(!pPatchDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();

		CString str;
		if(TRMask&VPB_PATCH_EMPTY)
			str=MSTR("Patch is empty. There is nothing to undo!");
		else if(TRMask&VPB_PATCH_ALREADY_EXIST)
			str=MSTR("Patch not exist in the phone (old data already exist in the phone). There is nothing to undo!");
		else if(bTestMode)
			str=MSTR("Undoing the patch can be successful!");
		else
		{
			str=MSTR("Undoing the patch is made successfully!");
			if(pPatchDevice==pDevice)
				DoPatchLogging(FALSE);
		}
		if(!m_strLastRepairPatchFileName.IsEmpty() || (!(!bTestMode && o_bDisableWarningsMsg) && !bYesToAll))
		{
			str+=Progress.MakePerformanceReport();
			if(!m_strLastRepairPatchFileName.IsEmpty())
			{
				str+=     _T("\n\n");
				str+=     msgRepairIn;
				str+=     _T("\n");
				str+=     m_strLastRepairPatchFileName;
			}
			AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
		}
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		else
			m_PatchDataCtrl.SetFocus();
		pPatchDevice->Abort();
		pPatchDevice->Close();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

void CPatchPage::OnBnClickedPatchApply()
{
	if(!UpdateData(TRUE))
		return;
	DoPatchApply(m_PatchData, pDevice);
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchApplyTest()
{
	if(!UpdateData(TRUE))
		return;
	DoPatchApply(m_PatchData, pDevice, FALSE, TRUE);
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchUndo()
{
	if(!UpdateData(TRUE))
		return;
	DoPatchUndo(m_PatchData, pDevice);
	m_PatchDataCtrl.SetFocus();
}

void CPatchPage::OnBnClickedPatchUndoTest()
{
	if(!UpdateData(TRUE))
		return;
	DoPatchUndo(m_PatchData, pDevice, FALSE, TRUE);
	m_PatchDataCtrl.SetFocus();
}



BOOL isdiract(LPCTSTR &pd, LPCTSTR lpstrDirectiveAction, BOOL bEnableErrorMessages, BOOL bJustThrowGeneralError=FALSE)
{
	if(bJustThrowGeneralError)
		throwUser(MSTR("Line %1:\nSyntax error in directive."), patchCurLine, bEnableErrorMessages);
	if(_tcsnicmp(pd, lpstrDirectiveAction, _tcsclen(lpstrDirectiveAction))==0)
	{
		pd+=_tcsclen(lpstrDirectiveAction);
		if(!v_istspace(*pd))
			throwUser(MSTR("Line %1:\nSyntax error in directive."), patchCurLine, bEnableErrorMessages);
		skipspaces(pd);
		return TRUE;
	}
	return FALSE;
}

BOOL isdirpar(LPCTSTR &pd, LPCTSTR lpstrDirectiveParameter, BOOL bEnableErrorMessages, BOOL bJustThrowGeneralError=FALSE)
{
	if(bJustThrowGeneralError)
		isdiract(pd, NULL, bEnableErrorMessages, TRUE);
	if(_tcsnicmp(pd, lpstrDirectiveParameter, _tcsclen(lpstrDirectiveParameter))==0)
	{
		pd+=_tcsclen(lpstrDirectiveParameter);
		skipspaces(pd);
		if(!isendofdataline(pd))
			isdiract(pd, NULL, bEnableErrorMessages, TRUE);
		return TRUE;
	}
	return FALSE;
}

// Convert patch from edit box to the binary data
BOOL CPatchPage::PatchDataGet(CString &PatchData, VPatchBlocks &PatchBlocks, BOOL bEnableErrorMessages/*=TRUE*/)
{
	BOOL bOk=TRUE;
	patchCurLine=0;
	patchCurCol=0;
	patchCurAddrCorrection=0;
	patchFlags=0;
	IsMultilineComment=FALSE;

	TRY
	{
		DWORD ps=(DWORD)PatchData.GetLength();
		CString line;
		_TCHAR *pd=NULL;
		_TCHAR *pcur=(_TCHAR*)(LPCTSTR)PatchData;
		_TCHAR *end;
		DWORD len, cbeg, cend;
		__int64 t;
		BOOL bAllIsComments=TRUE, curLineIsComments;
		VPatchBlock patchBlock;
		for(patchCurLine=1; *pcur; findendofline(pcur), patchCurLine++)
		{
			if(IsMultilineComment)
			{
				bAllIsComments=FALSE;
				for(;;)
				{
					pcur+=_tcscspn(pcur, _T("*\r\n"));
					if(*pcur==0)
					{
						throwUser(MSTR("Line %1:\nSymbols of comment closing ('*/') is not found."), patchCurLine, bEnableErrorMessages);
						break;
					}
					if(*pcur==_T('\r') || *pcur==_T('\n'))
						break;
					if(pcur[0]==_T('*') && pcur[1]==_T('/'))
					{
						IsMultilineComment=FALSE;
						pcur+=2;
						break;
					}
					pcur++;
				}
				if(IsMultilineComment)
					continue;
			}
			skipspaces(pcur);
			curLineIsComments=FALSE;
			if(*pcur==_TCHAR(';'))
				curLineIsComments=TRUE;
			if(*pcur!=0)
				if(pcur[0]==_T('/') && pcur[1]==_T('/'))
					curLineIsComments=TRUE;
			if(curLineIsComments)
				bAllIsComments=FALSE;
			if(*pcur==_TCHAR('*'))
				continue;
			//Make a line with no comments inside and stick together strings with concatenate sequence at the end.
			line.Truncate(0);
			for(len=0; pcur[len]; findendofline(pcur), patchCurLine++)
			{
				len=(DWORD)_tcscspn(pcur, _T("\r\n"));
				line.Append(pcur, len);
				if(len==0 || curLineIsComments)
					break;
				if(pcur[len-1]!=_T('\\'))
					break;
				if(pcur[len]==0)
					throwUser(MSTR("Line %1:\nAfter concatenating symbol must be a new line."), patchCurLine, bEnableErrorMessages);
				line.Truncate(line.GetLength()-1);
			}
			pd=(_TCHAR*)(LPCTSTR)line;
			for(cbeg=0, cend=0; ;)
			{
				cbeg+=(DWORD)_tcscspn(pd+cbeg, _T("/\r\n"));
				if(pd[cbeg]==_T('\r') || pd[cbeg]==_T('\n') || pd[cbeg]==0)
					break;
				if(pd[cbeg+1]==_T('*'))
				{
					bAllIsComments=FALSE;
					for(cend=cbeg+2; ; cend++)
					{
						cend+=(DWORD)_tcscspn(pd+cend, _T("*\r\n"));
						if(pd[cend]==_T('\r') || pd[cend]==_T('\n') || pd[cend]==0)
						{
							cend=-1;
							break;
						}
						if(pd[cend+1]==_T('/'))
						{
							cend+=2;
							len=(DWORD)_tcslen(pd+cend)+1;
							memmove(pd+cbeg, pd+cend, len*sizeof(_TCHAR));
							break;
						}
					}
					if(cend==-1)
						break;
				}
				else
					cbeg++;
			}
			//___
			skipspaces(pd);
			if(isendofdataline(pd))
				continue;


			if(*pd==_TCHAR('+') || *pd==_TCHAR('-'))
			{
				bAllIsComments=FALSE;
				t=VStrToSignedHex(pd, &end);
				len=(DWORD)(end-pd);
				pd=end;
				if(len == 0)
					throwUser(MSTR("Line %1:\nAddress correction number in format \"_HEX_NUMBER_WITH_SIGN_ [;comments]\" is expected."), patchCurLine, bEnableErrorMessages);
				skipspaces(pd);
				if(!isendofdataline(pd))
					throwUser(MSTR("Line %1:\nAddress correction number in format \"_HEX_NUMBER_WITH_SIGN_ [;comments]\" is expected."), patchCurLine, bEnableErrorMessages);
				patchCurAddrCorrection=t;
				continue;
			}

			if(*pd==_TCHAR('#'))
			{
				if(_tcsnicmp(pd, patchDirective, _tsizeof(patchDirective))==0)
				{
					pd+=_tsizeof(patchDirective);
					if(v_istspace(*pd))
					{
						bAllIsComments=FALSE;
						skipspaces(pd);
						if(isdiract(pd, patchDirActDisable, bEnableErrorMessages))
						{
							if(isdirpar(pd, patchDirParWarnOld, bEnableErrorMessages))
								patchFlags|=VPB_FLAG_DIS_WARN_APPLY_NO_OLD;
							else if(isdirpar(pd, patchDirParWarnOldNew, bEnableErrorMessages))
								patchFlags|=VPB_FLAG_DIS_WARN_APPLY_OLD_NOT_FOUND_BUT_NEW_ALREADY_EXIST;
							else if(isdirpar(pd, patchDirParUndo, bEnableErrorMessages))
								patchFlags|=VPB_FLAG_DIS_UNDO;
							else if(isdirpar(pd, patchDirParWarnNewNotFound, bEnableErrorMessages))
								patchFlags|=VPB_FLAG_DIS_WARN_UNDO_NEW_NOT_FOUND_BUT_OLD_ALREADY_EXIST;
							else if(isdirpar(pd, patchDirParOldEqualFF, bEnableErrorMessages))
								patchFlags&=~VPB_FLAG_ENA_OLD_EQUAL_FF;
							else
								isdirpar(pd, NULL, bEnableErrorMessages, TRUE);
						}
						else if(isdiract(pd, patchDirActEnable, bEnableErrorMessages))
						{
							if(isdirpar(pd, patchDirParWarnOld, bEnableErrorMessages))
								patchFlags&=~VPB_FLAG_DIS_WARN_APPLY_NO_OLD;
							else if(isdirpar(pd, patchDirParWarnOldNew, bEnableErrorMessages))
								patchFlags&=~VPB_FLAG_DIS_WARN_APPLY_OLD_NOT_FOUND_BUT_NEW_ALREADY_EXIST;
							else if(isdirpar(pd, patchDirParUndo, bEnableErrorMessages))
								patchFlags&=~VPB_FLAG_DIS_UNDO;
							else if(isdirpar(pd, patchDirParWarnNewNotFound, bEnableErrorMessages))
								patchFlags&=~VPB_FLAG_DIS_WARN_UNDO_NEW_NOT_FOUND_BUT_OLD_ALREADY_EXIST;
							else if(isdirpar(pd, patchDirParOldEqualFF, bEnableErrorMessages))
								patchFlags|=VPB_FLAG_ENA_OLD_EQUAL_FF;
							else
								isdirpar(pd, NULL, bEnableErrorMessages, TRUE);
						}
						else
							isdiract(pd, NULL, bEnableErrorMessages, TRUE);
					}
				}
				else if(_tcsnicmp(pd, patchRun, _tsizeof(patchRun))==0)
				{
					pd+=_tsizeof(patchRun);
					if(v_istspace(*pd))
					{
						bAllIsComments=FALSE;
						skipspaces(pd);
						if(_tcsnicmp(pd, patchRunTypeID, _tsizeof(patchRunTypeID))==0)
						{
							pd+=_tsizeof(patchRunTypeID);
							if(v_istspace(*pd))
							{
								skipspaces(pd);
								int type=0;
								if(_tcsnicmp(pd, patchRunTypeOnApply, _tsizeof(patchRunTypeOnApply))==0)
								{
									type=1;
									pd+=_tsizeof(patchRunTypeOnApply);
								}
								else if(_tcsnicmp(pd, patchRunTypeOnUndo, _tsizeof(patchRunTypeOnUndo))==0)
								{
									type=2;
									pd+=_tsizeof(patchRunTypeOnUndo);
								}
								if(!v_istspace(*pd) || type==0)
									isdiract(pd, NULL, bEnableErrorMessages, TRUE);
								skipspaces(pd);
								

								DWORD old_patchFlags=patchFlags;
								patchFlags|=VPB_FLAG_RUN | (type==2 ?  0 : VPB_FLAG_RUN_IS_ON_APPLY);
								for(int i=0; i<VPB_FUNC_NUM; i++)
								{
									if(pPatchBlockFunctions[i]->ParseTextLine(pd, bEnableErrorMessages))
									{
										PatchBlocks.Add(*pPatchBlockFunctions[i]);
										break;
									}
								}
								patchFlags=old_patchFlags;
								if(i>=VPB_FUNC_NUM)
									isdiract(pd, NULL, bEnableErrorMessages, TRUE);
							}
							else
								isdiract(pd, NULL, bEnableErrorMessages, TRUE);
						}
						else
							isdiract(pd, NULL, bEnableErrorMessages, TRUE);
					}
				}
				continue;
			}

			if(bAllIsComments)
			{
				if(_istxdigit(*pd))
				{
					VStrToSignedHex(pd, &end);
					if(pd < end)
					{
						if(v_istspace(end[0]) || end[0]==_TCHAR(':'))
							bAllIsComments=FALSE;
					}
				}
			}

			if(!bAllIsComments)
			{
				if(patchBlock.ParseTextLine(pd, bEnableErrorMessages))
					PatchBlocks.Add(patchBlock);
			}
		}
		bOk=TRUE;
	}
	CATCH_ALL(e)
	{
		if(bEnableErrorMessages)
			if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
				e->ReportError();
		bOk=FALSE;
	}
	END_CATCH_ALL;

	return bOk;
}

// Test the patch for errors and possibility to safely writing to the device
BOOL CPatchPage::PatchDataConvert(VPatchBlocks &PatchBlocks, VPatchStream &PatchStream, BOOL bApply,
	VDevice *pPatchDevice/*=NULL*/, UINT* lpTestResultMask/*=NULL*/, BOOL bYesToAll/*=FALSE*/, BOOL bTestMode/*=FALSE*/)
{
	if(lpTestResultMask!=NULL)
		*lpTestResultMask=0;
	m_strLastRepairPatchFileName.Empty();
	PatchStream.RemoveAll();
	BOOL bOk=FALSE;
	if(PatchBlocks.GetSize()==0)
	{
		if(lpTestResultMask!=NULL)
			*lpTestResultMask=VPB_PATCH_SAFE_TO_WRITE|VPB_PATCH_ALREADY_EXIST|VPB_PATCH_EMPTY;
		return TRUE;
	}
	UINT TRMask=0;
	UINT_PTR i, addrsearchidx, existNewCounter=0, noOldCounter=0;
	UINT_PTR count, step;
	BYTE *buf=NULL, *OldData, *NewData, *ptmp;
	buf=new BYTE[65536+8];
	DWORD buflen=0, curaddrbeg, curaddrend, t, Size;
	BOOL writenewanyway=bYesToAll;
	CString str, strNoOldWarning;
	VPatchBlock pb;
	POSITION pos, prevpos, findpos;
	int n, j;
	VPatchBlock *pbbeg, *pbend, *ppb, *ppbt;
	BOOL IsNeedToAddPhoneData;
	TRY
	{
		LPCTSTR msgOldExist, msgDescr, msgNoOldInPatch, msgRunUsedWarning;
		CString msgErrHeader=CString(MSTR("Error at address"))+_TCHAR(' ');
		LPCTSTR msgInfoTextLine=MSTR("Line: %s");
		LPCTSTR msgInfoTextBlAddr=MSTR("Block address: %s");
		CString msgInfoText(msgInfoTextLine);
		msgInfoText+=_T(", ");
		msgInfoText+=msgInfoTextBlAddr;
		msgInfoText+=_T('.');
		if(bApply)
		{
			msgOldExist=MSTR("WARNING!!!\nThe old data of %s from %s blocks of patch is not found in flash.\nMay be the similar patch has been applied or you have a wrong version of flash.\n\nDo you want to apply the patch anyway?");
			msgNoOldInPatch=MSTR("If you apply this patch you cannot undo his later, because some or all blocks in the patch do not have the old data!\n\nDo you want to continue?");
			msgDescr=msgApplyDescr;
			msgRunUsedWarning=MSTR("If you apply this patch you may cannot undo it later, because #run directive exist in patch.\n\nDo you want to continue?");
		}
		else
		{
			msgOldExist=MSTR("WARNING!!!\nThe new data of %s from %s blocks of patch is not found in flash.\nMay be the the similar patch has been applied or you have a wrong flash.\n\nDo you want to write the old data of the patch (do undo) anyway?");
			msgNoOldInPatch=MSTR("You can't do complete undo for this patch, because some or all blocks in the patch do not have the old data!\n\nDo you want to continue and do undo for that blocks, where old data is available?");
			msgDescr=msgUndoDescr;
			msgRunUsedWarning=MSTR("You may can't do complete undo for this patch, because #run directive exist in patch. For undo you may use REPAIR PATCH, which has been made when you applied this patch.\n\nDo you want to continue and do undo for that blocks, where old data is available?");
		}

		//calculating upper limit of progress bar.
		curaddrbeg=-1; curaddrend=0; buflen=0;
		UINT_PTR isShowMsgOldAbsent=-1, isShowMsgRunUsed=-1;
		memset(buf, FALSE, 65536);
		Size= /*pDevice==NULL ?*/ VD_DEFAULT_PAGE_SIZE /*: pDevice->m_PageSize*/;
		for(i=0; i<(UINT_PTR)PatchBlocks.GetSize(); i++)
		{
			ppb=&PatchBlocks[i];
			if(buflen < ppb->m_Size)
				buflen=ppb->m_Size;
			if(curaddrbeg > ppb->m_Addr)
				curaddrbeg=ppb->m_Addr;
			t=ppb->m_Addr+ppb->m_Size;
			if(curaddrend < t)
				curaddrend=t;
			if(ppb->m_Size!=0)
			{
				j=ppb->m_Addr/Size;
				n=(t-1)/Size;
				while(j<=n)
					buf[j++]=TRUE;
			}
			if(isShowMsgOldAbsent==-1 && ppb->m_OldData==NULL && !(ppb->m_Flags&VPB_FLAG_RUN))
			{
				TRMask|=VPB_PATCH_SOME_OLD_DATA_NOT_EXIST;
				if((bApply && !(ppb->m_Flags&(VPB_FLAG_DIS_WARN_APPLY_NO_OLD|VPB_FLAG_DIS_UNDO)))
				   || (!bApply && !(ppb->m_Flags&(VPB_FLAG_DIS_UNDO))))
					isShowMsgOldAbsent=i;
			}
			if(isShowMsgRunUsed==-1 && (ppb->m_Flags&VPB_FLAG_RUN))
				isShowMsgRunUsed=i;
		}
		for(t=0, i=0; i<65536; i++)
			if(buf[i])
				t+=Size;
		Size=t * (bTestMode ? 1 : 2);

		if(!bYesToAll && isShowMsgOldAbsent!=-1)
		{
			CString line, txtinfo;
			str.Format(addrformat, PatchBlocks[isShowMsgOldAbsent].m_Addr);
			line.Format(_T("%i"), patchCurLine=PatchBlocks[isShowMsgOldAbsent].m_TextLine);
			txtinfo.Format(msgInfoText, line, str);
			HilightPatchLine();
			if(AfxMessageBox(txtinfo+_T("\n\n")+msgNoOldInPatch+_T("\n\n")+msgDescr,
				MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON1)!=IDYES)
			{
				TRMask|=VPB_PATCH_USER_SAY_NO_ON_OLD_DATA_NOT_EXIST;
				return FALSE;
			}
			else if(m_strLastRepairPatchFileName.IsEmpty())
			{
				m_strLastRepairPatchFileName=RepairPatchGetFileName();
				if(!o_bIsRepairPatchCanSkip && m_strLastRepairPatchFileName.IsEmpty())
				{
					TRMask|=VPB_PATCH_USER_CANCELS_REPAIR_SAVE;
					return FALSE;
				}
			}
		}
		if(!bYesToAll && isShowMsgRunUsed!=-1)
		{
			CString txtinfo;
			str.Format(_T("%i"), patchCurLine=PatchBlocks[isShowMsgRunUsed].m_TextLine);
			txtinfo.Format(msgInfoTextLine, str);
			HilightPatchLine();
			if(AfxMessageBox(txtinfo+_T("\n\n")+msgRunUsedWarning+_T("\n\n")+msgDescr,
				MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON1)!=IDYES)
			{
				return FALSE;
			}
			else if(m_strLastRepairPatchFileName.IsEmpty())
			{
				m_strLastRepairPatchFileName=RepairPatchGetFileName();
				if(!o_bIsRepairPatchCanSkip && m_strLastRepairPatchFileName.IsEmpty())
				{
					TRMask|=VPB_PATCH_USER_CANCELS_REPAIR_SAVE;
					return FALSE;
				}
			}
		}

		if(pPatchDevice!=NULL)
			if(!pPatchDevice->Open())
				AfxThrowUserException();
		if(pProgress!=NULL && Size!=0)
			pProgress->SetUpper(Size);

		if(o_bIsPatchIgnogeNoOld)
			TRMask|=VPB_PATCH_IGNORE_NO_OLD;
		if(!o_bIsPatchIgnogeNoOld && pPatchDevice!=NULL)
		{
			for(buflen=0, i=0; i<(UINT_PTR)PatchBlocks.GetSize(); i++)
				if(PatchBlocks[i].m_Size>buflen)
					buflen=PatchBlocks[i].m_Size;
			if(buf==NULL || buflen+8 > 65536)
			{
				if(buf!=NULL)
					delete buf;
				buf=new BYTE[buflen+8];
			}
		}
		else if(buf!=NULL)
		{
			delete buf;
			buf=NULL;
		}
		i= bApply ? 0 : PatchBlocks.GetSize()-1;
		step= bApply ? +1 : -1;
		for(count=0; count<(UINT_PTR)PatchBlocks.GetSize(); count++, i+=step)
		{
			if(!bApply)
			{
				if(PatchBlocks[i].m_Flags&VPB_FLAG_DIS_UNDO)
				{
					TRMask|=VPB_PATCH_UNDO_DISABLE_EXIST;
					existNewCounter++;
					continue;
				}
			}

			if(!(PatchBlocks[i].m_Flags&VPB_FLAG_RUN))
			{
				if(PatchBlocks[i].m_OldData==NULL)
					TRMask|=VPB_PATCH_SOME_OLD_DATA_NOT_EXIST;
				if(PatchBlocks[i].m_NewData==NULL)
					TRMask|=VPB_PATCH_SOME_NEW_DATA_NOT_EXIST;
			}
			if(bApply)
			{
				OldData=PatchBlocks[i].m_OldData;
				NewData=PatchBlocks[i].m_NewData;
			}
			else
			{
				OldData=PatchBlocks[i].m_NewData;
				NewData=PatchBlocks[i].m_OldData;
			}
			Size=PatchBlocks[i].m_Size;

			if(NewData==NULL && !(PatchBlocks[i].m_Flags&VPB_FLAG_RUN))
			{
				existNewCounter++;
				continue;
			}

			if(pPatchDevice!=NULL && buf!=NULL)
				if(!pPatchDevice->Read(PatchBlocks[i].m_Addr, Size, buf))
					AfxThrowUserException();

			if(PatchBlocks[i].m_Flags&VPB_FLAG_RUN)
			{	// run function that make new data dynamically
				if((bApply && !(PatchBlocks[i].m_Flags&VPB_FLAG_RUN_IS_ON_APPLY)) ||
					(!bApply && (PatchBlocks[i].m_Flags&VPB_FLAG_RUN_IS_ON_APPLY)))
				{
					existNewCounter++;
					continue;
				}
				PatchBlocks[i].Run(PatchBlocks, PatchStream, pPatchDevice, buf);
				OldData=PatchBlocks[i].m_OldData;
				NewData=PatchBlocks[i].m_NewData;
				Size=PatchBlocks[i].m_Size;
				if(NewData==NULL)
				{
					existNewCounter++;
					continue;
				}
			}

////// convert
			IsNeedToAddPhoneData=FALSE;
			curaddrbeg=PatchBlocks[i].m_Addr;
			curaddrend=PatchBlocks[i].m_Addr+Size;
			for(pos=NULL, findpos=PatchStream.GetHeadPosition(); findpos!=NULL; pos=findpos, PatchStream.GetNext(findpos))
				if(PatchStream.GetAt(findpos).m_Addr > curaddrbeg)
					break;
			if(pos==NULL && findpos!=NULL)				//is cur. block overlap first block in list?
			{
				if(PatchStream.GetAt(findpos).m_Addr <= curaddrend)
					pos=findpos;
			}
			else if(findpos==NULL && pos!=NULL)			//else is cur. block overlap last block in list?
			{
				pbbeg=&PatchStream.GetAt(pos);
				if(pbbeg->m_Addr+pbbeg->m_Size >= curaddrbeg)
					findpos=pos;
			}
			if(pos==NULL && findpos!=NULL)				//if block is before all blocks in the list - add it before all blocks
			{
				pos=PatchStream.AddHead(PatchBlocks[i]);
				IsNeedToAddPhoneData=TRUE;
			}
			else if(findpos==NULL || pos==NULL)			//if block is after all blocks in the list - add it after all blocks
			{
				pos=PatchStream.AddTail(PatchBlocks[i]);
				IsNeedToAddPhoneData=TRUE;
			}
			else										//else - block is between some block, or it in the some one block, or it overlap several blocks.
			{
				pbbeg=&PatchStream.GetAt(pos);
				pbend=NULL;
				if(pbbeg->m_Addr+pbbeg->m_Size < curaddrbeg)	//If cur block lies after founded block,
					pbbeg=&PatchStream.GetAt(pos=findpos);		// assing the next block as founded.
				if(pbbeg->m_Addr > curaddrend)					//If cur block lies before founded block,
				{												// then just insert cur. block before founded block.
					pos=PatchStream.InsertBefore(pos, PatchBlocks[i]);
					IsNeedToAddPhoneData=TRUE;
				}
				else											//else - cur block lies inside of founded block, or it overlap founded block and 0 or more next blocks in list
				{
					pb.Clear();
					findpos=pos;
					PatchStream.GetNext(findpos);				//now we start to search how many blocks in list overlapped by current block.
					for(n=1, prevpos=NULL; findpos!=NULL; n++, prevpos=findpos, PatchStream.GetNext(findpos))
					{
						if(PatchStream.GetAt(findpos).m_Addr > curaddrend) //if we found some block, what is not overlaped and is not touching to the current block - stop seraching.
						{
							if(prevpos!=NULL)
								pbend=&PatchStream.GetAt(prevpos);
							break;
						}
					}
					if(pbend==NULL && n>1)
						pbend=&PatchStream.GetTail();
					pb.m_Addr=min(pbbeg->m_Addr, curaddrbeg);
					if(pbend!=NULL)
						pb.m_Size=max(pbend->m_Addr+pbend->m_Size, curaddrend) - pb.m_Addr;
					else
						pb.m_Size=max(pbbeg->m_Addr+pbbeg->m_Size, curaddrend) - pb.m_Addr;
					if(pbend==NULL && pbbeg->m_Size >= pb.m_Size)	//If cur. block overlap only 1 block, and size of cur. block not greater than this 1 block,
					{												// then cur. block completely lies inside of this 1 block from list.
						if(buf!=NULL)
							memcpy(buf, pbbeg->m_NewData+(curaddrbeg-pbbeg->m_Addr), Size);
						memcpy(pbbeg->m_NewData+(curaddrbeg-pbbeg->m_Addr), NewData, PatchBlocks[i].m_NewSize);
						if((OldData!=NULL || buf!=NULL) && pbbeg->m_OldData!=NULL)
							memcpy(pbbeg->m_OldData+(curaddrbeg-pbbeg->m_Addr), OldData!=NULL ? OldData : buf, Size);
						pbbeg->m_Flags=PatchBlocks[i].m_Flags;
						pbbeg->m_Stat=PatchBlocks[i].m_Stat;
						pbbeg->m_TextLine=PatchBlocks[i].m_TextLine;
					}
					else											//else - cur block in not completely lies inside of 1 block,
					{												// and, therefore, we need to create new block, copy all data from cur block and overlapped (and touching) blocks from list, then delete all overlapped blocks in list.
						pb.m_Flags=PatchBlocks[i].m_Flags;
						pb.m_Stat=PatchBlocks[i].m_Stat;
						pb.m_TextLine=PatchBlocks[i].m_TextLine;
						pos=PatchStream.InsertBefore(pos, pb);
						ppb=&PatchStream.GetAt(pos);

						ptmp= pbend==NULL ? NULL : pbend->m_PhoneData;
						if(buf!=NULL && pbbeg->m_PhoneData!=NULL && (pbend==NULL || ptmp!=NULL))
						{
							ppb->m_PhoneData=new BYTE[ppb->m_Size];
							memcpy(ppb->m_PhoneData+(pbbeg->m_Addr-ppb->m_Addr), pbbeg->m_PhoneData, pbbeg->m_Size);
							if(ptmp!=NULL)
								memcpy(ppb->m_PhoneData+(pbend->m_Addr-ppb->m_Addr), ptmp, pbend->m_Size);
							memcpy(ppb->m_PhoneData+(curaddrbeg-ppb->m_Addr), buf, Size);
						}

						ptmp= pbend==NULL ? NULL : pbend->m_OldData;
						if((OldData!=NULL || buf!=NULL) && pbbeg->m_OldData!=NULL && (pbend==NULL || ptmp!=NULL))
						{
							ppb->m_OldData=new BYTE[ppb->m_Size];
							memcpy(ppb->m_OldData+(pbbeg->m_Addr-ppb->m_Addr), pbbeg->m_OldData, pbbeg->m_Size);
							if(ptmp)
								memcpy(ppb->m_OldData+(pbend->m_Addr-ppb->m_Addr), ptmp, pbend->m_Size);
							memcpy(ppb->m_OldData+(curaddrbeg-ppb->m_Addr), OldData!=NULL ? OldData : buf, Size);
						}

						if(buf!=NULL)
						{
							for(findpos=pos, PatchStream.GetNext(findpos), j=0; j<n && findpos!=NULL; j++)
							{
								ppbt=&PatchStream.GetNext(findpos);
								t= ppbt->m_Addr < curaddrbeg ? curaddrbeg-ppbt->m_Addr : 0;
								memcpy(buf+(ppbt->m_Addr > curaddrbeg ? ppbt->m_Addr-curaddrbeg : 0),
									ppbt->m_NewData+t,
									(ppbt->m_Addr+ppbt->m_Size > curaddrend ? curaddrend-ppbt->m_Addr : ppbt->m_Size)-t);
							}
						}

						ppb->m_NewData=new BYTE[ppb->m_Size];
						if(buf!=NULL)
							memcpy(ppb->m_NewData+(curaddrbeg-ppb->m_Addr)+PatchBlocks[i].m_NewSize,
								buf+PatchBlocks[i].m_NewSize, Size-PatchBlocks[i].m_NewSize);
						memcpy(ppb->m_NewData+(pbbeg->m_Addr-ppb->m_Addr), pbbeg->m_NewData, pbbeg->m_Size);
						if(pbend!=NULL)
							memcpy(ppb->m_NewData+(pbend->m_Addr-ppb->m_Addr), pbend->m_NewData, pbend->m_Size);
						memcpy(ppb->m_NewData+(curaddrbeg-ppb->m_Addr), NewData, buf==NULL ? Size : PatchBlocks[i].m_NewSize);

						for(findpos=pos, PatchStream.GetNext(findpos), j=0; j<n && findpos!=NULL; j++)
						{
							prevpos=findpos;
							PatchStream.GetNext(findpos);
							PatchStream.RemoveAt(prevpos);
						}
					}
				}
			}
			ppb=&PatchStream.GetAt(pos);
			if(IsNeedToAddPhoneData)
			{
				if(!bApply)
				{
					ptmp=ppb->m_NewData;
					ppb->m_NewData=ppb->m_OldData;
					ppb->m_OldData=ptmp;
				}
				if(buf!=NULL)
				{
					if(ppb->m_PhoneData==NULL)
						ppb->m_PhoneData=new BYTE[ppb->m_Size];
					memcpy(ppb->m_PhoneData, buf, ppb->m_Size);
					memcpy(ppb->m_NewData+ppb->m_NewSize, buf+ppb->m_NewSize, ppb->m_Size-ppb->m_NewSize);
					if(ppb->m_OldData==NULL)
					{
						ppb->m_OldData=new BYTE[ppb->m_Size];
						memcpy(ppb->m_OldData, buf, ppb->m_Size);
					}
				}
			}
			NewData=ppb->m_NewData+(curaddrbeg-ppb->m_Addr);
///////

			if(buf!=NULL)
			{
				if(memcmp(buf, NewData, Size)==0)
				{
					PatchBlocks[i].m_Stat|=VPB_STAT_DO_NOT_WRITE;
					existNewCounter++;
				}
				if(OldData!=NULL)
				{
					if(memcmp(buf, OldData, Size)!=0)
					{
						noOldCounter++;
						if(!writenewanyway
							&& !((  ( bApply && PatchBlocks[i].m_Flags&VPB_FLAG_DIS_WARN_APPLY_OLD_NOT_FOUND_BUT_NEW_ALREADY_EXIST)
								|| (!bApply && PatchBlocks[i].m_Flags&VPB_FLAG_DIS_WARN_UNDO_NEW_NOT_FOUND_BUT_OLD_ALREADY_EXIST) )
								&& PatchBlocks[i].m_Stat&VPB_STAT_DO_NOT_WRITE))
						{
							for(addrsearchidx=0; addrsearchidx < Size
								&& buf[addrsearchidx]==OldData[addrsearchidx]; addrsearchidx++);
							CString info, strPh, strOld, strNew, infoText;
							str.Format(addrformat, PatchBlocks[i].m_Addr+addrsearchidx);
							strPh.Format(dataformat, buf[addrsearchidx]);
							strOld.Format(dataformat, PatchBlocks[i].m_OldData[addrsearchidx]);
							strNew.Format(dataformat, PatchBlocks[i].m_NewData[addrsearchidx]);
							info.Format(MSTR("Data in phone memory: %s, old data in patch: %s, new data in patch: %s."), strPh, strOld, strNew);
							strPh.Format(_T("%i"), patchCurLine=PatchBlocks[i].m_TextLine);
							strOld.Format(addrformat, PatchBlocks[i].m_Addr);
							infoText.Format(msgInfoText, strPh, strOld);
							strNoOldWarning=msgErrHeader+str+_T(":\n")+info+_TCHAR('\n')+infoText+_T("\n\n")+
								msgOldExist+_T("\n\n")+msgDescr;
							writenewanyway=TRUE;
						}
					}
				}
			}
		}
		if(existNewCounter==PatchBlocks.GetSize())
			TRMask|=VPB_PATCH_ALREADY_EXIST;
		if(!(TRMask&VPB_PATCH_ALREADY_EXIST) && !strNoOldWarning.IsEmpty())
		{
			CString strno, strall;
			strno.Format(_T("%i"), noOldCounter);
			strall.Format(_T("%i"), PatchBlocks.GetSize());
			str.Format(strNoOldWarning, strno, strall);
			PatchDataTest_ShowNoOldWarning(str, TRMask, bApply);
			strNoOldWarning.Empty();
		}

		//write RepairPatch to file, if needed.
		if(!RepairPatchSave(PatchStream, m_strLastRepairPatchFileName, bApply))
		{
			TRMask|=VPB_PATCH_ERROR_WHILE_SAVE_REPAIR;
			AfxThrowUserException();
		}
		TRMask|=VPB_PATCH_SAFE_TO_WRITE;
		if(!m_strLastRepairPatchFileName.IsEmpty())
			TRMask|=VPB_PATCH_REPAIR_SAVED;
		bOk=TRUE;
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		if(pPatchDevice!=NULL)
		{
			pPatchDevice->Abort();
			pPatchDevice->Close();
		}
		writenewanyway=FALSE;
	}
	END_CATCH_ALL;
	if(buf!=NULL)
		delete buf;
	if(lpTestResultMask!=NULL)
		*lpTestResultMask=TRMask;
	return bOk;
}

BOOL CPatchPage::PatchDataTest_ShowNoOldWarning(CString &strWarn, UINT &TRMask, BOOL bApply)
{
	HilightPatchLine();
	if(AfxMessageBox(strWarn, MB_YESNO|MB_ICONSTOP)!=IDYES)
	{
		if(bApply)
			TRMask|=VPB_PATCH_USER_SAY_NO_ON_OLD_NOT_FOUND;
		else
			TRMask|=VPB_PATCH_USER_SAY_NO_ON_NEW_NOT_FOUND;
		AfxThrowUserException();
		return FALSE;
	}
	if(m_strLastRepairPatchFileName.IsEmpty())
	{
		m_strLastRepairPatchFileName=RepairPatchGetFileName();
		if(!o_bIsRepairPatchCanSkip && m_strLastRepairPatchFileName.IsEmpty())
		{
			TRMask|=VPB_PATCH_USER_CANCELS_REPAIR_SAVE;
			AfxThrowUserException();
		}
	}
	return TRUE;
}

// Write the patch to the device
BOOL CPatchPage::PatchDataWrite(VPatchStream &Patch, VDevice *pPatchDevice)
{
	BOOL bOk=FALSE;
	if(Patch.GetHeadPosition()==NULL)
		return TRUE;
	POSITION pos;
	VPatchBlock *ppb;
	TRY
	{
		for(pos=Patch.GetHeadPosition(); pos!=NULL; )
		{
			ppb=&Patch.GetNext(pos);
			if(ppb->m_NewData==NULL)
				continue;
			if(!(ppb->m_Stat&VPB_STAT_DO_NOT_WRITE))
				if(!pPatchDevice->Write(ppb->m_Addr, ppb->m_Size, ppb->m_NewData))
					AfxThrowUserException();
		}
		bOk=TRUE;
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		pPatchDevice->Abort();
		pPatchDevice->Close();
	}
	END_CATCH_ALL;
	return bOk;
}

BOOL CPatchPage::SaveModifiedDocument(void)
{
	if(!m_PatchDataCtrl.GetModify())
		return TRUE;
	UINT ret=AfxMessageBox(MSTR("Save changes in current patch?"), MB_YESNOCANCEL|MB_DEFBUTTON1);
	if(ret==IDCANCEL)
		return FALSE;
	if(ret==IDNO)
		return TRUE;
	return SaveDocument(FALSE);
}

void CPatchPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(bShow, m_strDocFileName, m_PatchDataCtrl.GetModify());
}

afx_msg LRESULT CPatchPage::OnIsCanClose(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	if(m_PatchDataCtrl.GetModify() && pCKLayDlg && wTabIdx!=-1)
		((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
	return SaveModifiedDocument();
}

//Return Nonzero to receive more idle processing time; 0 if no more idle time is needed
afx_msg LRESULT CPatchPage::OnIdle(WPARAM wTabIdx, LPARAM lCount)
{
	UpdateRowColNum();
	CWnd *pwnd;
	pwnd=GetDlgItem(IDC_PATCH_RELOAD);
	if(pwnd!=NULL)
		pwnd->EnableWindow(!m_IsDocNew);
	//pwnd=GetDlgItem(IDC_PATCH_SAVE);
	//if(pwnd!=NULL)
	//	pwnd->EnableWindow(m_PatchDataCtrl.GetModify());
	return FALSE;
}

afx_msg LRESULT CPatchPage::OnAppOptionsChanged(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	m_RichEditDefaultCharFormat.yHeight=o_PatcherFontSize*20;
	CHARRANGE cr;
	m_PatchDataCtrl.GetSel(cr);
	if(cr.cpMax==cr.cpMin)
		m_PatchDataCtrl.SetSelectionCharFormat(m_RichEditDefaultCharFormat);
	return TRUE;
}


BOOL CPatchPage::OnInitDialog()
{
	msgApplyDescr=MSTR("YES - to write new data anyway, but before this a Repair Patch will be saved. Later you can load it and UNDO all changes, made by applying of current patch, to original (not to old data from current patch). DO NOT USE CURRENT PATCH FOR UNDO IN THIS CASE!\nNO - to cancel apply patch.");
	msgUndoDescr=MSTR("YES - to write old data anyway, but before this a Repair Patch will be saved. Later you can load it and UNDO (use UNDO button) all changes made, by undoing of current patch, to original (not to new data from current patch).  DO NOT USE CURRENT PATCH FOR APPLY IN THIS CASE!\nNO - to cancel undo patch.");
	msgRepairIn=MSTR("Repair Patch is saved in file:");

	SetDlgItemText(ID_PATCH_APPLY, MSTR("Apply Patch"));
	SetDlgItemText(ID_PATCH_UNDO, MSTR("Undo Patch"));
	SetDlgItemText(IDC_DATA_FORMAT_STATIC, (MSTR("Patch data in format: _ADDRESS_: [_OLD_DATA_] _NEW_DATA_")));
	SetDlgItemText(IDC_COLUMN_STATIC, MSTR("Column:"));
	SetDlgItemText(IDC_LINE_STATIC, MSTR("Line:"));

	CDialog::OnInitDialog();
	patchRichEditCtrl=&m_PatchDataCtrl;

	CRect r;
	m_pToolTipCtrl=new CToolTipCtrl;
	if(!m_pToolTipCtrl->Create(this))
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
	else
	{
		m_pToolTipCtrl->SetDelayTime(TTDT_AUTOPOP, 32000);
		if(SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0))
			m_pToolTipCtrl->SetMaxTipWidth(r.Size().cx-10);
	}

	CreateRichEditDoc();

	m_PatchDataCtrl.SetWindowText(NULL);
	m_PatchDataCtrl.LimitText(0x7FFFFFFF);
//#if _MFC_VER >=0x0700
	m_PatchDataCtrl.SetTextMode(TM_RICHTEXT | TM_MULTILEVELUNDO | TM_MULTICODEPAGE);
	m_PatchDataCtrl.SetAutoURLDetect(TRUE);
//#else
//	m_PatchDataCtrl.SendMessage(EM_SETTEXTMODE, TM_RICHTEXT | TM_MULTILEVELUNDO | TM_MULTICODEPAGE, 0);
//	m_PatchDataCtrl.SendMessage(EM_AUTOURLDETECT, TRUE, 0);
//#endif
	m_PatchDataCtrl.SetOptions(ECOOP_OR, ECO_NOHIDESEL | ECO_SAVESEL | ECO_SELECTIONBAR |
		ECO_AUTOVSCROLL | ECO_AUTOHSCROLL);
	m_PatchDataCtrl.SetEventMask(m_PatchDataCtrl.GetEventMask() | ENM_LINK | ENM_SELCHANGE | ENM_KEYEVENTS | ENM_CHANGE);

	m_PatchDataCtrl.GetDefaultCharFormat(m_RichEditDefaultCharFormat);
	memset(&m_RichEditDefaultCharFormat, 0, sizeof(m_RichEditDefaultCharFormat));
	m_RichEditDefaultCharFormat.cbSize=sizeof(m_RichEditDefaultCharFormat);
	m_RichEditDefaultCharFormat.dwMask=CFM_CHARSET|CFM_FACE|CFM_SIZE|CFM_COLOR|CFM_BACKCOLOR
		|CFM_OFFSET|CFM_CHARSET|CFM_WEIGHT|CFM_SPACING|CFM_KERNING|CFM_UNDERLINETYPE
		|CFM_LCID|CFM_STYLE|CFM_ANIMATION|CFM_REVAUTHOR
		|CFE_ALLCAPS|CFE_DISABLED|CFE_EMBOSS|CFE_HIDDEN|CFE_IMPRINT|CFE_ITALIC|CFE_LINK
		|CFE_OUTLINE|CFE_PROTECTED|CFE_REVISED|CFE_SHADOW|CFE_SMALLCAPS|CFE_STRIKEOUT|CFE_SUBSCRIPT
		|CFE_SUPERSCRIPT|CFE_UNDERLINE;
	m_RichEditDefaultCharFormat.dwEffects=CFE_AUTOBACKCOLOR|CFE_AUTOCOLOR;
	m_RichEditDefaultCharFormat.yHeight=o_PatcherFontSize*20;
	m_RichEditDefaultCharFormat.yOffset=0;
	m_RichEditDefaultCharFormat.bCharSet=DEFAULT_CHARSET;
	m_RichEditDefaultCharFormat.crTextColor=0;
	m_RichEditDefaultCharFormat.bPitchAndFamily=DEFAULT_PITCH|FF_MODERN;
	_tcscpy(m_RichEditDefaultCharFormat.szFaceName, _T("Courier New"));
	m_RichEditDefaultCharFormat.wWeight=FW_NORMAL;
	m_RichEditDefaultCharFormat.sSpacing=0;
	m_RichEditDefaultCharFormat.crBackColor=0;
	m_RichEditDefaultCharFormat.wKerning=0;
	m_RichEditDefaultCharFormat.bUnderlineType=CFU_UNDERLINE;
	m_RichEditDefaultCharFormat.dwReserved=0;
	m_RichEditDefaultCharFormat.lcid=MAKELCID(MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SORT_DEFAULT);
	m_RichEditDefaultCharFormat.sStyle=0;
	m_RichEditDefaultCharFormat.bAnimation=0;
	m_RichEditDefaultCharFormat.bRevAuthor=0;
	m_PatchDataCtrl.SetDefaultCharFormat(m_RichEditDefaultCharFormat);

	CWinApp* app=AfxGetApp();
	CButton* btn;

	btn=(CButton*)GetDlgItem(IDC_PATCH_NEW);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_NEW),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("New"));

	btn=(CButton*)GetDlgItem(IDC_PATCH_LOAD);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_OPEN),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Open..."));

	btn=(CButton*)GetDlgItem(IDC_PATCH_RELOAD);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_REOPEN),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Reopen"));

	btn=(CButton*)GetDlgItem(IDC_PATCH_APPEND);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_APPEND),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Append..."));

	btn=(CButton*)GetDlgItem(IDC_PATCH_SAVE);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_SAVE),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Save"));

	btn=(CButton*)GetDlgItem(IDC_PATCH_SAVE_AS);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_SAVE_AS),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Save As..."));

	m_pToolTipCtrl->AddTool(GetDlgItem(ID_PATCH_APPLY_TEST), MSTR("Test current patch for possibility to apply and on existence in the phone"));
	m_pToolTipCtrl->AddTool(GetDlgItem(ID_PATCH_UNDO_TEST), MSTR("Test current patch for possibility to undo and on non existence in the phone"));

//	CHARFORMAT cf;
//	cf.cbSize=sizeof(cf);
//	cf.dwMask=CFM_CHARSET;
//	cf.bCharSet=CHINESEBIG5_CHARSET;
//	m_PatchDataCtrl.SetDefaultCharFormat(cf);

	// Note that LoadAccelerator does not require DestroyAcceleratorTable
	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR_PATCH_PAGE));


	OnNewDocument(FALSE);

	m_DynSize.AttachDialog(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

afx_msg LRESULT CPatchPage::OnInitialUpdate(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	CString fname=((CKLayApp*)AfxGetApp())->GetCmdLineFileName();
	if(!fname.IsEmpty())
	{
		if(OpenDocument(fname))
		{
			((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
			if(o_cmdDoWrite)
				OnBnClickedPatchApply();
			if(o_cmdDoRead)
				OnBnClickedPatchUndo();
			return 0;
		}
	}
	return 1;
}

void CPatchPage::OnOK()
{
}

void CPatchPage::OnBnClickedPatchNew()
{
	if(SaveModifiedDocument())
		OnNewDocument();
	m_PatchDataCtrl.SetFocus();
}

CString CPatchPage::GetDefaultFileName(void)
{
	CString name=CString(_T("Patch_"))+COleDateTime::GetCurrentTime().Format(strFNTimeFormat)+_T(".vkp");
	return name;
}

BOOL CPatchPage::OnNewDocument(BOOL bEmptyDocument/*=TRUE*/)
{
	m_strDocFileName=GetDefaultFileName();
	m_IsDocNew=TRUE;
	m_IsDocRichText=FALSE;
	if(bEmptyDocument)
	{
		ReInsertClipboardData();
		m_PatchData.Empty();
		m_PatchDataCtrl.SetWindowText(NULL);
		m_PatchDataCtrl.SetDefaultCharFormat(m_RichEditDefaultCharFormat);
	}
	m_PatchDataCtrl.SetModify(FALSE);
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strDocFileName, m_PatchDataCtrl.GetModify());
	UpdateRowColNum();
	return TRUE;
}

TCHAR strrowcolformat[]=_T("%i ");
long cpp_oldPos=-1;
void CPatchPage::UpdateRowColNum(void)
{
	long b, e;
	m_PatchDataCtrl.GetSel(b,e);
	if(cpp_oldPos!=b)
	{
		cpp_oldPos=b;
		int i=m_PatchDataCtrl.LineFromChar(-1)+1;
		CString str;
		str.Format(strrowcolformat, i);
		SetDlgItemText(IDC_CUR_LINE, str);
		i=b-m_PatchDataCtrl.LineIndex(-1)+1;
		str.Format(strrowcolformat, i);
		SetDlgItemText(IDC_CUR_COL, str);
	}
}

void CPatchPage::OnDestroy()
{
	patchRichEditCtrl=NULL;
	ReInsertClipboardData();
	DestroyRichEditDoc();
	CDialog::OnDestroy();
	cpp_oldPos=-1;
}

void CPatchPage::OnPatchDataPaste()
{
	if(o_bIsPatcherSwapPasteKeys)
		OnPatchDataPastetextDo();
	else
		OnPatchDataPasteDo();
}

void CPatchPage::OnPatchDataPastetext()
{
	if(o_bIsPatcherSwapPasteKeys)
		OnPatchDataPasteDo();
	else
		OnPatchDataPastetextDo();
}

void CPatchPage::OnPatchDataPasteDo()
{
	m_PatchDataCtrl.Paste();
}

void CPatchPage::OnPatchDataPastetextDo()
{
	UINT format=IsClipboardFormatAvailable(CF_TEXT) ? CF_TEXT : 0;
#ifdef _UNICODE
	if(IsClipboardFormatAvailable(CF_UNICODETEXT))
		format=CF_UNICODETEXT;
#endif
	if(!format)
	{
		OnPatchDataPaste();
	}
	else
	{
		m_PatchDataCtrl.PasteSpecial(format);
		//correction of RichEditBug: when paste clipboard text, the RichEdit do not insert last CRLF symbols.
		if(IsClipboardFormatAvailable(format))
		{
			if(OpenClipboard())
			{
				BOOL IsNeedAddCRLF=FALSE;
				char *lptstr;
				size_t len;
				HGLOBAL hglb = GetClipboardData(format); 
				if(hglb != NULL) 
				{
					lptstr = (char*)GlobalLock(hglb); 
					if(lptstr != NULL) 
					{
						if(format==CF_UNICODETEXT)
						{
							len=wcslen((wchar_t*)lptstr)*2;
							lptstr+=len-2;
						}
						else
						{
							len=strlen(lptstr);
							lptstr+=len-1;
						}
						if(len>0)
						{
							if(*lptstr=='\r' || *lptstr=='\n')
							{
								char strCRLF[]="\r\n";
								RichStreamIn(&m_PatchDataCtrl, (LPCTSTR)strCRLF, sizeof(strCRLF)-1, SF_TEXT | SFF_SELECTION);
							}
						}
						GlobalUnlock(hglb); 
					} 
				}
				CloseClipboard();
			}
		}
	}
}

void CPatchPage::OnPatchDataCut()
{
	m_PatchDataCtrl.Cut();
	m_IsDataInClipboard=TRUE;
}

void CPatchPage::OnPatchDataCopy()
{
	m_PatchDataCtrl.Copy();
	m_IsDataInClipboard=TRUE;
}

BOOL CPatchPage::ReInsertClipboardData(void)
{
	if(!m_IsDataInClipboard)
		return TRUE;
	m_IsDataInClipboard=FALSE;
	//if(::GetClipboardOwner()!=m_PatchDataCtrl.GetSafeHwnd())
	//	return TRUE;
	UINT uFormat=RegisterClipboardFormat(CF_RTF);
	//if(!IsClipboardFormatAvailable(uFormat))
	//	return TRUE;
	if(OpenClipboard())
	{
		HGLOBAL hglb, hglbRTF=NULL;
		size_t len;
		char *lptstr, *lptstrCopy;
		if(IsClipboardFormatAvailable(uFormat))
		{
			hglb = GetClipboardData(uFormat); 
			if (hglb != NULL) 
			{
				lptstr = (char*)GlobalLock(hglb); 
				if (lptstr != NULL) 
				{
					len=strlen(lptstr);
					hglbRTF = GlobalAlloc(GMEM_MOVEABLE, (len+1) * sizeof(char)); 
					if(hglbRTF != NULL) 
					{ 
						lptstrCopy = (char*)GlobalLock(hglbRTF); 
						if(lptstrCopy!=NULL)
						{
							memcpy(lptstrCopy, lptstr, (len+1) * sizeof(char)); 
							GlobalUnlock(hglbRTF); 
						}
					}
					GlobalUnlock(hglb); 
				} 
			}
		}

		//uFormat=RegisterClipboardFormat(CF_RETEXTOBJ);
		//if(IsClipboardFormatAvailable(uFormat))
		//{
		//	hglb = GetClipboardData(uFormat); 
		//	if (hglb != NULL) 
		//	{
		//		lptstr = (char*)GlobalLock(hglb); 
		//		if (lptstr != NULL) 
		//		{
		//			len=strlen(lptstr);
		//			hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len+1) * sizeof(char)); 
		//			if(hglbCopy != NULL) 
		//			{ 
		//				lptstrCopy = (char*)GlobalLock(hglbCopy); 
		//				if(lptstrCopy!=NULL)
		//				{
		//					memcpy(lptstrCopy, lptstr, (len+1) * sizeof(char)); 
		//					GlobalUnlock(hglbCopy); 
		//					SetClipboardData(uFormat, hglbCopy); 
		//				}
		//			}
		//			GlobalUnlock(hglb); 
		//		} 
		//	}
		//}

		//uFormat=RegisterClipboardFormat(CF_RTFNOOBJS);
		//if(IsClipboardFormatAvailable(uFormat))
		//{
		//	hglb = GetClipboardData(uFormat); 
		//	if (hglb != NULL) 
		//	{
		//		lptstr = (char*)GlobalLock(hglb); 
		//		if (lptstr != NULL) 
		//		{
		//			len=strlen(lptstr);
		//			hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len+1) * sizeof(char)); 
		//			if(hglbCopy != NULL) 
		//			{ 
		//				lptstrCopy = (char*)GlobalLock(hglbCopy); 
		//				if(lptstrCopy!=NULL)
		//				{
		//					memcpy(lptstrCopy, lptstr, (len+1) * sizeof(char)); 
		//					GlobalUnlock(hglbCopy); 
		//					SetClipboardData(uFormat, hglbCopy); 
		//				}
		//			}
		//			GlobalUnlock(hglb); 
		//		} 
		//	}
		//}

		HGLOBAL hglbUnicodeText=NULL;
#ifdef _UNICODE
		if(IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			wchar_t *lptwstr, *lptwstrCopy;
			hglb = GetClipboardData(CF_UNICODETEXT); 
			if (hglb != NULL) 
			{
				lptwstr = (wchar_t*)GlobalLock(hglb); 
				if (lptwstr != NULL) 
				{
					len=wcslen(lptwstr);
					hglbUnicodeText = GlobalAlloc(GMEM_MOVEABLE, (len+1) * sizeof(wchar_t)); 
					if(hglbUnicodeText != NULL) 
					{ 
						lptwstrCopy = (wchar_t*)GlobalLock(hglbUnicodeText); 
						if(lptwstrCopy!=NULL)
						{
							memcpy(lptwstrCopy, lptwstr, (len+1) * sizeof(wchar_t)); 
							GlobalUnlock(hglbUnicodeText); 
						}
					}
					GlobalUnlock(hglb); 
				} 
			} 
		}
#endif
		HGLOBAL hglbText=NULL;
		if(IsClipboardFormatAvailable(CF_TEXT))
		{
			hglb = GetClipboardData(CF_TEXT); 
			if (hglb != NULL) 
			{
				lptstr = (char*)GlobalLock(hglb); 
				if (lptstr != NULL) 
				{
					len=strlen(lptstr);
					hglbText = GlobalAlloc(GMEM_MOVEABLE, (len+1) * sizeof(char)); 
					if(hglbText != NULL) 
					{ 
						lptstrCopy = (char*)GlobalLock(hglbText); 
						if(lptstrCopy!=NULL)
						{
							memcpy(lptstrCopy, lptstr, (len+1) * sizeof(char)); 
							GlobalUnlock(hglbText); 
						}
					}
					GlobalUnlock(hglb); 
				} 
			}
		}

		EmptyClipboard();
		if(hglbText!=NULL)
			SetClipboardData(CF_TEXT, hglbText); 
		if(hglbUnicodeText!=NULL)
			SetClipboardData(CF_UNICODETEXT, hglbUnicodeText); 
		if(hglbRTF!=NULL)
			SetClipboardData(RegisterClipboardFormat(CF_RTF), hglbRTF); 
		CloseClipboard(); 
	}
	return TRUE;
}

void CPatchPage::OnPatchDataUndo()
{
	m_PatchDataCtrl.Undo();
}

void CPatchPage::OnPatchDataRedo()
{
#if _MFC_VER >=0x0700
	m_PatchDataCtrl.Redo();
#else
	m_PatchDataCtrl.SendMessage(EM_REDO, 0, 0);
#endif
}

void CPatchPage::OnPatchDataSelectAll()
{
	m_PatchDataCtrl.SetSel(0, -1);
}

void CPatchPage::OnPatchDataDelete()
{
	m_PatchDataCtrl.Clear();
}

void CPatchPage::OnSize(UINT nType, int cx, int cy)
{
	m_DynSize.OnSize(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

BOOL CPatchPage::PreTranslateMessage(MSG* pMsg)
{
	// this code is taken from Mfccalc MSDN sample ---
	// CCalcDlg::PreTransateMessage
	if (m_hAccel != NULL && TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
		return TRUE;
	// end sample

	if(m_pToolTipCtrl!=NULL)
		m_pToolTipCtrl->RelayEvent(pMsg);

//	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB))  
//	{
//   m_bTab=true; 
//	 m_rtf.m_bClick =false;
//	 // get the char index of the caret position
//     CPoint nPos = m_rtf.GetCaretPos();  
//	 // select zero chars  
//     //m_rtf.SetSel(nPos.x, nPos.x);  
//     // then replace that selection with a TAB  
//     m_rtf.ReplaceSel("\t", TRUE);
//     return true;
//	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPatchPage::OnEnLinkPatchData(NMHDR *pNMHDR, LRESULT *pResult)
{
	ENLINK *pEnLink = reinterpret_cast<ENLINK *>(pNMHDR);
	*pResult = 0;
	if(pEnLink->msg==WM_LBUTTONDOWN)
	{
		CString str;
		// the min and max range is given with not counting the LF char at the end of each row.
		// Therefore we must correct this:
		DWORD lines=m_PatchDataCtrl.LineFromChar(pEnLink->chrg.cpMin);
		CHARRANGE cr;
		cr.cpMin=pEnLink->chrg.cpMin+lines;
		cr.cpMax=pEnLink->chrg.cpMax+lines;
		//now in cr we have true link range.
		RichStreamOutTextPartial(&m_PatchDataCtrl, str, cr.cpMin, cr.cpMax);
		SHELLEXECUTEINFO sei;
		::memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.hwnd = m_hWnd;
		sei.lpVerb= _T("open" );
		sei.lpFile = str;
		sei.nShow = SW_SHOWNORMAL;
		if((::ShellExecuteEx(&sei)) && (__int64(sei.hInstApp)>32))				
			*pResult = 1;
	}
}

void CPatchPage::OnPatchDataFont()
{
	CHARRANGE cr;
	m_PatchDataCtrl.GetSel(cr);
	BOOL isselected= cr.cpMax!=cr.cpMin;
	CHARFORMAT cf;
	cf.cbSize=sizeof(cf);
	m_PatchDataCtrl.GetSelectionCharFormat(cf);
	CFontDialog dlg(cf, CF_SCREENFONTS | CF_EFFECTS, 0, this);

	_tcscpy(dlg.m_cf.lpLogFont->lfFaceName, cf.szFaceName);
	dlg.m_cf.iPointSize=cf.yHeight;
	dlg.m_cf.lpLogFont->lfItalic= (cf.dwEffects&CFE_ITALIC) != 0;
	dlg.m_cf.lpLogFont->lfWeight= cf.dwEffects&CFE_BOLD ? FW_BOLD : FW_NORMAL;
	dlg.m_cf.lpLogFont->lfStrikeOut= (cf.dwEffects&CFE_STRIKEOUT) != 0;
	dlg.m_cf.lpLogFont->lfUnderline= (cf.dwEffects&CFE_UNDERLINE) != 0;
	dlg.m_cf.rgbColors=cf.crTextColor;
	dlg.m_cf.lpLogFont->lfCharSet=cf.bCharSet;

	if(dlg.DoModal()==IDOK)
	{
		dlg.GetCharFormat(cf);
		if(isselected)
			m_PatchDataCtrl.SetSelectionCharFormat(cf);
		else
			m_PatchDataCtrl.SetWordCharFormat(cf);
		m_IsDocRichText=TRUE;
	}
}

void CPatchPage::OnPatchDataBackcolor()
{
	CHARRANGE cr;
	m_PatchDataCtrl.GetSel(cr);
	BOOL isselected= cr.cpMax!=cr.cpMin;
	CHARFORMAT2 cf;
	cf.cbSize=sizeof(cf);
	m_PatchDataCtrl.GetSelectionCharFormat(cf);
	COLORREF initColor=cf.crBackColor;
	if(cf.dwMask&CFM_BACKCOLOR)
		if(cf.dwEffects&CFE_AUTOBACKCOLOR)
			initColor=GetSysColor(COLOR_WINDOW);
	CColorDialog dlg(initColor, CC_RGBINIT, this);

	if(dlg.DoModal()==IDOK)
	{
		cf.crBackColor=dlg.GetColor();
		if(cf.crBackColor!=initColor)
		{
			cf.dwMask=CFM_BACKCOLOR;
			cf.dwEffects=0;
			if(isselected)
				m_PatchDataCtrl.SetSelectionCharFormat(cf);
			else
				m_PatchDataCtrl.SetWordCharFormat(cf);
		}
		m_IsDocRichText=TRUE;
	}
}

BOOL CPatchPage::IsRichText(LPCTSTR strTxt)
{
	if(strTxt==NULL)
		return FALSE;
	size_t size=strlen((const char*)strTxt);
	if(size<5)
		return FALSE;
	if(memcmp(strTxt, "{\\rtf", sizeof("{\\rtf")-1)==0)
		return TRUE;
	if(memcmp(strTxt, "{urtf", sizeof("{urtf")-1)==0)
		return TRUE;
	return FALSE;
}

void CPatchPage::CreateRichEditDoc(void)
{
	if(m_pRichEditDoc==NULL)
	{
		m_pRichEditDoc= new VKLayRichEditDoc::CRichEditDoc(_T("V_KLayRichEditOle"));
		m_pRichEditDoc->Create(this->GetSafeHwnd(), m_PatchDataCtrl.GetSafeHwnd());
	}
}

void CPatchPage::DestroyRichEditDoc(void)
{
	if(m_pRichEditDoc!=NULL)
	{
		m_pRichEditDoc->Close();
		delete m_pRichEditDoc;
		m_pRichEditDoc=NULL;
	}
}

void CPatchPage::OnEnSelchangePatchData(NMHDR *pNMHDR, LRESULT *pResult)
{
	SELCHANGE *pSelChange = reinterpret_cast<SELCHANGE *>(pNMHDR);
	// TODO:  The control will not send this notification unless you override the
	// CDialog::OnInitDialog() function to send the EM_SETEVENTMASK message
	// to the control with the ENM_SELCHANGE flag ORed into the lParam mask.
	UpdateRowColNum();
	*pResult = 0;
}


void CPatchPage::OnEnMsgfilterPatchData(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	// TODO:  The control will not send this notification unless you override the
	// CDialog::OnInitDialog() function to send the EM_SETEVENTMASK message
	// to the control with either the ENM_KEYEVENTS or ENM_MOUSEEVENTS flag 
	// ORed into the lParam mask.

	*pResult = 0;
	if(pMsgFilter->msg==WM_KEYDOWN)
	{
		if(((GetKeyState(VK_CONTROL) & SHIFTED) && 
			(pMsgFilter->wParam=='C' || pMsgFilter->wParam=='X' || pMsgFilter->wParam==VK_INSERT)) ||
			((GetKeyState(VK_SHIFT) & SHIFTED) && pMsgFilter->wParam==VK_DELETE))
			m_IsDataInClipboard=TRUE;
		if(((GetKeyState(VK_CONTROL) & SHIFTED) && pMsgFilter->wParam=='V') ||
			((GetKeyState(VK_SHIFT) & SHIFTED) && pMsgFilter->wParam==VK_INSERT))
			*pResult = 1;
	}
}

// Get file name for RepairPatch
CString CPatchPage::RepairPatchGetFileName(void)
{
	CString udir, utitle, uext, fname;
	udir=GetFileDir(m_strDocFileName);
	utitle=GetFileTitle(m_strDocFileName);
	uext=GetFileExt(m_strDocFileName);
	CFileDialog fdlg(FALSE, _T("vkp"), udir+utitle+_T("_REPAIR")+_TCHAR('.')+uext,
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		vkpfiltersave, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=MSTR("Save Repair Patch As...");
	if(m_IsDocNew || udir.IsEmpty())
		fdlg.m_ofn.lpstrInitialDir=GetCurPatchDir();
	if(fdlg.DoModal()==IDOK)
	{
		SaveCurPatchDir(fdlg.GetPathName());
		fname=fdlg.GetPathName();
	}
	return fname;
}

CString MergeStringsCenter(CString strDest, CString strSrc)
{
	strSrc.Insert(0, _TCHAR(' '));
	strSrc+=_TCHAR(' ');
	int pos=(strDest.GetLength()-strSrc.GetLength())/2;
	if(pos>0)
	{
		int len=max(strDest.GetLength(), strSrc.GetLength());
		CString::CopyChars(strDest.GetBuffer()+pos, strSrc, strSrc.GetLength());
		strDest.ReleaseBuffer(len);
	}
	else
		strDest=strSrc;
	return strDest;
}

//_TCHAR strCPPRepairPatchSaveDataHeader[]=_T(";       Data in phone    New patch data    Old patch data\r\n" \
//											";addr.  0 1 2 3 4 5 6 7  0 1 2 3 4 5 6 7   0 1 2 3 4 5 6 7r\n");
_TCHAR strCPPRepairPatchSaveDataHeader[]=_T(";       Old data in the phone:           Current (new) data in the phone: ;Old data in the patch:\r\n") \
										 _T(";addr.  0 1 2 3 4 5 6 7 8 9 A B C D E F  0 1 2 3 4 5 6 7 8 9 A B C D E F  ;0 1 2 3 4 5 6 7 8 9 A B C D E F\r\n");
// Write data of RepairPatch to file
BOOL CPatchPage::RepairPatchSave(VPatchStream& Patch, CString strRepairPatchFileName, BOOL bApply)
{
	if(strRepairPatchFileName.IsEmpty())
		return TRUE;
		//strRepairPatchFileName=_T("d:\\!\\!!!.vkp");
	CString strWork=MSTR("Creating repair patch...");
	CString strWorkOk=MSTR("Ok.");
	if(pProgress)
		pProgress->SetStatus(strWork);
	POSITION pos;
	VPatchBlock *ppb;
	BOOL bOk=FALSE;
	TRY
	{
		CFile f(strRepairPatchFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		CString patch, str, line, header;
		header=_T(";");
		header+=MergeStringsCenter(separatorPatchText, MSTR("*** REPAIR PATCH ***"));
		UINT64 ver = GetAppVersion();
		str.Format(_T("\r\n; Made by V_KLay v%i.%i.%i.%i\r\n; "),
			(int)((ver >> 48) & 0xffff), (int)((ver >> 32) & 0xffff), (int)((ver >> 16) & 0xffff), (int)((ver >> 0) & 0xffff));
		header+=str;
		header+=MSTR("Press \"Undo Patch\" button to repair phone.");
		header+=_T("\r\n\r\n;  ");
		if(bApply)
			header+=MSTR("!!!WARNING!!! DO NOT UNDO THIS PATCH, IF YOU MAKE SOME MODIFICATION\r\n OF PHONE AFTER APPLYING PATCH WITH NAME:");
		else
			header+=MSTR("!!!WARNING!!! DO NOT UNDO THIS PATCH, IF YOU MAKE SOME MODIFICATION\r\n OF PHONE AFTER UNDOING PATCH WITH NAME:");
		header+=_T("\r\n\r\n;");
		header+=MergeStringsCenter(separatorPatchText, MSTR("PATCH FILE NAME:"));
		header+=_T("\r\n;  ");
		header+=m_strDocFileName;
		header+=_T("\r\n\r\n");

		if(!o_bIsRepairPatchTextSimple)
		{
			header+=_T(';');
			header+=MergeStringsCenter(separatorPatchText, MSTR("PATCH DESCRIPTION:"));
			header+=_T("\r\n;\r\n");

			CString sp;
			RichStreamOutTextPartial(&m_PatchDataCtrl, sp, 0, 16*1024);
			int blanks, lines;
			LPCTSTR pend, psp=sp;
			for(blanks=0, lines=0; *psp; )
			{
				pend=psp;
				//skipspaces(pend);
				//if(*pend==_TCHAR('+') || *pend==_TCHAR('-'))
				//	break;
				//if(*pend==_TCHAR('#'))
				//	break;
				//if(_istxdigit(*pend))
				//{
				//	VStrToHex(pend, (LPTSTR*)&pend, 16);
				//	if(v_istspace(*pend) || *pend==_TCHAR(':'))
				//			break;
				//}
				if(lines>32)
				{
					header+=_T("; ...\r\n");
					break;
				}
				for(;*pend!=0 && *pend!=_T('\r') && *pend!=_T('\n'); pend++);
				line.SetString(psp, (int)(pend-psp));
				psp=pend;
				findendofline(psp);

				str=line;
				str.Trim();
				if(str.IsEmpty())
				{
					blanks++;
					continue;
				}
				if(lines!=0 && blanks!=0)
					for(; blanks; blanks--)
						header+=_T("\r\n");
				lines++;
				blanks=0;
				header+=line;
				header+=_T("\r\n");
			}

			header+=_TCHAR(';');
			header+=separatorPatchText;
			header+=_T("\r\n\r\n");
		}

		patch=header;
		CommentAllLinesOfText(patch);

		patch+=_T("#pragma disable warn_if_old_exist_on_undo\r\n\r\n");
		// save different old and new data
		if(!o_bIsRepairPatchTextSimple)
		{
			patch+=_T(";different phone data and old data from the patch:\r\n");
			patch+=strCPPRepairPatchSaveDataHeader;
		}
		for(pos=Patch.GetHeadPosition(); pos!=NULL; )
		{
			ppb=&Patch.GetNext(pos);
			if(ppb->m_NewData==NULL || ppb->m_OldData==NULL)
			{
				AfxMessageBox(_T("RepairPatch save error: missing old or new data"));
				AfxThrowUserException();
			}
			if(o_bIsRepairPatchTextSimple)
				patch+=ppb->MakeTextLine(VPB_MAKETXT_PHONE_DATA, VPB_MAKETXT_NEW_DATA, VPB_MAKETXT_NO_DATA,
					VPB_TYPE_ANY, 0, 0, TRUE);
			else
				patch+=ppb->MakeTextLine(VPB_MAKETXT_PHONE_DATA, VPB_MAKETXT_NEW_DATA, VPB_MAKETXT_OLD_DATA,
					VPB_TYPE_DIFFERENT, VPB_MAKETXT_PHONE_DATA, VPB_MAKETXT_OLD_DATA);
		}

		// save same old and new data
		if(!o_bIsRepairPatchTextSimple)
		{
			patch+=_T("\r\n;same phone data and old data from the patch:\r\n");
			patch+=strCPPRepairPatchSaveDataHeader;
			for(pos=Patch.GetHeadPosition(); pos!=NULL; )
			{
				ppb=&Patch.GetNext(pos);
				if(ppb->m_NewData==NULL || ppb->m_OldData==NULL)
				{
					AfxMessageBox(_T("RepairPatch save error: missing old or new data"));
					AfxThrowUserException();
				}
				patch+=ppb->MakeTextLine(VPB_MAKETXT_PHONE_DATA, VPB_MAKETXT_NEW_DATA, VPB_MAKETXT_OLD_DATA,
					VPB_TYPE_EQUAL, VPB_MAKETXT_PHONE_DATA, VPB_MAKETXT_OLD_DATA);
			}
		}
		patch+=_T("\r\n#pragma enable warn_if_old_exist_on_undo\r\n");

		BYTE *buff;
		UINT64 fsize;
		ConvertStringToDataForPatchFile(patch, &buff, &fsize);
		if(buff!=NULL)
		{
			f.Write(buff, (UINT)fsize);
			delete buff;
		}
		f.Close();
		bOk=TRUE;
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
	}
	END_CATCH_ALL;
	if(bOk && pProgress)
		pProgress->SetStatus(strWork+strWorkOk);
	return bOk;
}

// Place a semicolon before each lines in text, if this line do not have a semicolon at his begin
BOOL CPatchPage::CommentAllLinesOfText(CString& strText)
{
	int linebeg;
	for(int i=0; i<strText.GetLength();)
	{
		linebeg=i;
		for(; strText[i]==_TCHAR('\t') || strText[i]==_TCHAR(' ') && i<strText.GetLength(); i++);
		if(i>=strText.GetLength())
			break;
		if(strText[i]!=_TCHAR(';')/* && !v_istspace(strText[i])*/)
			strText.Insert(linebeg, _TCHAR(';'));
		for(; strText[i]!=_TCHAR('\n') && strText[i]!=_TCHAR('\r') && i<strText.GetLength(); i++);
		if(++i >= strText.GetLength())
			break;
		if(strText[i-1]==_TCHAR('\n'))
		{
			if(strText[i]==_TCHAR('\r'))
				i++;
		}
		else
		{
			if(strText[i]==_TCHAR('\n'))
				i++;
		}
	}
	return TRUE;
}

void CPatchPage::OnPatchDataFind()
{
	FRStart(TRUE);
}

void CPatchPage::OnPatchDataReplace()
{
	FRStart(FALSE);
}

LPFRHOOKPROC FROldHookProcPtr=NULL;

UINT_PTR CALLBACK FRHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	UINT_PTR ret=FROldHookProcPtr(hdlg, uiMsg, wParam, lParam);
	if(uiMsg==WM_INITDIALOG)
		return FALSE;
	return ret;
}

BOOL CPatchPage::FRStart(BOOL bOnlyFind)
{
	if(m_pFRDlg!=NULL)
		return FALSE;
	m_pFRDlg = new CFindReplaceDialog();  // Must be created on the heap
	m_pFRDlg->m_fr.Flags|=FR_ENABLEHOOK;
	FROldHookProcPtr=m_pFRDlg->m_fr.lpfnHook;
	m_pFRDlg->m_fr.lpfnHook=FRHookProc;
	m_pFRDlg->m_fr.Flags|=FR_ENABLETEMPLATE;
	m_pFRDlg->m_fr.hInstance=AfxGetInstanceHandle();
	m_pFRDlg->m_fr.lpTemplateName=MAKEINTRESOURCE(bOnlyFind ? IDD_FINDDLG : IDD_REPLACEDLG);
	m_pFRDlg->Create(bOnlyFind, m_FRLastFindStr, m_FRLastReplaceStr, FR_DOWN, this);
	CRect rf, rp;
	m_pFRDlg->GetWindowRect(rf);
	m_PatchDataCtrl.GetWindowRect(rp);
	rf.OffsetRect(0, -(rf.bottom-rp.top));
	m_pFRDlg->MoveWindow(rf);
	m_pFRDlg->SetWindowText(bOnlyFind ?
		MSTR("Find") :
		MSTR("Replace"));
	m_pFRDlg->SetDlgItemText(stc1, MSTR("Fi&nd what:"));
	m_pFRDlg->SetDlgItemText(stc2, MSTR("Re&place with:"));
	m_pFRDlg->SetDlgItemText(chx1, MSTR("Match &whole word only"));
	m_pFRDlg->SetDlgItemText(chx2, MSTR("Match &case"));
	m_pFRDlg->SetDlgItemText(grp1, MSTR("Direction"));
	m_pFRDlg->SetDlgItemText(rad1, MSTR("&Up"));
	m_pFRDlg->SetDlgItemText(rad2, MSTR("&Down"));
	m_pFRDlg->SetDlgItemText(IDOK, MSTR("&Find Next"));
	m_pFRDlg->SetDlgItemText(psh1, MSTR("&Replace"));
	m_pFRDlg->SetDlgItemText(psh2, MSTR("Replace &All"));
	m_pFRDlg->SetDlgItemText(IDCANCEL, MSTR("Cancel"));
	m_pFRDlg->SetDlgItemText(pshHelp, MSTR("&Help"));

	m_pFRDlg->ShowWindow(SW_SHOWNORMAL); 
	m_pFRDlg->UpdateWindow();

	m_PatchDataCtrl.GetSel(m_FRRange);
	m_FRCount=0;
	return TRUE;
}

long CPatchPage::FRFindNext(CFindReplaceDialog* pDlg, BOOL bForReplace/*=FALSE*/)
{
	FINDTEXTEX ft;
	m_PatchDataCtrl.GetSel(ft.chrg);
	ft.lpstrText=m_FRLastFindStr;
	DWORD flags=0;
	if(pDlg->MatchCase())
		flags|=FR_MATCHCASE;
	if(pDlg->MatchWholeWord())
		flags|=FR_WHOLEWORD;
	if(pDlg->SearchDown())
	{
		if(!bForReplace)
			ft.chrg.cpMin=ft.chrg.cpMax;
		flags|=FR_DOWN;
		ft.chrg.cpMax=-1;
	}
	else
	{
		flags&=~FR_DOWN;
		ft.chrg.cpMax=0;
	}
	long pos;
#ifdef UNICODE
	pos= (long)m_PatchDataCtrl.SendMessage(EM_FINDTEXTEXW, flags, (LPARAM)&ft);
#else
	pos= (long)m_PatchDataCtrl.SendMessage(EM_FINDTEXTEX, flags, (LPARAM)&ft);
#endif
	if(pos!=-1)
	{
		m_PatchDataCtrl.SetSel(ft.chrgText);
		m_FRCount++;
	}
	return pos;
}

LRESULT CPatchPage::OnFindReplace(WPARAM wparam, LPARAM lparam)
{
	CFindReplaceDialog *pDlg = CFindReplaceDialog::GetNotifier(lparam);
	if(pDlg!=NULL)
	{
		m_FRLastFindStr=pDlg->GetFindString();
		m_FRLastReplaceStr=pDlg->GetReplaceString();
		if(pDlg->FindNext())
		{
			if(FRFindNext(pDlg)==-1)
				AfxMessageBox(MSTR("Next occurrence of string is not found."), MB_OK|MB_ICONINFORMATION);
		}
		else if(pDlg->ReplaceAll())
		{
			BOOL rep=FALSE;
			for(long pos=FRFindNext(pDlg, TRUE); pos!=-1; pos=FRFindNext(pDlg, TRUE))
			{
				m_PatchDataCtrl.ReplaceSel(m_FRLastReplaceStr, TRUE);
				rep=TRUE;
			}
			if(rep)
				AfxMessageBox(MSTR("Reached the end of text."), MB_OK|MB_ICONINFORMATION);
			else
				AfxMessageBox(MSTR("Next occurrence of string is not found."), MB_OK|MB_ICONINFORMATION);
		}
		else if(pDlg->ReplaceCurrent())
		{
			CHARRANGE cr;
			m_PatchDataCtrl.GetSel(cr);
			if(cr.cpMin!=cr.cpMax)
				if(FRFindNext(pDlg, TRUE)!=-1)
					m_PatchDataCtrl.ReplaceSel(m_FRLastReplaceStr, TRUE);
			if(FRFindNext(pDlg, TRUE)==-1)
				AfxMessageBox(MSTR("Next occurrence of string is not found."), MB_OK|MB_ICONINFORMATION);
		}
		else if(pDlg->IsTerminating())
		{
			m_pFRDlg=NULL;
			FROldHookProcPtr=NULL;
		}
	}
	return TRUE;
}

void CPatchPage::OnEnChangePatchData()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strDocFileName, m_PatchDataCtrl.GetModify());
}

BOOL CPatchPage::ModifyMenuString(CMenu* pPopup, UINT nItemID, LPCTSTR lpszNewItem/*=NULL*/)
{
	CString str;
	pPopup->GetMenuString(nItemID, str, MF_BYCOMMAND|MF_STRING);
	int i=str.ReverseFind(_TCHAR('\t'));
	if(i!=-1)
		lpszNewItem=str=CString(lpszNewItem)+str.Right(str.GetLength()-i);
	pPopup->ModifyMenu(nItemID, MF_BYCOMMAND|MF_STRING, nItemID, lpszNewItem);
/*	MENUITEMINFO mii;
	mii.cbSize=sizeof(mii);
	pPopup->GetMenuItemInfo(nItemID, &mii, FALSE);
	mii.fType|=MFT_RIGHTJUSTIFY;
	pPopup->SetMenuItemInfo(nItemID, &mii, FALSE);*/
	return TRUE;
}

void CPatchPage::OnPatchDataComment()
{
	AddRemoveStrAtBeginOfSelectedLinesOfPatchDataCtrl(TRUE, o_CommentGroupString);
}

void CPatchPage::OnPatchDataUncomment()
{
	AddRemoveStrAtBeginOfSelectedLinesOfPatchDataCtrl(FALSE, o_CommentGroupString);
}


BOOL CPatchPage::AddRemoveStrAtBeginOfSelectedLinesOfPatchDataCtrl(BOOL bAdd, LPCTSTR strIdent)
{
	int i, curlen, sbeg, single, len;
	if(strIdent==NULL)
		return TRUE;
	len=(int)_tcslen(strIdent);
	if(len==0)
		return TRUE;
	CString str;
	CHARRANGE cr;
	SETTEXTEX st;
	GETTEXTEX gt;
#ifdef _UNICODE
	st.codepage=gt.codepage=1200;
#else
	st.codepage=gt.codepage=CP_ACP;
#endif
	st.flags=ST_SELECTION|ST_KEEPUNDO;
	gt.flags=GT_SELECTION|GT_DEFAULT;
	m_PatchDataCtrl.GetSel(cr);
	single= cr.cpMin==cr.cpMax ? cr.cpMin : -1;
	cr.cpMin=m_PatchDataCtrl.LineIndex(m_PatchDataCtrl.LineFromChar(cr.cpMin));
	i=m_PatchDataCtrl.LineIndex(m_PatchDataCtrl.LineFromChar(cr.cpMax));
	if(i!=cr.cpMax || cr.cpMin==cr.cpMax)
		cr.cpMax=m_PatchDataCtrl.LineIndex(m_PatchDataCtrl.LineFromChar(cr.cpMax)+1);
	m_PatchDataCtrl.SetSel(cr);
	m_PatchDataCtrl.GetSel(cr);
	gt.cb=(cr.cpMax-cr.cpMin+1)*sizeof(_TCHAR);
	m_PatchDataCtrl.SendMessage(EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)str.GetBuffer(cr.cpMax-cr.cpMin+1));
	str.ReleaseBuffer(cr.cpMax-cr.cpMin);
	if(str.GetLength()!=0)
	{
		for(i=0, sbeg=cr.cpMin; str[i];)
		{
			if(bAdd)
			{
				m_PatchDataCtrl.SetSel(sbeg+i, sbeg+i);
				m_PatchDataCtrl.SendMessage(EM_SETTEXTEX, (WPARAM)&st, (LPARAM)strIdent);
				if(single!=-1)
					if(single>=sbeg+i)
						single+=len;
				sbeg+=len;
				cr.cpMax+=len;
			}
			else
			{
				for(; str[i]==_T(' ') || str[i]==_T('\t'); i++);
				if(str[i]==0)
					break;
				for(curlen=0; str[i+curlen] && curlen<len; curlen++);
				if(curlen<len)
					break;
				if(_tcsnccmp(((LPCTSTR)str)+i, strIdent, len)==0)
				{
					m_PatchDataCtrl.SetSel(sbeg+i, sbeg+i+len);
					m_PatchDataCtrl.SendMessage(EM_SETTEXTEX, (WPARAM)&st, (LPARAM)(_T("")));
					if(single!=-1)
						if(single>sbeg+i)
							if(single>sbeg+i+len)
								single-=len;
							else
								single=sbeg+i;
					sbeg-=len;
					cr.cpMax-=len;
				}
			}
			for(; str[i]!=_T('\n') && str[i]!=_T('\r') && str[i]!=0; i++);
			if(str[i]==0)
				break;
			i++;
		}
	}
	if(single!=-1)
		cr.cpMax=cr.cpMin=single;
	m_PatchDataCtrl.SetSel(cr);
	return TRUE;
}

BOOL CPatchPage::SaveCurPatchDir(LPCTSTR strDirOrFilePath)
{
	o_strPatchDir=GetFileDir(strDirOrFilePath);
	return TRUE;
}

CString CPatchPage::GetCurPatchDir(void)
{
	return o_strPatchDir;
}



