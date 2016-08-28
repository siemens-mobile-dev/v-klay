// FlasherPage.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "FlasherPage.h"
#include "KlayDlg.h"
#include "ConsoleDlg.h"
#include "HexViewDlg.h"

// CFlasherPage dialog

IMPLEMENT_DYNAMIC(CFlasherPage, CDialog)
CFlasherPage::CFlasherPage(CWnd* pParent /*=NULL*/)
	: CDialog(CFlasherPage::IDD, pParent)
{
	m_DynSize.AddControl(IDC_MEMORY_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BUFFER_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BUFFER_NOTE_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_OPERATIONS_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_READ_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SAVE_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_OPEN_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_WRITE_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BUFFER_VIEW, EL(100), EL(0), EL(100), EL(0));


	m_pToolTipCtrl=NULL;
	m_From=0x000000;
	m_Size=0x000000;
	m_IsModified=FALSE;
	m_pBuffer=NULL;
	m_BufferSize=0;
	m_BufferLastFrom = -1;
	Create(CFlasherPage::IDD, pParent);

	CRect r;
	((CTabCtrl*)pParent)->InsertItem(((CTabCtrl*)pParent)->GetItemCount(), MSTR("Flasher"));
	((CTabCtrl*)pParent)->GetClientRect(&r);
	((CTabCtrl*)pParent)->AdjustRect(FALSE, &r);
	r.left-=2;
	MoveWindow(&r);
}

CFlasherPage::~CFlasherPage()
{
	if(m_pToolTipCtrl!=NULL)
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
	if(m_pBuffer)
	{
		delete m_pBuffer;
		m_pBuffer=NULL;
	}
}

void CFlasherPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FLASHER_FROM, m_FromCtrl);
	DDX_Control(pDX, IDC_FLASHER_SIZE, m_SizeCtrl);
	DDX_Control(pDX, IDC_BUFFER_OFFSET, m_BufferOffsetCtrl);
	DDX_Control(pDX, IDC_BUFFER_SIZE, m_BufferSizeCtrl);
	DDX_Control(pDX, IDC_BUFFER_LAST_FROM, m_BufferLastFromCtrl);
	CString str;
	if(pDX->m_bSaveAndValidate)
	{
		m_FromCtrl.GetWindowText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox(MSTR("Enter a number in \"From Addres\" field"));
			pDX->Fail();
		}
		m_From=(DWORD)VStrToSignedHex(str);
		m_SizeCtrl.GetWindowText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox(MSTR("Enter a number in \"Size\" field"));
			pDX->Fail();
		}
		m_Size=(DWORD)VStrToSignedHex(str);
		m_BufferOffsetCtrl.GetWindowText(str);
		if(str.IsEmpty())
		{
			AfxMessageBox(MSTR("Enter a number in \"Buffer offset\" field"));
			pDX->Fail();
		}
		m_BufferOffset=(DWORD)VStrToSignedHex(str);
	}
	{
		str.Format(addr64format, m_From);
		m_FromCtrl.SetWindowText(str);
		str.Format(addr64format, m_Size);
		m_SizeCtrl.SetWindowText(str);
		str.Format(addr64format, m_BufferOffset);
		m_BufferOffsetCtrl.SetWindowText(str);
		str.Format(addr64format, m_BufferSize);
		m_BufferSizeCtrl.SetWindowText(str);
		str.Empty();
		if(m_BufferLastFrom != -1)
			str.Format(addr64format, m_BufferLastFrom);
		m_BufferLastFromCtrl.SetWindowText(str);
	}
}


BEGIN_MESSAGE_MAP(CFlasherPage, CDialog)
	ON_MESSAGE(VM_IS_CAN_CLOSE, OnIsCanClose)
	ON_MESSAGE(VM_INITIAL_UPDATE, OnInitialUpdate)
	ON_MESSAGE(VM_DEVICE_CHANGED, OnDeviceChanged)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_FLASHER_MEMORY_LOAD, OnBnClickedMemoryLoad)
	ON_BN_CLICKED(ID_FLASHER_MEMORY_WRITE, OnBnClickedMemoryWrite)
	ON_BN_CLICKED(IDC_FLASHER_FILE_LOAD, OnBnClickedFileLoad)
	ON_BN_CLICKED(IDC_FLASHER_FILE_WRITE, OnBnClickedFileWrite)
	ON_WM_SIZE()
	ON_CBN_SELENDOK(IDC_FLASHER_FROM, OnCbnSelendokFlasherFrom)
	ON_BN_CLICKED(IDC_FLASHER_MEMORY_RESTORE_BOOTCORE, OnBnClickedFlasherMemoryRestoreBootcore)
	ON_BN_CLICKED(IDC_FLASHER_MEMORY_MAP, OnBnClickedFlasherMemoryMap)
	ON_BN_CLICKED(IDC_BUFFER_VIEW, OnBnClickedBufferView)
