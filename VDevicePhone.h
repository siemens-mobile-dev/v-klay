#pragma once
#include "vdevice.h"

#define VDP_READ_PAGE_SIZE_START					0x10000
#define VDP_READ_BIG_PAGE_SIZE_MIN					0x4000
#define VDP_READ_PAGE_SIZE_MIN						0x80
#define VDP_READ_PAGE_SIZE_TRY_COUNT				5
#define VDP_READ_BIG_PAGE_SIZE_TRY_COUNT			2

#define VDP_COMM_READ_BLOCK_SIZE					0x400
#define VDP_COMM_WRITE_BLOCK_SIZE					0x2000

#define INGORE_ANSWER								((void*)1)
#define VDP_ASK_USER								(-23)

#define VDP_MEMORY_READ_TIMEOUT						1000
#define VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT			5*60*1000
#define VDP_MESSAGE_TIMEOUT							10		//ms
#define VDP_PHONE_FOR_1STBOOT_READY_TIMEOUT			100		//10

#define VDP_READ_INFO_TIMEOUT						1000
#define VDP_READ_INFO_INTERVAL_TIMEOUT				100
#define VDP_READ_INFO_DATA_TRY_COUNT				4
#define VDP_READ_INFO_DECODED_DATA_TRY_COUNT		4

#define VDP_IS_LOADER_READY_ANS_TIMEOUT				1000
#define VDP_BOOT_ANS_READ_TIMEOUT					5000

#define VDP_WRITE_AFTER_BAD_CRC_SKIP_TIMEOUT		2000
#define VDP_READ_AFTER_ERROR_SKIP_TIMEOUT			1000
#define VDP_STOP_LOADER_SKIP_TIMEOUT				50
#define VDP_BEFORE_SENDING_BOOTS_SKIP_TIMEOUT		1000


#pragma pack(push, 1)
typedef struct VDP_ANS_CMD_LOADER_INFO_tag
{
	BYTE phoneInfo0[0x150];
	BYTE FWVersion;
	BYTE phoneInfo1[0x0F];
	BYTE strLangpackName[0x10];
	BYTE strModelName[0x10];
	BYTE strManufacturerName[0x10];
	BYTE strSomeInfo[0x10];
	BYTE phoneInfo2[0x60];
	DWORD phoneID0;
	DWORD phoneID1;							//for C30 phoneID0 and phone ID1 its a one QWORD phone ID
	DWORD flash0Type;						//flash1 IC Manufacturer (LOWORD) and device ID (HIWORD)
	DWORD flash1Type;						//flash2 IC Manufacturer (LOWORD) and device ID (HIWORD)
	WORD flashhcram;
	WORD BC35_Type;							//number of flashes (but with a C35 "style")
	BYTE B5009[10];
	WORD lockStatus;						//-+ for IMEI
	WORD factoryRegister[4];				// |
	BYTE originalB5009[8];					//-+
} VDP_ANS_CMD_LOADER_INFO;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VDP_ANS_CMD_LOADER_INFO_V2_tag
{
	BYTE FWVersion;
	BYTE phoneInfo1[0x0F];
	BYTE strLangpackName[0x10];
	BYTE strModelName[0x10];
	BYTE strManufacturerName[0x10];
	BYTE strSomeInfo[0x10];
	BYTE phoneInfo2[0x60];
	DWORD phoneID0;
	DWORD phoneID1;							//for C30 phoneID0 and phone ID1 its a one QWORD phone ID
	DWORD flash0Type;						//flash1 IC Manufacturer (LOWORD) and device ID (HIWORD)
	DWORD flash1Type;						//flash2 IC Manufacturer (LOWORD) and device ID (HIWORD)
	WORD flashhcram;
	WORD BC35_Type;							//number of flashes (but with a C35 "style")
	BYTE B5009[10];
	WORD lockStatus;						//-+ for IMEI
	WORD factoryRegister[4];				// |
	BYTE originalB5009[8];					//-+
} VDP_ANS_CMD_LOADER_INFO_V2;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VDP_ANS_CMD_LOADER_INFO_V3_tag
{
	BYTE strModelName[16];					// - model
	BYTE strManufacturerName[16];			// - manufacturer
	BYTE strIMEI[16];						//- IMEI (in ASCII)
	BYTE reserved0[16];						// - (reserved)
	DWORD flashBaseAddr;					// - base address of flash (ROM)
	BYTE reserved1[12];						// - (reserved)
	DWORD flash0Type;						//flash1 IC Manufacturer (LOWORD) and device ID (HIWORD)
	BYTE flashSizePow;						// - N, CFI byte 27h. Size of flash = 2^N
	WORD writeBufferSize;					// - CFI bytes 2Ah-2Bh size of write-buffer (not used by program)
	BYTE flashRegionsNum;					// - CFI byte 2Ch - number of regions.
	WORD flashRegion0BlocksNumMinus1;		// - N, CFI number of blocks in 1st region = N+1
	WORD flashRegion0BlockSizeDiv256;		// - N, CFI size of blocks in 1st region = N*256
	WORD flashRegion1BlocksNumMinus1;		// - N, CFI number of blocks in 2nd region = N+1
	WORD flashRegion1BlockSizeDiv256;		// - N, CFI size of blocks in 2nd region = N*256
	BYTE reserved2[32];						// - (reserved)
} VDP_ANS_CMD_LOADER_INFO_V3;
#pragma pack(pop)



