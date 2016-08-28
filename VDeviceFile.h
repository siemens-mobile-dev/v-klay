#pragma once
#include "vdevice.h"

class VDeviceFile :
	public VDevice
{
public:
	VDeviceFile(void);
	~VDeviceFile(void);
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
	BOOL LoadOptions(void);
	// Save the options for current device to the registry
	BOOL SaveOptions(void);
	// Main program must call this function, when parameters of the device is changed
	virtual BOOL OnChangeParameters(void);
	// Return the name of current working device
	virtual CString GetName(void);
	// Open File
	BOOL OpenDevice(LPCTSTR strFileName, INT64 PartAddr, INT64 PartSize);
	// Return bit mask of supported operations
	virtual DWORD GetSupportedOperations(DWORD checkMask=-1);
	// Get memory map of flash IC of phone
	virtual BOOL GetMemoryMap(VDevMemMapArray &arrayMemMap);

protected:
	CString m_strFileName;
	INT64 m_PartAddr;
	INT64 m_PartSize;

	CFile* m_pFile;

	// Sets the state of dialog items for this device
	BOOL ActivateDialogCtrls(BOOL bActive);
};
