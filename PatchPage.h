#pragma once
#include "afxwin.h"
#include "vdynctrlsize.h"
#include "ReDoc.h"


#define VPB_FLAG_DIS_WARN_APPLY_NO_OLD									0x01
#define VPB_FLAG_DIS_WARN_APPLY_OLD_NOT_FOUND_BUT_NEW_ALREADY_EXIST		0x02
#define VPB_FLAG_DIS_UNDO												0x04
#define VPB_FLAG_DIS_WARN_UNDO_NEW_NOT_FOUND_BUT_OLD_ALREADY_EXIST		0x08
#define VPB_FLAG_ENA_OLD_EQUAL_FF										0x10
#define VPB_FLAG_RUN													0x20
#define VPB_FLAG_RUN_IS_ON_APPLY										0x40

#define VPB_STAT_DO_NOT_WRITE											0x01

// PatchDataTest TestResultMask
#define VPB_PATCH_SAFE_TO_WRITE											0x0001
#define VPB_PATCH_REPAIR_SAVED											0x0002
#define VPB_PATCH_ALREADY_EXIST											0x0004
#define VPB_PATCH_EMPTY													0x0008
#define VPB_PATCH_SOME_OLD_DATA_NOT_EXIST								0x0010
#define VPB_PATCH_SOME_NEW_DATA_NOT_EXIST								0x0020
#define VPB_PATCH_UNDO_DISABLE_EXIST									0x0040
#define VPB_PATCH_USER_CANCELS_REPAIR_SAVE								0x0080
#define VPB_PATCH_USER_SAY_NO_ON_OLD_NOT_FOUND							0x0100
#define VPB_PATCH_USER_SAY_NO_ON_NEW_NOT_FOUND							0x0200
#define VPB_PATCH_ERROR_WHILE_SAVE_REPAIR								0x0400
#define VPB_PATCH_USER_SAY_NO_ON_OLD_DATA_NOT_EXIST						0x0800
#define VPB_PATCH_IGNORE_NO_OLD											0x1000

#define VPB_TYPE_ANY													0
#define VPB_TYPE_EQUAL													1
#define VPB_TYPE_DIFFERENT												2

#define VPB_MAKETXT_NO_DATA												0
#define VPB_MAKETXT_OLD_DATA											1
#define VPB_MAKETXT_NEW_DATA											2
#define VPB_MAKETXT_PHONE_DATA											3

class VPatchBlock;

typedef CArray<VPatchBlock> VPatchBlocks;
typedef CList<VPatchBlock> VPatchStream;

class VPatchBlock
{
public:
	DWORD m_Addr;
	DWORD m_Size;
	DWORD m_NewSize;							//size of trully new data (if NewSize<Size, then last (Size-NewSize) bytes of NewData is last (Size-NewSize) bytes of OldData. m_NewSize always equal or less than m_Size.
	BYTE* m_OldData;
	BYTE* m_NewData;
	BYTE* m_PhoneData;
	DWORD m_Flags;
	DWORD m_Stat;
	DWORD m_TextLine;

	VPatchBlock();
	virtual ~VPatchBlock();
	//clear member varriables;
	virtual BOOL Clear(void);
	//copy parameter of other patch block to this
	virtual VPatchBlock& operator=(const VPatchBlock& src);
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
	// Convert data in block to the text string of patch
	virtual CString MakeTextLine(UINT dataOld=VPB_MAKETXT_OLD_DATA, UINT dataNew=VPB_MAKETXT_NEW_DATA, UINT dataComments=VPB_MAKETXT_NO_DATA, UINT MakeType=VPB_TYPE_ANY, UINT dataCmp1=VPB_MAKETXT_NO_DATA, UINT dataCmp2=VPB_MAKETXT_NO_DATA, BOOL bNoDataIdent=FALSE);
protected:
	BYTE* GetDataPtrFromDataCode(UINT dataCode);
};

class VPatchBlockFunc : public VPatchBlock
{
public:
	LPCTSTR m_FuncName;

	VPatchBlockFunc();
	//clear member varriables;
	virtual BOOL Clear(void);
	//copy parameter of other patch block to this
	virtual VPatchBlockFunc& operator=(const VPatchBlockFunc& src);
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
};

class VPatchBlockFuncMemset : public VPatchBlockFunc
{
public:
	BYTE* m_ValueData;
	DWORD m_ValueSize;

