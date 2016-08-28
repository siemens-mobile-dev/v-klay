#pragma once

#define VD_DEFAULT_PAGE_SIZE						0x020000
#define VD_KEEPALIVE_TICK							250				//in ms


extern TCHAR strFullflashName[];

typedef struct VDEVCACHE_PAGE_INF_tag
{
	BYTE* data;
	BYTE isChanged;
	INT64 addr;
	INT64 size;
} VDEVCACHE_PAGE_INF;

typedef CArray<VDEVCACHE_PAGE_INF> VDevCachePages;

typedef struct VDEVCACHE_MEMGEOMETRY_INF_tag
{
	INT64 startAddr;
	INT64 pageSize;
} VDEVCACHE_MEMGEOMETRY_INF;

typedef CArray<VDEVCACHE_MEMGEOMETRY_INF> VDevCacheMemGeometry;

class VDevice;

class VDevCache
{
public:
	VDevCache(VDevice *pDevice=NULL);
	BOOL ParseParameters(LPCTSTR strParam);
	void Init(void);
	void ClearCache(void);
	void ClearParameters(void);
	// check if the parameters of cache is valid
	BOOL IsParametersValid(void);
	int AddParameterMemGeometry(INT64 startAddr, INT64 pageSize);
	// Return block flash IC size at specified address or -1 if it is not exist information about block size for this address
	INT64 GetRealPageSizeAtAddr(INT64 addr);
	// Return flash IC block size at specified address. Use edge sizes for address for what block sizes is unknown
	INT64 GetPageSizeAtAddr(INT64 addr);

	// Returns the number of cached block
	int GetCachedPagesNum(void) {return (int)m_Pages.GetSize();}

	// Returns the reference to the cache block info
	VDEVCACHE_PAGE_INF* GetCachePage(int Idx) {return &m_Pages.GetAt(Idx);}
	// Return info about cache block at specified address or NULL if cache block is not exist at this address
	VDEVCACHE_PAGE_INF* GetCachePageAtAddr(INT64 addr, BOOL *pbAddPageIfNotExistAndSetThisIfAdded=NULL);

	VDevCacheMemGeometry m_MemGeometry;
protected:
	VDevCachePages m_Pages;
	VDevice *m_pDevice;

	// Make a new cache page info for page what contain specified address
	BOOL MakeNewCachePageInfoForAddr(INT64 addr, VDEVCACHE_PAGE_INF* pPageInfo);
	// Return memory geometry info index for specified address or -1 if it is not exist such information for this address.
	int GetMemGeometryInfoIdxForAddr(INT64 addr, BOOL bExtrapolate);
public:
	// Sets the pointer on device that uses this cache
	void SetDevice(VDevice* pDevice);
	// Returns the number of items in list of memory geometry
	int GetMemGeometryInfosNum(void);
	// Get memory geometry info from specified index in list
	BOOL GetMemGeometryInfoAtIdx(int idx, VDEVCACHE_MEMGEOMETRY_INF& MemInfo);
};

class AreaInf
{
public:
	enum AreaInfFlags
	{
		bootcore		=0x01,
		nowrite			=0x02,
		noread			=0x04,
	};
	INT64 Address;
	INT64 Size;
	CString Name;
	DWORD Flags;
public:
	AreaInf(void){Init();}
	AreaInf(INT64 addr, INT64 size){Init(); Address=addr; Size=size;}
	AreaInf(INT64 addr, INT64 size, LPCTSTR pName){Init(); Address=addr; Size=size; Name=pName;}
	BOOL ParseParameters(LPCTSTR strParam);
	void Init(void);
};

#define VDA_FULLFLASH	0
#define VDA_EEPROM		1

#define VDT_NONE		0
#define VDT_PHONE		1
#define VDT_FILE		2

typedef CArray<AreaInf, const AreaInf&> CAreaInfArray;



struct VDEVMEMMAP_BLOCK
{
	INT64 addr;
	INT64 size;
	BOOL isEmpty;
};

typedef CArray<VDEVMEMMAP_BLOCK> VDevMemMapArray;

class VDCSec;
class VDCSecSaveState;

class VDevice
{
public:
	enum Operations
	{
		read					=0x01,
		write					=0x02,
		restorebootcore			=0x04,
		getmemorymap			=0x08,
	};

