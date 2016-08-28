#include "StdAfx.h"
#include "vdevice.h"
#include "V_KLay.h"
#include ".\vdevice.h"



TCHAR strFullflashName[]=_T("fullflash");

VDevice::VDevice(void)
{
	VDCSec::Reset(this);
	m_ErrorsCorrectedCount=0;
	m_Type=VDT_NONE;
	m_bDetachFromMainWnd=FALSE;
	m_bFlushNeeded=FALSE;
	m_bCanCancel=TRUE;
	m_MemAreaStart=0;
	m_Cache.SetDevice(this);
	m_Cache.Init();
}

VDevice::~VDevice(void)
{
	Close();
	Destroy();
}

// open the device
BOOL VDevice::Open(void)
{
	m_bCanCancel=TRUE;
	m_ErrorsCorrectedCount=0;
	return TRUE;
}

// Close the device
BOOL VDevice::Close(void)
{
	m_bCanCancel=TRUE;
	Flush();
	return TRUE;
}

// read the data from device at specified address and size
BOOL VDevice::Read(INT64 Address, INT64 Size, void* InputBuf)
{
	return TRUE;
}

// Write the data from buffer to the device at specified address and size
BOOL VDevice::Write(INT64 Address, INT64 Size, void* InputBuf)
{
	return TRUE;
}

// Save the data, that is changed, to the device
BOOL VDevice::Flush(void)
{
	return TRUE;
}

// Aborts the work with device and prevents to change any data in the device
BOOL VDevice::Abort(void)
{
	m_bFlushNeeded=FALSE;
	return TRUE;
}

// Initialize the internal data of device class
BOOL VDevice::Init(void)
{
	LoadOptions();
	return TRUE;
}

// Destroy internal data and free all resources of the device class
BOOL VDevice::Destroy(void)
{
	SaveOptions();
	return TRUE;
}

// check for user cancel of operation.
BOOL VDevice::IsCancel()
{
	if(pProgress!=NULL)
	{
		if(pProgress->m_bUnknownUpper)
			pProgress->StepIt();
		if(pProgress->m_bAbort)
		{
			pProgress->CheckCancelButton();
			pProgress->m_bAbort=FALSE;
			AfxThrowUserException();
		}
		if(!m_bCanCancel)
		{
			pProgress->PumpMessages();
			return FALSE;
		}
		return pProgress->CheckCancelButton();
	}
	return FALSE;
}

// Return TRUE if user can cancel the operations.
BOOL VDevice::IsCanCancel(void)
{
	return m_bCanCancel;
}

// set possibility of user cancel
//  Return FLASE if user already cancelled operation.
BOOL VDevice::SetMayCancel(BOOL bCanCancel)
{
	m_bCanCancel=TRUE;
	BOOL cancel=IsCancel();
	m_bCanCancel=bCanCancel;
	return !cancel;
}

// Loads the options for current device from the registry
BOOL VDevice::LoadOptions(void)
{
	return TRUE;
}

// Save the options for current device to the registry
BOOL VDevice::SaveOptions(void)
{
	return TRUE;
}
BOOL VDevice::SaveOptionsPhoneName(void)
{
	return TRUE;
}

// //Reset the device
BOOL VDevice::Reset(void)
{
	Close();
	return TRUE;
}

// return the string with detailed info of parameters of device
CString VDevice::ReadDeviceInfoString(void)
{
	return CString();
}

// Main program must call this function, when parameters of the device is changed
BOOL VDevice::OnChangeParameters(void)
{
	return TRUE;
}

// Return the name of current working device
CString VDevice::GetName(void)
{
	return CString();
}

// Return the unique name of current connected device
CString VDevice::GetUniqueName(void)
{
	return GetName();
}

// Return device type
int VDevice::GetType(void)
{
	return m_Type;
}




//////////////////////////////////////////////////////////////////////////




LPCTSTR strAreaInfFlagName[]={
	_T("bootcore"),
	_T("nowrite"),
	_T("noread"),
};