	VPatchBlockFuncMemset();
	//clear member varriables;
	virtual BOOL Clear(void);
	//copy parameter of other patch block to this
	virtual VPatchBlockFuncMemset& operator=(const VPatchBlockFuncMemset& src);
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
};

class VPatchBlockFuncMemcpy : public VPatchBlockFunc
{
public:
	DWORD m_AddrFrom;

	VPatchBlockFuncMemcpy();
	//clear member varriables;
	virtual BOOL Clear(void) {VPatchBlockFunc::Clear(); m_AddrFrom=0; return TRUE;}
	//copy parameter of other patch block to this
	virtual VPatchBlockFuncMemcpy& operator=(const VPatchBlockFuncMemcpy& src) {VPatchBlockFunc::operator=(src); m_AddrFrom=src.m_AddrFrom; return *this;}
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
};

class VPatchBlockFuncMemreplace : public VPatchBlockFunc
{
public:
	BYTE* m_FindData;
	BYTE* m_ReplaceData;
	DWORD m_FindReplaceSize;
	int m_MaskByte;

	VPatchBlockFuncMemreplace();
	//clear member varriables;
	virtual BOOL Clear(void);
	//copy parameter of other patch block to this
	virtual VPatchBlockFuncMemreplace& operator=(const VPatchBlockFuncMemreplace& src);
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
};

class VPatchBlockFuncMemwritefromfile : public VPatchBlockFunc
{
public:
	CString m_FileName;
	DWORD m_FileOffset;

	VPatchBlockFuncMemwritefromfile();
	//clear member varriables;
	virtual BOOL Clear(void) {VPatchBlockFunc::Clear(); m_FileName.Empty(); m_FileOffset=0; return TRUE;}
	//copy parameter of other patch block to this
	virtual VPatchBlockFuncMemwritefromfile& operator=(const VPatchBlockFuncMemwritefromfile& src) {VPatchBlockFunc::operator=(src); m_FileName=src.m_FileName; m_FileOffset=src.m_FileOffset; return *this;}
	//parse text line and fill member variables from it;
	virtual BOOL ParseTextLine(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
protected:
	BOOL ParseFuncArguments(LPCTSTR  &strLine, BOOL bEnableErrorMessages=TRUE);
};

class VPatchBlockFuncMemreadtofile : public VPatchBlockFuncMemwritefromfile
{
public:

