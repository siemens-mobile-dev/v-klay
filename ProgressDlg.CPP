//  ProgProgressDlg.cpp : implementation file
// CG: This file was added by the Progress Dialog component

#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "V_KLay.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define DPOS(lp) ((short)((m_nUpper-m_nLower)?((((__int64)(lp))-m_nLower)*(((__int64)16383))/(m_nUpper-m_nLower)):0))
#define DSTEP(ls) ((short)((m_nUpper-m_nLower)?(((__int64)(ls))*((__int64)16383)/(m_nUpper-m_nLower)):0))



BOOL CProgressDlg_IsPumpingMessages=FALSE;

/////////////////////////////////////////////////////////////////////////////
// CProgProgressDlg dialog

CProgressDlg::CProgressDlg(LPCTSTR strCaption, CWnd *pParent)
{
	m_strCaption = strCaption;

    m_bCancel=FALSE;
	m_bAbort=FALSE;
    m_nPos=m_nLower=0;
    m_nUpper=1;
	m_bUnknownUpper=TRUE;
	m_bMakeStepItInTimer=FALSE;
    m_nStep=0;
	m_TimerID=0;
	m_bFlashWindow=FALSE;
	m_bFlashWindowCount=0;
    //{{AFX_DATA_INIT(CProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;

	m_StartTime=0;
	m_StopTime=0;
	m_TimerIDSpeed=0;
	m_nOldPos=-1;
	m_CurSpeed=0;
	m_IsShowSpeed=FALSE;
	m_IsMustCalcSpeed=FALSE;
	Create(pParent);
}

CProgressDlg::~CProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
}

BOOL CProgressDlg::DestroyWindow()
{
	if(m_StopTime==0)
		EndPerformanceMeasure();
	if(pProgress==this)
		pProgress=NULL;
    ReEnableParent();
	if(m_TimerID!=0)
		KillTimer(m_TimerID);
	m_TimerID=0;
	if(m_TimerIDSpeed!=0)
		KillTimer(m_TimerIDSpeed);
	m_TimerIDSpeed=0;
	if(m_TimerIDPerf!=0)
		KillTimer(m_TimerIDPerf);
	m_TimerIDPerf=0;
    return CDialog::DestroyWindow();
}

void CProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CProgressDlg::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

	if(pProgress==NULL)
		pProgress=this;

	m_TimerID=SetTimer(61, 50, NULL);
	m_TimerIDSpeed=SetTimer(62, 2000, NULL);
	m_TimerIDPerf=SetTimer(63, 200, NULL);

    return TRUE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressDlg)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CProgressDlg)
    //}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CProgressDlg::SetStatus(LPCTSTR lpszMessage)
{
	if(m_bCancel)
		return;
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(CG_IDC_PROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
	//CWnd *pwnd=GetDlgItem(IDCANCEL);
	//if(pwnd!=NULL)
	//	pwnd->RedrawWindow();
	PumpMessages();
}

void CProgressDlg::SetStatus(UINT nMessageID)
{
	CString str;
	str.LoadString(nMessageID);
	SetStatus((LPCTSTR)str);
}

void CProgressDlg::OnCancel()
{
    m_bCancel=FALSE;
	SetStatus(MSTR("Cancel in progress, wait please...   Shift + Cancel - abort immediately."));
    m_bCancel=TRUE;
	if((GetKeyState(VK_SHIFT)&SHIFTED)
		&& !(GetKeyState(VK_CONTROL)&SHIFTED)
		&& !(GetKeyState(VK_MENU)&SHIFTED))
		m_bAbort=TRUE;
}

void CProgressDlg::SetRange(INT64 nLower, INT64 nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
	m_bMakeStepItInTimer=FALSE;
	m_Progress.SetPos(m_bUnknownUpper ? DPOS(0) : DPOS(m_nPos));
	m_bUnknownUpper=FALSE;

	BeginPerformanceMeasure();
}

BOOL CProgressDlg::BeginPerformanceMeasure(void)
{
	m_StartTime=::GetTickCount();
	m_StopTime=0;
	m_nOldPos=m_nPos;
	m_CurSpeed=0;
	m_IsMustCalcSpeed=FALSE;
	return TRUE;
}

BOOL CProgressDlg::EndPerformanceMeasure(void)
{
	if(m_StopTime==0)
		m_StopTime=::GetTickCount();
	return TRUE;
}
  
INT64 CProgressDlg::SetPos(INT64 nPos)
{
	INT64 old=m_nPos;
    PumpMessages();
	m_nPos=nPos;
	if(m_StartTime==0 && m_nPos!=0)
		BeginPerformanceMeasure();
	if(m_bUnknownUpper)
	{
		m_bMakeStepItInTimer=TRUE;
		return old;
	}
	if(m_nPos>m_nUpper)
		m_nPos=m_bUnknownUpper ? 0 : m_nUpper;
    m_Progress.SetPos(DPOS(m_nPos));
	return old;
}

INT64 CProgressDlg::SetStep(INT64 nStep)
{
	INT64 old=m_nStep;
    m_nStep = nStep; // Store for later use in calculating percentage
	return old;
}

INT64 CProgressDlg::OffsetPos(INT64 nPos)
{
	return SetPos(m_nPos+nPos);
}

INT64 CProgressDlg::StepIt()
{
	return OffsetPos(m_nStep);
}

void CProgressDlg::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
	CProgressDlg_IsPumpingMessages++;
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
	if(CProgressDlg_IsPumpingMessages)
		CProgressDlg_IsPumpingMessages--;
}