#define VDP_ANS_CMD_LOADER_INFO_MAX_LEN				(16+max(sizeof(VDP_ANS_CMD_LOADER_INFO_V3), max(sizeof(VDP_ANS_CMD_LOADER_INFO), sizeof(VDP_ANS_CMD_LOADER_INFO_V2))))




typedef struct VPHONE_INFO_tag
{
	int num;
	CString fileName;
} VPHONE_INFO;

class VDevicePhone;

class VPhoneBoot
{
public:
	CString m_Name;
	BOOL m_bNoSendLen;
	BOOL m_bNoSendCheckSum;
	BYTE *m_pData;
	DWORD m_Length;
	BYTE *m_pAnswer;
	DWORD m_AnsLen;

	DWORD m_Baud;
	DWORD m_DelayBefore;
	DWORD m_DelayAfter;
	DWORD m_AnswerTimeout;
	BYTE m_SizeLen;
	BOOL m_bUseIgnition;
	DWORD m_TryCount;

	CString	m_SndBefore;
	CString	m_SndAfter;

	VPhoneBoot(void);
	BOOL ReadInfo(VDevicePhone* pDev, LPCTSTR lpstrDrvFile);
	BOOL LoadToPhone(void);
	void ClearInfo(void);
	~VPhoneBoot(void);

protected:
	VDevicePhone *m_pDev;
public:
	void Init(void);
protected:
	BOOL LoadToPhone_SendBoot(void);
};

typedef CArray<VPhoneBoot, const VPhoneBoot&> VPhoneBootsArray;



typedef struct MEMORY_RANGE_tag
{
	INT64 beg;
	INT64 end;
} MEMORY_RANGE;

typedef CArray<MEMORY_RANGE, const MEMORY_RANGE&> VPhoneMemRangeArray;




typedef struct VDEVICEPHONE_BAUDCODE_tag
{
	DWORD baud;
	DWORD code;
} VDEVICEPHONE_BAUDCODE;

class VDevicePhoneCmdBaudsCodes : public CArray<VDEVICEPHONE_BAUDCODE>
{
public:
	VDevicePhoneCmdBaudsCodes(void);
	// Initialising the class by parameters from string
	BOOL ParseParameters(LPCTSTR strParam);
	// Return code for specified baud rate or -1 if code is not found
	DWORD GetCode(DWORD baud);
	// Init members of class
	void Init(void);
};



class VPhoneLoaderOptions
{
public:
	BOOL m_IsInfoCmdDis;
	BOOL m_IsBaudCmdDis;
	VDevicePhoneCmdBaudsCodes m_LdrCmdBaudsCodes;

	BOOL m_IsTstEmptyCmdEn;
	BOOL m_IsRstBootcoreCmdEn;
	int m_IsKeepAliveCmdEnAndInterval;
	int m_Authorization;

	int m_LoaderUploadTryCount;
	int	m_LoaderUploadDelay;
	CString m_LoaderUploadOkSnd;
	CString	m_LoaderUploadErrorSnd;

	BYTE m_CmdAddrAndSizeLen;

	DWORD m_ReadCmdSkipBytesAfterCmdParameters;
	BOOL  m_IsExistReadCmdSkipBytesAfterData;
	DWORD m_ReadCmdSkipBytesAfterData;
	BOOL  m_IsExistReadCmdSkipBytesAfterOK;
	DWORD m_ReadCmdSkipBytesAfterOK;
	BOOL  m_IsExistReadCmdSkipBytesAfterCheckSum;
	DWORD m_ReadCmdSkipBytesAfterCheckSum;
	BOOL  m_IsExistReadCmdAnswerOK;
	BYTE *m_ReadCmdAnswerOK;
	DWORD m_ReadCmdAnswerOKLen;
	int m_ReadCmdAnswerCheckSumType;
	BOOL m_ReadCmdAnswerOrderCheckSumThenOK;