	VPatchBlockFuncMemreadtofile();
	// Calculate new data dynamically
	virtual BOOL Run(VPatchBlocks& PatchBlocks, VPatchStream& PatchStream, VDevice* pPatchDevice, BYTE* pCurPhoneData);
};







// CPatchPage dialog

class CPatchPage : public CDialog
{
	DECLARE_DYNAMIC(CPatchPage)

public:
	CPatchPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPatchPage();

// Dialog Data
	enum { IDD = IDD_PATCH_PROPPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	CToolTipCtrl *m_pToolTipCtrl;
	VKLayRichEditDoc::CRichEditDoc *m_pRichEditDoc;
	CHARFORMAT2 m_RichEditDefaultCharFormat;
	BOOL m_IsDataInClipboard;
	CFindReplaceDialog *m_pFRDlg;
	CString m_FRLastFindStr;
	CString m_FRLastReplaceStr;
	CHARRANGE m_FRRange;
	DWORD m_FRCount;
	HACCEL m_hAccel;

public:
	afx_msg void OnBnClickedPatchLoad();
	afx_msg void OnBnClickedPatchSaveAs();
	CRichEditCtrl m_PatchDataCtrl;
	CString m_PatchData;
	BOOL SaveModifiedDocument(void);
	CString m_strDocFileName;
	CString m_strLastRepairPatchFileName;
	BOOL m_IsDocNew;
	BOOL m_IsDocRichText;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnIsCanClose(WPARAM wTabIdx, LPARAM pCKLayDlg);
	afx_msg LRESULT OnAppOptionsChanged(WPARAM wTabIdx, LPARAM pCKLayDlg);
	afx_msg LRESULT OnIdle(WPARAM wTabIdx, LPARAM lCount);
	afx_msg LRESULT OnInitialUpdate(WPARAM wTabIdx, LPARAM pTabCtrl);
	afx_msg LRESULT OnGetPatchMenu(WPARAM wUnused, LPARAM pOleCallbackMenuDataStruct);
	BOOL SaveDocument(BOOL bSaveAs=TRUE, LPCTSTR toFileWithNoQuestions=NULL);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPatchUndo();
	afx_msg void OnBnClickedPatchApply();
	// Convert patch from edit box to the binary data
	BOOL PatchDataGet(CString &PatchData, VPatchBlocks &PatchBlocks, BOOL bEnableErrorMessages=TRUE);
	// Test the patch for errors and possibility to safely writing to the device
	BOOL PatchDataConvert(VPatchBlocks &PatchBlocks, VPatchStream &PatchStream, BOOL bApply, VDevice *pPatchDevice=NULL, UINT* lpTestResultMask=NULL, BOOL bYesToAll=FALSE, BOOL bTestMode=FALSE);
	BOOL PatchDataTest_ShowNoOldWarning(CString &strWarn, UINT &TRMask, BOOL bApply);
	// Write the patch to the device
	BOOL PatchDataWrite(VPatchStream &Patch, VDevice *pPatchDevice);
protected:
	virtual void OnOK();
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg LONG OnFindReplace(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedPatchNew();
	BOOL OnNewDocument(BOOL bEmptyDocument=TRUE);
	void UpdateRowColNum(void);
	BOOL OpenDocument(LPCTSTR strDocFileName, BOOL bAppend=FALSE);

	void OnPatchDataPastetextDo();
	void OnPatchDataPasteDo();

	afx_msg void OnPatchDataPastetext();
	afx_msg void OnPatchDataPaste();
	afx_msg void OnPatchDataCut();
	afx_msg void OnPatchDataCopy();
	afx_msg void OnPatchDataUndo();
	afx_msg void OnPatchDataRedo();
	afx_msg void OnPatchDataSelectAll();
	afx_msg void OnPatchDataDelete();
	VDynCtrlSize m_DynSize;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// Do a applying patch
	BOOL DoPatchApply(CString &PatchData, VDevice *pPatchDevice, BOOL bYesToAll=FALSE, BOOL bTestMode=FALSE);
	// Do undo patch
	BOOL DoPatchUndo(CString &PatchData, VDevice *pPatchDevice, BOOL bYesToAll=FALSE, BOOL bTestMode=FALSE);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedPatchAppend();
	afx_msg void OnEnLinkPatchData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPatchDataFont();
	BOOL IsRichText(LPCTSTR strTxt);
	DWORD PatchDataStreamIn(LPCTSTR data, DWORD dataLen, int nFormat);
	void CreateRichEditDoc(void);
	void DestroyRichEditDoc(void);
	BOOL ReInsertClipboardData(void);
	afx_msg void OnDestroy();
	afx_msg void OnEnSelchangePatchData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnMsgfilterPatchData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPatchDataBackcolor();
	// Get file name for RepairPatch
	CString RepairPatchGetFileName(void);
	// Write data of RepairPatch to file
	BOOL RepairPatchSave(VPatchStream& Patch, CString strRepairPatchFileName, BOOL bApply);
	// Convert string to data for writing in text patch file. Return the address of buffer with data - DELETE it after all.
	BOOL ConvertStringToDataForPatchFile(LPCTSTR strPatch, BYTE** pDataBuff, UINT64* pDataLen, BOOL* pbIsConvertErr=NULL);
	// Place a semicolon before eachl lines in text, if this line do not have a semicolon at his begin
	BOOL CommentAllLinesOfText(CString& strText);
	afx_msg void OnPatchDataFind();
	afx_msg void OnPatchDataReplace();
protected:
	long FRFindNext(CFindReplaceDialog* pDlg, BOOL bForReplace=FALSE);
	BOOL ModifyMenuString(CMenu* pPopup, UINT nItemID, LPCTSTR lpszNewItem=NULL);
public:
	BOOL FRStart(BOOL bOnlyFind);
	afx_msg void OnBnClickedPatchSave();
	afx_msg void OnEnChangePatchData();
	afx_msg void OnBnClickedPatchApplyTest();
	afx_msg void OnBnClickedPatchUndoTest();
	afx_msg void OnBnClickedPatchReload();
	afx_msg void OnPatchDataComment();
	afx_msg void OnPatchDataUncomment();
	BOOL AddRemoveStrAtBeginOfSelectedLinesOfPatchDataCtrl(BOOL bAdd, LPCTSTR strIdent);
	BOOL SaveCurPatchDir(LPCTSTR strDirOrFilePath);
	CString GetCurPatchDir(void);
	CString GetDefaultFileName(void);
	BOOL DoPatchLogging(BOOL bOnApply=TRUE);
};