BOOL CProgressDlg::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}

    
/////////////////////////////////////////////////////////////////////////////
// CProgProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog() 
{
	SetDlgItemText(CG_IDC_PROGDLG_STATUS, MSTR("Please wait..."));
	SetDlgItemText(IDCANCEL, MSTR("Cancel"));

    CDialog::OnInitDialog();
    m_Progress.SetRange(DPOS(m_nLower), DPOS(m_nUpper));
    m_Progress.SetPos(DPOS(m_nLower));
	m_nPos=m_nLower;

	if(m_strCaption)
	{
//		CString strCaption;
//		VERIFY(strCaption.LoadString(m_nCaptionID));
		SetWindowText(m_strCaption);
	}

    return TRUE;  
}

INT64 CProgressDlg::GetStep()
{
	return m_nStep;
}

INT64 CProgressDlg::ModifyUpper(INT64 offset)
{
	INT64 old=m_nUpper;
	SetRange(m_nLower, m_nUpper+offset);
	return old;
}

INT64 CProgressDlg::SetUpper(INT64 nUpper)
{
	INT64 old=m_nUpper;
	SetRange(m_nLower, nUpper);
	return old;
}

CString CProgressDlg::GetStatus()
{
	CString str;
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(CG_IDC_PROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
	{
		pWndStatus->GetWindowText(str);
	}
	return str;
}

void CProgressDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==62)
	{
		m_IsMustCalcSpeed=TRUE;
	}
	else if(nIDEvent==61)
	{
		if(m_bMakeStepItInTimer)
		{
			m_bMakeStepItInTimer=FALSE;
			int pos=m_Progress.GetPos();
			pos+=16383/50;
			if(pos>16383)
				pos=0;
			m_Progress.SetPos(pos);
		}
		if(m_bFlashWindow)
		{
			if(m_bFlashWindowCount==0)
			{
				m_bFlashWindowCount=30;
				SetForeground(this);
			}
			FlashWindow(TRUE);
			if((--m_bFlashWindowCount)==0)
				m_bFlashWindow=FALSE;
		}
	}
	else if(nIDEvent==63)
	{
		if(m_IsShowSpeed && m_StartTime!=0)
		{
			CString title, snum, str, stmp;
			DWORD eltime=::GetTickCount()-m_StartTime;

			if(m_IsMustCalcSpeed)
			{
				if(m_StartTime!=0 && m_nOldPos!=m_nPos)
				{
					m_IsMustCalcSpeed=FALSE;
					m_CurSpeed=(int)(((INT64)(m_nPos))*1000L/eltime);
					m_nOldPos=m_nPos;
				}
			}

			int sp=m_CurSpeed;
			if(sp==0 && eltime!=0)
				sp=(int)(((INT64)(m_nPos))*1000L/eltime);
			snum.Format(_T("%#1.1f"), sp/1024.0);
			str.Format(MSTR("Speed: %s KB/s"), snum);
			title+=str;

			snum.Format(_T("%i:%2.2i:%2.2i"), eltime/(1000*60*60), eltime/(1000*60)%60, eltime/1000%60);
			str.Format(MSTR("Time elapsed: %s"), snum);
			title+=_T("  ");
			title+=str;

			if(!m_bUnknownUpper)
			{
				if(m_CurSpeed!=0)
				{
					DWORD lefttime=(DWORD)(((INT64)m_nUpper)/m_CurSpeed);
					if(lefttime > eltime/1000)
						lefttime-=eltime/1000;
					else
						lefttime=0;
					snum.Format(_T("%i:%2.2i:%2.2i"), lefttime/(60*60), lefttime/60%60, lefttime%60);
				}
				else
					snum=_T("-:--:--");
				str.Format(MSTR("left: %s"), snum);
				title+=_T("  ");
				title+=str;
			}

			SetWindowText(title);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

// Make a string with info about work time and average speed
CString CProgressDlg::MakePerformanceReport(BOOL bAddNewLinesBefore/*=TRUE*/)
{
	CString per;
	if(m_StopTime==0)
		EndPerformanceMeasure();
	if(m_StartTime!=0)
	{
		DWORD eltime=m_StopTime-m_StartTime;
		CString snum, stmp;
		if(eltime!=0)
		{
			if(bAddNewLinesBefore)
				per=_T("\n\n");
			per+=MSTR("Time:");
			if(eltime>=1000)
				snum.Format(_T(" %i:%2.2i:%2.2i"), eltime/(1000*60*60), eltime/(1000*60)%60, eltime/1000%60);
			else
				snum.Format(_T(" %g"), eltime/1000.0);
			per+=snum;
			per+=_TCHAR('\n');
			stmp.Format(_T("%#1.1f"), (double)((INT64)m_nPos*1000/eltime)/1024.0);
			snum.Format(MSTR("Speed: %s KB/s"), stmp);
			per+=snum;
		}
		if(pDevice!=NULL)
		{
			if(pDevice->m_ErrorsCorrectedCount!=0)
			{
				per+=_TCHAR('\n');
				stmp.Format(uintformat, pDevice->m_ErrorsCorrectedCount);
				snum.Format(MSTR("Errors corrected: %s"), stmp);
				per+=snum;
			}
		}
	}
	return per;
}


BOOL CProgressDlg::ShowPerformanceInfo(BOOL bShow/*=TRUE*/)
{
	m_IsShowSpeed=bShow;
	if(!m_IsShowSpeed)
		if(m_hWnd!=NULL)
			SetWindowText(AfxGetApp()->m_pszAppName);
	return TRUE;
}
