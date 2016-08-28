// OptionsAdvancedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "OptionsAdvancedDlg.h"


// COptionsAdvancedDlg dialog

IMPLEMENT_DYNAMIC(COptionsAdvancedDlg, CDialog)
COptionsAdvancedDlg::COptionsAdvancedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsAdvancedDlg::IDD, pParent)
	, m_bAutoignDisable(FALSE)
	, m_AutoignType(0)
	, m_bIsRepairPatchCanSkip(FALSE)
	, m_bIsPatchIgnoreNoOld(FALSE)
	, m_bCommRTS(FALSE)
	, m_bCommDTR(FALSE)
	, m_bIsEnableBootcoreWritting(FALSE)
{
	m_AutoignitionType=AUTOIGN_AUTO;
	m_bCommSimple=FALSE;
	m_bIsRepairPatchTextSimple=FALSE;
}

COptionsAdvancedDlg::~COptionsAdvancedDlg()
{
}

void COptionsAdvancedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ADVUSER_IGNORE_CANCEL_REPAIR_PATCH, m_bIsRepairPatchCanSkip);
	DDX_Check(pDX, IDC_ADVUSER_IGNORE_NO_OLD, m_bIsPatchIgnoreNoOld);
	DDX_Check(pDX, IDC_COM_SIMPLE_MODE, m_bCommSimple);
	DDX_Check(pDX, IDC_COM_RTS, m_bCommRTS);
	DDX_Check(pDX, IDC_COM_DTR, m_bCommDTR);
	DDX_Check(pDX, IDC_ADVUSER_REPAIR_PATCH_TEXT_SIMPLE, m_bIsRepairPatchTextSimple);
	DDX_Check(pDX, IDC_ADVUSER_ENBLE_BOOTCORE_WRITING, m_bIsEnableBootcoreWritting);
	if(!pDX->m_bSaveAndValidate)
	{
		if(m_AutoignitionType==AUTOIGN_NONE)
		{
			m_bAutoignDisable=TRUE;
			m_AutoignType=0;
		}
		else
		{
			m_bAutoignDisable=FALSE;
			m_AutoignType=m_AutoignitionType;
		}
		DDX_Check(pDX, IDC_AUTOIGN_ENABLE, m_bAutoignDisable);
		DDX_Radio(pDX, IDC_AUTOIGN_AUTO, m_AutoignType);
	}
	else
	{
		DDX_Check(pDX, IDC_AUTOIGN_ENABLE, m_bAutoignDisable);
		DDX_Radio(pDX, IDC_AUTOIGN_AUTO, m_AutoignType);
		if(!m_bAutoignDisable)
			m_AutoignitionType=m_AutoignType;
		else
			m_AutoignitionType=AUTOIGN_NONE;
	}
}


BEGIN_MESSAGE_MAP(COptionsAdvancedDlg, CDialog)
	ON_BN_CLICKED(IDC_AUTOIGN_ENABLE, OnBnClickedAutoignEnable)
END_MESSAGE_MAP()


// COptionsAdvancedDlg message handlers

void COptionsAdvancedDlg::SetAutoignCtrlsState()
{
	BOOL bEnable=!((CButton*)GetDlgItem(IDC_AUTOIGN_ENABLE))->GetCheck();
	GetDlgItem(IDC_AUTOIGN_AUTO)->EnableWindow(bEnable);
	GetDlgItem(IDC_AUTOIGN_SIMPL)->EnableWindow(bEnable);
	GetDlgItem(IDC_AUTOIGN_ORIGSIE)->EnableWindow(bEnable);
	GetDlgItem(IDC_AUTOIGN_TYPE_TEXT)->EnableWindow(bEnable);
}

BOOL COptionsAdvancedDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(MSTR("Advanced options..."));
	SetDlgItemText(IDOK, MSTR("OK"));
	SetDlgItemText(IDCANCEL, MSTR("Cancel"));
	SetDlgItemText(IDC_COMPATIBILITY_STATIC, IdentStr(MSTR("Compatibility options:")));
	SetDlgItemText(IDC_COMPATIBILITY_WARN, IdentStr(MSTR("WARNING!  Changing this options may limiting some functionality of program or lead to stop responding of program in some situation.\n Try to change this options ONLY if program is not working with its default values."), 3));
	SetDlgItemText(IDC_COM_SIMPLE_MODE, MSTR("Work with COM port in simple mode (NON OVERLAPPED)"));
	SetDlgItemText(IDC_COM_RTS, MSTR("RTS"));
	SetDlgItemText(IDC_COM_DTR, MSTR("DTR"));
	SetDlgItemText(IDC_AUTOIGN_ENABLE, MSTR("Disable Autoignition"));
	SetDlgItemText(IDC_AUTOIGN_TYPE_TEXT, MSTR("Autoignition Type"));
	SetDlgItemText(IDC_AUTOIGN_AUTO, MSTR("Autodetect"));
	SetDlgItemText(IDC_AUTOIGN_SIMPL, MSTR("Simple (at begin set +12V on DTR)"));
	SetDlgItemText(IDC_AUTOIGN_ORIGSIE, MSTR("Original Siemens (on DTR: 2 sec +12V, -12V (ignition), +12V)"));

	SetDlgItemText(IDC_ADVUSER_STATIC, IdentStr(MSTR("For advanced users:")));
	SetDlgItemText(IDC_ADVUSER_IGNORE_CANCEL_REPAIR_PATCH, MSTR("Do not abort operation on 'Cancel' in 'Save Repair Patch As...' dialog."));
	SetDlgItemText(IDC_ADVUSER_IGNORE_NO_OLD, MSTR("Do not check existence of old (when apply patch) or new (when undo) data in the phone. Ignore all errors!"));
	SetDlgItemText(IDC_ADVUSER_REPAIR_PATCH_TEXT_SIMPLE, MSTR("Generate minimum amount of comments in repair patch."));
	SetDlgItemText(IDC_ADVUSER_ENBLE_BOOTCORE_WRITING, MSTR("Enable writing in bootcore of phones."));


	SetAutoignCtrlsState();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsAdvancedDlg::OnBnClickedAutoignEnable()
{
	SetAutoignCtrlsState();
}
