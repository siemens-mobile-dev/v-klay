// ConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "ConsoleDlg.h"
#include ".\consoledlg.h"


// CConsoleDlg dialog

IMPLEMENT_DYNAMIC(CConsoleDlg, CDialog)
CConsoleDlg::CConsoleDlg(CWnd* pParent /*=NULL*/, LPCTSTR pText/*=NULL*/, LPCTSTR pStatusText/*=NULL*/, LPCTSTR pCaption/*=NULL*/)
	: CDialog(CConsoleDlg::IDD, pParent)
{
	m_Text=pText;
	m_StatusText=pStatusText;
	m_Caption=pCaption;
}

CConsoleDlg::~CConsoleDlg()
{
}

void CConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConsoleDlg, CDialog)
	ON_EN_SETFOCUS(IDC_TEXT, OnEnSetfocusText)
END_MESSAGE_MAP()


// CConsoleDlg message handlers

BOOL CConsoleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(!m_Caption.IsEmpty())
		SetWindowText(m_Caption);

	SetText(m_Text);
	SetStatusText(m_StatusText);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// Set text in the console
BOOL CConsoleDlg::SetText(LPCTSTR strText)
{
	m_Text=strText;
	CWnd *pwnd=GetDlgItem(IDC_TEXT);
	if(pwnd!=NULL)
		pwnd->SetWindowText(strText);
	return TRUE;
}

// Set text what is shown above console window
BOOL CConsoleDlg::SetStatusText(LPCTSTR strStatusText)
{
	m_StatusText=strStatusText;
	CWnd *pwnd=GetDlgItem(IDC_STATUS_TEXT);
	if(pwnd!=NULL)
		pwnd->SetWindowText(strStatusText);
	return TRUE;
}

void CConsoleDlg::OnEnSetfocusText()
{
	static BOOL isCleared=FALSE;
	if(!isCleared)
	{
		CEdit *pwnd=(CEdit*)GetDlgItem(IDC_TEXT);
		if(pwnd!=NULL)
			pwnd->SetSel(0,0);
		isCleared=TRUE;
	}
}
