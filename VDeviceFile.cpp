#include "StdAfx.h"
#include "vdevicefile.h"
#include "V_KLay.h"

VDeviceFile::VDeviceFile(void)
{
	m_Type=VDT_FILE;
	m_pFile=NULL;
}

VDeviceFile::~VDeviceFile(void)
{
	Close();
	Destroy();
}

_TCHAR vdfaddrformat[]=_T("0x%6.6x");
_TCHAR vdfaddrformatneg[]=_T("-0x%6.6x");

// Open File
BOOL VDeviceFile::OpenDevice(LPCTSTR strFileName, INT64 PartAddr, INT64 PartSize)
{
	Close();
	if(!VDevice::Open())
		return FALSE;
	TRY
	{
		m_strFileName=strFileName;
		m_PartAddr=PartAddr;
		m_PartSize=PartSize;
		m_MemArea.RemoveAll();
		m_MemAreaStart=0;
		m_MemArea.InsertAt(VDA_FULLFLASH, AreaInf(m_PartAddr, m_PartSize, _T("flash part")));
		m_pFile= new CFile(m_strFileName, CFile::modeReadWrite | CFile::typeBinary);
		m_pFile->Seek(0, CFile::begin);
	}
	CATCH_ALL(e)
	{
		Close();
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// open the device
BOOL VDeviceFile::Open(void)
{
	CString strFileName, num;
	INT64 PartAddr;
	INT64 PartSize;

	if(m_bDetachFromMainWnd)
	{
		strFileName=m_strFileName;
		PartAddr=m_PartAddr;
		PartSize=m_PartSize;
	}
	else
	{
		CDialog *dlg=(CDialog*)AfxGetMainWnd();
		if(dlg!=NULL && !m_bDetachFromMainWnd)
		{
			CWnd *wnd;
			dlg->GetDlgItemText(IDC_FLASH_FNAME, strFileName);
			if(strFileName.IsEmpty())
			{
				if((wnd=dlg->GetDlgItem(IDC_FLASH_BROWSE))!=NULL)
					wnd->SetFocus();
				AfxMessageBox(MSTR("Select the file with part of flash first."));
				return FALSE;
			}

			dlg->GetDlgItemText(IDC_FLASH_FROM, num);
			if(num.IsEmpty())
			{
				if((wnd=dlg->GetDlgItem(IDC_FLASH_FROM))!=NULL)
					wnd->SetFocus();
				AfxMessageBox(MSTR("\"From address\" must be set to the address, where this part of flash has to be downloaded from."));
				return FALSE;
			}
			PartAddr=(int)VStrToSignedHex(num);
			if(PartAddr>=0)
				num.Format(vdfaddrformat, PartAddr);
			else
				num.Format(vdfaddrformatneg, -PartAddr);
			dlg->SetDlgItemText(IDC_FLASH_FROM, num);

			dlg->GetDlgItemText(IDC_FLASH_SIZE, num);
			if(num.IsEmpty())
			{
				if((wnd=dlg->GetDlgItem(IDC_FLASH_SIZE))!=NULL)
					wnd->SetFocus();
				AfxMessageBox(MSTR("\"Size\" must be set. Try to browse file again."));
				return FALSE;
			}
			PartSize=VStrToSignedHex(num);
			num.Format(vdfaddrformat, PartSize);
			dlg->SetDlgItemText(IDC_FLASH_SIZE, num);
		}
	}

	return OpenDevice(strFileName, PartAddr, PartSize);
}

// Close the device
BOOL VDeviceFile::Close(void)
{
	Flush();
	if(m_pFile)
	{
		m_pFile->Close();
		delete m_pFile;
		m_pFile=NULL;
	}
	VDevice::Close();
	return TRUE;
}

// read the data from device at specified address and size
BOOL VDeviceFile::Read(INT64 Address, INT64 Size, void* InputBuf)
{
	if(m_pFile==NULL)
		return FALSE;
	if(Address<m_PartAddr || (Address+Size)>m_PartAddr+m_PartSize)
	{
		AfxMessageBox(MSTR("You trying to read from the address, that is lies outside of the selected file with part of flash."));
		return FALSE;
	}
	TRY
	{
		ASSERT(AfxIsValidAddress(InputBuf, (UINT)Size));
		m_pFile->Seek(Address-m_PartAddr, CFile::begin);
		m_pFile->Read(InputBuf, (UINT)Size);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// Write the data from buffer to the device at specified address and size
BOOL VDeviceFile::Write(INT64 Address, INT64 Size, void* InputBuf)
{
	if(m_pFile==NULL)
		return FALSE;
	if(Address<m_PartAddr || (Address+Size)>m_PartAddr+m_PartSize)
	{
		AfxMessageBox(MSTR("You trying to write to the address, that is lies outside of the selected file with part of flash."));
		return FALSE;
	}
	TRY
	{
		m_pFile->Seek(Address-m_PartAddr, CFile::begin);
		m_pFile->Write(InputBuf, (UINT)Size);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;
	m_bFlushNeeded=TRUE;;
	return TRUE;
}

// Save the data, that is changed, to the device
BOOL VDeviceFile::Flush(void)
{
	if(!m_bFlushNeeded)
		return TRUE;
	if(m_pFile)
		m_pFile->Flush();

	VDevice::Flush();
	m_bFlushNeeded=FALSE;
	return TRUE;
}

// Initialize the internal data of device class
BOOL VDeviceFile::Init(void)
{
	VDevice::Init();
	ActivateDialogCtrls(TRUE);
	LoadOptions();
	return TRUE;
}

// Destroy internal data and free all resources of the device class
BOOL VDeviceFile::Destroy(void)
{
	SaveOptions();
	VDevice::Destroy();
	ActivateDialogCtrls(FALSE);
	return TRUE;
}

// Sets the state of dialog items for this device
BOOL VDeviceFile::ActivateDialogCtrls(BOOL bActive)
{
	if(m_bDetachFromMainWnd)
		return TRUE;

	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	if(dlg==NULL)
		return FALSE;
	int show=bActive ? SW_SHOW : SW_HIDE;
	dlg->GetDlgItem(IDC_FLASH_FNAME)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_FNAME)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_FLASH_BROWSE)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_BROWSE)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_FLASH_FILE_TEXT)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_FILE_TEXT)->EnableWindow(bActive);

	dlg->GetDlgItem(IDC_FLASH_FROM_TEXT)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_FROM_TEXT)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_FLASH_FROM)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_FROM)->EnableWindow(bActive);
	((CEdit*)dlg->GetDlgItem(IDC_FLASH_FROM))->SetReadOnly(FALSE);
	dlg->GetDlgItem(IDC_FLASH_SIZE_TEXT)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_SIZE_TEXT)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_FLASH_SIZE)->ShowWindow(show);
	dlg->GetDlgItem(IDC_FLASH_SIZE)->EnableWindow(bActive);
	((CEdit*)dlg->GetDlgItem(IDC_FLASH_SIZE))->SetReadOnly(TRUE);
	
	return TRUE;
}