	BYTE  m_WriteCmdVersion;
	BOOL  m_IsExistWriteCmdSkipBytesAfterCmdPatameters;
	DWORD m_WriteCmdSkipBytesAfterCmdPatameters;
	BOOL  m_IsExistWriteCmdSkipBytesAfterBlockSize;
	DWORD m_WriteCmdSkipBytesAfterBlockSize;
	BOOL  m_IsExistWriteCmdSkipBytesAfterDataWithCheckSum;
	DWORD m_WriteCmdSkipBytesAfterDataWithCheckSum;
	BOOL  m_IsExistWriteCmdSkipBytesAfterEraseStep;
	DWORD m_WriteCmdSkipBytesAfterEraseStep;
	BOOL  m_IsExistWriteCmdSkipBytesAfterWriteStep;
	DWORD m_WriteCmdSkipBytesAfterWriteStep;
	BOOL  m_IsExistWriteCmdSkipBytesAfterWittenCheckSum;
	DWORD m_WriteCmdSkipBytesAfterWittenCheckSum;

	VPhoneLoaderOptions(VDevicePhone *pDev=NULL);
	// Load options from the loader file
	BOOL Load(LPCTSTR phoneInfoSec, LPCTSTR strLdrFileName);
	// initialization of options (set to default values)
	void Init(void);

	VDevicePhone *m_pDev;
};



