// VDynCtrlSize.cpp: implementation of the VDynCtrlSize class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VDynCtrlSize.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VDynCtrlSize::VDynCtrlSize()
	: m_OrigSize(0,0)
{
	m_pDlg=NULL;
}

VDynCtrlSize::~VDynCtrlSize()
{

}

void VDynCtrlSize::AttachDialog(CWnd *pDialog)
{
	m_pDlg=pDialog;
	CRect r;
	m_pDlg->GetClientRect(r);
	m_OrigSize=r.Size();
	VDynCtrlSizeInf *inf;
	CWnd *wnd;
	for(int i=0; i<GetSize(); i++)
	{
		inf=&ElementAt(i);
		wnd=m_pDlg->GetDlgItem(inf->m_ID);
		if(wnd!=NULL)
		{
			wnd->GetWindowRect(inf->m_OrigPos);
			m_pDlg->ScreenToClient(inf->m_OrigPos);
		}
	}
}

int VDynCtrlSize::AddControl(UINT ID, BYTE ElLeft, BYTE ElTop, BYTE ElRight, BYTE ElBottom)
{
	VDynCtrlSizeInf inf;
	inf.m_ID=ID;
	inf.m_ElTop=ElTop;
	inf.m_ElLeft=ElLeft;
	inf.m_ElBottom=ElBottom;
	inf.m_ElRight=ElRight;
	return (int)Add(inf);
}

void VDynCtrlSize::OnSize(int newcx, int newcy)
{
	if(m_pDlg==NULL)
		return;
	VDynCtrlSizeInf *inf;
	CRect r;
	CWnd *wnd;
	for(int i=0; i<GetSize(); i++)
	{
		inf=&ElementAt(i);
		wnd=m_pDlg->GetDlgItem(inf->m_ID);
		if(wnd!=NULL)
		{
			r.left=inf->m_OrigPos.left+(newcx-m_OrigSize.cx)*inf->m_ElLeft/DS_ELASTANCE_MAX;
			r.right=inf->m_OrigPos.right+(newcx-m_OrigSize.cx)*inf->m_ElRight/DS_ELASTANCE_MAX;
			r.top=inf->m_OrigPos.top+(newcy-m_OrigSize.cy)*inf->m_ElTop/DS_ELASTANCE_MAX;
			r.bottom=inf->m_OrigPos.bottom+(newcy-m_OrigSize.cy)*inf->m_ElBottom/DS_ELASTANCE_MAX;
			wnd->MoveWindow(r, FALSE);
		}
	}
	m_pDlg->RedrawWindow();
}

BOOL VDynCtrlSize::RemoveControl(UINT ID)
{
	for(int i=0; i<GetSize(); i++)
	{
		if(ElementAt(i).m_ID==ID)
		{
			RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}
