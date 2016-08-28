#pragma once

#include "PPDumpCtrl.h"
#include "vdynctrlsize.h"

// CHexViewDlg dialog

class CHexViewDlg : public CDialog
{
	DECLARE_DYNAMIC(CHexViewDlg)

public:
	BYTE* m_pData;
	INT64 m_DataSize;
	INT64 m_DataAddress;
	BOOL m_IsModified;


	CHexViewDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHexViewDlg();

// Dialog Data
	enum { IDD = IDD_HEX_VIEW_DIALOG };

protected:
	HICON m_hIcon;
	CPPDumpCtrl * m_HexCtrl;
	VDynCtrlSize m_DynSize;
	CPoint m_MinSize;
	CFont m_ToolTipFont;

protected:
	virtual void OnCancel();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void NotifyEditData(NMHDR * pNMHDR, LRESULT * result);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