class VDevicePhone :
	public VDevice
{
public:
	VDevicePhone(void);
	~VDevicePhone(void);
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
	// Loads the options for current device from the registry
	virtual BOOL LoadOptions(void);
	// Save the options for current device to the registry
	virtual BOOL SaveOptions(void);
	virtual BOOL SaveOptionsPhoneName(void);
	// Reset the loader in the phone
	virtual BOOL Reset(void);
	// Return the name of current working device
	virtual CString GetName(void);
	// Return the unique name of current connected device
	virtual CString GetUniqueName(void);
	// Sent the Size field to the comm port
	DWORD SendSizeComm(DWORD Size, int SizeFieldLength=0);
	// Search the laders and fill the combo box by file names of loaders
	static BOOL FillByLoadersComboBox(CComboBox* pCB);
	// Empty the combobox with phonenames and destroy the associated item data
	static BOOL EmptyComboBox(CComboBox* pCB);
	// return the string with detailed info of parameters of device
	virtual CString ReadDeviceInfoString(void);
	// Make a Flash info string from the data from phone
	CString MakeFlashInfoString(CString* pManufac=NULL, CString* pModel=NULL, CString* pLangPack=NULL, int* pFwVer=NULL, UINT64* pbcdIMEI=NULL, DWORD* pPhoneID0=NULL, DWORD* pPhoneID1=NULL);
	BOOL OpenCurBootFile(void);
	// Main program must call this function, when parameters of the device is changed
	virtual BOOL OnChangeParameters(void);
	// Set the speed of connection between PC and Phone
	BOOL LoaderSetConnectionSpeed(DWORD Baud, BOOL bOnlyForComPort=FALSE);
	// Check, if skipping of writing to some part of this block is needed
	BOOL IsSkipWritingInBlock(INT64 Address, INT64 Size);
	// Skip data when reading / writing in loader command, using skiping parameters from loader file
	BOOL LoaderSkipData(DWORD Timeout=VDP_MESSAGE_TIMEOUT, DWORD SkipCount=-1, BOOL IsSkip=TRUE);
	// Check is need to skip loader load/unload
	BOOL IsSkipLoaderLoadUnload(void);
	// Load the all Phone info, what is exist in specified section of driver file
	BOOL LoadPhoneInfo(LPCTSTR lpstrPhoneInfoSec, LPCTSTR lpstrDrvFile);
	// Restore bootcore of phone to original state
	virtual BOOL RestoreBootcore(void);
	// Get memory map of flash IC of phone
	virtual BOOL GetMemoryMap(VDevMemMapArray &arrayMemMap);
	// Return bit mask of supported operations
	virtual DWORD GetSupportedOperations(DWORD checkMask=-1);
	// called for sending keep alive packets for loader
	virtual BOOL KeepAlive(void);

protected:
	// Write data to the comm port, wait until all data is written, then return number of written bytes
	DWORD CommWrite(void* pBuf, DWORD dwLen);
	// Read the data from Comm port with wait exact size of data with TimeoutMS timeout.
	DWORD CommRead(void* pBuf, DWORD dwLen, DWORD FirstByteTimeoutMS=TIMEOUT_INFINITY, DWORD NextBytesTimeoutMS=-2, BOOL bUseTotalAndIntervalTimeouts=FALSE);
	// Clear all comm buffers
	void CommReset(void);

	DWORD DeviceSendCommand(BYTE* cmd, void* pBufAnswer=NULL, DWORD bufLenAnswer=-1, DWORD TimeoutMS=VDP_MESSAGE_TIMEOUT, DWORD MessageTimeoutMS=-2);
	// Wait specified answer from phone
	BOOL DeviceWaitCmdAnswer(BYTE *pAns, DWORD AnsLen, DWORD TimeoutMS=TIMEOUT_INFINITY);
	DWORD DeviceSkipData(DWORD TimeoutMS=VDP_MESSAGE_TIMEOUT, DWORD SkipCount=-1);

	// Stop the work of loader in the phone
	BOOL LoaderStopLoader(void);
	// Determine if the loader is ready
	BOOL GetReadyStatus(BOOL& bIsReady);
	// Check, is the loader ready to work
	BOOL LoaderIsReady(DWORD TimeoutMS=VDP_IS_LOADER_READY_ANS_TIMEOUT);
	// Returned the check sum of the given data
	BYTE GetCheckSum(void * pBuf, DWORD dwBufLen);
	// Sets the state of dialog items for this device
	BOOL ActivateDialogCtrls(BOOL bActive);
	// open phone
	BOOL OpenDevice(BOOL bOpenOnlyComPort=FALSE);
	// Closes the handler to comm port and other
	BOOL CloseDevice(BOOL unloadLoader = TRUE);
	// Read the data from phone device memory by mean of loader
	INT64 LoaderReadMemory(INT64 Address, INT64 Size, void* pBuf);
	void LoaderReadMemory_CheckOK_tb(BOOL &isErr, DWORD &tmpbuflen);
	void LoaderReadMemory_CheckCheckSum_tb(BOOL &isErr, void* pBuf, INT64 curSize, DWORD &tmpbuflen);
	void LoaderReadMemory_CheckOK(BOOL &isErr);
	void LoaderReadMemory_CheckCheckSum(BOOL &isErr, void* pBuf, INT64 curSize);
	//ensure that m_LoaderReadMemoryTmpBuf has needed size
	void LoaderReadMemoryTmpBufLenCheck(DWORD size);
	// Write the data to phone device memory by mean of loader
	INT64 LoaderWriteMemory(INT64 Address, INT64 Size, void* pBuf);
	void ThrowCommException(LPCTSTR msg=NULL, BOOL bThrowException=TRUE);
	void ThrowOpenException(LPCTSTR msg=NULL);
	// read the flash info from phone
	BOOL LoaderReadFlashInfo(void);
	void SetLdrStateLoaded(BOOL isLoaded=TRUE);
	// Test, is memory range is empty
	BOOL LoaderTestIsEmpty(INT64 Address, INT64 Size);
	//Checking is phone is flashing first time and save fullflash backup if new phone.
	BOOL IsPhoneNewAndSaveFuBu(void);
	// Authorize the loader
	BOOL LoaderAuthorization(void);

protected:
	friend class VPhoneBoot;
	friend class VPhoneLoaderOptions;
	CString m_Name;
	CString m_PhoneName;
	CString m_PhoneType;
	CString m_ComPort;
	DWORD m_ComBaud;

	BOOL m_bLdrStateLoaded;

	INT64 m_LoaderReadMemoryLastGoodPageSize;
	BYTE* m_LoaderReadMemoryTmpBuf;
	DWORD m_LoaderReadMemoryTmpBufLen;

	
	BYTE m_PhoneInfo[VDP_ANS_CMD_LOADER_INFO_MAX_LEN];
	int m_PhoneInfoLen;


	HANDLE m_hCom;
	OVERLAPPED m_oWrite;
	OVERLAPPED m_oRead;
	COMMTIMEOUTS m_CommTimeouts;

	VPhoneBootsArray m_Boot;
	CString m_Comments;
	CString m_Copyright;

	VPhoneLoaderOptions m_LdrOpt;
	INT64 m_MemAreaStartAddrLdr;

	BOOL m_bSkipWritingBootCore;
	BOOL m_bEnableNoWrite;

	static CString m_strReadingStat;
	static CString m_strReadingStatOk;
	static CString m_strWritingStat;
	static CString m_strWritingStatOk;
	static CString m_strTstIsEmptyStat;

public:
	// Test, if loader is ready, and, if not - throw exception
	void ThrowExceptionIfLoaderNotReady(void);
};
