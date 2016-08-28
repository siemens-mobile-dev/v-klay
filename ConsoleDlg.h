#pragma once


// CConsoleDlg dialog

class CConsoleDlg : public CDialog
{
	DECLARE_DYNAMIC(CConsoleDlg)

public:
	CConsoleDlg(CWnd* pParent = NULL, LPCTSTR pText=NULL, LPCTSTR pStatusText=NULL, LPCTSTR pCaption=NULL);   // standard constructor
	virtual ~CConsoleDlg();

// Dialog Data
	enum { IDD = IDD_CONSOLE_DIALOG };

protected:
	CString m_Text;
	CString m_StatusText;
	CString m_Caption;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// Set text in the console
	BOOL SetText(LPCTSTR strText);
	// Set text what is shown above console window
	BOOL SetStatusText(LPCTSTR strStatusText);
	afx_msg void OnEnSetfocusText();
};