_TCHAR vdfsec[]=_T("DeviceFile");
_TCHAR vdfkeyfname[]=_T("FileName");
_TCHAR vdfkeypaddr[]=_T("PartAddr");
_TCHAR vdfkeypsize[]=_T("PartSize");

// Loads the options for current device from the registry
BOOL VDeviceFile::LoadOptions(void)
{
	if(m_bDetachFromMainWnd)
		return TRUE;
	if(!VDevice::LoadOptions())
		return FALSE;

	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	CWinApp* app=AfxGetApp();
	if(dlg==NULL || app==NULL)
		return FALSE;
/*	CString str;
	DWORD num;

	str=app->GetProfileString(vdfsec, vdfkeyfname);
	dlg->GetDlgItem(IDC_FLASH_FNAME)->SetWindowText(str);


	num=app->GetProfileInt(vdfsec, vdfkeypaddr, -1);
	if(num==-1)
		str.Empty();
	else
		str.Format(vdfaddrformat, num);
	dlg->GetDlgItem(IDC_FLASH_FROM)->SetWindowText(str);

	num=app->GetProfileInt(vdfsec, vdfkeypsize, -1);
	if(num==-1)
		str.Empty();
	else
		str.Format(vdfaddrformat, num);
	dlg->GetDlgItem(IDC_FLASH_SIZE)->SetWindowText(str);*/
	return TRUE;
}

