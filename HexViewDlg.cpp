// HexViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "HexViewDlg.h"
#include ".\hexviewdlg.h"


// CHexViewDlg dialog

IMPLEMENT_DYNAMIC(CHexViewDlg, CDialog)
CHexViewDlg::CHexViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHexViewDlg::IDD, pParent)
	, m_MinSize(0,0)
{
	m_DynSize.AddControl(2, EL(0), EL(0), EL(100), EL(100));

	m_hIcon = AfxGetApp()->LoadIcon(IDI_VKLAY);
	m_HexCtrl = NULL;
	m_pData = NULL;
	m_DataSize = 0;
	m_DataAddress = 0;
	m_IsModified = FALSE;
}

CHexViewDlg::~CHexViewDlg()
{
}

void CHexViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHexViewDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(UDM_PPDUMPCTRL_CHANGE_DATA, 2, NotifyEditData)
END_MESSAGE_MAP()


// CHexViewDlg message handlers

BOOL CHexViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect r;
	CEdit * pEdit = (CEdit*)GetDlgItem(IDC_HEX_EDIT);
	pEdit->GetWindowRect(&r);
	ScreenToClient(&r);
	pEdit->ShowWindow(FALSE);

	m_HexCtrl = new CPPDumpCtrl;
	m_HexCtrl->Create(r, this, 2);
	m_HexCtrl->SetPointerData((DWORD)m_DataSize, m_pData);
	m_HexCtrl->SetOffsetViewAddress((DWORD)m_DataAddress);
	m_HexCtrl->SetNotify();
	//Customize the tooltip
	CString str = _T("Address: %R Hex%n-------- Data ---------%nHex:%t%H%nDec:%t%D%nOct:%t%O%nBin:%t%B%nAscii:%t%A");
	m_HexCtrl->SetTooltipText(str);
	CToolTipCtrl * tooltip = m_HexCtrl->GetTooltip();
	m_ToolTipFont.CreateFont(-11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, _T("Courier"));
	tooltip->SetFont(&m_ToolTipFont);
	tooltip->SetDelayTime(TTDT_AUTOPOP, 32000);

	GetWindowRect(&r);
	m_MinSize.SetPoint(r.Size().cx, r.Size().cy);



	// Set the icon for this dialog.
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_DynSize.AttachDialog(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CHexViewDlg::DestroyWindow()
{
	if(m_HexCtrl)
		delete m_HexCtrl;
	m_HexCtrl = NULL;
	return CDialog::DestroyWindow();
}

void CHexViewDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CHexViewDlg::OnOK()
{
//	CDialog::OnOK();
}

void CHexViewDlg::OnSize(UINT nType, int cx, int cy)
{
	m_DynSize.OnSize(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

void CHexViewDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize=m_MinSize;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CHexViewDlg::NotifyEditData(NMHDR * pNMHDR, LRESULT * result)
{
	//User was changed data
	*result = 0;
	NM_PPDUMP_CTRL * pNotify = (NM_PPDUMP_CTRL*)pNMHDR;
	if(pNotify->iAddress < m_DataSize && m_pData != NULL)
	{
		m_pData[pNotify->iAddress] = LOBYTE(pNotify->iValue);
		m_IsModified = TRUE;
		m_HexCtrl->RedrawWindow();
	}
}

