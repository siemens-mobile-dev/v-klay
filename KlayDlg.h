// KLayDlg.h : header file
//

#pragma once
#include "vkey.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "vdynctrlsize.h"
#include "atltypes.h"
#include "staticcolor.h"
#include "jrtslinkctrl.h"


#define VKL_PATCH_PAGE		0
#define VKL_LAYOUT_PAGE		1
#define VKL_FLASHER_PAGE	2
#define VKL_PAGES_COUNT		3

#define VKL_LDR_LOADED_COLOR	RGB(255, 0, 0)
#define VKL_LDR_UNLOADED_COLOR	RGB(0, 255, 0)

#define VKL_LDR_STATE_HIDE	-2


// CKLayDlg dialog
class CKLayDlg : public CDialog
{
// Construction
public:
	CKLayDlg(CWnd* pParent = NULL);	// standard constructor
	~CKLayDlg();

// Dialog Data
	enum { IDD = IDD_V_KLAY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
	CToolTipCtrl *m_pToolTipCtrl;
	afx_msg void OnBnClickedAbout();
	afx_msg void OnBnClickedFlashBrowse();
	CDialog* pPage[VKL_PAGES_COUNT];
	BOOL GetFlashFileName(LPCTSTR strFNameToOpen=NULL);
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);
	CTabCtrl m_TabCtrl;
	int SetCurPage(int PageIdxToSet);
	BOOL UpdateMainTitle(BOOL bUpdate=FALSE, LPCTSTR lpstrFilePath=NULL, BOOL bModified=FALSE);
	afx_msg void OnBnClickedSelectPhone();
	afx_msg void OnBnClickedSelectFile();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedPhoneReset();
	CComboBox m_PhonesListCtrl;
	afx_msg void OnCbnSelchangePhonesList();
	afx_msg void OnBnClickedOptions();
	BOOL SendMessageToAllPages(UINT msg);
	afx_msg void OnBnClickedPhoneQueryInfo();
	VDynCtrlSize m_DynSize;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	CPoint m_MinSize;
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnEnKillfocusFlashFrom();
	afx_msg void OnEnKillfocusFlashSize();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedToolsOptions();
	// Set the icon and tooltip for Tool button
	BOOL SetToolButton(UINT idToolButton, LPCTSTR strPathRegKey, UINT idDefaultIcon, LPCTSTR strTTipFormat, LPCTSTR strDefaultTTipToolName=NULL);
	afx_msg void OnBnClickedToolExtlay();
	afx_msg void OnBnClickedToolExtpic();
	afx_msg void OnBnClickedToolExt1();
	afx_msg void OnBnClickedToolExt2();
	afx_msg void OnBnClickedToolExt3();
	// Run the external tool
	BOOL RunExternalTool(LPCTSTR strToolPathKey, LPCTSTR strToolArgKey);
	afx_msg void OnBnClickedToolExtfnt();
	CStaticColor m_LdrStateCtrl;
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnEnChangeFlashFrom();
//	afx_msg void OnEnChangeFlashSize();
//	BOOL UpdateFlashPartInf(BOOL bSaveAndValidate=FALSE);
//	afx_msg void OnEnChangeFlashFrom();
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// for updating dialog box controls 
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM);
	// Keep alive timer handler
	static void CALLBACK EXPORT KeepAliveTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime);

protected:
	HICON m_hIcon;
	int m_DeviceType;
	int m_iUpdateTimer;
	HACCEL m_hAccelerator;
	UINT_PTR m_hTimerKeepAlive;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	// Set current device type, that is selected in dialog
	BOOL SetCurDeviceType(int devType);
	virtual void OnOK();
	afx_msg LRESULT OnInitialUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSwitchPanel(void);
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	BOOL SelectPhone(int phoneNum=-2);
	CJRTSLinkCtrl m_DonateLinkCtrl;
	afx_msg void OnBnClickedDonateLink();
	afx_msg void OnActivatedDonateLink(NMHDR *pNotifyStruct, LRESULT *result );
};