// Save the options for current device to the registry
BOOL VDeviceFile::SaveOptions(void)
{
	if(m_bDetachFromMainWnd)
		return TRUE;
	if(!VDevice::SaveOptions())
		return FALSE;
	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	CWinApp* app=AfxGetApp();
	if(dlg==NULL || app==NULL)
		return FALSE;
	CString str;

	dlg->GetDlgItem(IDC_FLASH_FNAME)->GetWindowText(str);
	str.TrimLeft(); str.TrimRight();
	app->WriteProfileString(vdfsec, vdfkeyfname, str);

	dlg->GetDlgItem(IDC_FLASH_FROM)->GetWindowText(str);
	app->WriteProfileInt(vdfsec, vdfkeypaddr, (int)VStrToSignedHex(str));

	dlg->GetDlgItem(IDC_FLASH_SIZE)->GetWindowText(str);
	app->WriteProfileInt(vdfsec, vdfkeypsize, (int)VStrToSignedHex(str));
	return TRUE;
}

// Main program must call this function, when parameters of the device is changed
BOOL VDeviceFile::OnChangeParameters(void)
{
	if(!VDevice::OnChangeParameters())
		return FALSE;
	m_MemArea.RemoveAll();
	m_MemAreaStart=0;
	if(!m_bDetachFromMainWnd)
	{
		CDialog* dlg=(CDialog*)AfxGetMainWnd();
		if(dlg==NULL)
			return FALSE;
		CString str;
		dlg->GetDlgItemText(IDC_FLASH_FROM, str);
		m_PartAddr=VStrToSignedHex(str);
		dlg->GetDlgItemText(IDC_FLASH_SIZE, str);
		m_PartSize=VStrToSignedHex(str);
		dlg->GetDlgItemText(IDC_FLASH_FNAME, m_strFileName);
	}
	m_MemArea.InsertAt(VDA_FULLFLASH, AreaInf(m_PartAddr, m_PartSize, _T("flash part")));
	return TRUE;
}

// Return the name of current working device
CString VDeviceFile::GetName(void)
{
	CString name, str;
	CWnd* dlg=AfxGetMainWnd();
	if(!m_bDetachFromMainWnd && dlg!=NULL)
	{
		dlg->GetDlgItemText(IDC_FLASH_FNAME, name);
		name=GetFileTitle(name);
	}
	else
		name=_T("fffile");
	str=VDevice::GetName();
	if(!str.IsEmpty())
		str+=_T('_');
	return str+name;
}

// Return bit mask of supported operations
DWORD VDeviceFile::GetSupportedOperations(DWORD checkMask/*=-1*/)
{
	DWORD res=0;
	if(checkMask&VDevice::read)
		res|=VDevice::read;
	if(checkMask&VDevice::write)
		res|=VDevice::write;
	if(checkMask&VDevice::getmemorymap)
		res|=VDevice::getmemorymap;
	return res;
}

// Get memory map of flash IC of phone
BOOL VDeviceFile::GetMemoryMap(VDevMemMapArray &arrayMemMap)
{
	arrayMemMap.RemoveAll();
	INT64 addrend=m_MemArea[VDA_FULLFLASH].Size;
	VDEVMEMMAP_BLOCK mmb;
	BYTE *buf=NULL;
	DWORD bufsize=0;
	INT64 i;
	TRY
	{
		for(mmb.addr=0; mmb.addr<addrend; mmb.addr+=mmb.size)
		{
			mmb.size=0x10000;									//m_Cache.GetPageSizeAtAddr(mmb.addr);
			if(mmb.size>bufsize)
			{
				if(buf!=NULL)
					delete buf;
				bufsize=(DWORD)(mmb.size+16);
				buf=new BYTE[bufsize];
			}
			if(!Read(mmb.addr, mmb.size, buf))
				AfxThrowUserException();
			mmb.isEmpty=TRUE;
			for(i=0; i<mmb.size; i++)
			{
				if(buf[i]!=0xff)
				{
					mmb.isEmpty=FALSE;
					break;
				}
			}
			arrayMemMap.Add(mmb);
		}
	}
	CATCH_ALL(e)
	{
		if(buf!=NULL)
			delete buf;
		buf=NULL;
		THROW_LAST();
	}
	END_CATCH_ALL;
	if(buf!=NULL)
		delete buf;
	buf=NULL;
	return TRUE;
}