	CAreaInfArray m_MemArea;
	INT64 m_MemAreaStart;
	BOOL m_bDetachFromMainWnd;
	int m_Type;
	DWORD m_ErrorsCorrectedCount;

	VDevCache m_Cache;


	VDevice(void);
	virtual ~VDevice(void);
	// open the device
	virtual BOOL Open(void);
	// Close the device
	virtual BOOL Close(void);
	// read the data from device at specified address and size
	virtual BOOL Read(INT64 Address, INT64 Size, void* InputBuf);
	// Write the data from buffer to the device at specified address and size
	virtual BOOL Write(INT64 Address, INT64 Size, void* InputBuf);
	// Save the data, that is changed, to the device
	virtual BOOL Flush(void);
	// Initialize the internal data of device class
	virtual BOOL Init(void);
	// Destroy internal data and free all resources of the device class
	virtual BOOL Destroy(void);
	// Aborts the work with device and prevents to change any data in the device
	virtual BOOL Abort(void);
	// Loads the options for current device from the registry
	virtual BOOL LoadOptions(void);
	// Save the options for current device to the registry
	virtual BOOL SaveOptions(void);
	virtual BOOL SaveOptionsPhoneName(void);
	// Reset the device
	virtual BOOL Reset(void);
	// Main program must call this function, when parameters of the device is changed
	virtual BOOL OnChangeParameters(void);
	// Return the name of current working device
	virtual CString GetName(void);
	// Return the unique name of current connected device
	virtual CString GetUniqueName(void);
	// Return device type
	int GetType(void);
	// Restore bootcore of phone to original state
	virtual BOOL RestoreBootcore(void);
	// Get memory map of flash IC of phone
	virtual BOOL GetMemoryMap(VDevMemMapArray &arrayMemMap);
	// Return bit mask of supported operations
	virtual DWORD GetSupportedOperations(DWORD checkMask=-1);
	// return the string with detailed info of parameters of device
	virtual CString ReadDeviceInfoString(void);
	// called for sending keep alive packets for loader
	virtual BOOL KeepAlive(void);

protected:
	friend VDCSec;
	friend VDCSecSaveState;

	int m_BusyCounter;
	BOOL m_bCanCancel;
	BOOL m_bFlushNeeded;
	// check for user cancel of operation.
	BOOL IsCancel();
	// Return TRUE if user can cancel the operations.
	BOOL IsCanCancel(void);
	// set possibility of user cancel
	BOOL SetMayCancel(BOOL bCanCancel=TRUE);
public:
	// Make a delay, that can be cancelled by user
	BOOL Delay(int delayMS);
};


//	Device critical section implementation.
class VDCSec
{
protected:
	VDevice* m_pDev;

public:
	VDCSec(VDevice* pDev)
	{
		m_pDev = pDev;
		if(m_pDev != NULL)
			m_pDev->m_BusyCounter++;
	}

	~VDCSec()
	{
		if(m_pDev != NULL)
			if(m_pDev->m_BusyCounter != 0)
				m_pDev->m_BusyCounter--;
	}

	static BOOL IsLocked(VDevice* pDev)
	{
		if(pDev != NULL)
			return pDev->m_BusyCounter != 0;
		return FALSE;
	}

	static void Reset(VDevice* pDev)
	{
		if(pDev != NULL)
			pDev->m_BusyCounter = 0;
	}

	static int GetLockCount(VDevice* pDev)
	{
		if(pDev != NULL)
			return pDev->m_BusyCounter;
		return 0;
	}

	void SetLockCount(VDevice* pDev, int CounterValue)
	{
		if(pDev != NULL)
			pDev->m_BusyCounter = CounterValue;
	}
};

class VDCSecSaveState
{
protected:
	int m_SavedCounter;
	VDevice* m_pDev;

public:
	VDCSecSaveState(VDevice* pDev)
	{
		m_pDev = pDev;
		if(m_pDev != NULL)
			m_SavedCounter = m_pDev->m_BusyCounter;
		else
			m_SavedCounter = 0;
	}

	~VDCSecSaveState()
	{
		if(m_pDev != NULL)
			m_pDev->m_BusyCounter = m_SavedCounter;
	}
};
