#pragma once


// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)

public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();

// Dialog Data
	enum { IDD = IDD_OPTIONS_DIALOG };

protected:
	CString m_strSystemDefault;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bDisableWarningsMsg;
	BOOL m_bDisableRTFSaveWarningsMsg;
	BOOL m_bIsPatcherSwapPasteKeys;
	CString m_strLocale;
	virtual BOOL OnInitDialog();
	CComboBox m_LangCtrl;
	CString m_strLanguage;
	int m_PatcherFontSize;
	afx_msg void OnCbnSelchangeLang();
	afx_msg void OnBnClickedLayoutExtlayPathBrowse();
	BOOL BrowseForToolPath(CEdit* pathEditBox);
	afx_msg void OnBnClickedToolExtpicPathBrowse();
	afx_msg void OnBnClickedToolExt1PathBrowse();
	afx_msg void OnBnClickedToolExt2PathBrowse();
	afx_msg void OnBnClickedToolExt3PathBrowse();
	afx_msg void OnBnClickedToolExtfntPathBrowse();
	afx_msg void OnBnClickedOptionsAdvanced();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedToolTmpPathBrowse();
	afx_msg void OnBnClickedSetToolArg(UINT nId);
};