//Parse parameters string in format:
//_name_, _offset_in_MCU_, _size_, _block_type0_, _block_type1_, ..., _block_typeN_
// where: _block_typeX_ - the list of types of block. Can consist with one or more words: 
//   bootcore - block with bootcore. A warning will be generated if some data will be written in this block.
//   nowrite  - the loader do not support writing to that block.
//   noread   - the loader do not support reading data from that block.
BOOL AreaInf::ParseParameters(LPCTSTR strParam)
{
	CString str, par(strParam);
	par.Trim();
	if(par.IsEmpty())
		return FALSE;
	Init();
	int pos=0;

	Name=VGetParameterFromList(par, pos);
	str=VGetParameterFromList(par, pos);
	if(!str.IsEmpty())
		Address=VStrToInt(str);
	str=VGetParameterFromList(par, pos);
	if(!str.IsEmpty())
		Size=VStrToInt(str);

	int i;
	for(;;)
	{
		str=VGetParameterFromList(par, pos);
		if(str.IsEmpty())
			break;
		for(i=0; i < sizeof(strAreaInfFlagName)/sizeof(strAreaInfFlagName[0]); i++)
		{
			if(str.CompareNoCase(strAreaInfFlagName[i])==0)
			{
				Flags|=((DWORD)1)<<i;
				break;
			}
		}
	}

	return Address!=-1 && Size!=-1;
}

void AreaInf::Init(void)
{
	Address=-1;
	Size=-1;
	Flags=0;
	Name.Empty();
}





//////////////////////////////////////////////////////////////////////////


VDevCache::VDevCache(VDevice *pDevice/*=NULL*/)
{
	m_pDevice=pDevice;
}

// Sets the pointer on device that uses this cache
void VDevCache::SetDevice(VDevice* pDevice)
{
	m_pDevice=pDevice;
}

BOOL VDevCache::ParseParameters(LPCTSTR strParam)
{
	CString str, par(strParam);
	par.Trim();
	if(par.IsEmpty())
		return TRUE;
	Init();
	int i, pos;
	VDEVCACHE_MEMGEOMETRY_INF mgi;
	for(pos=0, i=0; ; i++)
	{
		str=VGetParameterFromList(par, pos);
		if(str.IsEmpty())
			break;
		mgi.startAddr=VStrToInt(str);

		str=VGetParameterFromList(par, pos);
		if(str.IsEmpty())
		{
			if(i==0 && mgi.startAddr==0)
			{
				return TRUE;
			}
			else
			{
				Init();
				return FALSE;
			}
		}
		mgi.pageSize=VStrToInt(str);
		AddParameterMemGeometry(mgi.startAddr, mgi.pageSize);
	}
	return TRUE;
}

void VDevCache::Init(void)
{
	if(m_pDevice==NULL)
	{
		AfxMessageBox(_T("VDevCache::VDevCache: m_pDevice=NULL"));
		ExitProcess(-1);
	}
	ClearCache();
	ClearParameters();
}

void VDevCache::ClearCache(void)
{
	for(int i=0; i<m_Pages.GetSize(); i++)
	{
		if(m_Pages[i].data!=NULL)
		{
			delete m_Pages[i].data;
			m_Pages[i].data=NULL;
		}
	}
	m_Pages.RemoveAll();
}

void VDevCache::ClearParameters(void)
{
	m_MemGeometry.RemoveAll();
}

// check if the parameters of cache is valid
BOOL VDevCache::IsParametersValid(void)
{
	return !m_MemGeometry.IsEmpty();
}

//add a parameter of memory geometry and arrange addresses from small to big.
int VDevCache::AddParameterMemGeometry(INT64 startAddr, INT64 pageSize)
{
	VDEVCACHE_MEMGEOMETRY_INF mgi;
	mgi.startAddr=startAddr;
	mgi.pageSize=pageSize;
	for(int i=0; i<m_MemGeometry.GetSize(); i++)
		if(m_MemGeometry[i].startAddr > mgi.startAddr)
			break;
	m_MemGeometry.InsertAt(i, mgi);
	return i;
}

// Return memory geometry info index for specified address or -1 if it is not exist such information for this address.
int VDevCache::GetMemGeometryInfoIdxForAddr(INT64 addr, BOOL bExtrapolate)
{
	int idx=-1;
	addr+=m_pDevice->m_MemAreaStart;
	for(int i=0; i<m_MemGeometry.GetSize(); i++)
	{
		if(m_MemGeometry[i].startAddr > addr)
			break;
		idx=i;
	}
	if(bExtrapolate && idx==-1 && m_MemGeometry.GetSize()!=0)
		idx=0;
	return idx;
}

