// ProgProgressDlg.h : header file
// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CProgProgressDlg dialog

#pragma once

extern BOOL CProgressDlg_IsPumpingMessages;


class CProgressDlg : public CDialog
{
// Construction / Destruction
public:
	BOOL m_bAbort;
	CString GetStatus();
	INT64 SetUpper(INT64 nUpper);
	INT64 ModifyUpper(INT64 offset);
	INT64 GetStep();
    CProgressDlg(LPCTSTR strCaption = NULL, CWnd *pParent=NULL);   // standard constructor
    ~CProgressDlg();


    // Checking for Cancel button
    BOOL CheckCancelButton();
    // Progress Dialog manipulation
    void SetStatus(LPCTSTR lpszMessage);
    void SetStatus(UINT nMessageID);
    void SetRange(INT64 nLower, INT64 nUpper);
    INT64 SetStep(INT64 nStep);
    INT64 SetPos(INT64 nPos);
    INT64 OffsetPos(INT64 nPos);
    INT64 StepIt();
	BOOL m_bUnknownUpper;
    void PumpMessages();

        
// Dialog Data
    //{{AFX_DATA(CProgProgressDlg)
    enum { IDD = CG_IDD_PROGRESS };
    //}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    CProgressCtrl	m_Progress;
    BOOL Create(CWnd *pParent=NULL);
	LPCTSTR m_strCaption;
    INT64 m_nLower;
    INT64 m_nUpper;
    INT64 m_nStep;
	INT64 m_nPos;
    
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

	UINT_PTR m_TimerID;
	BOOL m_bMakeStepItInTimer;

	UINT_PTR m_TimerIDSpeed;
	UINT_PTR m_TimerIDPerf;
	INT64 m_nOldPos;
	int m_CurSpeed;
	BOOL m_IsMustCalcSpeed;
	DWORD m_StartTime;
	DWORD m_StopTime;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 

    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
	BOOL m_IsShowSpeed;
	BOOL m_bFlashWindow;
	int m_bFlashWindowCount;
	afx_msg void OnTimer(UINT nIDEvent);
	// Make a string with info about work time and average speed
	CString MakePerformanceReport(BOOL bAddNewLinesBefore=TRUE);
	BOOL BeginPerformanceMeasure(void);
	BOOL EndPerformanceMeasure(void);
	BOOL ShowPerformanceInfo(BOOL bShow=TRUE);
};