END_MESSAGE_MAP()


// CFlasherPage message handlers

void CFlasherPage::OnCancel()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

BOOL CFlasherPage::OnInitDialog()
{
	SetDlgItemText(IDC_MEMORY_STATIC, IdentStr(MSTR("Memory of the phone:")));
	SetDlgItemText(IDC_FROM_STATIC, MSTR("From Address:"));
	SetDlgItemText(IDC_MEMORY_SIZE_STATIC, MSTR("Size:"));
	SetDlgItemText(IDC_BUFFER_STATIC, IdentStr(MSTR("Buffer of the program:")));
	SetDlgItemText(IDC_BUFFER_OFFSET_STATIC, MSTR("Use From Offset:"));
	SetDlgItemText(IDC_BUFFER_SIZE_STATIC, MSTR("Size:"));
	SetDlgItemText(IDC_BUFFER_NOTE_STATIC, IdentStr(MSTR("Note: Offset - works only with \"Write Memory\" operation.")));
	SetDlgItemText(IDC_OPERATIONS_STATIC, IdentStr(MSTR("Operations:")));
	SetDlgItemText(ID_FLASHER_MEMORY_LOAD, MSTR("Read Memory"));
	SetDlgItemText(IDC_FLASHER_FILE_WRITE, MSTR("Save File ..."));
	SetDlgItemText(IDC_FLASHER_FILE_LOAD, MSTR("Open File ..."));
	SetDlgItemText(ID_FLASHER_MEMORY_WRITE, MSTR("Write Memory"));
	SetDlgItemText(IDC_FLASHER_MEMORY_RESTORE_BOOTCORE, MSTR("Restore Bootcore"));
	SetDlgItemText(IDC_FLASHER_MEMORY_MAP, MSTR("Memory Map"));
	SetDlgItemText(IDC_BUFFER_LAST_FROM_STATIC, MSTR("Last accessed memory of phone address:"));
	SetDlgItemText(IDC_BUFFER_VIEW, MSTR("View ..."));

	CDialog::OnInitDialog();

	CButton *btn;
	CRect r;
	btn = (CButton*)GetDlgItem(IDC_BUFFER_VIEW);
	btn->GetWindowRect(r); ScreenToClient(r); r.top--; btn->MoveWindow(r);


	m_DynSize.AttachDialog(this);

	m_pToolTipCtrl=new CToolTipCtrl;
	if(!m_pToolTipCtrl->Create(this))
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
	else
	{
		CRect r;
		m_pToolTipCtrl->SetDelayTime(TTDT_AUTOPOP, 32000);
		if(SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0))
			m_pToolTipCtrl->SetMaxTipWidth(r.Size().cx-10);
	}

	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(ID_FLASHER_MEMORY_LOAD), MSTR("Read the data from Memory to internal Buffer of the program"));
	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(IDC_FLASHER_FILE_WRITE), MSTR("Save the data from internal Buffer of the program to the File"));
	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(IDC_FLASHER_FILE_LOAD), MSTR("Load the data from File to the internal Buffer of the program"));
	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(ID_FLASHER_MEMORY_WRITE), MSTR("Write the data from internal Buffer of the program to the Memory"));
	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(IDC_FLASHER_MEMORY_RESTORE_BOOTCORE), MSTR("Restore Bootcore to its original state"));
	m_pToolTipCtrl->AddTool((CButton*)GetDlgItem(IDC_FLASHER_MEMORY_MAP), MSTR("Get the Memory Map of empty memory blocks"));


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

