#pragma once


// COptionsAdvancedDlg dialog

class COptionsAdvancedDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsAdvancedDlg)

public:
	COptionsAdvancedDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsAdvancedDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS_ADVANCED_DIALOG };
	BOOL m_bCommSimple;
	int m_AutoignitionType;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL m_bAutoignDisable;
	int m_AutoignType;
	void SetAutoignCtrlsState();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAutoignEnable();
	BOOL m_bIsRepairPatchCanSkip;
	BOOL m_bIsPatchIgnoreNoOld;
	BOOL m_bIsRepairPatchTextSimple;
	BOOL m_bCommRTS;
	BOOL m_bCommDTR;
	BOOL m_bIsEnableBootcoreWritting;
};