// Return block flash IC size at specified address or -1 if it is not exist information about block size for this address
INT64 VDevCache::GetRealPageSizeAtAddr(INT64 addr)
{
	int i=GetMemGeometryInfoIdxForAddr(addr, FALSE);
	if(i==-1)
		return -1;
	return m_MemGeometry[i].pageSize;
}

// Return flash IC block size at specified address. Use edge sizes for address for what block sizes is unknown
INT64 VDevCache::GetPageSizeAtAddr(INT64 addr)
{
	int i=GetMemGeometryInfoIdxForAddr(addr, TRUE);
	if(i==-1)
		return VD_DEFAULT_PAGE_SIZE;
	return m_MemGeometry[i].pageSize;
}

// Return info about cache block at specified address or NULL if cache block is not exist at this address
VDEVCACHE_PAGE_INF* VDevCache::GetCachePageAtAddr(INT64 addr, BOOL *pbAddPageIfNotExistAndSetThisIfAdded/*=NULL*/)
{
	if(pbAddPageIfNotExistAndSetThisIfAdded!=NULL)
		*pbAddPageIfNotExistAndSetThisIfAdded=FALSE;
	VDEVCACHE_PAGE_INF *pci=NULL;
	INT_PTR i, n=m_Pages.GetSize();
	for(i=0; i<n; i++)
	{
		pci=&m_Pages[i];
		if(addr < pci->addr)
		{
			pci=NULL;
			break;
		}
		if(addr >= pci->addr && addr < pci->addr+pci->size)
			break;
	}
	if(i>=n)
		pci=NULL;
	if(pci==NULL && pbAddPageIfNotExistAndSetThisIfAdded!=NULL)
	{
		VDEVCACHE_PAGE_INF pi;
		if(MakeNewCachePageInfoForAddr(addr, &pi))
		{
			m_Pages.InsertAt(i, pi);
			pci=&m_Pages[i];
			*pbAddPageIfNotExistAndSetThisIfAdded=TRUE;
		}
	}
	return pci;
}

// Make a new cache page info for page what contain specified address
BOOL VDevCache::MakeNewCachePageInfoForAddr(INT64 addr, VDEVCACHE_PAGE_INF* pPageInfo)
{
	if(pPageInfo==NULL)
		return FALSE;
	pPageInfo->isChanged=FALSE;
	pPageInfo->data=NULL;
	int i=GetMemGeometryInfoIdxForAddr(addr, TRUE);
	INT64 a=0, s=VD_DEFAULT_PAGE_SIZE;
	if(i!=-1)
	{
		a=m_MemGeometry[i].startAddr-m_pDevice->m_MemAreaStart;
		s=m_MemGeometry[i].pageSize;
	}
	pPageInfo->size=s;
	pPageInfo->addr=(addr-a)/s;
	pPageInfo->addr=pPageInfo->addr*s+a;
	pPageInfo->data=new BYTE[(size_t)pPageInfo->size];
	return pPageInfo->data!=NULL;
}



// Restore bootcore of phone to original state
BOOL VDevice::RestoreBootcore(void)
{
	return FALSE;
}

// Get memory map of flash IC of phone
BOOL VDevice::GetMemoryMap(VDevMemMapArray &arrayMemMap)
{
	return FALSE;
}

// Return bit mask of supported operations
DWORD VDevice::GetSupportedOperations(DWORD checkMask/*=-1*/)
{
	return 0;
}

// Returns the number of items in list of memory geometry
int VDevCache::GetMemGeometryInfosNum(void)
{
	return (int)m_MemGeometry.GetSize();
}

// Get memory geometry info from specified index in list
BOOL VDevCache::GetMemGeometryInfoAtIdx(int idx, VDEVCACHE_MEMGEOMETRY_INF& MemInfo)
{
	memset(&MemInfo, 0, sizeof(MemInfo));
	if(idx<0 || idx>=m_MemGeometry.GetSize() || pDevice==NULL)
		return FALSE;
	MemInfo=m_MemGeometry[idx];
	MemInfo.startAddr-=pDevice->m_MemAreaStart;
	return TRUE;
}

// called for sending keep alive packets for loader
BOOL VDevice::KeepAlive(void)
{
	return TRUE;
}

// Make a delay, that can be cancelled by user
BOOL VDevice::Delay(int delayMS)
{
	if(delayMS != -1)
		for(VTIMER tim = SetTimerEventAfter(delayMS); !IsTimerEvent(tim); Sleep(1))
			if(IsCancel())
				AfxThrowUserException();
	return TRUE;
}
