#pragma once
#include "vdynctrlsize.h"
#include "afxwin.h"

#define SYM_LISTS_COUNT		24

class CINIFileInfo
{
public:
	CString m_Name;
	CString m_Author;
	CString m_Comments;
	CString m_Phone;
	CString m_FWVersion;
	CString m_FilePath;
	CINIFileInfo& operator=(const CINIFileInfo& src);
};

typedef CArray<CINIFileInfo> CINIFileInfos;

class CINIPhoneFiles
{
public:
	CString m_Name;
	CINIFileInfos m_Files;

	CINIPhoneFiles& operator=(const CINIPhoneFiles& src);
};

typedef CArray<CINIPhoneFiles> CINIPhones;

// CLayoutPage dialog
class CLayoutPage : public CDialog
{
	DECLARE_DYNAMIC(CLayoutPage)

public:
	CLayoutPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayoutPage();

// Dialog Data
	enum { IDD = IDD_LAYOUT_PROPPAGE };

protected:
	BOOL m_IsModified;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString m_strDocFileName;
	VKey m_SymList[SYM_LISTS_COUNT];
	afx_msg void OnEnChangeKeySym(UINT nID);
	afx_msg void OnEnChangeKeySymCode(UINT nID);
	afx_msg void OnBnClickedKeysLoad();
	afx_msg void OnBnClickedKeysSave();
	afx_msg void OnBnClickedUpdate();
	BOOL SaveModifiedDocument(void);
	BOOL SaveDocument(void);
	afx_msg void OnBnClickedFlashLoad();
	afx_msg LRESULT OnDeviceChanged(WPARAM wTabIdx, LPARAM pTabCtrl);
	CToolTipCtrl *m_pToolTipCtrl;
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnIsCanClose(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg LRESULT OnInitialUpdate(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg LRESULT OnOptionsChanged(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	BOOL UpdateMainTitle(BOOL pUpdate=TRUE);
	afx_msg void OnBnClickedIniBrowse();
	BOOL OpenDocument(LPCTSTR strDocFileName);
	VDynCtrlSize m_DynSize;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CComboBox m_ININameCtrl;
	CComboBox m_INIPhoneCtrl;
	CINIPhones m_INIPhones;
	afx_msg void OnCbnSelchangeIniPhone();
	afx_msg void OnCbnSelchangeIniName();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
