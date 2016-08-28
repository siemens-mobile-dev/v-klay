#pragma once
#include "afxwin.h"
#include "vdynctrlsize.h"


// CFlasherPage dialog

class CFlasherPage : public CDialog
{
	DECLARE_DYNAMIC(CFlasherPage)

public:
	CFlasherPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlasherPage();

// Dialog Data
	enum { IDD = IDD_FLASHER_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	afx_msg LRESULT OnIsCanClose(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg LRESULT OnInitialUpdate(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg LRESULT OnDeviceChanged(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	CToolTipCtrl *m_pToolTipCtrl;
	virtual BOOL OnInitDialog();
	CComboBox m_FromCtrl;
	CComboBox m_SizeCtrl;
	CComboBox m_BufferOffsetCtrl;
	CEdit m_BufferSizeCtrl;
	CEdit m_BufferLastFromCtrl;
	CString m_strFileName;
	INT64 m_From;
	INT64 m_Size;
	INT64 m_BufferOffset;
	BOOL m_IsModified;
	BYTE* m_pBuffer;
	INT64 m_BufferSize;
	INT64 m_BufferLastFrom;
	BOOL SaveModifiedDocument(void);
	BOOL SaveDocument(void);
	afx_msg void OnBnClickedMemoryLoad();
	afx_msg void OnBnClickedMemoryWrite();
	afx_msg void OnBnClickedFileLoad();
	afx_msg void OnBnClickedFileWrite();
protected:
	virtual void OnOK();
public:
	BOOL OpenDocument(LPCTSTR strDocFileName);
	VDynCtrlSize m_DynSize;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CString GetDefaultFileName(void);
	afx_msg void OnCbnSelendokFlasherFrom();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedFlasherMemoryRestoreBootcore();
	afx_msg void OnBnClickedFlasherMemoryMap();
	afx_msg void OnBnClickedBufferView();
};