afx_msg LRESULT CFlasherPage::OnInitialUpdate(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	m_FromCtrl.SetCurSel(0);
	m_SizeCtrl.SetCurSel(0);
	m_BufferOffsetCtrl.SetCurSel(0);

	CString fname=((CKLayApp*)AfxGetApp())->GetCmdLineFileName();
	if(!fname.IsEmpty())
	{
		if(fname.GetLength()>=4)
		{
			CString ext=fname.Right(4);
			if(ext.CompareNoCase(_T(".bin"))==0 || ext.CompareNoCase(_T(".fls"))==0)
                if(OpenDocument(fname))
				{
					((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
					if(o_cmdDoWrite)
						OnBnClickedMemoryWrite();
					return 0;
				}
		}
	}
	return 1;
}


afx_msg LRESULT CFlasherPage::OnIsCanClose(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	if(m_IsModified && pCKLayDlg && wTabIdx!=-1)
		((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
	return SaveModifiedDocument();
}


void CFlasherPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(bShow, m_strFileName, m_IsModified);
}

BOOL CFlasherPage::SaveModifiedDocument(void)
{
	if(!m_IsModified)
		return TRUE;
	UINT ret=AfxMessageBox(MSTR("You have unsaved data in the Buffer. Do you want to save it?"),
		MB_YESNOCANCEL|MB_DEFBUTTON1);
	if(ret==IDCANCEL)
		return FALSE;
	if(ret==IDNO)
		return TRUE;
	return SaveDocument();
}


BOOL CFlasherPage::SaveDocument(void)
{
	if(m_pBuffer==NULL || m_BufferSize==0)
	{
		AfxMessageBox(MSTR("There is nothing to save. Read the Memory first."));
		m_IsModified=FALSE;
		return TRUE;
	}

	CString strDocFileName;
	if(m_strFileName.IsEmpty())
		strDocFileName=GetDefaultFileName();
	else
		strDocFileName=m_strFileName;

	CFileDialog fdlg(FALSE, _T("bin"), strDocFileName, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		flashfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=saveDialogTitle;
	fdlg.m_ofn.lpstrInitialDir=o_strFlasherDir;
	if(fdlg.DoModal()!=IDOK)
		return FALSE;
	o_strFlasherDir=GetFileDir(fdlg.GetPathName());

	strDocFileName=fdlg.GetPathName();
	CString strtemp=strDocFileName+_T(".tmp");

	TRY
	{
		TRY{CFile::Remove(strtemp);}CATCH_ALL(e){}END_CATCH_ALL;
		CFile f(strtemp, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		f.Write(m_pBuffer, (UINT)m_BufferSize);
		f.Close();
		TRY{CFile::Remove(strDocFileName);}CATCH_ALL(e){}END_CATCH_ALL;
		CFile::Rename(strtemp, strDocFileName);
		m_IsModified=FALSE;
		m_strFileName=strDocFileName;
		((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strFileName, m_IsModified);
		return TRUE;
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		TRY{CFile::Remove(strtemp);}CATCH_ALL(e){}END_CATCH_ALL;
		return FALSE;
	}
	END_CATCH_ALL;
}

void CFlasherPage::OnBnClickedMemoryLoad()
{
	if(!UpdateData(TRUE))
		return;
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For Reading select the device first!"));
		return;
	}
	if(m_Size==0)
	{
		AfxMessageBox(MSTR("For Reading select the Size of the phone Memory first!"));
		return;
	}
	if(!SaveModifiedDocument())
		return;
	if(m_pBuffer)
		delete m_pBuffer;
	m_pBuffer = NULL;
	m_BufferSize = 0;
	m_BufferLastFrom = m_From;
	m_IsModified = FALSE;
	m_strFileName = GetDefaultFileName();
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strFileName, m_IsModified);
	TRY
	{
		CProgressDlg Progress(0, this);
		Progress.ShowPerformanceInfo();

		m_pBuffer=new BYTE[(size_t)m_Size];
		memset(m_pBuffer, 0, (size_t)m_Size);

		if(!pDevice->Open())
			AfxThrowUserException();
		Progress.SetUpper(m_Size);
		if(!pDevice->Read(m_From, m_Size, m_pBuffer))
			AfxThrowUserException();

		m_BufferSize=m_Size;
		m_IsModified=TRUE;
		UpdateData(FALSE);
		((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strFileName, m_IsModified);

		if(!pDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();
		if(!o_bDisableWarningsMsg)
			AfxMessageBox(CString(MSTR("Memory read successfully."))
				+Progress.MakePerformanceReport(), MB_OK|MB_ICONINFORMATION);
	}
	CATCH(CUserException, e)
	{
		UpdateData(FALSE);
	}
	AND_CATCH_ALL(e)
	{
		UpdateData(FALSE);
		e->ReportError();
	}
	END_CATCH_ALL;
}

void CFlasherPage::OnBnClickedMemoryWrite()
{
	if(!UpdateData(TRUE))
		return;
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For Writing you must select the device first!"));
		return;
	}
	if(m_Size==0)
	{
		AfxMessageBox(MSTR("For Writing select the Size of phone Memory first!"));
		return;
	}
	CString strNothingWr=MSTR("There is nothing to write.");
	if(m_pBuffer==NULL || m_BufferSize==0)
	{
		AfxMessageBox(strNothingWr);
		return;
	}
	if(m_BufferOffset>m_BufferSize)
	{
		AfxMessageBox(strNothingWr + _T("\n\n") +
			MSTR("The Offset of the Buffer cannot be greater than the Size of the Buffer."));
		return;
	}
	if(m_BufferSize-m_BufferOffset==0)
	{
		AfxMessageBox(strNothingWr + _T("\n\n") +
			MSTR("The Offset of the Buffer can not equal to the Size of the Buffer."));
		return;
	}
	INT64 size=m_Size;
	if(size > m_BufferSize-m_BufferOffset)
	{
		if(AfxMessageBox(MSTR("The amount of the data in the Buffer (minus Offset) is less, than you want to write down in Memory.\nDo you want to reduce the Size of Memory and write down all data which is in the Buffer?"),
				MB_YESNO)!=IDYES)
			return;
		size=m_BufferSize-m_BufferOffset;
	}
	if(!o_bDisableWarningsMsg)
	{
		CString snum, str=MSTR("Write the current Buffer data to the Memory?");
		if(!m_strFileName.IsEmpty())
		{
			str+=_T("\n\n");
			str+=MSTR("Current file:");
			str+=_TCHAR('\n');
			str+=m_strFileName;
		}

		str+=_T("\n\n");
		str+=MSTR("Memory of the phone:");
		str+=_T("  ");
		str+=MSTR("From Address:");
		str+=_TCHAR(' ');
		str+=VSignedHexToStr(m_From, addr64format);
		str+=_T(", ");
		str+=MSTR("Size:");
		str+=_TCHAR(' ');
		str+=VSignedHexToStr(m_Size, addr64format);

		str+=_T("\n\n");
		str+=MSTR("Buffer of the program:");
		str+=_T("  ");
		str+=MSTR("Use From Offset:");
		str+=_TCHAR(' ');
		str+=VSignedHexToStr(m_BufferOffset, addr64format);
		str+=_T(", ");
		str+=MSTR("Size:");
		str+=_TCHAR(' ');
		str+=VSignedHexToStr(m_BufferSize, addr64format);

		if(AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION)!=IDYES)
			return;
	}
	m_BufferLastFrom = m_From;
	UpdateData(FALSE);
	TRY
	{
		CProgressDlg Progress(0, this);
		Progress.ShowPerformanceInfo();
		if(!pDevice->Open())
			AfxThrowUserException();
		Progress.SetUpper(size);
		if(!pDevice->Write(m_From, size, m_pBuffer+m_BufferOffset))
			AfxThrowUserException();
		if(!pDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();
		if(!o_bDisableWarningsMsg)
			AfxMessageBox(CString(MSTR("Memory written successfully."))
				+Progress.MakePerformanceReport(), MB_OK|MB_ICONINFORMATION);
	}
	CATCH(CUserException, e)
	{
		UpdateData(FALSE);
	}
	AND_CATCH_ALL(e)
	{
		UpdateData(FALSE);
		e->ReportError();
	}
	END_CATCH_ALL;
}

BOOL CFlasherPage::OpenDocument(LPCTSTR strDocFileName)
{
	CString strFlashFileName=strDocFileName;
	CString str;
	TRY
	{
		CFile f(strFlashFileName, CFile::modeRead|CFile::typeBinary);
		m_IsModified=FALSE;
        m_Size=m_BufferSize=(DWORD)f.GetLength();
		str.Format(addr64format, m_BufferSize);
		m_BufferSizeCtrl.SetWindowText(str);
		str.Format(addr64format, m_Size);
		m_SizeCtrl.SetCurSel(-1);
		m_SizeCtrl.SetWindowText(str);
		if(m_pBuffer)
			delete m_pBuffer;
		m_pBuffer=new BYTE[(size_t)m_BufferSize];
		f.Read(m_pBuffer, (UINT)m_BufferSize);
		f.Close();
		INT64 a=GetAddrFromFileName(strFlashFileName);
		if(a!=-1)
		{
			m_From=a;
			str.Format(addr64format, m_From);
			m_FromCtrl.SetCurSel(-1);
			m_FromCtrl.SetWindowText(str);
		}
		else
		{
			if(m_FromCtrl.GetCurSel()!=-1)
				m_FromCtrl.SetCurSel(0);	//-1);
		}
		m_BufferOffset=0;
		str.Format(addr64format, m_BufferOffset);
		m_BufferOffsetCtrl.SetCurSel(-1);
		m_BufferOffsetCtrl.SetWindowText(str);
		m_strFileName=strFlashFileName;
		((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strFileName, m_IsModified);

		CKLayDlg* dlg=(CKLayDlg*)AfxGetMainWnd();
		if(dlg!=NULL)
		{
			CComboBox *ldcb=(CComboBox*)dlg->GetDlgItem(IDC_PHONES_LIST);
			if(ldcb!=NULL)
			{
				str=GetFileName(strFlashFileName);
				str.Replace(_T(' '), _T('_'));
				CString phone;
				int i;
				for(i=0; i < ldcb->GetCount(); i++)
				{
					ldcb->GetLBText(i, phone);
					if(!phone.IsEmpty())
						if(str.GetLength() >= phone.GetLength()+1)
						{
							phone.Replace(_T(' '), _T('_'));
							if(_tcsncicmp(str, phone, phone.GetLength()) == 0)
								if(str[phone.GetLength()] == _T('_'))
									break;
						}
				}
				if(i < ldcb->GetCount() && ldcb->GetCurSel()!=i)
					dlg->SelectPhone(i);
			}
		}
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

void CFlasherPage::OnBnClickedFileLoad()
{
	if(!SaveModifiedDocument())
		return;
	CFileDialog fdlg(TRUE, NULL, m_strFileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, flashfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=openDialogTitle;
	if(GetFileDir(m_strFileName).IsEmpty())
		fdlg.m_ofn.lpstrInitialDir=o_strFlasherDir;
	if(fdlg.DoModal()!=IDOK)
		return;
	o_strFlasherDir=GetFileDir(fdlg.GetPathName());
	OpenDocument(fdlg.GetPathName());
}

void CFlasherPage::OnBnClickedFileWrite()
{
	SaveDocument();
}

void CFlasherPage::OnOK()
{
}

afx_msg LRESULT CFlasherPage::OnDeviceChanged(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	if(!pDevice)
		return FALSE;
	CString str, format, strFrom, strSize;
	int idxf, idxs;
	BOOL isDevFile= pDevice->GetType()==VDT_FILE;

	idxf=m_FromCtrl.GetCurSel();
	m_FromCtrl.GetWindowText(strFrom);
	if(!isDevFile)
		m_FromCtrl.ResetContent();
	else if(pDevice->m_MemArea.GetSize() && m_FromCtrl.GetCount()!=0)
	{
		m_FromCtrl.GetLBText(0, format);
		if(format.Find(pDevice->m_MemArea[VDA_FULLFLASH].Name)!=-1)
			m_FromCtrl.DeleteString(0);
	}
	m_FromCtrl.SetWindowText(strFrom);

	idxs=m_SizeCtrl.GetCurSel();
	m_SizeCtrl.GetWindowText(strSize);
	if(!isDevFile)
		m_SizeCtrl.ResetContent();
	else if(pDevice->m_MemArea.GetSize() && m_SizeCtrl.GetCount()!=0)
	{
		m_SizeCtrl.GetLBText(0, format);
		if(format.Find(pDevice->m_MemArea[VDA_FULLFLASH].Name)!=-1)
			m_SizeCtrl.DeleteString(0);
	}
	m_SizeCtrl.SetWindowText(strSize);

	m_BufferOffsetCtrl.GetWindowText(str);
	if(!isDevFile)
		m_BufferOffsetCtrl.ResetContent();
	else if(pDevice->m_MemArea.GetSize() && m_BufferOffsetCtrl.GetCount()!=0)
	{
		m_BufferOffsetCtrl.DeleteString(0);
		if(m_BufferOffsetCtrl.GetCount()!=0)
		{
			m_BufferOffsetCtrl.GetLBText(0, format);
			if(format.Find(pDevice->m_MemArea[VDA_FULLFLASH].Name)!=-1)
				m_BufferOffsetCtrl.DeleteString(0);
		}
	}
	m_BufferOffsetCtrl.SetWindowText(str);

	int i, n=(int)pDevice->m_MemArea.GetSize();
	format=_T("%s (%s)");
	str.Format(format, VSignedHexToStr(0, addr64format), _T("start"));
	if(n)
	{
		m_BufferOffsetCtrl.InsertString(0, str);
		INT64 FFStart=pDevice->m_MemAreaStart;
		for(i=0; i<n; i++)
		{
			str.Format(format, VSignedHexToStr(pDevice->m_MemArea[i].Address-FFStart, addr64format), (LPCTSTR)pDevice->m_MemArea[i].Name);
			m_FromCtrl.InsertString(i, str);
			m_BufferOffsetCtrl.InsertString(i+1, str);
			str.Format(format, VSignedHexToStr(pDevice->m_MemArea[i].Size, addr64format), (LPCTSTR)pDevice->m_MemArea[i].Name);
			m_SizeCtrl.InsertString(i, str);
		}
	}
	else
	{
		m_BufferOffsetCtrl.InsertString(0, str);
		str.Format(format, VSignedHexToStr(0, addr64format), _T("flash"));
		m_FromCtrl.InsertString(0, str);
	}
	if(idxf!=-1 || strFrom.IsEmpty())
		m_FromCtrl.SetCurSel(0);
	if(idxs!=-1 || strSize.IsEmpty())
		m_SizeCtrl.SetCurSel(0);

	CWnd *pwnd=NULL;
	pwnd=GetDlgItem(IDC_FLASHER_MEMORY_RESTORE_BOOTCORE);
	if(pwnd!=NULL)
		if(pwnd->IsWindowVisible())
			pwnd->EnableWindow(pDevice->GetSupportedOperations(VDevice::restorebootcore));

	pwnd=GetDlgItem(IDC_FLASHER_MEMORY_MAP);
	if(pwnd!=NULL)
		if(pwnd->IsWindowVisible())
			pwnd->EnableWindow(pDevice->GetSupportedOperations(VDevice::getmemorymap));

	return TRUE;
}



void CFlasherPage::OnSize(UINT nType, int cx, int cy)
{
	m_DynSize.OnSize(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

CString CFlasherPage::GetDefaultFileName(void)
{
	return GetDefaultFlashFileName(m_From);
/*	CString strDocFileName, strDevName;
	strDevName=_T("Mem");
	if(pDevice!=NULL)
	{
		strDocFileName=pDevice->GetName();
		if(!strDocFileName.IsEmpty())
			strDevName=strDocFileName;
		strDevName.Replace(_T(' '), _T('_'));
	}
	strDocFileName.Format(strFlasherDefFName, m_From>>16);
	strDocFileName=strDevName+'_'+COleDateTime::GetCurrentTime().Format(strFNTimeFormat)+strDocFileName;
	return strDocFileName;*/
}

void CFlasherPage::OnCbnSelendokFlasherFrom()
{
	// TODO: Add your control notification handler code here
	int cur=m_FromCtrl.GetCurSel();
	if(cur!=CB_ERR)
		m_SizeCtrl.SetCurSel(cur);
}

BOOL CFlasherPage::PreTranslateMessage(MSG* pMsg)
{
	if(m_pToolTipCtrl!=NULL)
		m_pToolTipCtrl->RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CFlasherPage::OnBnClickedFlasherMemoryRestoreBootcore()
{
	if(!UpdateData(TRUE))
		return;
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For Restoring bootcore you must select the device first!"));
		return;
	}
	//if(!o_bDisableWarningsMsg)
	{
		if(AfxMessageBox(MSTR("! ! ! WARNING ! ! !\n\nIf you restore bootcore to its original state you will can't use flasher in future.\n\nContinue operation?")
			, MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)!=IDYES)
			return;
	}
	TRY
	{
		CProgressDlg Progress(0, this);
		if(!pDevice->Open())
			AfxThrowUserException();
		if(!pDevice->RestoreBootcore())
			AfxThrowUserException();
		if(!pDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();
		if(!o_bDisableWarningsMsg)
			AfxMessageBox(MSTR("Bootcore restored successfully.")
				, MB_OK|MB_ICONINFORMATION);
	}
	CATCH(CUserException, e)
	{
	}
	AND_CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
}

void CFlasherPage::OnBnClickedFlasherMemoryMap()
{
	if(!UpdateData(TRUE))
		return;
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For getting memory map you must select the device first!"));
		return;
	}
	TRY
	{
		VDevMemMapArray mm;
		CProgressDlg Progress(NULL, this);
		if(!pDevice->Open())
			AfxThrowUserException();
		Progress.SetUpper(pDevice->m_MemArea[VDA_FULLFLASH].Size);
		if(!pDevice->GetMemoryMap(mm))
			AfxThrowUserException();
		if(!pDevice->Close())
			AfxThrowUserException();
		Progress.DestroyWindow();
		int i;
		VDEVMEMMAP_BLOCK *pmmb;
		CString str, num, strmap;
		str=VSignedHexToStr(0, addr64format);
		i=str.GetLength();

		num=MSTR("Address");
		if(num.GetLength()<i)
			num+=CString(_T(' '), (i-num.GetLength())*2+1);
		strmap+=num;

		strmap+=_T('\t');
		num=MSTR("Size");
		if(num.GetLength()<i)
			num+=CString(_T(' '), (i-num.GetLength())*2+1);
		strmap+=num;

		strmap+=_T('\t');
		strmap+=MSTR("State");

		int j;
		INT64 addr, addrend;
		BOOL namefound;
		CString stre=MSTR("empty");
		CString stru=MSTR("used");
		j=max(stre.GetLength(), stru.GetLength());
		if(stre.GetLength()<j)
			stre+=CString(_T(' '), (j-stre.GetLength())*2+1);
		if(stru.GetLength()<j)
			stru+=CString(_T(' '), (j-stru.GetLength())*2+1);
		for(int i=0; i<mm.GetSize(); i++)
		{
			strmap+=_T("\r\n");
			pmmb=&mm[i];
			str.Format(_T("%s\t%s\t"), VSignedHexToStr(pmmb->addr, addr64format), VSignedHexToStr(pmmb->size, addr64format));
			if(pmmb->isEmpty)
				str+=stre;
			else
				str+=stru;
			addr=pmmb->addr+pDevice->m_MemAreaStart;
			addrend=pmmb->addr+pmmb->size+pDevice->m_MemAreaStart;
			namefound=FALSE;
			for(j=0; j<pDevice->m_MemArea.GetSize(); j++)
			{
				if(j!=VDA_FULLFLASH)
				{
					if(addr < pDevice->m_MemArea[j].Address+pDevice->m_MemArea[j].Size &&
						addrend > pDevice->m_MemArea[j].Address)
					{
						if(!namefound)
						{
							str+=_T("\t(");
							namefound=TRUE;
						}
						else
						{
							str+=_T(", ");
						}
						str+=pDevice->m_MemArea[j].Name;
					}
				}
			}
			if(namefound)
				str+=_T(')');
			strmap+=str;
		}
		strmap+=_T("\r\n");
		CConsoleDlg dlg(this, strmap, IdentStr(MSTR("List of blocks of phone flash and its state.")));
		dlg.DoModal();
	}
	CATCH(CUserException, e)
	{
	}
	AND_CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
}

void CFlasherPage::OnBnClickedBufferView()
{
	CHexViewDlg dlg;
	dlg.m_pData = m_pBuffer;
	dlg.m_DataSize = m_BufferSize;
	dlg.m_DataAddress = m_From;
	dlg.DoModal();
	if(dlg.m_IsModified)
		m_IsModified = TRUE;
}
