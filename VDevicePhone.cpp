#include "StdAfx.h"
#include "V_KLay.h"
#include "KLayDlg.h"
#include "vdevicephone.h"
#include ".\vdevicephone.h"


//#define VDP_EMULATE_WRITING

CString strCurPhoneModelName;

_TCHAR drvsecPhoneCommonInfo[]=_T("PhoneCommonInfo");
_TCHAR drvsecPhone[]=_T("Phone%2.2i");
_TCHAR drvkeyName[]=_T("Name");
_TCHAR drvkeyType[]=_T("Type");
_TCHAR drvkeyComments[]=_T("Comments");
_TCHAR drvkeyCopyright[]=_T("Copyright");
_TCHAR drvkeyBoots[]=_T("Boots");

_TCHAR drvkeyOptionsPrefix[]=_T("opt");
_TCHAR drvkeyOptInfoCmdDis[]=_T("InfoCmdDisable");
_TCHAR drvkeyOptBaudCmdDis[]=_T("BaudCmdDisable");
_TCHAR drvkeyOptBaudCmdCodes[]=_T("BaudCmdCodes");
_TCHAR drvkeyOptTstEmptyCmdEn[]=_T("TestEmptyCmdEnable");
_TCHAR drvkeyOptRstBootcoreCmdEn[]=_T("RestoreBootcoreCmdEnable");
_TCHAR drvkeyOptKeepAliveCmdEnAndInterval[]=_T("KeepAliveCmdEnableAndSetInterval");
_TCHAR drvkeyOptAuthorization[]=_T("Authorization");
_TCHAR drvkeyOptLoaderUploadTryCount[]=_T("LoaderUploadTryCount");
_TCHAR drvkeyOptLoaderUploadDelay[]=_T("LoaderUploadDelay");
_TCHAR drvkeyOptLoaderUploadOkSnd[]=_T("LoaderUploadOkSnd");
_TCHAR drvkeyOptLoaderUploadErrorSnd[]=_T("LoaderUploadErrorSnd");

_TCHAR drvkeyOptCmdAddrAndSizeLen[]=_T("CmdAddrAndSizeLen");

_TCHAR drvkeyOptReadCmdSkipBytesAfterCmdParameters[]=_T("ReadCmdSkipBytesAfterCmdParameters");
_TCHAR drvkeyOptReadCmdSkipBytesAfterData[]=_T("ReadCmdSkipBytesAfterData");
_TCHAR drvkeyOptReadCmdSkipBytesAfterOK[]=_T("ReadCmdSkipBytesAfterOK");
_TCHAR drvkeyOptReadCmdSkipBytesAfterCheckSum[]=_T("ReadCmdSkipBytesAfterCheckSum");
_TCHAR drvkeyOptReadCmdAnswerOK[]=_T("ReadCmdAnswerOK");
_TCHAR drvkeyOptReadCmdAnswerCheckSumType[]=_T("ReadCmdAnswerCheckSumType");
_TCHAR drvkeyOptReadCmdAnswerOrderCheckSumThenOK[]=_T("ReadCmdAnswerOrderCheckSumThenOK");

_TCHAR drvkeyOptWriteCmdVersion[]=_T("WriteCmdVersion");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterCmdPatameters[]=_T("WriteCmdSkipBytesAfterCmdPatameters");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterBlockSize[]=_T("WriteCmdSkipBytesAfterBlockSize");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterDataWithCheckSum[]=_T("WriteCmdSkipBytesAfterDataWithCheckSum");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterEraseStep[]=_T("WriteCmdSkipBytesAfterEraseStep");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterWriteStep[]=_T("WriteCmdSkipBytesAfterWriteStep");
_TCHAR drvkeyOptWriteCmdSkipBytesAfterWittenCheckSum[]=_T("WriteCmdSkipBytesAfterWittenCheckSum");

_TCHAR drvkeyMemGeometry[]=_T("MCUMemGeometry");

_TCHAR drvkeyMemFlashBase[]=_T("MCUMemFlashBase");
_TCHAR drvsecMemFF[]=_T("MCUMemFuBu");
_TCHAR drvsecMemArea[]=_T("MCUMemArea%2.2i");

_TCHAR drvsecBoot[]=_T("Boot%2.2i");
_TCHAR drvkeyBootName[]=_T("Name");
_TCHAR drvkeyBootData[]=_T("Data");
_TCHAR drvkeyBootAns[]=_T("Answer");
_TCHAR drvkeyBootNoLen[]=_T("NoSendLen");
_TCHAR drvkeyBootNoCRC[]=_T("NoSendCheckSum");
_TCHAR drvkeyBootUseIgn[]=_T("UseIgnition");
_TCHAR drvkeyBootTryCount[]=_T("TryCount");
_TCHAR drvkeyBootBaud[]=_T("PortSpeed");
_TCHAR drvkeyBootSizeLen[]=_T("SizeLen");
_TCHAR drvkeyBootDelayBefore[]=_T("DelayBefore");
_TCHAR drvkeyBootDelayAfter[]=_T("DelayAfter");
_TCHAR drvkeyBootAnswerTimeout[]=_T("AnswerTimeout");
_TCHAR drvkeyBootSndBefore[]=_T("SndBefore");
_TCHAR drvkeyBootSndAfter[]=_T("SndAfter");


_TCHAR vdpsec[]=_T("DevicePhone");
_TCHAR vdpkeyname[]=_T("Name");
_TCHAR vdpkeyport[]=_T("Port");
_TCHAR vdpkeybaud[]=_T("Baud");
_TCHAR vdpkeyUsedPhone[] = _T("UsedPhoneID%3.3i");




BYTE vdpCmdIsDeviceReady[]=					"U";
#define vdpCmdIsDeviceReadyLen				(sizeof(vdpCmdIsDeviceReady)-1)
BYTE vdpAnsDeviceReady[]=					{0xA0,0};
#define vdpAnsDeviceReadyLen				(sizeof(vdpAnsDeviceReady)-1)


BYTE vdpCmdLoaderSetBaud[]=					"H";
#define vdpCmdLoaderSetBaudLen				(sizeof(vdpCmdLoaderSetBaud)-1)
BYTE vdpCmdLoaderSetBaudParametersOk[]=		"h";
#define vdpCmdLoaderSetBaudParametersOkLen	(sizeof(vdpCmdLoaderSetBaudParametersOk)-1)
BYTE vdpCmdLoaderBaudIsSet[]=				"A";
#define vdpCmdLoaderBaudIsSetLen			(sizeof(vdpCmdLoaderBaudIsSet)-1)
BYTE vdpCmdLoaderSetBaudOk[]=				"H";
#define vdpCmdLoaderSetBaudOkLen			(sizeof(vdpCmdLoaderSetBaudOk)-1)
_TCHAR vdpCmdLoaderBaudListDefault[]=_T("57600: 0x00, 115200: 0x01, 230400: 0x02, 460800: 0x03, 921600: 0x04");

BYTE vdpCmdLoaderExit[]=					"Q";
#define vdpCmdLoaderExitLen					(sizeof(vdpCmdLoaderExit)-1)
BYTE vdpCmdLoaderExit2[]=					"Z";
#define vdpCmdLoaderExit2Len				(sizeof(vdpCmdLoaderExit)-1)

BYTE vdpCmdIsLoaderReady[]=					"A";
#define vdpCmdIsLoaderReadyLen				(sizeof(vdpCmdIsLoaderReady)-1)
BYTE vdpAnsLoaderReady[]=					"R";
#define vdpAnsLoaderReadyLen				(sizeof(vdpAnsLoaderReady)-1)

BYTE vdpCmdLoaderFlashInfo[]=				"I";
#define vdpCmdLoaderFlashInfoLen			(sizeof(vdpCmdLoaderFlashInfo)-1);

BYTE vdpCmdLoaderRead[]=					"R";
#define vdpCmdLoaderReadLen					(sizeof(vdpCmdLoaderRead)-1)
BYTE vdpAnsLoaderReadOk[]=					"OK";
#define vdpAnsLoaderReadOkLen				(sizeof(vdpAnsLoaderReadOk)-1)
#define VDP_CMD_LOADER_READ_ADDR(addr)		((addr)+m_MemAreaStart)
#define VDP_CMD_LOADER_READ_CHECKSUM_LEN	2

BYTE vdpCmdLoaderWrite[]=					"F";
#define vdpCmdLoaderWriteLen				(sizeof(vdpCmdLoaderWrite)-1)

BYTE vdpCmdLoaderTstIsEmpty[]=				"T";
#define VDP_CMD_LOADER_TSTISEMPTY_ANS_LEN	1

BYTE vdpCmdLoaderKeepAlive[]=				".";
#define VDP_CMD_LOADER_KEEPALIVE_ANS_TIMEOUT	1

BYTE vdpCmdLoaderWithNoAnswer[]=			"\xF1";

BYTE vdpCmdLoaderAuthorization[]=			"U";


#pragma pack(push, 1)
typedef struct VDP_CMD_LOADER_WRITE_ADDR_ANS_tag
{
	WORD flashBase;							//start MCU data seg (DPP) of current flash IC.
											//After dataseg (address to write) is sended:
											//If flashbase=ADDRESS_TOO_FAR_NAK=0xEEEE - error - dataseg is out of flash (next data is
											//  not sending by the phone).
											//If flashbase=UNKNOWN_FLASH_NAK=0xCCCC - error - flash IC type is unknown for loader.
											// Then loader sends a WORD=flashtype (LOBYTE - LOBYTE(manufacturer),
											// HIBYTE - LOBYTE(IC ID)) and exits (reset MCU).
	WORD offset;							//Start offset in MCU data seg (DPP) of the current block of flash.
	WORD seg;								//Start MCU data seg (DPP) of the current block of flash.
	WORD dummy;								//not used, ignored.
	WORD flashEnd;							//not used, ignored.
	WORD blockSize;							//(blocksize*4096) = size of current block of flash to write.
	WORD id;								//always = 0xffff.
} VDP_CMD_LOADER_WRITE_ADDR_ANS;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VDP_CMD_LOADER_WRITE_DATA_ANS_tag
{
	WORD flashBase;							//After data and check sum is sended:
											//If flashbase=RAM_ERROR=0xFFFF - error - phone RAM error (after this loader send
											// CRC_ERROR_NAK=0xBBBB and next data is not sended by the phone)
											//If flashbase=CRC_ERROR_NAK=0xBBBB - error - XORChecksum is not correct. Loader is cancel
											//  current operation and next data is not sended.
	WORD seg;
	WORD offset;
} VDP_CMD_LOADER_WRITE_DATA_ANS;
#pragma pack(pop)

#define VDP_CMDLDR_WRITE_ERR_ADDRESS_TOO_FAR			0xEEEE
#define VDP_CMDLDR_WRITE_ERR_UNKNOWN_FLASH				0xCCCC
#define VDP_CMDLDR_WRITE_DATARECEIVED_ERR_CRC_NAK		0xBBBB
#define VDP_CMDLDR_WRITE_OK_ID							0xFFFF
#define VDP_CMDLDR_WRITE_CALC_BLOCK_SIZE(ansBlockSize)	((ansBlockSize)*4096)
#define VDP_CMDLDR_WRITE_DATARECEIVED_ERR_RAM_NAK		0xFFFF
#define VDP_CMDLDR_WRITE_OK_ERASE						0x0202
#define VDP_CMDLDR_WRITE_OK_ACK							0x0303
#define VDP_CMDLDR_WRITE_OUT_CRC_LEN					2
BYTE vdpAnsLoaderWriteOk[]=								"OK";
#define vdpAnsLoaderWriteOkLen							(sizeof(vdpAnsLoaderWriteOk)-1)
#define VDP_CMDLDR_WRITE_CALC_ADDR(addr)				(((addr)+(m_MemAreaStart-m_MemAreaStartAddrLdr))/((INT64)0x1000))
#define VDP_CMDLDR_WRITE_UNCALC_ADDR(addr)				((addr)*((INT64)0x1000)-(m_MemAreaStart-m_MemAreaStartAddrLdr))
#define VDP_CMDLDR_WRITE_ADDR_LEN						2
//v2 specific
#define VDP_CMDLDR_WRITEV2_CALC_ADDR(addr)					((addr)+m_MemAreaStart)
#define VDP_CMDLDR_WRITEV2_UNCALC_ADDR(addr)				((addr)-m_MemAreaStart)
#define VDP_CMDLDR_WRITEV2_DATARECEIVED_ACK					0x0101
#define VDP_CMDLDR_WRITEV2_DATARECEIVED_ACCESS_DENIED_NAK	0xEEEE
#define VDP_CMDLDR_WRITEV2_DATARECEIVED_ERR_BOUNDS_NAK		0xFFFF
#define VDP_CMDLDR_WRITEV2_DATARECEIVED_UNKNOWN_FLASH_NAK	0xCCCC
//

////////////////////////////////////////////////////////////////////////////////
VPhoneLoaderOptions::VPhoneLoaderOptions(VDevicePhone *pDev/*=NULL*/)
{
	m_pDev=pDev;
	m_ReadCmdAnswerOK = NULL;
	Init();
}

// initialization of options (set to default values)
void VPhoneLoaderOptions::Init(void)
{
	m_IsInfoCmdDis=FALSE;

	m_IsBaudCmdDis=FALSE;
	m_LdrCmdBaudsCodes.ParseParameters(vdpCmdLoaderBaudListDefault);

	m_IsTstEmptyCmdEn=FALSE;
	m_IsRstBootcoreCmdEn=FALSE;
	m_IsKeepAliveCmdEnAndInterval=0;
	m_Authorization = 0;

	m_LoaderUploadTryCount = 0;
	m_LoaderUploadDelay = 0;
	m_LoaderUploadOkSnd.Empty();
	m_LoaderUploadErrorSnd.Empty();

	m_CmdAddrAndSizeLen=3;

	m_ReadCmdSkipBytesAfterCmdParameters = 0;
	m_IsExistReadCmdSkipBytesAfterData = FALSE;
	m_ReadCmdSkipBytesAfterData = 0;
	m_IsExistReadCmdSkipBytesAfterOK = FALSE;
	m_ReadCmdSkipBytesAfterOK = 0;
	m_IsExistReadCmdSkipBytesAfterCheckSum = FALSE;
	m_ReadCmdSkipBytesAfterCheckSum = 0;
	m_IsExistReadCmdAnswerOK = FALSE;
	if(m_ReadCmdAnswerOK != NULL)
		delete m_ReadCmdAnswerOK;
	m_ReadCmdAnswerOKLen = vdpAnsLoaderReadOkLen;
	m_ReadCmdAnswerOK = new BYTE[m_ReadCmdAnswerOKLen];
	memcpy(m_ReadCmdAnswerOK, vdpAnsLoaderReadOk, m_ReadCmdAnswerOKLen);
	m_ReadCmdAnswerCheckSumType = 1;
	m_ReadCmdAnswerOrderCheckSumThenOK = 0;

	m_WriteCmdVersion=1;
	m_IsExistWriteCmdSkipBytesAfterCmdPatameters=FALSE;
	m_WriteCmdSkipBytesAfterCmdPatameters=0;
	m_IsExistWriteCmdSkipBytesAfterBlockSize=FALSE;
	m_WriteCmdSkipBytesAfterBlockSize=0;
	m_IsExistWriteCmdSkipBytesAfterDataWithCheckSum=FALSE;
	m_WriteCmdSkipBytesAfterDataWithCheckSum=0;
	m_IsExistWriteCmdSkipBytesAfterEraseStep=FALSE;
	m_WriteCmdSkipBytesAfterEraseStep=0;
	m_IsExistWriteCmdSkipBytesAfterWriteStep=FALSE;
	m_WriteCmdSkipBytesAfterWriteStep=0;
	m_IsExistWriteCmdSkipBytesAfterWittenCheckSum=FALSE;
	m_WriteCmdSkipBytesAfterWittenCheckSum=0;
}

// Load options from the loader file
BOOL VPhoneLoaderOptions::Load(LPCTSTR phoneInfoSec, LPCTSTR strLdrFileName)
{
	if(phoneInfoSec==NULL || strLdrFileName==NULL)
		return FALSE;
	CString prefix=drvkeyOptionsPrefix;
// check is any "opt..." parameter is exist. if yes - initialise all options.
	CStringList keylist;
	VGetPrivateProfileSection(phoneInfoSec, &keylist, strLdrFileName);
	for(POSITION pos=keylist.GetHeadPosition(); pos;)
	{
		if(_tcsncmp(keylist.GetNext(pos), prefix, prefix.GetLength())==0)
		{
			Init();
			break;
		}
	}

	BOOL isExist;
	int i;
	BYTE *ptr;
	DWORD dw;

	m_IsInfoCmdDis=(BOOL)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptInfoCmdDis,
		m_IsInfoCmdDis, strLdrFileName);


	m_IsBaudCmdDis=(BOOL)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptBaudCmdDis,
		m_IsBaudCmdDis, strLdrFileName);

	if(!m_LdrCmdBaudsCodes.ParseParameters(
		VGetPrivateProfileString(phoneInfoSec, prefix+drvkeyOptBaudCmdCodes, NULL, strLdrFileName)))
		return FALSE;


	m_IsTstEmptyCmdEn=(BOOL)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptTstEmptyCmdEn,
		m_IsTstEmptyCmdEn, strLdrFileName);
	m_IsRstBootcoreCmdEn=(BOOL)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptRstBootcoreCmdEn,
		m_IsRstBootcoreCmdEn, strLdrFileName);

	i=(int)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptKeepAliveCmdEnAndInterval,
		m_IsKeepAliveCmdEnAndInterval*VD_KEEPALIVE_TICK, strLdrFileName);
	m_IsKeepAliveCmdEnAndInterval=i/VD_KEEPALIVE_TICK;
	if(m_IsKeepAliveCmdEnAndInterval==0 && i!=0)
		m_IsKeepAliveCmdEnAndInterval=1;

	m_CmdAddrAndSizeLen=(BYTE)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptCmdAddrAndSizeLen,
		m_CmdAddrAndSizeLen, strLdrFileName);

	m_Authorization=(int)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptAuthorization,
		m_Authorization, strLdrFileName);

    
	m_ReadCmdSkipBytesAfterCmdParameters=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptReadCmdSkipBytesAfterCmdParameters,
		m_ReadCmdSkipBytesAfterCmdParameters, strLdrFileName);

	m_ReadCmdSkipBytesAfterData=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptReadCmdSkipBytesAfterData,
		m_ReadCmdSkipBytesAfterData, strLdrFileName, &isExist);
	m_IsExistReadCmdSkipBytesAfterData|=isExist;

	m_ReadCmdSkipBytesAfterOK=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptReadCmdSkipBytesAfterOK,
		m_ReadCmdSkipBytesAfterOK, strLdrFileName, &isExist);
	m_IsExistReadCmdSkipBytesAfterOK |= isExist;

	m_ReadCmdSkipBytesAfterCheckSum=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix + drvkeyOptReadCmdSkipBytesAfterCheckSum,
		m_ReadCmdSkipBytesAfterCheckSum, strLdrFileName, &isExist);
	m_IsExistReadCmdSkipBytesAfterCheckSum |= isExist;

	if(!VGetPrivateProfileData(phoneInfoSec, prefix + drvkeyOptReadCmdAnswerOK, ptr, dw, strLdrFileName, &isExist))
	{
		AfxMessageBox(CString(_T("Reading phone driver file: error while loading parameter \""))
			+ prefix + drvkeyOptReadCmdAnswerOK + _T("\" from section \"") + phoneInfoSec
			+ _T("\"\nFile: ") + strLdrFileName);
		return FALSE;
	}
	if(isExist)
	{
		if(ptr == NULL && dw == 0)
			ptr = new BYTE[1];
		if(m_ReadCmdAnswerOK != NULL)
			delete m_ReadCmdAnswerOK;
		m_IsExistReadCmdAnswerOK |= isExist;
		m_ReadCmdAnswerOK = ptr;
		m_ReadCmdAnswerOKLen = dw;
	}
	else
	{
		if(ptr != NULL)
			delete ptr;
	}

	m_ReadCmdAnswerCheckSumType=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptReadCmdAnswerCheckSumType,
		m_ReadCmdAnswerCheckSumType, strLdrFileName);

	m_ReadCmdAnswerOrderCheckSumThenOK=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptReadCmdAnswerOrderCheckSumThenOK,
		m_ReadCmdAnswerOrderCheckSumThenOK, strLdrFileName);


	m_WriteCmdVersion=(BYTE)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdVersion,
		m_WriteCmdVersion, strLdrFileName);

	m_WriteCmdSkipBytesAfterCmdPatameters=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterCmdPatameters,
		m_WriteCmdSkipBytesAfterCmdPatameters, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterCmdPatameters|=isExist;

	m_WriteCmdSkipBytesAfterBlockSize=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterBlockSize,
		m_WriteCmdSkipBytesAfterBlockSize, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterBlockSize|=isExist;

	m_WriteCmdSkipBytesAfterDataWithCheckSum=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterDataWithCheckSum,
		m_WriteCmdSkipBytesAfterDataWithCheckSum, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterDataWithCheckSum|=isExist;

	m_WriteCmdSkipBytesAfterEraseStep=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterEraseStep,
		m_WriteCmdSkipBytesAfterEraseStep, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterEraseStep|=isExist;

	m_WriteCmdSkipBytesAfterWriteStep=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterWriteStep,
		m_WriteCmdSkipBytesAfterWriteStep, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterWriteStep|=isExist;

	m_WriteCmdSkipBytesAfterWittenCheckSum=(DWORD)VGetPrivateProfileHex(phoneInfoSec, prefix+drvkeyOptWriteCmdSkipBytesAfterWittenCheckSum,
		m_WriteCmdSkipBytesAfterWittenCheckSum, strLdrFileName, &isExist);
	m_IsExistWriteCmdSkipBytesAfterWittenCheckSum|=isExist;


	m_LoaderUploadTryCount = (int)VGetPrivateProfileHex(phoneInfoSec, prefix + drvkeyOptLoaderUploadTryCount,
		m_LoaderUploadTryCount, strLdrFileName);
	m_LoaderUploadDelay = (int)VGetPrivateProfileHex(phoneInfoSec, prefix + drvkeyOptLoaderUploadDelay,
		m_LoaderUploadDelay, strLdrFileName);

	m_LoaderUploadOkSnd = AddPathToFileNameAndExpandEnvStr(
		VGetPrivateProfileString(phoneInfoSec, prefix + drvkeyOptLoaderUploadOkSnd, m_LoaderUploadOkSnd, strLdrFileName),
		strLdrFileName);

	m_LoaderUploadErrorSnd = AddPathToFileNameAndExpandEnvStr(
		VGetPrivateProfileString(phoneInfoSec, prefix + drvkeyOptLoaderUploadErrorSnd, m_LoaderUploadErrorSnd, strLdrFileName),
		strLdrFileName);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////

VPhoneBoot::VPhoneBoot(void)
{
	Init();
}

void VPhoneBoot::Init(void)
{
	m_bNoSendCheckSum=FALSE;
	m_bNoSendLen=FALSE;
	m_pDev=NULL;
	m_pData=NULL;
	m_Length=0;
	m_pAnswer=NULL;
	m_AnsLen=0;

	m_Baud=-1;
	m_DelayBefore=-1;
	m_DelayAfter=-1;
	m_AnswerTimeout=0;
	m_SizeLen=0;
	m_bUseIgnition=FALSE;
	m_TryCount=0;

	m_SndBefore.Empty();
	m_SndAfter.Empty();
}


BOOL VPhoneBoot::ReadInfo(VDevicePhone* pDev, LPCTSTR lpstrDrvFile)
{
	ClearInfo();
	CString str, strBootSec;
	for(int i=1; ; i++)
	{
		strBootSec.Format(drvsecBoot, i);
		str=VGetPrivateProfileString(strBootSec, drvkeyBootName, NULL, lpstrDrvFile);
		if(str.IsEmpty())
			return FALSE;
		str.Trim();
		if(str.CompareNoCase(m_Name)==0)
			break;
	}
	m_pDev=pDev;
	if(m_pDev==NULL)
		return FALSE;
	m_bNoSendCheckSum=(BOOL)VGetPrivateProfileInt(strBootSec, drvkeyBootNoCRC, m_bNoSendCheckSum, lpstrDrvFile);
	m_bNoSendLen=(BOOL)VGetPrivateProfileInt(strBootSec, drvkeyBootNoLen, m_bNoSendLen, lpstrDrvFile);
	m_Baud=(DWORD)VGetPrivateProfileInt(strBootSec, drvkeyBootBaud, m_Baud, lpstrDrvFile);
	if(m_Baud==0)
		m_Baud=-1;
	m_DelayBefore=(DWORD)VGetPrivateProfileInt(strBootSec, drvkeyBootDelayBefore, m_DelayBefore, lpstrDrvFile);
	m_DelayAfter=(DWORD)VGetPrivateProfileInt(strBootSec, drvkeyBootDelayAfter, m_DelayAfter, lpstrDrvFile);
	m_AnswerTimeout=(DWORD)VGetPrivateProfileInt(strBootSec, drvkeyBootAnswerTimeout, m_AnswerTimeout, lpstrDrvFile);
	m_SizeLen=(BYTE)VGetPrivateProfileInt(strBootSec, drvkeyBootSizeLen, m_SizeLen, lpstrDrvFile);
	m_bUseIgnition=(BOOL)VGetPrivateProfileInt(strBootSec, drvkeyBootUseIgn, m_bUseIgnition, lpstrDrvFile);
	m_TryCount=(DWORD)VGetPrivateProfileInt(strBootSec, drvkeyBootTryCount, m_TryCount, lpstrDrvFile);
	if(!VGetPrivateProfileData(strBootSec, drvkeyBootData, m_pData, m_Length, lpstrDrvFile))
	{
		AfxMessageBox(CString(_T("Reading phone driver file: error while loading parameter \"")) + drvkeyBootData + _T("\" from section \"") + strBootSec
			+ _T("\", boot name: \"") + m_Name + _T("\"\nFile: ") + lpstrDrvFile);
		return FALSE;
	}
	if(m_pData == NULL && m_Length == 0)
	{
		BYTE *pdat = NULL;
		BYTE *ptmp;
		DWORD len;
		str = CString(drvkeyBootData) + _T("%2.2i");
		CString key;
		for(int i=1; ; i++)
		{
			key.Format(str, i);
			if(!VGetPrivateProfileData(strBootSec, key, pdat, len, lpstrDrvFile))
			{
				AfxMessageBox(CString(_T("Reading phone driver file: error while loading parameter \"")) + key + _T("\" from section \"") + strBootSec
					+ _T("\", boot name: \"") + m_Name + _T("\"\nFile: ") + lpstrDrvFile);
				return FALSE;
			}
			if(pdat == NULL)
				break;
			if(m_pData != NULL)
			{
				ptmp = new BYTE[m_Length + len + 16];
				memcpy(ptmp, m_pData, m_Length);
				memcpy(ptmp + m_Length, pdat, len);
				delete m_pData;
				delete pdat;
				m_pData = ptmp;
				m_Length += len;
			}
			else
			{
				m_pData = pdat;
				m_Length = len;
			}
		}
	}
	if(!VGetPrivateProfileData(strBootSec, drvkeyBootAns, m_pAnswer, m_AnsLen, lpstrDrvFile))
	{
		AfxMessageBox(CString(_T("Reading phone driver file: error while loading parameter \"")) + drvkeyBootAns + _T("\" from section \"") + strBootSec
			+ _T("\", boot name: \"") + m_Name + _T("\"\nFile: ") + lpstrDrvFile);
		return FALSE;
	}

	m_SndBefore = AddPathToFileNameAndExpandEnvStr(
		VGetPrivateProfileString(strBootSec, drvkeyBootSndBefore, m_SndBefore, lpstrDrvFile), lpstrDrvFile);

	m_SndAfter = AddPathToFileNameAndExpandEnvStr(
		VGetPrivateProfileString(strBootSec, drvkeyBootSndAfter, m_SndAfter, lpstrDrvFile), lpstrDrvFile);

	return TRUE;
}

BOOL VPhoneBoot::LoadToPhone_SendBoot(void)
{
	if(m_pData!=NULL)
	{
		if(!m_bNoSendLen)
			m_pDev->SendSizeComm(m_Length, m_SizeLen);
		m_pDev->CommWrite(m_pData, m_Length);
		if(!m_bNoSendCheckSum)
		{
			BYTE cs=m_pDev->GetCheckSum(m_pData, m_Length);
			m_pDev->CommWrite(&cs, 1);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL VPhoneBoot::LoadToPhone(void)
{
	if(m_pDev==NULL)
		return FALSE;
	VTIMER tim;
	if(pProgress)
		pProgress->SetStatus(CString(MSTR("Connected. Sending boots..."))+m_Name);

//play sound before, if needed.
	if(!m_SndBefore.IsEmpty())
		SndMsg(m_SndBefore, 1);

//wait some time before sending boot, if needed.
	m_pDev->Delay(m_DelayBefore);

//set some port speed before sending boot, if needed
	if(m_Baud!=-1)
	{
		m_pDev->LoaderSetConnectionSpeed(m_Baud, TRUE);
	}
	int trycount=m_TryCount==0 ? 1 : m_TryCount;
	BOOL bOk=FALSE;
	DWORD anstimeout=m_AnswerTimeout;

//sending boot.
	if(m_bUseIgnition)
	{
	//sending a boot with ignition (connection boot).
		BYTE* ansbuf=NULL;
		if(anstimeout==0)
			anstimeout=VDP_PHONE_FOR_1STBOOT_READY_TIMEOUT;
		TRY
		{
			if(m_pAnswer!=NULL)
				ansbuf=new BYTE[m_AnsLen+16];
			int ZeroRd=0;
			DWORD len;
			tim=0;
			bOk=TRUE;
			if(o_AutoignitionType!=AUTOIGN_NONE)
			{
				tim=SetTimerEventAfter(500);				//автоигнишин через диод должен сработать раньше чем через пол сек.
				EscapeCommFunction(m_pDev->m_hCom, SETDTR);	// подаем питание для зарядки, и сразу
				for(;;)
				{
					LoadToPhone_SendBoot();					//шлем запрос на коннект к бутлоадеру.
					if(IsTimerEvent(tim))					//если время автоигнишина через диод вышло
						break;								// - пробуем по другому
					if(m_pAnswer==NULL)
					{
						tim=0;
						break;
					}
					if((len=m_pDev->CommRead(ansbuf, m_AnsLen, anstimeout))==0) // и дальше читаем ответ, сравниваем, и так до тех пор пока не законнектимся с бутлоадером
						continue;
					if(len>=m_AnsLen)
					{
						if(memcmp(ansbuf, m_pAnswer, m_AnsLen)==0)
						{
							tim=0;
							break;
						}
					}
					if(len==1 && ansbuf[0]==0)
						ZeroRd++;
					if(m_pDev->IsCancel())
						AfxThrowUserException();
					if(trycount!=-1)
						trycount--;
					if(!trycount)
					{
						bOk=FALSE;
						break;
					}
				}
			}
			if(bOk && (tim!=0 || o_AutoignitionType==AUTOIGN_NONE))
			{
				if(o_AutoignitionType==AUTOIGN_AUTO || o_AutoignitionType==AUTOIGN_ORIGINAL)
				{
					EscapeCommFunction(m_pDev->m_hCom, SETDTR);	//выключаем зарядку
					if(ZeroRd!=1)
						m_pDev->CommRead(ansbuf, 1, 1500);		//ждем пока выключится (при этом прийдет 1 байт = 0?)
					tim=SetTimerEventAfter(500);				//будем пытаться автоигнишиться по семёновски пол секунды.
					EscapeCommFunction(m_pDev->m_hCom, CLRDTR);	//включаем зарядку, и сразу:
					for(;;)
					{
						LoadToPhone_SendBoot();					//шлем запрос на коннект к бутлоадеру.
						if(IsTimerEvent(tim))					//если время автоигнишина по семёновски вышло
							break;								// - будем пытаться автоигнищится через диод
						if(m_pAnswer==NULL)
						{
							tim=0;
							break;
						}
						if((len=m_pDev->CommRead(ansbuf, m_AnsLen, anstimeout))==0)
							continue;							// если ничего не прочли - запрашиваем заново
						if(len>=m_AnsLen)						//проверяем че прочли, если то шо нада - все ок.
						{
							if(memcmp(ansbuf, m_pAnswer, m_AnsLen)==0)
							{
								tim=0;
								break;
							}
						}
						if(len==1 && ansbuf[0]==0)				//если прочли не то что надо, но прочелся один нолик (что может говорить о том что напряжение зарядки вырубилось,
							break;								// значит скорее всего автоигнишин у нас не семёновский, а обычный - через диод, тогда идем и автоигнишимся через диод.
						if(m_pDev->IsCancel())
							AfxThrowUserException();
						if(trycount!=-1)
							trycount--;
						if(!trycount)
						{
							bOk=FALSE;
							break;
						}
					}
				}
				if(bOk && (tim!=0 || o_AutoignitionType==AUTOIGN_NONE))	//если автоигнишина по семёновски не вышло (или не надо было)
				{												// будем использовать юзера как автоигнишин.
					if(pProgress)								//Говорим ему об этом...
					{											//
						pProgress->SetStatus(MSTR("Connecting... Shortly press Power button or try to disconnect then connect phone again, when it is off."));
						pProgress->m_bFlashWindow=TRUE;
					}
					EscapeCommFunction(m_pDev->m_hCom, o_bCommDTR ? SETDTR : CLRDTR);	// подаем питание для зарядки, и сразу
					EscapeCommFunction(m_pDev->m_hCom, o_bCommRTS ? SETRTS : CLRRTS);

					for(;;)
					{
						LoadToPhone_SendBoot();					//шлем запрос на коннект к бутлоадеру.
						if(m_pAnswer==NULL)
							break;
						if((len=m_pDev->CommRead(ansbuf, m_AnsLen, anstimeout))==0) // и дальше читаем ответ, сравниваем и так до тех пор пока не законнектимся с бутлоадером
							continue;
						if(len>=m_AnsLen)
							if(memcmp(ansbuf, m_pAnswer, m_AnsLen)==0)
								break;
						if(m_pDev->IsCancel())
							AfxThrowUserException();
						if(trycount!=-1)
							trycount--;
						if(!trycount)
						{
							bOk=FALSE;
							break;
						}
					}
					if(pProgress)
						pProgress->m_bFlashWindow=FALSE;
				}
			}
			if(ansbuf!=NULL)
				delete ansbuf;
			ansbuf=NULL;
			EscapeCommFunction(m_pDev->m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
			EscapeCommFunction(m_pDev->m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
		}
		CATCH_ALL(e)
		{
			if(ansbuf!=NULL)
				delete ansbuf;
			ansbuf=NULL;
			THROW_LAST();
		}
		END_CATCH_ALL;
	}
	else
	{
		//sending a boot without ignition (ordinal boot).
		if(anstimeout==0)
			anstimeout=VDP_BOOT_ANS_READ_TIMEOUT;
		while(!bOk && trycount)
		{
			if(m_pData!=NULL)
			{
				//if(!PurgeComm(m_pDev->m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
				//	AfxThrowFileException(CFileException::OsErrorToException(GetLastError()),
				//		GetLastError(), m_pDev->m_ComPort);
				LoadToPhone_SendBoot();
			}
			if(m_pAnswer!=NULL)
			{
				if(m_pDev->DeviceWaitCmdAnswer(m_pAnswer, m_AnsLen, anstimeout))
					bOk=TRUE;
			}
			else
				bOk=TRUE;
			if(trycount!=-1)
				trycount--;
		}
	}

//if answer is not received - throw an error exception.
	if(!bOk)
		AfxThrowFileException(CFileException::OsErrorToException(ERROR_NOT_READY),
			ERROR_NOT_READY, m_pDev->m_ComPort+_T(" - Loader: Open: No answer from boot ")+m_Name);

//play sound after sending boot, if needed.
	if(!m_SndAfter.IsEmpty())
		SndMsg(m_SndAfter, 1);

//wait some time after sending boot, if needed.
	m_pDev->Delay(m_DelayAfter);

	return TRUE;
}

void VPhoneBoot::ClearInfo(void)
{
	if(m_pData!=NULL)
		delete m_pData;
	if(m_pAnswer!=NULL)
		delete m_pAnswer;
	Init();
}

VPhoneBoot::~VPhoneBoot(void)
{
	ClearInfo();
}




////////////////////////////////////////////////////////////////////////////////////


CString VDevicePhone::m_strReadingStat;
CString VDevicePhone::m_strReadingStatOk;
CString VDevicePhone::m_strWritingStat;
CString VDevicePhone::m_strWritingStatOk;
CString VDevicePhone::m_strTstIsEmptyStat;

VDevicePhone::VDevicePhone(void)
{
	m_LdrOpt.m_pDev=this;
	m_LdrOpt.Init();
	m_Cache.Init();

	m_Type=VDT_PHONE;
	m_strReadingStat.Format(MSTR("Reading block from address: %s, size: %s ..."), addr64format, addr64format);
	m_strReadingStat+=_TCHAR(' ');
	m_strWritingStat.Format(MSTR("Writing block to address: %s, size: %s ..."), addr64format, addr64format);
	m_strWritingStat+=_TCHAR(' ');
	m_strReadingStatOk=m_strWritingStatOk=MSTR("Ok.");
	m_strTstIsEmptyStat.Format(MSTR("Testing, is block empty at address: %s, size: %s ..."), addr64format, addr64format);
	m_strTstIsEmptyStat+=_TCHAR(' ');

	m_hCom=INVALID_HANDLE_VALUE;
	m_bFlushNeeded=FALSE;
	memset(&m_oWrite, 0, sizeof(m_oWrite));
	m_oWrite.hEvent=NULL;
	memset(&m_oRead, 0, sizeof(m_oRead));
	m_oRead.hEvent=NULL;
	m_CommTimeouts.ReadIntervalTimeout=-2;
	m_CommTimeouts.ReadTotalTimeoutConstant=-2;
	m_CommTimeouts.ReadTotalTimeoutMultiplier=-2;
	m_CommTimeouts.WriteTotalTimeoutConstant=-2;
	m_CommTimeouts.WriteTotalTimeoutMultiplier=-2;
	m_LoaderReadMemoryLastGoodPageSize=VDP_READ_PAGE_SIZE_START;

	m_ComBaud=0;

	m_bEnableNoWrite=m_bSkipWritingBootCore=VDP_ASK_USER;

	memset(&m_PhoneInfo, 0, sizeof(m_PhoneInfo));
	m_PhoneInfoLen=0;

	m_MemAreaStartAddrLdr=0;

	m_bLdrStateLoaded=FALSE;

	m_LoaderReadMemoryTmpBufLen=65536;
	m_LoaderReadMemoryTmpBuf=new BYTE[m_LoaderReadMemoryTmpBufLen];
	if(m_LoaderReadMemoryTmpBuf==NULL)
	{
		AfxMessageBox(_T("VDevicePhone::VDevicePhone: Memory allocation error."));
		ExitProcess(-1);
	}
}

VDevicePhone::~VDevicePhone(void)
{
	CloseDevice();
	Destroy();
	if(m_LoaderReadMemoryTmpBuf!=NULL)
	{
		delete m_LoaderReadMemoryTmpBuf;
		m_LoaderReadMemoryTmpBufLen=0;
	}
}

// open the device
BOOL VDevicePhone::Open(void)
{
	m_LoaderReadMemoryLastGoodPageSize=VDP_READ_PAGE_SIZE_START;
	m_bEnableNoWrite=m_bSkipWritingBootCore=VDP_ASK_USER;
	if(!VDevice::Open())
		return FALSE;
	return OpenDevice();
}

// open the file with loader for current phone.
BOOL VDevicePhone::OpenCurBootFile(void)
{
	if(m_bFlushNeeded)
	{
		AfxMessageBox(_T("Reading phone driver file: Cannot load new phone driver file while working with current driver!"));
		return FALSE;
	}
	m_PhoneName.Empty();
	m_PhoneType.Empty();
	m_MemArea.RemoveAll();
	m_MemAreaStart=0;
	m_MemAreaStartAddrLdr=0;
	m_Boot.RemoveAll();
	m_LdrOpt.Init();
	m_Cache.Init();
	m_Comments.Empty();
	m_Copyright.Empty();

	CString str;
	CWnd *dlg=AfxGetMainWnd();
	if(dlg==NULL)
		return FALSE;
	CComboBox *cb=(CComboBox *)dlg->GetDlgItem(IDC_PHONES_LIST);
	if(cb==NULL)
		return FALSE;
	int i=cb->GetCurSel();
	if(i==-1)
		return TRUE;
	if(i==CB_ERR)
		return FALSE;
	VPHONE_INFO *pi=(VPHONE_INFO*)cb->GetItemDataPtr(i);
	if(pi==NULL)
		return TRUE;
	INT_PTR n;

	//reading phone info:
	CString phoneInfoSec=drvsecPhoneCommonInfo;
	if(!LoadPhoneInfo(phoneInfoSec, pi->fileName))
	{
		AfxMessageBox(CString(_T("Reading phone driver file: error reading phone info section:")) + phoneInfoSec);
		return FALSE;
	}
	for(i=1; i <= pi->num; i++)
	{
		phoneInfoSec.Format(drvsecPhone, i);
		if(!LoadPhoneInfo(phoneInfoSec, pi->fileName))
		{
			AfxMessageBox(CString(_T("Reading phone driver file: error reading phone info section:")) + phoneInfoSec);
			return FALSE;
		}
	}

	//check all phone infos on errors:
	if(m_MemArea.GetSize()==0)
	{
		AfxMessageBox(CString(_T("Reading phone driver file: it is not exist a critical parameter:")) + drvsecMemFF);
		return FALSE;
	}

	//if(m_PageSize==-1)
	//{
	//	m_PageSize=VD_DEFAULT_PAGE_SIZE;
	//	str.Format(addrformat, VD_DEFAULT_PAGE_SIZE);
	//	AfxMessageBox(CString(_T("Reading phone driver file: WARNING! The maximal size of flash block is not found!\nThe default value of this parameter is set ("))
	//		+ str + _T(").\nTo eliminate this warning add to phone driver file in section [")
	//		+ phoneInfoSec +_T("] parameter ") + drvkeyOptBlockSize
	//		+ _T("=_max_size_of_flash_block_in_bytes_"));
	//}

	//loading data of boots:
	n=m_Boot.GetSize();
	for(i=0; i<n; i++)
	{
		if(!m_Boot[i].ReadInfo(this, pi->fileName))
		{
			AfxMessageBox(CString(_T("Reading phone driver file: Error loading boot:\n"))
				+ m_Boot[i].m_Name);
			return FALSE;
		}
	}

	return TRUE;
}

// Load the all Phone info, what is exist in specified section of driver file
BOOL VDevicePhone::LoadPhoneInfo(LPCTSTR lpstrPhoneInfoSec, LPCTSTR lpstrDrvFile)
{
	m_PhoneName=VGetPrivateProfileString(lpstrPhoneInfoSec, drvkeyName, m_PhoneName, lpstrDrvFile);
	m_PhoneType=VGetPrivateProfileString(lpstrPhoneInfoSec, drvkeyType, m_PhoneType, lpstrDrvFile);
	m_Comments=VGetPrivateProfileString(drvsecPhoneCommonInfo, drvkeyComments, m_Comments, lpstrDrvFile);
	m_Copyright=VGetPrivateProfileString(drvsecPhoneCommonInfo, drvkeyCopyright, m_Copyright, lpstrDrvFile);

	if(!m_Cache.ParseParameters(
		VGetPrivateProfileString(lpstrPhoneInfoSec, drvkeyMemGeometry, NULL, lpstrDrvFile)))
		return FALSE;

	if(!m_LdrOpt.Load(lpstrPhoneInfoSec, lpstrDrvFile))
		return FALSE;

	CString str;
	int i, p;
	INT_PTR n;
	AreaInf ai;
	BOOL IsExist;
	VGetPrivateProfileHex(lpstrPhoneInfoSec, drvkeyMemFlashBase, 0, lpstrDrvFile, &IsExist);
	str=VGetPrivateProfileString(lpstrPhoneInfoSec, drvsecMemFF, NULL, lpstrDrvFile);
	if(IsExist || ai.ParseParameters(str))
	{
		m_MemArea.RemoveAll();
		m_MemAreaStart=0;
		m_MemAreaStartAddrLdr=0;
		m_MemArea.Add(AreaInf());
		if(m_MemArea[VDA_FULLFLASH].ParseParameters(
			VGetPrivateProfileString(lpstrPhoneInfoSec, drvsecMemFF, NULL, lpstrDrvFile)))
		{
			m_MemAreaStart=m_MemAreaStartAddrLdr=m_MemArea[VDA_FULLFLASH].Address;
			for(i=1; ; i++)
			{
				str.Format(drvsecMemArea, i);
				n=m_MemArea.Add(AreaInf());
				if(!m_MemArea[n].ParseParameters(
					VGetPrivateProfileString(lpstrPhoneInfoSec, str, NULL, lpstrDrvFile)))
				{
					m_MemArea.RemoveAt(n);
					break;
				}
			}
			m_MemAreaStartAddrLdr=(DWORD)VGetPrivateProfileHex(lpstrPhoneInfoSec, drvkeyMemFlashBase,
				m_MemAreaStartAddrLdr, lpstrDrvFile);
		}
		else
			m_MemArea.RemoveAll();
	}


	CString boots=VGetPrivateProfileString(lpstrPhoneInfoSec, drvkeyBoots, NULL, lpstrDrvFile);
	if(!boots.IsEmpty())
	{
		m_Boot.RemoveAll();
		for(i=1, p=0; ; i++)
		{
			str=VGetParameterFromList(boots, p);
			if(str.IsEmpty())
				break;
			n=m_Boot.Add(VPhoneBoot());
			m_Boot[n].m_Name=str;
		}
	}
	return TRUE;
}

// open phone
BOOL VDevicePhone::OpenDevice(BOOL bOpenOnlyComPort/*=FALSE*/)
{
	BOOL bNoLoader=IsSkipLoaderLoadUnload();
	DWORD len, baud;
	DCB ComState;
	CString str, name;
	CDialog *dlg;
	BOOL isloaderready;
	BOOL ldrUploadTryCounter = 0;
	VTIMER tim;
	CComboBox *lpcb;

	VDCSec CriticalSectionOfDevice(this);		//this critical section will be released by CloseDevice() in the TRY block below.

	TRY
	{
LoaderUpload:
		m_LoaderReadMemoryLastGoodPageSize=VDP_READ_PAGE_SIZE_START;
		m_bEnableNoWrite=m_bSkipWritingBootCore=VDP_ASK_USER;

		if(pProgress)
			pProgress->SetStatus(MSTR("Connecting... Wait, please."));
		dlg=(CDialog*)AfxGetMainWnd();
		if(dlg==NULL)
			AfxThrowUserException();
		dlg->GetDlgItemText(IDC_PHONE_PORTS_LIST, str);
		str.TrimLeft();
		str.TrimRight();
		lpcb=(CComboBox*)dlg->GetDlgItem(IDC_PHONES_LIST);
		lpcb->GetWindowText(name);
		len=lpcb->GetCurSel();
		if(name.IsEmpty() || len==-1 || lpcb->GetItemData(len)==NULL)
		{
			AfxMessageBox(MSTR("Select the phone model first."));
			AfxThrowUserException();
		}
		baud=dlg->GetDlgItemInt(IDC_PHONE_BAUDS_LIST);
		if(m_hCom!=INVALID_HANDLE_VALUE && !m_ComPort.IsEmpty() && m_ComPort==str &&
			!m_Name.IsEmpty() && m_Name==name && m_ComBaud!=0 && m_ComBaud==baud)
		{
			if(bOpenOnlyComPort)
				goto Exit_ok;
			else if(LoaderIsReady())
			{
				SetLdrStateLoaded();
				goto Exit_ok;
			}
		}
		CloseDevice();


		if(!bOpenOnlyComPort)
			if(!OpenCurBootFile())
				AfxThrowUserException();

		m_Name=name;
		m_ComPort=str;
		m_ComBaud=baud;
		if(str.ReverseFind(_T('\\'))==-1)
			str=CString("\\\\.\\")+str;
		m_hCom=CreateFile(str, GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, o_bCommSimple ? FILE_ATTRIBUTE_NORMAL : FILE_FLAG_OVERLAPPED, NULL);
		if(m_hCom==INVALID_HANDLE_VALUE)
			ThrowOpenException();


		if(!SetupComm(m_hCom, 0x20000, 0x20000 /*66000, 66000*/))
			ThrowOpenException();
		m_CommTimeouts.ReadIntervalTimeout=0;							//MAXDWORD;
		m_CommTimeouts.ReadTotalTimeoutMultiplier=VDP_MESSAGE_TIMEOUT;	//MAXDWORD;
		m_CommTimeouts.ReadTotalTimeoutConstant=5*60*1000;
		m_CommTimeouts.WriteTotalTimeoutMultiplier=0;
		m_CommTimeouts.WriteTotalTimeoutConstant=0;
		if(!SetCommTimeouts(m_hCom, &m_CommTimeouts))
			ThrowOpenException();
		ComState.DCBlength=sizeof(DCB);
		if(!GetCommState(m_hCom, &ComState))
			ThrowOpenException();
		if(bOpenOnlyComPort || bNoLoader || m_LdrOpt.m_IsBaudCmdDis)
			ComState.BaudRate=m_ComBaud;
		else if(m_ComBaud>115200)
            ComState.BaudRate=115200;
		else
            ComState.BaudRate=m_ComBaud;				// current baud rate
		ComState.ByteSize=8;							// number of bits/byte, 4-8
		ComState.Parity=0;								// 0-4=no,odd,even,mark,space
		ComState.StopBits=ONESTOPBIT;					// 0,1,2 = 1, 1.5, 2
		ComState.fBinary=TRUE;							// binary mode, no EOF check
		ComState.fParity=FALSE;							// enable parity checking
		ComState.fOutxCtsFlow=FALSE;					// CTS output flow control
		ComState.fOutxDsrFlow=FALSE;					// DSR output flow control
		ComState.fDsrSensitivity=FALSE;					// DSR sensitivity
		ComState.fDtrControl=DTR_CONTROL_DISABLE;		// DTR flow control type
		ComState.fRtsControl=RTS_CONTROL_ENABLE;		// RTS flow control
		ComState.fTXContinueOnXoff=FALSE;				// XOFF continues Tx
		ComState.fOutX=FALSE;							// XON/XOFF out flow control
		ComState.fInX=FALSE;							// XON/XOFF in flow control
		ComState.fErrorChar=FALSE;						// enable error replacement
		ComState.fNull=FALSE;				 			// enable null stripping
		ComState.fAbortOnError=FALSE;					// abort reads/writes on error
		ComState.XonLim=2048;							// transmit XON threshold
		ComState.XoffLim=512;							// transmit XOFF threshold
		ComState.XonChar=0x11;							// Tx and Rx XON character
		ComState.XoffChar=0x13;							// Tx and Rx XOFF character
		ComState.ErrorChar=0;							// error replacement character
		ComState.EofChar=(char)0;						// end of input character
		ComState.EvtChar=0;								// received event character
		if(!SetCommState(m_hCom, &ComState))
			ThrowOpenException();
		//if(!SetCommMask(m_hCom, EV_RXCHAR | EV_ERR))
		//	ThrowOpenException();
		if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
			ThrowOpenException();

//BYTE *bufff=new BYTE[1024*1024];
//LoaderWriteMemory(0x1FE0000, 0x20000, bufff);
//LoaderReadMemory(0x1FE0000, 0x20000, bufff);
//RestoreBootcore();
//VDevMemMapArray mm;
//GetMemoryMap(mm);

		if(!bOpenOnlyComPort)
		{
			isloaderready=FALSE;
			if(bNoLoader)
			{
				if(o_bCommDTR!=FALSE)
					EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
				if(o_bCommRTS!=TRUE)
					EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
				for(tim=SetTimerEventAfter(100); !IsTimerEvent(tim);)	//wait 0.1 sec.
					if(IsCancel())
						AfxThrowUserException();
				isloaderready=LoaderIsReady(VDP_PHONE_FOR_1STBOOT_READY_TIMEOUT);
			}
			if(!isloaderready)
			{
				if(bNoLoader)
				{
					bNoLoader=FALSE;
					if(m_LdrOpt.m_IsBaudCmdDis)
						ComState.BaudRate=m_ComBaud;
					else if(m_ComBaud>115200)
						ComState.BaudRate=115200;
					else
						ComState.BaudRate=m_ComBaud;				// current baud rate
					if(!SetCommState(m_hCom, &ComState))
						ThrowOpenException();
					for(tim=SetTimerEventAfter(2000); !IsTimerEvent(tim);)	//wait 2 sec.
						if(IsCancel())
							AfxThrowUserException();
					if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
						ThrowOpenException();
				}
				if(pProgress)
					pProgress->SetStatus(MSTR("Connected. Sending boots..."));
//str.Format(_T("%2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X %2.2X "),
//	m_Boot[2].m_pData[0x40], m_Boot[2].m_pData[0x41], m_Boot[2].m_pData[0x42], m_Boot[2].m_pData[0x43], 
//	m_Boot[2].m_pData[0x44], m_Boot[2].m_pData[0x45], m_Boot[2].m_pData[0x46], m_Boot[2].m_pData[0x47], 
//	m_Boot[2].m_pData[0x48], m_Boot[2].m_pData[0x49], m_Boot[2].m_pData[0x4a], m_Boot[2].m_pData[0x4b], 
//	m_Boot[2].m_pData[0x4c], m_Boot[2].m_pData[0x4d], m_Boot[2].m_pData[0x4e], m_Boot[2].m_pData[0x4f]
//	);
//AfxMessageBox(str);
				TRY
				{
					for(int i=0; i<m_Boot.GetSize(); i++)
						m_Boot[i].LoadToPhone();
				}
				CATCH(CFileException, e)
				{
					ldrUploadTryCounter++;
					if(m_LdrOpt.m_LoaderUploadTryCount != -1
						&& ((DWORD)ldrUploadTryCounter) >= ((DWORD)m_LdrOpt.m_LoaderUploadTryCount))
					{
						THROW_LAST();
					}
					else
					{
						pProgress->SetStatus(MSTR("Error while uploading loader... Waiting before next try..."));
						if(m_hCom!=INVALID_HANDLE_VALUE)
						{
							EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
							EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
						}
						CloseDevice();

						if(!m_LdrOpt.m_LoaderUploadErrorSnd.IsEmpty())
							SndMsg(m_LdrOpt.m_LoaderUploadErrorSnd, 3);

						Delay(m_LdrOpt.m_LoaderUploadDelay);

						goto LoaderUpload;
					}
				}
				END_CATCH;
				if(!m_LdrOpt.m_LoaderUploadOkSnd.IsEmpty())
					SndMsg(m_LdrOpt.m_LoaderUploadOkSnd, 1);

				EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
				EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
				DeviceSkipData();
				if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
					ThrowOpenException();
			}
			else
			{
				EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
				EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
			}

			if(!LoaderIsReady())
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_NOT_READY),
					ERROR_NOT_READY, m_ComPort+_T(" - Loader: Open: Loader not ready"));

			if(!bNoLoader)
				LoaderSetConnectionSpeed(m_ComBaud);

			SetLdrStateLoaded();

			if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
				ThrowOpenException();

			LoaderReadFlashInfo();
			dlg->SetDlgItemText(IDC_PHONE_FLASH_INFO, MakeFlashInfoString());

			for(int i=0; i < VDP_READ_INFO_DECODED_DATA_TRY_COUNT && !m_Cache.IsParametersValid(); i++)
			{
				ThrowExceptionIfLoaderNotReady();
				LoaderReadFlashInfo();
				dlg->SetDlgItemText(IDC_PHONE_FLASH_INFO, MakeFlashInfoString());
			}
			if(!m_Cache.IsParametersValid())
				ThrowCommException(_T("Unknown geometry of ROM (flash) in the phone. Specify it in the parameter optMemGeometry in .vkd file for this phone or use loader that support returning such info in \"Get phone information\" command."));

			LoaderAuthorization();

			ThrowExceptionIfLoaderNotReady();

			IsPhoneNewAndSaveFuBu();

		}
		else
		{
			EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
			EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
		}

	}
	CATCH_ALL(e)
	{
		if(m_hCom!=INVALID_HANDLE_VALUE)
		{
			EscapeCommFunction(m_hCom, o_bCommDTR ? SETDTR : CLRDTR);
			EscapeCommFunction(m_hCom, o_bCommRTS ? SETRTS : CLRRTS);
		}
		CloseDevice();
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
			e->ReportError();
		return FALSE;
	}
	END_CATCH_ALL;

Exit_ok:
	SaveOptions();
	if(pProgress)
	{
		pProgress->SetStatus(MSTR("Working with phone. Please wait..."));
		pProgress->BeginPerformanceMeasure();
	}

	return TRUE;
}

void VDevicePhone::ThrowOpenException(LPCTSTR msg/*=NULL*/)
{
	DWORD err=GetLastError();
	CloseHandle(m_hCom);
	m_hCom=INVALID_HANDLE_VALUE;
	SetLastError(err);
	ThrowCommException(msg);
}

#define ERR_MSG_BUFF_LEN	2048
_TCHAR errMsgBuff[ERR_MSG_BUFF_LEN+1];

void VDevicePhone::ThrowCommException(LPCTSTR msg/*=NULL*/, BOOL bThrowException/*=TRUE*/)
{
	DWORD err=GetLastError();
	size_t i=0;
	int n=m_ComPort.GetLength();
	if(i+n < ERR_MSG_BUFF_LEN)
	{
		_tcsncpy(errMsgBuff+i, m_ComPort, n);
		i+=n;
	}
	if(i<ERR_MSG_BUFF_LEN && msg!=NULL)
	{
		if(i+1 < ERR_MSG_BUFF_LEN)
			errMsgBuff[i++]=_TCHAR('\n');
		int len=(int)min(_tcsclen(msg), ERR_MSG_BUFF_LEN-i);
		_tcsncpy(errMsgBuff+i, msg, len);
		i+=len;
	}
	if(err!=0 && i+1+n+2 < ERR_MSG_BUFF_LEN)
	{
		errMsgBuff[i++]=_TCHAR('\n');
		_tcsncpy(errMsgBuff+i, m_ComPort, n);
		i+=n;
		errMsgBuff[i++]=_TCHAR(':');
		errMsgBuff[i++]=_TCHAR(' ');

		if(i < ERR_MSG_BUFF_LEN)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				errMsgBuff+i, DWORD(ERR_MSG_BUFF_LEN-i),   NULL );
			i=_tcsclen(errMsgBuff);
		}
	}
	errMsgBuff[i]=0;

	if(err==0)
		err=1;
	if(bThrowException)
		AfxThrowFileException(CFileException::OsErrorToException(err),
			err, errMsgBuff);
}

// Set the speed of connection between PC and Phone
BOOL VDevicePhone::LoaderSetConnectionSpeed(DWORD Baud, BOOL bOnlyForComPort/*=FALSE*/)
{
	if(m_LdrOpt.m_IsBaudCmdDis && !bOnlyForComPort)
		return TRUE;

	DWORD len;
	CString num;
	num.Format(uintformat, Baud);
	CString str, strError(_T(" - Loader: SetConnectionSpeed:"));
	CString strCantCom(MSTR("The COM Port speed %s bps cannot be set.\nTry to select other speed."));
	CString strCantPhone(MSTR("Phone do not support speed %s bps.\nTry to select other speed."));
	CString strCancelling(MSTR("Setting communication speed failed. Cancelling..."));
	CString strSet(MSTR("Trying to set communication speed..."));
	BYTE buf[max(vdpCmdLoaderSetBaudParametersOkLen, vdpCmdLoaderSetBaudOkLen)];

	DCB ComState;
	ComState.DCBlength=sizeof(DCB);
	if(!GetCommState(m_hCom, &ComState))
		ThrowCommException();
	DWORD oldBaud=ComState.BaudRate;
	ComState.BaudRate=Baud;
	ComState.fDtrControl=o_bCommDTR ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE;		// DTR flow control type
	ComState.fRtsControl=o_bCommRTS ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE;		// RTS flow control

	VDCSec CriticalSectionOfDevice(this);

	if(!bOnlyForComPort)
	{
		if(pProgress)
			pProgress->SetStatus(strSet);
		DWORD baudCode=m_LdrOpt.m_LdrCmdBaudsCodes.GetCode(Baud);
		if(baudCode==-1)
		{
			if(Baud <= 115200)
				return TRUE;
			else
			{
				str.Format(strCantPhone, num);
				AfxMessageBox(str);
				if(pProgress)
					pProgress->SetStatus(strCancelling);
				AfxThrowUserException();
			}
		}

		DeviceSendCommand(vdpCmdLoaderSetBaud);

		CommWrite(&baudCode, 1);

		len=CommRead(buf, vdpCmdLoaderSetBaudParametersOkLen, VDP_MEMORY_READ_TIMEOUT);
		if(len<vdpCmdLoaderSetBaudParametersOkLen)
		{
			str.Format(_T(" Unknown answer on write parameter readed: len=%i, but must be %i."), len, vdpCmdLoaderSetBaudParametersOkLen);
			strError+=str;
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
				ERROR_INVALID_FUNCTION, m_ComPort+strError);
		}
		if(memcmp(buf, vdpCmdLoaderSetBaudParametersOk, vdpCmdLoaderSetBaudParametersOkLen)!=0)
		{
			str.Format(_T(" Unknown answer on write parameter readed: %c, but must be %c."), (TCHAR)buf[0], (TCHAR)vdpCmdLoaderSetBaudParametersOk[0]);
			strError+=str;
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
				ERROR_INVALID_FUNCTION, m_ComPort+strError);
		}
	}

	if(!SetCommState(m_hCom, &ComState))
	{
		if(!bOnlyForComPort)
		{
			ComState.BaudRate=oldBaud;
			str.Format(strCantCom, num);
			goto seterr;
		}
		else
			ThrowCommException();
	}
	if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		ThrowCommException();

	if(!bOnlyForComPort)
	{
		CommWrite(vdpCmdLoaderBaudIsSet, vdpCmdLoaderBaudIsSetLen);

		len=CommRead(buf, vdpCmdLoaderSetBaudOkLen, VDP_MEMORY_READ_TIMEOUT);
		if(len<vdpCmdLoaderSetBaudOkLen || memcmp(buf, vdpCmdLoaderSetBaudOk, vdpCmdLoaderSetBaudOkLen)!=0)
		{
			ComState.BaudRate=115200;
			str.Format(strCantPhone, num);
seterr:
			VTIMER tim=SetTimerEventAfter(5000);    //wait 5 sec.
			AfxMessageBox(str);
			if(pProgress)
				pProgress->SetStatus(strCancelling);
			while(!IsTimerEvent(tim) && !IsCancel());	
			SetCommState(m_hCom, &ComState);
			PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
			AfxThrowUserException();
		}
		if(pProgress)
			pProgress->SetStatus(strSet+m_strWritingStatOk);
	}

	return TRUE;
}

//Checking is phone is flashing first time and save fullflash backup if new phone.
BOOL VDevicePhone::IsPhoneNewAndSaveFuBu(void)
{
	if(o_bIsDisableFubuAutosave)
		return FALSE;

	VDCSecSaveState SaveCriticalSectionStateOfDevice(this);
	VDCSec::Reset(this);			// allow to send keep alive packets.

	BOOL isPhoneNew = TRUE;
	DWORD PhoneID0, PhoneID1;
	UINT64 bcdIMEI;
	MakeFlashInfoString(NULL, NULL, NULL, NULL, &bcdIMEI, &PhoneID0, &PhoneID1);
	if(bcdIMEI==0 || bcdIMEI==-1)
		bcdIMEI=((((unsigned __int64)PhoneID1)<<32)|PhoneID0);
	CKLayApp *app = (CKLayApp*)AfxGetApp();
	if(app == NULL)
		AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
			ERROR_INVALID_FUNCTION, _T("IsNewPhone: err app"));
	UINT64 *pbcdIMEIreaded;
	UINT len;
	int i;
	CString str;
	for(i = 0; ; i++)
	{
		str.Format(vdpkeyUsedPhone, i);
		if(!app->GetProfileBinary(vdpsec, str, (LPBYTE*)&pbcdIMEIreaded, &len))
		{
			isPhoneNew = TRUE;
			break;
		}
		if(*pbcdIMEIreaded == bcdIMEI)
		{
			isPhoneNew = FALSE;
			break;
		}
		delete [] pbcdIMEIreaded;
	}
	if(isPhoneNew)
	{
		CString vdpsecForCurPhone = str;
		if(pProgress != NULL)
		{
			TRY
			{
				pProgress->SetStatus(MSTR("New phone detected... Preparing to make fullflash backup (fubu)..."));
				Delay(5000);
			}
			CATCH(CUserException, e)
			{
				isPhoneNew = FALSE;
			}
			END_CATCH;
		}
//		if(AfxMessageBox(MSTR("!!! WARNING !!!\n\nThis phone is connected for the first time!\n\nIT IS STRONGLY RECOMMENDED, THAT YOU MAKE A FULLFLASH BACKUP (FUBU) OF THE PHONE FLASH!\nWithout fullflash backup you can't repair this phone in future if something going wrong!\nBut with fullflash backup, in most cases, this phone can be repaired.\n\nDo you want to make a fullflash backup now (answer YES - is recommended)?"),
//			MB_YESNO | MB_DEFBUTTON1 | MB_ICONSTOP) == IDYES)
		if(isPhoneNew)
		{
			if(m_MemArea.GetSize() <= VDA_FULLFLASH)
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, _T("IsNewPhone: no fubu info"));
			DWORD len = (DWORD)m_MemArea[VDA_FULLFLASH].Size;
			BYTE *pData = new BYTE[len + 1024];
			if(pData == NULL)
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, _T("IsNewPhone: err mem alloc"));
			str = app->m_AppPath + _T("backup\\");
			CreateDirectory(str, NULL);
			str += GetDefaultFlashFileName(0, GetUniqueName());
			CFile ft(str, CFile::typeBinary | CFile::modeWrite | CFile::modeCreate);
			pData[0] = 0xff;
			ft.Write(pData, 1);						//we cannot write len bytes because in some cases it take a lot of time and KeepAlive packets is not sended during this time.
			ft.Close();
			if(!DeleteFile(str))
				ThrowCommException();
			BOOL oldUnkUpper;
			INT64 oldUpper;
			BOOL oldIsShowInf;
			if(pProgress != NULL)
			{
				oldIsShowInf = pProgress->m_IsShowSpeed;
				if(!oldIsShowInf)
					pProgress->ShowPerformanceInfo();
				oldUnkUpper = pProgress->m_bUnknownUpper;
				oldUpper = pProgress->SetUpper(len);
			}
			if(!Read(0, len, pData))
				AfxThrowUserException();
			CFile f(str, CFile::typeBinary | CFile::modeWrite | CFile::modeCreate);
			f.Write(pData, len);
			f.Close();
			CString msg;
			str = CString(_T('\n')) + str + CString(_T('\n'));
//			msg.Format(MSTR("Fullflash backup (fubu) is successfuly saved to file: %s In case of problems with this phone write saved fullflash backup to it with help of \"Flasher\" tab."),
//				str);
			msg.Format(MSTR("This phone is connected for the first time!\nThe fullflash backup (fubu) of phone flash was successfully read and saved in file:\n%s\nIn case of problems with this phone write saved fullflash backup to it with help of \"Flasher\" tab.\n\nPress OK to continue operation."),
				str);
			AfxMessageBox(msg);
			app->WriteProfileBinary(vdpsec, vdpsecForCurPhone, (LPBYTE)&bcdIMEI, sizeof(bcdIMEI));
			if(pProgress != NULL)
			{
				pProgress->SetUpper(oldUpper);
				if(!oldIsShowInf)
					pProgress->ShowPerformanceInfo(FALSE);
				pProgress->m_bUnknownUpper = oldUnkUpper;
			}
		}
		else
		{
			isPhoneNew = FALSE;
		}
	}
	return isPhoneNew;
}

// Sent the Size field to the comm port
DWORD VDevicePhone::SendSizeComm(DWORD Size, int SizeFieldLength/*=0*/)
{
	if(SizeFieldLength==0)
	{
		for(SizeFieldLength=sizeof(Size)-1; SizeFieldLength>0 && ((BYTE*)&Size)[SizeFieldLength]==0; SizeFieldLength--);
		SizeFieldLength++;
	}
	CommWrite((BYTE*)&Size, SizeFieldLength);
	return SizeFieldLength;
}

// Close the device
BOOL VDevicePhone::Close(void)
{
	BOOL ret=Flush();
	m_Cache.ClearCache();
	VDevice::Close();
	m_bFlushNeeded=FALSE;
	m_LoaderReadMemoryLastGoodPageSize=VDP_READ_PAGE_SIZE_START;
	m_bEnableNoWrite=m_bSkipWritingBootCore=VDP_ASK_USER;
	VDCSec::Reset(this);

	if(pProgress!=NULL)
		pProgress->EndPerformanceMeasure();
	return ret;
}

// Closes the handler to comm port and other
BOOL VDevicePhone::CloseDevice(BOOL unloadLoader /*= TRUE*/)
{
	if(m_hCom!=INVALID_HANDLE_VALUE)
	{
		TRY{Abort();}CATCH_ALL(e){}END_CATCH_ALL;
		m_bFlushNeeded=FALSE;
		TRY{Close();}CATCH_ALL(e){}END_CATCH_ALL;
		PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
		if(unloadLoader)
		{
			TRY{LoaderStopLoader();}CATCH_ALL(e){}END_CATCH_ALL;
		}
		if(m_oRead.hEvent!=NULL)
		{
			CloseHandle(m_oRead.hEvent);
			m_oRead.hEvent=NULL;
		}
		if(m_oWrite.hEvent!=NULL)
		{
			CloseHandle(m_oWrite.hEvent);
			m_oWrite.hEvent=NULL;
		}
		if(unloadLoader)
		{
			CloseHandle(m_hCom);
			m_hCom=INVALID_HANDLE_VALUE;
		}
		m_LoaderReadMemoryLastGoodPageSize=VDP_READ_PAGE_SIZE_START;
	}

	VDCSec::Reset(this);
//	if(unloadLoader)
//	{
//		SetLdrStateLoaded(FALSE);
//	}
	return TRUE;
}

// read the data from device at specified address and size
BOOL VDevicePhone::Read(INT64 Address, INT64 Size, void* InputBuf)
{
	TRY
	{
		INT64 offs, len, bufrest;
		VDEVCACHE_PAGE_INF *page;
		BOOL ispageNew=TRUE;
		for(bufrest=Size; bufrest;)
		{
			page=m_Cache.GetCachePageAtAddr(Address, &ispageNew);
			offs=Address-page->addr;
			len=page->size-offs;
			if(ispageNew)
				LoaderReadMemory(page->addr, page->size, page->data);
			len=min(len, bufrest);
			memcpy(((BYTE*)InputBuf)+(Size-bufrest), page->data+offs, (size_t)len);
			Address+=len;
			bufrest-=len;
		}
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
		{
			CloseDevice();
			e->ReportError();
		}
		else
			CloseDevice(FALSE);
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// Write the data from buffer to the device at specified address and size
BOOL VDevicePhone::Write(INT64 Address, INT64 Size, void* InputBuf)
{
	TRY
	{
		INT64 offs, len, bufrest;
		BOOL bCanCompare;
		VDEVCACHE_PAGE_INF *page;
		BOOL ispageNew=TRUE;

//		page=VDP_GET_PAGE(Address);
//		len=VDP_GET_PAGE(Address+Size);
//		offs=VDP_GET_OFFSET(Address+Size);
//		if(offs!=0)
//			len++;
//		IsSkipWritingInBlock(VD_GET_ADDRESS(page,0), (len-page)*m_PageSize);

		for(bufrest=Size; bufrest;)
		{
			page=m_Cache.GetCachePageAtAddr(Address, &ispageNew);
			offs=Address-page->addr;
			len=page->size-offs;
			len=min(len, bufrest);
			bCanCompare=TRUE;
			if(ispageNew)
			{
				if(len==page->size && offs==0)
					bCanCompare=FALSE;
				else
					LoaderReadMemory(page->addr, page->size, page->data);
			}
			if(bCanCompare)
				bCanCompare= (memcmp(page->data+offs, ((BYTE*)InputBuf)+(Size-bufrest), (size_t)len) == 0);
			if(!bCanCompare)
			{
				memcpy(page->data+offs, ((BYTE*)InputBuf)+(Size-bufrest), (size_t)len);
				m_bFlushNeeded=TRUE;
				if(!page->isChanged)
					if(!IsSkipWritingInBlock(page->addr, page->size))
						page->isChanged=TRUE;
			}
			Address+=len;
			bufrest-=len;
		}
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
		{
			CloseDevice();
			e->ReportError();
		}
		else
			CloseDevice(FALSE);
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// Save the data, that is changed, to the device
BOOL VDevicePhone::Flush(void)
{
	TRY
	{
		if(m_bFlushNeeded)
		{
			//Write all cached data to the phone device.
			VDEVCACHE_PAGE_INF *pci;
			for(int i=0; i<m_Cache.GetCachedPagesNum(); i++)
			{
				pci=m_Cache.GetCachePage(i);
				if(pci->isChanged)
				{
					if(pci->data!=NULL)
						LoaderWriteMemory(pci->addr, pci->size, pci->data);
					pci->isChanged=FALSE;
				}
			}
		}

		VDevice::Flush();
		m_bFlushNeeded=FALSE;
	}
	CATCH_ALL(e)
	{
		if(!e->IsKindOf(RUNTIME_CLASS(CUserException)))
		{
			CloseDevice();
			e->ReportError();
		}
		else
			CloseDevice(FALSE);
		return FALSE;
	}
	END_CATCH_ALL;
	return TRUE;
}

// Initialize the internal data of device class
BOOL VDevicePhone::Init(void)
{
	VDevice::Init();
	ActivateDialogCtrls(TRUE);
	LoadOptions();
	CWnd *wnd=AfxGetMainWnd();
	if(wnd)
		wnd->SetDlgItemText(IDC_PHONE_FLASH_INFO,
			MSTR("Flash Info: ...press Refresh..."));
	return TRUE;
}

// Destroy internal data and free all resources of the device class
BOOL VDevicePhone::Destroy(void)
{
	SaveOptions();
	VDevice::Destroy();
	ActivateDialogCtrls(FALSE);
	CloseDevice();
	return TRUE;
}

// Sets the state of dialog items for this device
BOOL VDevicePhone::ActivateDialogCtrls(BOOL bActive)
{
	CKLayDlg* dlg=(CKLayDlg*)AfxGetMainWnd();
	if(dlg==NULL)
		return FALSE;
	int show=bActive ? SW_SHOW : SW_HIDE;
	dlg->GetDlgItem(IDC_PHONES_LIST)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONES_LIST)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_PORTS_LIST_TEXT)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_PORTS_LIST_TEXT)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_PORTS_LIST)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_PORTS_LIST)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_BAUDS_LIST_TEXT)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_BAUDS_LIST_TEXT)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_BAUDS_LIST)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_BAUDS_LIST)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_RESET)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_RESET)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_LDRSTATE)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_LDRSTATE)->EnableWindow(bActive);
	SetLdrStateLoaded(bActive ? FALSE : VKL_LDR_STATE_HIDE);
	dlg->GetDlgItem(IDC_PHONE_QUERY_INFO)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_QUERY_INFO)->EnableWindow(bActive);
	dlg->GetDlgItem(IDC_PHONE_FLASH_INFO)->ShowWindow(show);
	dlg->GetDlgItem(IDC_PHONE_FLASH_INFO)->EnableWindow(bActive);
	return TRUE;
}

// Stop the work of loader in the phone
BOOL VDevicePhone::LoaderStopLoader(void)
{
	if(IsSkipLoaderLoadUnload())
		return TRUE;

	VDCSec CriticalSectionOfDevice(this);

	TRY
	{
		DeviceSendCommand(vdpCmdLoaderExit);
		DeviceSkipData(VDP_STOP_LOADER_SKIP_TIMEOUT);
	}
	CATCH_ALL(e)
	{
		if(e->IsKindOf(RUNTIME_CLASS(CUserException)))
			THROW_LAST();
	}
	END_CATCH_ALL;
	TRY
	{
		DeviceSendCommand(vdpCmdLoaderExit2);
		DeviceSkipData(VDP_STOP_LOADER_SKIP_TIMEOUT);
	}
	CATCH_ALL(e)
	{
		if(e->IsKindOf(RUNTIME_CLASS(CUserException)))
			THROW_LAST();
	}
	END_CATCH_ALL;

	SetLdrStateLoaded(FALSE);
	return TRUE;
}










// Write data to the comm port, wait until all data is written, then return number of written bytes
DWORD VDevicePhone::CommWrite(void* pBuf, DWORD dwLen)
{
	if(m_hCom==INVALID_HANDLE_VALUE)
		AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_HANDLE),
			ERROR_INVALID_HANDLE, m_ComPort);
	if(m_oWrite.hEvent!=NULL)
		AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_HANDLE),
			ERROR_INVALID_HANDLE, m_ComPort);
	DWORD len=0, writenlen=0;
	if(dwLen==0 || pBuf==NULL)
		return writenlen;
	DWORD curlen;
	TRY
	{
		for(curlen=0, writenlen=0; dwLen!=0;)
		{
			curlen= dwLen > VDP_COMM_WRITE_BLOCK_SIZE ? VDP_COMM_WRITE_BLOCK_SIZE : dwLen;
			len=0;

			if(o_bCommSimple)
			{
				if(!WriteFile(m_hCom, pBuf, curlen, &len, NULL))
					AfxThrowFileException(0,0,NULL);
			}
			else
			{
				m_oWrite.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
				if(m_oWrite.hEvent==NULL)
					AfxThrowFileException(0,0,NULL);

				if(!WriteFile(m_hCom, pBuf, curlen, &len, &m_oWrite))
				{
					len=0;
					if(GetLastError()!=ERROR_IO_PENDING)
						AfxThrowFileException(0,0,NULL);
					for(int ws=WAIT_TIMEOUT; ws!=WAIT_OBJECT_0; )
					{
						ws=WaitForSingleObject(m_oWrite.hEvent, 1);
						if(ws!=WAIT_TIMEOUT && ws!=WAIT_OBJECT_0)
							AfxThrowFileException(0,0,NULL);
						if(IsCancel())
							AfxThrowUserException();
					}
					if(!GetOverlappedResult(m_hCom, &m_oWrite, &len, FALSE))
						AfxThrowFileException(0,0,NULL);
				}
				CloseHandle(m_oWrite.hEvent);
				m_oWrite.hEvent=NULL;
			}

			writenlen+=len;
			if(len!=curlen)
				break;
			pBuf=(void*)(((BYTE*)pBuf)+curlen);
			dwLen-=curlen;
		}
	}
	CATCH_ALL(e)
	{
		int lasterr=GetLastError();
		COMSTAT st;
		ClearCommError(m_hCom, &len, &st);
#ifdef _UNICODE
		if(!o_bCommSimple)
			VCancelIo(m_hCom);
#endif
		if(m_oWrite.hEvent!=NULL)
			CloseHandle(m_oWrite.hEvent);
		m_oWrite.hEvent=NULL;
		SetLastError(lasterr);
		if(e->IsKindOf(RUNTIME_CLASS(CFileException)))
			AfxThrowFileException(CFileException::OsErrorToException(lasterr), lasterr, m_ComPort);
		THROW_LAST();
	}
	END_CATCH_ALL;
	return writenlen;
}


// Clear all comm buffers
void VDevicePhone::CommReset(void)
{
	if(!PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR))
		ThrowCommException();
}









// Read the data from Comm port with wait exact size of data.
DWORD VDevicePhone::CommRead(void* pBuf, DWORD dwLen, DWORD FirstByteTimeoutMS/*=TIMEOUT_INFINITY*/, DWORD NextBytesTimeoutMS/*=-2*/, BOOL bUseTotalAndIntervalTimeouts/*=FALSE*/)
{
	if(m_hCom==INVALID_HANDLE_VALUE)
		AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_HANDLE),
			ERROR_INVALID_HANDLE, m_ComPort);
	DWORD len=0, readedlen=0;
	if(dwLen==0 || pBuf==NULL)
		return readedlen;
	if(NextBytesTimeoutMS==-2)
	{
		if(bUseTotalAndIntervalTimeouts)
			NextBytesTimeoutMS=0;
		else
			NextBytesTimeoutMS=FirstByteTimeoutMS;
	}

	TRY
	{
		int ws;
		if(bUseTotalAndIntervalTimeouts)
		{
			//set comm timeouts to wait bytes
			//if(m_CommTimeouts.ReadIntervalTimeout!=NextBytesTimeoutMS ||		//закомментировано, т.к. некоторым кабелям нравится, когда таймауты программируются перед каждым чтением.
			//	m_CommTimeouts.ReadTotalTimeoutMultiplier!=0 ||
			//	m_CommTimeouts.ReadTotalTimeoutConstant!=FirstByteTimeoutMS)
			{
				m_CommTimeouts.ReadIntervalTimeout=NextBytesTimeoutMS;
				m_CommTimeouts.ReadTotalTimeoutMultiplier=0;
				m_CommTimeouts.ReadTotalTimeoutConstant=FirstByteTimeoutMS;
				//if(!SetCommTimeouts(m_hCom, &m_CommTimeouts))
				//	AfxThrowFileException(0,0,NULL);
			}
		}
		else
		{
			if(o_bCommSimple)
			{
				//set comm timeouts to wait bytes
				if(NextBytesTimeoutMS==TIMEOUT_INFINITY)
					NextBytesTimeoutMS=30000;
				if(FirstByteTimeoutMS==TIMEOUT_INFINITY)
					FirstByteTimeoutMS=30000;
				else if(FirstByteTimeoutMS==0)
					FirstByteTimeoutMS=1;
				//if(m_CommTimeouts.ReadIntervalTimeout!=NextBytesTimeoutMS ||		//закомментировано, т.к. некоторым кабелям нравится, когда таймауты программируются перед каждым чтением.
				//	m_CommTimeouts.ReadTotalTimeoutMultiplier!=NextBytesTimeoutMS ||
				//	m_CommTimeouts.ReadTotalTimeoutConstant!=FirstByteTimeoutMS)
				{
					m_CommTimeouts.ReadIntervalTimeout=NextBytesTimeoutMS;
					m_CommTimeouts.ReadTotalTimeoutMultiplier=NextBytesTimeoutMS;
					m_CommTimeouts.ReadTotalTimeoutConstant=FirstByteTimeoutMS;
					//if(!SetCommTimeouts(m_hCom, &m_CommTimeouts))
					//	AfxThrowFileException(0,0,NULL);
				}
			}
			else
			{
				if(FirstByteTimeoutMS!=0 && FirstByteTimeoutMS!=TIMEOUT_INFINITY)
				{
					//wait a first byte with FirstByteTimeoutMS timeout.
					COMSTAT stat;
					VTIMER te=SetTimerEventAfter(FirstByteTimeoutMS);
					for(;;)
					{
						if(IsCancel())
							AfxThrowUserException();
						if(!ClearCommError(m_hCom, &len, &stat))
							AfxThrowFileException(0,0,NULL);
						if(stat.cbInQue!=0)
							break;
						Sleep(1);
						if(IsTimerEvent(te))
							return len;
					}
				}

				//set comm timeouts to wait next bytes with NextBytesTimeoutMS timeout
				if(NextBytesTimeoutMS==TIMEOUT_INFINITY)
					NextBytesTimeoutMS=0;
				else if(NextBytesTimeoutMS==0)
					NextBytesTimeoutMS=MAXDWORD;
				//if(m_CommTimeouts.ReadIntervalTimeout!=NextBytesTimeoutMS ||		//закомментировано, т.к. некоторым кабелям нравится, когда таймауты программируются перед каждым чтением.
				//	m_CommTimeouts.ReadTotalTimeoutMultiplier!=0 ||
				//	m_CommTimeouts.ReadTotalTimeoutConstant!=0)
				{
					m_CommTimeouts.ReadIntervalTimeout=NextBytesTimeoutMS;
					m_CommTimeouts.ReadTotalTimeoutMultiplier=0;
					m_CommTimeouts.ReadTotalTimeoutConstant=0;
					//if(!SetCommTimeouts(m_hCom, &m_CommTimeouts))
					//	AfxThrowFileException(0,0,NULL);
				}
			}
		}












		//read first byte, and then, other bytes with NextBytesTimeoutMS timeout if needed.
		DWORD curlen;
		for(curlen=0, readedlen=0; dwLen!=0;)
		{
			curlen= dwLen > VDP_COMM_READ_BLOCK_SIZE ? VDP_COMM_READ_BLOCK_SIZE : dwLen;
			len=0;

			if(!SetCommTimeouts(m_hCom, &m_CommTimeouts))		//некоторым кабелям нравится, когда таймауты программируются перед каждым чтением.
				AfxThrowFileException(0,0,NULL);

			if(o_bCommSimple)
			{
				if(!ReadFile(m_hCom, pBuf, curlen, &len, NULL))
					AfxThrowFileException(0,0,NULL);
			}
			else
			{
				m_oRead.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
				if(m_oRead.hEvent==NULL)
					AfxThrowFileException(0,0,NULL);
				if(!ReadFile(m_hCom, pBuf, curlen, &len, &m_oRead))
				{
					len=0;
					if(GetLastError()!=ERROR_IO_PENDING)
						AfxThrowFileException(0,0,NULL);
					for(ws=WAIT_TIMEOUT; ws!=WAIT_OBJECT_0; )
					{
						ws=WaitForSingleObject(m_oRead.hEvent, 1);
						if(ws!=WAIT_TIMEOUT && ws!=WAIT_OBJECT_0)
							AfxThrowFileException(0,0,NULL);
						if(IsCancel())
							AfxThrowUserException();
					}
					if(!GetOverlappedResult(m_hCom, &m_oRead, &len, FALSE))
						AfxThrowFileException(0,0,NULL);
				}
				CloseHandle(m_oRead.hEvent);
				m_oRead.hEvent=NULL;
			}

			readedlen+=len;
			if(len!=curlen)
				break;
			pBuf=(void*)(((BYTE*)pBuf)+len);
			dwLen-=len;
		}
	}
	CATCH_ALL(e)
	{
		int lasterr=GetLastError();
#ifdef _UNICODE
		if(!o_bCommSimple)
			VCancelIo(m_hCom);
#endif
		if(m_oRead.hEvent!=NULL)
			CloseHandle(m_oRead.hEvent);
		m_oRead.hEvent=NULL;
		COMSTAT st;
		ClearCommError(m_hCom, &len, &st);
		SetLastError(lasterr);
		if(e->IsKindOf(RUNTIME_CLASS(CFileException)))
			AfxThrowFileException(CFileException::OsErrorToException(lasterr), lasterr, m_ComPort);
		THROW_LAST();
	}
	END_CATCH_ALL;
	return readedlen;
}










//Send command to the phone
//Return the number of bytes that is readed as answer
//TimeoutMS - time while wait answer message (delay before first byte of answer is received.
//            Rest bytes from answer message will be readed with MessageTimeoutMS ms timeout between bytes)
//if pBuf==NULL, - then function do not read any answers and return immidiatly.
//if pBuf==INGORE_ANSWER, then function is skip all data, what is coming with TimeoutMS milliseconds interval.
DWORD VDevicePhone::DeviceSendCommand(BYTE* cmd, void* pBufAnswer/*=NULL*/, DWORD bufLenAnswer/*=-1*/, DWORD TimeoutMS/*=VDP_MESSAGE_TIMEOUT*/, DWORD MessageTimeoutMS/*=-2*/)
{
	CommWrite(cmd, (DWORD)strlen((const char*)cmd));
	if(pBufAnswer==NULL)
		return 0;
	if(pBufAnswer==INGORE_ANSWER)
		return DeviceSkipData(TimeoutMS, bufLenAnswer);
	return CommRead(pBufAnswer, bufLenAnswer, TimeoutMS, MessageTimeoutMS);
}

//skip all data, that is coming with specified interval
DWORD VDevicePhone::DeviceSkipData(DWORD TimeoutMS/*=VDP_MESSAGE_TIMEOUT*/, DWORD SkipCount/*=-1*/)
{
	if(SkipCount==0)
		return 0;
	BYTE ans;
	DWORD len;
	for(len=0; SkipCount && CommRead(&ans, 1, TimeoutMS); len++)
		if(SkipCount!=-1)
			SkipCount--;
	return len;
}

//Wait specified answer from phone
//Return TRUE if answer is received and FALSE if not.
//TimeoutMS - time while wait answer message (delay before first byte of answer is received.
//            Rest bytes from answer message will be readed with VDP_MESSAGE_TIMEOUT ms timeout between bytes)
BOOL VDevicePhone::DeviceWaitCmdAnswer(BYTE *pAns, DWORD AnsLen, DWORD TimeoutMS/*=TIMEOUT_INFINITY*/)
{
	if(pAns==NULL || AnsLen==0)
		return TRUE;
//pAns=new BYTE[5];
//pAns[0]='1'; pAns[1]='2'; pAns[2]='3'; pAns[3]='4'; pAns[4]='5';
//AnsLen=5;
	DWORD len=0;
	BYTE *ans=new BYTE[AnsLen];
//ans[0]='4'; ans[1]='1'; ans[2]='2'; ans[3]='3'; ans[4]='4';
	DWORD i=0, j;
	BOOL ret=FALSE;
	VTIMER te=SetTimerEventAfter(TimeoutMS);
	do
	{
		len=CommRead(ans+i, AnsLen-i, TimeoutMS);
		if(len!=AnsLen-i)
			break;

		//search part or all bytes of pAns in ans
		for(i=0; i<AnsLen; i++)
		{
			for(j=0; j<AnsLen-i; j++)
			{
				if(ans[i+j]!=pAns[j])
					break;
			}
			if(j==AnsLen-i)
				break;
		}

		if(i==0)
		{	//if all bytes from pAns is found in ans - return true
			ret=TRUE;
			break;
		}
		else if(i!=AnsLen)					//if part from begin of pAns is found in ans
			memmove(ans, ans+i, AnsLen-i);	// move this part to begin of ans, and try to read rest data.
		i=AnsLen-i;
	}
	while(!IsTimerEvent(te));

	delete ans;
	return ret;
}












// Check, is the loader ready to work
BOOL VDevicePhone::LoaderIsReady(DWORD TimeoutMS/*=VDP_IS_LOADER_READY_ANS_TIMEOUT*/)
{
	if(m_hCom==INVALID_HANDLE_VALUE)
		return FALSE;
	VDCSec CriticalSectionOfDevice(this);
	TRY
	{
		BYTE buf[vdpAnsLoaderReadyLen];
		if(DeviceSendCommand(vdpCmdIsLoaderReady, buf, vdpAnsLoaderReadyLen, TimeoutMS)!=vdpAnsLoaderReadyLen)
			return FALSE;
		return memcmp(buf, vdpAnsLoaderReady, vdpAnsLoaderReadyLen)==0;
	}
	CATCH_ALL(e)
	{
		if(e->IsKindOf(RUNTIME_CLASS(CUserException)))
			THROW_LAST();
	}
	END_CATCH_ALL;
	return FALSE;
}

// Returned the check sum of the given data
BYTE VDevicePhone::GetCheckSum(void * pBuf, DWORD dwBufLen)
{
	BYTE cs=0;
	for(; dwBufLen; dwBufLen--)
		cs^=((BYTE*)pBuf)[dwBufLen-1];
	return cs;
}

// Loads the options for current device from the registry
BOOL VDevicePhone::LoadOptions(void)
{
	if(!VDevice::LoadOptions())
		return FALSE;
	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	CWinApp* app=AfxGetApp();
	if(dlg==NULL || app==NULL)
		return FALSE;
	CString str;

	int i=CB_ERR;
	CComboBox *ldcb=(CComboBox*)dlg->GetDlgItem(IDC_PHONES_LIST);
	if(VDevicePhone::FillByLoadersComboBox(ldcb))
	{
		if(strCurPhoneModelName.IsEmpty())
			str=o_cmdExist_Phone ? o_cmdPhone : app->GetProfileString(vdpsec, vdpkeyname, NULL);
		else
			str=strCurPhoneModelName;
		i=ldcb->FindStringExact(-1, str);
		if(i!=CB_ERR)
			ldcb->SetCurSel(cklaydlg_oldphonelistitem=i);
	}
	if(i==CB_ERR)
		ldcb->SetCurSel(0);

	str=o_cmdExist_PhonePortName ? o_cmdPhonePortName : app->GetProfileString(vdpsec, vdpkeyport, _T("COM1"));
	dlg->GetDlgItem(IDC_PHONE_PORTS_LIST)->SetWindowText(str);

	dlg->SetDlgItemInt(IDC_PHONE_BAUDS_LIST, 
		o_cmdExist_PhoneBaud ? o_cmdPhoneBaud : app->GetProfileInt(vdpsec, vdpkeybaud, 115200));

	return TRUE;
}

// Save the options for current device to the registry
BOOL VDevicePhone::SaveOptions(void)
{
	if(!VDevice::SaveOptions())
		return FALSE;
	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	CWinApp* app=AfxGetApp();
	if(dlg==NULL || app==NULL)
		return FALSE;
	CString str;

	o_cmdExist_Phone=FALSE;

	dlg->GetDlgItemText(IDC_PHONE_PORTS_LIST, str);
	str.TrimLeft(); str.TrimRight();
	dlg->SetDlgItemText(IDC_PHONE_PORTS_LIST, str);
	app->WriteProfileString(vdpsec, vdpkeyport, str);
	o_cmdExist_PhonePortName=FALSE;

	DWORD baud=dlg->GetDlgItemInt(IDC_PHONE_BAUDS_LIST);
	dlg->SetDlgItemInt(IDC_PHONE_BAUDS_LIST, baud);
	app->WriteProfileInt(vdpsec, vdpkeybaud, baud);
	o_cmdExist_PhoneBaud=FALSE;

	return TRUE;
}

BOOL VDevicePhone::SaveOptionsPhoneName(void)
{
	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	CWinApp* app=AfxGetApp();
	if(dlg==NULL || app==NULL)
		return FALSE;
	CComboBox* lpcb=(CComboBox*)dlg->GetDlgItem(IDC_PHONES_LIST);
	if(lpcb==NULL)
		return FALSE;
	CString str;
	lpcb->GetWindowText(str);
	if(!(str.IsEmpty() || lpcb->GetCurSel()==-1 || lpcb->GetItemData(lpcb->GetCurSel())==NULL))
		app->WriteProfileString(vdpsec, vdpkeyname, str);
	o_cmdExist_Phone=FALSE;
	return TRUE;
}

// Skip data when reading / writing in loader command, using skiping parameters from loader file
BOOL VDevicePhone::LoaderSkipData(DWORD Timeout/*=VDP_MESSAGE_TIMEOUT*/, DWORD SkipCount/*=-1*/, BOOL IsSkip/*=TRUE*/)
{
	if(!IsSkip)
		return TRUE;

	VDCSec CriticalSectionOfDevice(this);

	if(SkipCount!=-1)
		return DeviceSkipData(Timeout, SkipCount) == SkipCount;
	else
		DeviceSkipData();
	return TRUE;
}

// Test, is memory range is empty
BOOL VDevicePhone::LoaderTestIsEmpty(INT64 Address, INT64 Size)
{
	BOOL isEmpty=FALSE;
	INT64 t;
	int i, j;
	INT64 asmax=(INT64)(((UINT64)1)<<(8 * m_LdrOpt.m_CmdAddrAndSizeLen));
	BYTE buf[VDP_CMD_LOADER_TSTISEMPTY_ANS_LEN + 2*7  +4];
	if(m_LdrOpt.m_CmdAddrAndSizeLen>7)
	{
		AfxMessageBox(_T("VDevicePhone::LoaderTestIsEmpty: address and size fields len (optCmdAddrAndLenSize) too big (must be <= 7)"));
		AfxThrowUserException();
	}

	CString str, strStat, strEvent;
	BOOL oldcancelstate=IsCanCancel();
	BOOL canceling=FALSE;

	VDCSec CriticalSectionOfDevice(this);

	for(;;)
	{
		if(pProgress)
		{
			strStat.Format(m_strTstIsEmptyStat, Address, Size);
			pProgress->SetStatus(strStat+strEvent);
		}

		//while(!IsLoaderReady(VDP_MEMORY_READ_TIMEOUT));

		if(!SetMayCancel(FALSE))
			AfxThrowUserException();

		j=0;
		t=VDP_CMD_LOADER_READ_ADDR(Address);
		if(t < 0/* || addr >= asmax*/)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderTestIsEmpty: address for loader is out of range."));
			AfxThrowUserException();
		}
		for(i=m_LdrOpt.m_CmdAddrAndSizeLen; i!=0; i--)
			buf[j++]=*(((BYTE*)(&t))+i-1);

		if(Size < 0 || Size >= asmax)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderTestIsEmpty: read block size for loader is out of range."));
			AfxThrowUserException();
		}
		for(i=m_LdrOpt.m_CmdAddrAndSizeLen; i!=0; i--)
			buf[j++]=*(((BYTE*)(&Size))+i-1);

		DeviceSendCommand(vdpCmdLoaderTstIsEmpty);			//send test is empty command

		CommWrite(buf, 2*m_LdrOpt.m_CmdAddrAndSizeLen);		//send address and size of block to test is empty.

		canceling=!SetMayCancel(oldcancelstate);
		SetMayCancel(FALSE);

		//read answer
		memset(buf, 0, sizeof(buf));
		t=CommRead(buf, VDP_CMD_LOADER_TSTISEMPTY_ANS_LEN, VDP_MEMORY_READ_TIMEOUT);

		if(!SetMayCancel(oldcancelstate))
			AfxThrowUserException();
		if(canceling)
			AfxThrowUserException();

		if(t!=VDP_CMD_LOADER_TSTISEMPTY_ANS_LEN || (buf[0]!=0xff && buf[0]!=0x00))
		{
			if(pProgress)
			{
				strEvent=MSTR("Bad answer - testing again...");
				strEvent+=_T(' ');
				pProgress->SetStatus(strStat+strEvent);
			}
			DeviceSkipData(VDP_READ_AFTER_ERROR_SKIP_TIMEOUT);
			m_ErrorsCorrectedCount++;
			continue;
		}

		isEmpty= buf[0]==0xff;

		if(pProgress)
		{
			pProgress->OffsetPos(Size);
			pProgress->SetStatus(strStat+m_strReadingStatOk);
		}
		break;
	}

	return isEmpty;
}

//ensure that m_LoaderReadMemoryTmpBuf has needed size
void VDevicePhone::LoaderReadMemoryTmpBufLenCheck(DWORD size)
{
	if(size > m_LoaderReadMemoryTmpBufLen)
	{
		m_LoaderReadMemoryTmpBufLen = size + 1024;
		delete m_LoaderReadMemoryTmpBuf;
		m_LoaderReadMemoryTmpBuf = new BYTE[m_LoaderReadMemoryTmpBufLen];
		if(m_LoaderReadMemoryTmpBuf == NULL)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderReadMemory: Memory allocation error."));
			AfxThrowUserException();
		}
	}
}

void VDevicePhone::LoaderReadMemory_CheckOK_tb(BOOL &isErr, DWORD &tmpbuflen)
{
	if(isErr)
		return;
	if(m_LdrOpt.m_ReadCmdAnswerOKLen != 0 &&
		(!m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData
		|| m_LdrOpt.m_IsExistReadCmdAnswerOK || m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK))
	{
		isErr = (memcmp(m_LoaderReadMemoryTmpBuf + tmpbuflen, m_LdrOpt.m_ReadCmdAnswerOK, m_LdrOpt.m_ReadCmdAnswerOKLen) != 0);
		tmpbuflen+=m_LdrOpt.m_ReadCmdAnswerOKLen;
	}
	if(isErr)
		return;
	if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK)
		tmpbuflen += m_LdrOpt.m_ReadCmdSkipBytesAfterOK;
}

void VDevicePhone::LoaderReadMemory_CheckCheckSum_tb(BOOL &isErr, void* pBuf, INT64 curSize, DWORD &tmpbuflen)
{
	if(isErr)
		return;
	//Check the check sum
	WORD CheckSum;
	if(m_LdrOpt.m_ReadCmdAnswerCheckSumType != 0)
	{
		CheckSum = GetCheckSum(pBuf, (DWORD)curSize);
		if(CheckSum != *((WORD*)(m_LoaderReadMemoryTmpBuf + tmpbuflen)))
			isErr = 2;
		else
			isErr = FALSE;
		tmpbuflen += sizeof(WORD);
	}
	if(isErr)
		return;
	if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterCheckSum)
		tmpbuflen += m_LdrOpt.m_ReadCmdSkipBytesAfterCheckSum;
}

void VDevicePhone::LoaderReadMemory_CheckOK(BOOL &isErr)
{
	if(isErr)
		return;
	if(m_LdrOpt.m_ReadCmdAnswerOKLen != 0 &&
		(!m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData
		|| m_LdrOpt.m_IsExistReadCmdAnswerOK || m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK))
	{
		isErr = TRUE;
		if(CommRead(m_LoaderReadMemoryTmpBuf, m_LdrOpt.m_ReadCmdAnswerOKLen,
			VDP_MEMORY_READ_TIMEOUT) == m_LdrOpt.m_ReadCmdAnswerOKLen)
			if(memcmp(m_LoaderReadMemoryTmpBuf, m_LdrOpt.m_ReadCmdAnswerOK, m_LdrOpt.m_ReadCmdAnswerOKLen) == 0)
				isErr = FALSE;
	}
	if(isErr)
		return;
	if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK)
		isErr = !LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_ReadCmdSkipBytesAfterOK);
}

void VDevicePhone::LoaderReadMemory_CheckCheckSum(BOOL &isErr, void* pBuf, INT64 curSize)
{
	if(isErr)
		return;
	//Check the check sum
	WORD CheckSum;
	BYTE buf[sizeof(CheckSum)];
	if(m_LdrOpt.m_ReadCmdAnswerCheckSumType != 0)
	{
		CheckSum = GetCheckSum(pBuf, (DWORD)curSize);
		isErr = CommRead(buf, sizeof(CheckSum), VDP_MEMORY_READ_TIMEOUT);
		if(isErr < sizeof(CheckSum))
			isErr = TRUE;
		else if(CheckSum != *((WORD*)buf))
			isErr = 2;
		else
			isErr = FALSE;
	}
	if(isErr)
		return;
	if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterCheckSum)
		isErr = !LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_ReadCmdSkipBytesAfterCheckSum);
}

// Read the data from phone device memory by mean of loader
INT64 VDevicePhone::LoaderReadMemory(INT64 Address, INT64 Size, void* pBuf)
{
	INT64 t, restSize, CurAddr=Address, curSize, PageSize=m_LoaderReadMemoryLastGoodPageSize;
	int i, j;
	BOOL isErr;
	INT64 asmax=(INT64)(((UINT64)1)<<(8 * m_LdrOpt.m_CmdAddrAndSizeLen));
	BYTE buf[sizeof(WORD) + 2*7  +4];
	if(m_LdrOpt.m_CmdAddrAndSizeLen > 7)
	{
		AfxMessageBox(_T("VDevicePhone::LoaderReadMemory: address and size fields len (optCmdAddrAndLenSize) too big (must be <= 7)"));
		AfxThrowUserException();
	}

	CString str, strStat, strEvent;
	BOOL oldcancelstate=IsCanCancel();
	BOOL canceling=FALSE;
	DWORD tmpbuflen, tmpbuflen0;
	BOOL canusetmpbuf= !(m_LdrOpt.m_ReadCmdSkipBytesAfterCmdParameters == -1)
        && !(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData     && m_LdrOpt.m_ReadCmdSkipBytesAfterData == -1)
		&& !(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK       && m_LdrOpt.m_ReadCmdSkipBytesAfterOK == -1)
		&& !(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterCheckSum && m_LdrOpt.m_ReadCmdSkipBytesAfterCheckSum == -1);
	if(canusetmpbuf)
	{
		tmpbuflen0 = m_LdrOpt.m_ReadCmdSkipBytesAfterCmdParameters + (DWORD)PageSize + sizeof(WORD);
		if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData)
			tmpbuflen0 += m_LdrOpt.m_ReadCmdSkipBytesAfterData;
		if(!m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData
			|| m_LdrOpt.m_IsExistReadCmdAnswerOK || m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK)
			tmpbuflen0 += m_LdrOpt.m_ReadCmdAnswerOKLen;
		if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterOK)
			tmpbuflen0 += m_LdrOpt.m_ReadCmdSkipBytesAfterOK;
		if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterCheckSum)
			tmpbuflen0 += m_LdrOpt.m_ReadCmdSkipBytesAfterCheckSum;
		LoaderReadMemoryTmpBufLenCheck(tmpbuflen0);
		tmpbuflen0-=(DWORD)PageSize;
	}

	LoaderReadMemoryTmpBufLenCheck(m_LdrOpt.m_ReadCmdAnswerOKLen);

	VDCSec CriticalSectionOfDevice(this);

	int decSizeTryCount=0;
	for(restSize=Size, curSize=0; restSize != 0 ; )
	{
		curSize= PageSize <= restSize ? PageSize : restSize;

		if(pProgress)
		{
			strStat.Format(m_strReadingStat, CurAddr, curSize);
			pProgress->SetStatus(strStat+strEvent);
		}

//		while(!IsLoaderReady(VDP_MEMORY_READ_TIMEOUT));

		if(!SetMayCancel(FALSE))
			AfxThrowUserException();

		j=0;
		t=VDP_CMD_LOADER_READ_ADDR(CurAddr);
		if(t < 0/* || t >= asmax*/)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderReadMemory: address for loader is out of range."));
			AfxThrowUserException();
		}
		for(i=m_LdrOpt.m_CmdAddrAndSizeLen; i!=0; i--)
			buf[j++]=*(((BYTE*)(&t))+i-1);

		if(curSize < 0 || curSize >= asmax)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderReadMemory: read block size for loader is out of range."));
			AfxThrowUserException();
		}
		for(i=m_LdrOpt.m_CmdAddrAndSizeLen; i!=0; i--)
			buf[j++]=*(((BYTE*)(&curSize))+i-1);

		DeviceSendCommand(vdpCmdLoaderRead);				//send read command

		CommWrite(buf, 2*m_LdrOpt.m_CmdAddrAndSizeLen);		//send address and size of block to read.

		canceling=!SetMayCancel(oldcancelstate);
		SetMayCancel(FALSE);


		if(canusetmpbuf)
		{
			tmpbuflen = tmpbuflen0 + (DWORD)curSize;
			//read answer
			isErr= (CommRead(m_LoaderReadMemoryTmpBuf, tmpbuflen, VDP_MEMORY_READ_TIMEOUT)!=tmpbuflen);
			if(!isErr)
			{
				tmpbuflen=m_LdrOpt.m_ReadCmdSkipBytesAfterCmdParameters;

				//copy data
				memcpy(pBuf, m_LoaderReadMemoryTmpBuf+tmpbuflen, (DWORD)curSize);
				tmpbuflen+=(DWORD)curSize;

				if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData)
					tmpbuflen += m_LdrOpt.m_ReadCmdSkipBytesAfterData;

				if(m_LdrOpt.m_ReadCmdAnswerOrderCheckSumThenOK)
				{
					LoaderReadMemory_CheckCheckSum_tb(isErr, pBuf, curSize, tmpbuflen);
					LoaderReadMemory_CheckOK_tb(isErr, tmpbuflen);
				}
				else
				{
					LoaderReadMemory_CheckOK_tb(isErr, tmpbuflen);
					LoaderReadMemory_CheckCheckSum_tb(isErr, pBuf, curSize, tmpbuflen);
				}
			}
		}
		else
		{
			isErr=!LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_ReadCmdSkipBytesAfterCmdParameters);

			if(!isErr)
			{
				//read data
				isErr=((INT64)CommRead(pBuf, (DWORD)curSize, VDP_MEMORY_READ_TIMEOUT)!=curSize);

				if(!isErr)
				{
					if(m_LdrOpt.m_IsExistReadCmdSkipBytesAfterData)
						isErr = !LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_ReadCmdSkipBytesAfterData);

					if(m_LdrOpt.m_ReadCmdAnswerOrderCheckSumThenOK)
					{
						LoaderReadMemory_CheckCheckSum(isErr, pBuf, curSize);
						LoaderReadMemory_CheckOK(isErr);
					}
					else
					{
						LoaderReadMemory_CheckOK(isErr);
						LoaderReadMemory_CheckCheckSum(isErr, pBuf, curSize);
					}
				}
			}
		}
		if(!SetMayCancel(oldcancelstate))
			AfxThrowUserException();
		if(canceling)
			AfxThrowUserException();


		if(isErr)		//isErr - IsError
		{
			if(pProgress)
			{
				if(isErr==2)
					strEvent=MSTR("Received data CRC error - reading again...");
				else
					strEvent=MSTR("Error when receiving data - reading again...");
				strEvent+=_T(' ');
				pProgress->SetStatus(strStat+strEvent);
			}
			if(++decSizeTryCount >= (PageSize > VDP_READ_BIG_PAGE_SIZE_MIN ?
				VDP_READ_BIG_PAGE_SIZE_TRY_COUNT : VDP_READ_PAGE_SIZE_TRY_COUNT))
			{
				decSizeTryCount=0;
				if(PageSize>VDP_READ_PAGE_SIZE_MIN)
					PageSize/=2;
			}
			ThrowExceptionIfLoaderNotReady();
			m_ErrorsCorrectedCount++;
			continue;
		}

		if(pProgress)
		{
			pProgress->OffsetPos(curSize);
			pProgress->SetStatus(strStat+m_strReadingStatOk);
		}
		restSize-=curSize;
		CurAddr+=curSize;
		pBuf=((BYTE*)pBuf)+curSize;
		strEvent.Empty();
	}

	if(PageSize >= 0x4000)
		m_LoaderReadMemoryLastGoodPageSize=PageSize;	//Remember last block size, and start reading next block
														// with last good block size.
	return Size-restSize;
}


// Write the data to phone device memory by mean of loader
// Address must be aligned on m_FlashPageSize boundary.
// Size must be multiplier of m_FlashPageSize.
INT64 VDevicePhone::LoaderWriteMemory(INT64 Address, INT64 Size, void* pBuf)
{
	if(!(Size==-1 && pBuf==NULL))
	{
		if(Size < 0)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderWriteMemory: Size must be >= 0"));
			AfxThrowUserException();
		}
		if(pBuf==NULL)
		{
			AfxMessageBox(_T("VDevicePhone::LoaderWriteMemory: pBuff=NULL"));
			AfxThrowUserException();
		}
	}
	CString str;
	WORD checkSum, checkSumReaded;
	VDP_CMD_LOADER_WRITE_ADDR_ANS addrAns;
	VDP_CMD_LOADER_WRITE_DATA_ANS dataAns;
	INT64 addr, curSize, restSize;
	DWORD len;

	int i, j;
	BOOL oldcancelstate=IsCanCancel();
	BOOL canceling=FALSE;
	BYTE aslen= m_LdrOpt.m_WriteCmdVersion==1 ? VDP_CMDLDR_WRITE_ADDR_LEN : m_LdrOpt.m_CmdAddrAndSizeLen;
	INT64 asmax=(INT64)(((UINT64)1)<<(8 * aslen));
	BYTE buf[vdpAnsLoaderWriteOkLen + 2*7 +4];
	if(aslen>7)
	{
		AfxMessageBox(_T("VDevicePhone::LoaderWriteMemory: address and size fields len (optCmdAddrAndLenSize) too big (must be <= 7)"));
		AfxThrowUserException();
	}

	CString strError(_T(" - Loader: WriteMemory:"));
	CString strStat, strEvent;

	VDCSec CriticalSectionOfDevice(this);

	for(restSize=Size, curSize=0; restSize!=0;)
	{
		if(Size==-1 && pBuf==NULL)
		{
			restSize=Size=0;
			addr=Address=0;
		}
		else
		{
			if(m_LdrOpt.m_WriteCmdVersion==1)
			{
				addr=VDP_CMDLDR_WRITE_CALC_ADDR(Address);
				curSize=VDP_CMDLDR_WRITE_UNCALC_ADDR(addr);
			}
			else if(m_LdrOpt.m_WriteCmdVersion==2)
			{
				addr=VDP_CMDLDR_WRITEV2_CALC_ADDR(Address);
				curSize=VDP_CMDLDR_WRITEV2_UNCALC_ADDR(addr);
				if(addr < 0 || addr >= asmax)
				{
					AfxMessageBox(_T("VDevicePhone::LoaderWriteMemory: address for loader is out of range."));
					AfxThrowUserException();
				}
				if(restSize < 0 || restSize >= asmax)
				{
					AfxMessageBox(_T("VDevicePhone::LoaderWriteMemory: read block size for loader is out of range."));
					AfxThrowUserException();
				}
			}

			if(IsSkipWritingInBlock(curSize, restSize))
			{
				if(IsCancel())
					AfxThrowUserException();
				break;
			}
		}

//		while(!IsLoaderReady(VDP_MEMORY_READ_TIMEOUT));

		if(!SetMayCancel(FALSE))
			AfxThrowUserException();

#ifndef VDP_EMULATE_WRITING
		//send command
		DeviceSendCommand(vdpCmdLoaderWrite);

		//send block address. Hight byte first
		for(j=0, i=aslen; i!=0; i--)
			buf[j++]=*(((BYTE*)(&addr))+i-1);
		CommWrite(buf, aslen);

		//read the answer after command and its parameters
		memset(&addrAns, 0, sizeof(addrAns));
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterCmdPatameters)
		{
			if(!LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterCmdPatameters))
			{
				SetMayCancel(oldcancelstate);
				str.Format(_T(" Unknown answer on address readed, length must be 0x%8.8X."), m_LdrOpt.m_WriteCmdSkipBytesAfterCmdPatameters);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
		}
		else
		{
			if(m_LdrOpt.m_WriteCmdVersion==1)
			{
				len=CommRead(&addrAns, 10, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				if(len<10)
				{
					SetMayCancel(oldcancelstate);
					if(addrAns.flashBase==VDP_CMDLDR_WRITE_ERR_ADDRESS_TOO_FAR)
					{
						str.Format(_T(" Address %s is too far."), VSignedHexToStr(Address, addr64format));
						strError+=str;
						AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
							ERROR_INVALID_FUNCTION, m_ComPort+strError);
					}
					else if(addrAns.flashBase==VDP_CMDLDR_WRITE_ERR_UNKNOWN_FLASH)
					{
						str.Format(_T(" Unknown Flash Type: 0x%4.4X."), addrAns.offset);
						strError+=str;
						AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
							ERROR_INVALID_FUNCTION, m_ComPort+strError);
					}
					else
					{
						str.Format(_T(" Unknown answer on address readed: length=0x%8.8X, but must be 0x%8.8X."), len, 10);
						strError+=str;
						AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
							ERROR_INVALID_FUNCTION, m_ComPort+strError);
					}
				}
			}
		}

		if(m_LdrOpt.m_WriteCmdVersion==1)
		{
			//read the block size
			len=CommRead(&addrAns.blockSize, sizeof(addrAns.blockSize), VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
			if(len!=sizeof(addrAns.blockSize))
			{
				SetMayCancel(oldcancelstate);
				str.Format(_T(" Unknown BlockSize readed: length=0x%8.8X, but must be 0x%8.8X."), len, sizeof(addrAns.blockSize));
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
			curSize=VDP_CMDLDR_WRITE_CALC_BLOCK_SIZE(addrAns.blockSize);
			if(curSize>restSize)
			{
				CString blSize=_T("unknown");
				INT64 pagesize=m_Cache.GetRealPageSizeAtAddr(Address);
				if(pagesize!=-1)
					blSize.Format(addr64format, pagesize);
				str.Format(CString(_T(" FATAL ERROR!\nThe loader need more data for write to current block than V_KLay have!\nFor prevent this error in the future increase the block size value in parameter "))
					+ drvkeyMemGeometry + _T(" for address range in what lies MCU address ") + addr64format
					+ _T(" in the section of current phone of phone driver file.\nLoader wants %I64u bytes of data.\nV_KLay have %I64u bytes of data.\nIn current phone driver file block size is %s."),
					Address, curSize, restSize, blSize);
				strError+=str;
				i=0xff;
				for(len=0; len<curSize; len++)
					CommWrite(&i, 1);
				i='V';
				CommWrite(&i, 1);
				len=0;
				CommRead(&len, 2, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				if(len!=VDP_CMDLDR_WRITE_DATARECEIVED_ERR_CRC_NAK)
					strError+=_T("\nCanceling error.");
				SetMayCancel(oldcancelstate);
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
		}
		else if(m_LdrOpt.m_WriteCmdVersion==2)
		{
			curSize=restSize;
			//sending the block size Hight byte first
			for(j=0, i=aslen; i!=0; i--)
				buf[j++]=*(((BYTE*)(&curSize))+i-1);
			CommWrite(buf, aslen);
		}

		// read the answer after block size
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterBlockSize)
		{
			if(!LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterBlockSize))
			{
				SetMayCancel(oldcancelstate);
				str.Format(_T(" Unknown answer after BlockSize readed, length must be 0x%8.8X."), m_LdrOpt.m_WriteCmdSkipBytesAfterBlockSize);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
		}
		else
		{
			if(m_LdrOpt.m_WriteCmdVersion==1)
			{
				len=CommRead(&addrAns.id, sizeof(addrAns.id), VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				if(len!=sizeof(addrAns.id))
				{
					SetMayCancel(oldcancelstate);
					str.Format(_T(" Unknown answer after BlockSize readed: length=0x%8.8X, but must be 0x%8.8X."), len, sizeof(addrAns.id));
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
				if(addrAns.id!=VDP_CMDLDR_WRITE_OK_ID)
				{
					SetMayCancel(oldcancelstate);
					str.Format(_T(" Unknown id in answer on address readed: id=0x%4.4X, but must be 0x%4.4X."), addrAns.id, VDP_CMDLDR_WRITE_OK_ID);
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
#else // VDP_EMULATE_WRITING
		curSize=restSize;
		Sleep(100);
#endif // VDP_EMULATE_WRITING

		//sending data and checksum
		if(pProgress)
		{
			strStat.Format(m_strWritingStat, Address, curSize);
			pProgress->SetStatus(strStat+strEvent);
		}

		if(pProgress)
			pProgress->SetStatus(strStat+strEvent+MSTR("Sending data..."));

		if(pBuf!=NULL)
		{
			i=GetCheckSum(pBuf, (DWORD)curSize);
#ifndef VDP_EMULATE_WRITING
			CommWrite(pBuf, (DWORD)curSize);
			CommWrite(&i, 1);
#endif
		}

		canceling=!SetMayCancel(oldcancelstate);
		SetMayCancel(FALSE);

		//reading answer after data sent
#ifndef VDP_EMULATE_WRITING
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterDataWithCheckSum)
		{
			if(!LoaderSkipData(VDP_MEMORY_READ_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterDataWithCheckSum))
			{
				if(!canceling)  //this check is needed if we do not want see next error message when we clicked Cancel.
				{
					if(pProgress)
					{
						strEvent=MSTR("Sended data error - sending again...");
						strEvent+=_T(' ');
						pProgress->SetStatus(strStat+strEvent);
					}
					DeviceSkipData(VDP_WRITE_AFTER_BAD_CRC_SKIP_TIMEOUT);
					ThrowExceptionIfLoaderNotReady();
					m_ErrorsCorrectedCount++;
					continue;
				}
			}
		}
		else
		{
			if(m_LdrOpt.m_WriteCmdVersion==1)
			{
				memset(&dataAns, 0, sizeof(dataAns));
				len=CommRead(&dataAns, sizeof(dataAns), VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				i=FALSE;					//i - IsError and error code
				if(len!=sizeof(dataAns))
					i= dataAns.flashBase==0 ? -1 : dataAns.flashBase;
			}
			else if(m_LdrOpt.m_WriteCmdVersion==2)
			{
				memset(buf, 0, sizeof(buf));
				len=CommRead(buf, 2, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				i=*((WORD*)buf);			//i - IsError and error code
				if(len==2 && i==VDP_CMDLDR_WRITEV2_DATARECEIVED_ACK)
					i=FALSE;
				else if(i==0)
					i=-1;
			}
			if(!canceling && i)				//i - IsError
			{
				if(m_LdrOpt.m_WriteCmdVersion==1 && i==VDP_CMDLDR_WRITE_DATARECEIVED_ERR_RAM_NAK)
				{
					strError+=_T(" Phone RAM is bad.");
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
				else if(m_LdrOpt.m_WriteCmdVersion==2 && i==VDP_CMDLDR_WRITEV2_DATARECEIVED_ERR_BOUNDS_NAK)
				{
					strError+=_T(" Bounds error (address is not on the edge of block).");
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
				else if(m_LdrOpt.m_WriteCmdVersion==2 && i==VDP_CMDLDR_WRITEV2_DATARECEIVED_UNKNOWN_FLASH_NAK)
				{
					memset(buf, 0, sizeof(buf));
					CommRead(buf, 2, VDP_MESSAGE_TIMEOUT);
					str.Format(_T(" Flash IC type 0x%4.4X is unknown for loader."), *((WORD*)buf));
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
				else if(m_LdrOpt.m_WriteCmdVersion==2 && i==VDP_CMDLDR_WRITEV2_DATARECEIVED_ACCESS_DENIED_NAK)
				{
					strError+=_T(" Protected area. Access denied.");
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
				else if(i==VDP_CMDLDR_WRITE_DATARECEIVED_ERR_CRC_NAK)
				{
					if(pProgress)
					{
						strEvent=MSTR("Sended data CRC error - sending again...");
						strEvent+=_T(' ');
						pProgress->SetStatus(strStat+strEvent);
					}
					DeviceSkipData(VDP_WRITE_AFTER_BAD_CRC_SKIP_TIMEOUT);
					ThrowExceptionIfLoaderNotReady();
					m_ErrorsCorrectedCount++;
					continue;
				}
				else
				{
					str.Format(_T(" Unknown answer on data readed: length=0x%8.8X."), len);
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
#else	 //#ifndef VDP_EMULATE_WRITING
		Sleep(200);
#endif	 //#ifndef VDP_EMULATE_WRITING

		//reading answer after flash erase
		if(!canceling && pProgress)
			pProgress->SetStatus(strStat+strEvent+MSTR("Erasing..."));
#ifndef VDP_EMULATE_WRITING
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterEraseStep)
		{
			if(!LoaderSkipData(VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterEraseStep))
			{
				if(!canceling)  //this check is needed if we do not want see next error message when we clicked Cancel.
				{
					str.Format(_T(" Unknown answer after erase flash readed, length must be 0x%8.8X."), m_LdrOpt.m_WriteCmdSkipBytesAfterEraseStep);
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
		else
		{
			checkSum=0;
			len=CommRead(&checkSum, 2, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
			if(!canceling && len!=2)
			{
				str.Format(_T(" Unknown answer after erase flash readed: length=0x%8.8X, but must be 0x%8.8X."), len, 2);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
			else if(!canceling && checkSum!=VDP_CMDLDR_WRITE_OK_ERASE)
			{
				str.Format(_T(" Unknown answer after erase flash readed: 0x%4.4X, but must be 0x%4.4X."), checkSum, VDP_CMDLDR_WRITE_OK_ERASE);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}

			if(m_LdrOpt.m_WriteCmdVersion==1)
			{
				//read ans after erase ok.
				memset(&dataAns, 0, sizeof(dataAns));
				len=CommRead(&dataAns, sizeof(dataAns), VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
				if(!canceling && len!=sizeof(dataAns))
				{
					str.Format(_T(" Unknown start write ack readed: length=0x%8.8X, but must be 0x%8.8X."), len, sizeof(dataAns));
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
#else	 //#ifndef VDP_EMULATE_WRITING
		Sleep(200);
#endif	 //#ifndef VDP_EMULATE_WRITING

		//reading answer after flash write
		if(!canceling && pProgress)
			pProgress->SetStatus(strStat+strEvent+MSTR("Writing..."));
#ifndef VDP_EMULATE_WRITING
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterWriteStep)
		{
			if(!LoaderSkipData(VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterWriteStep))
			{
				if(!canceling)  //this check is needed if we do not want see next error message when we clicked Cancel.
				{
					str.Format(_T(" Unknown answer after write flash readed, length must be 0x%8.8X."), m_LdrOpt.m_WriteCmdSkipBytesAfterWriteStep);
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
		else
		{
			checkSum=0;
			len=CommRead(&checkSum, 2, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
			if(!canceling && len!=2)
			{
				str.Format(_T(" Unknown answer after write flash readed: length=0x%8.8X, but must be 0x%8.8X."), len, 2);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
			else if(!canceling && checkSum!=VDP_CMDLDR_WRITE_OK_ACK)
			{
				str.Format(_T(" Unknown answer after write flash readed: 0x%4.4X, but must be 0x%4.4X."), checkSum, VDP_CMDLDR_WRITE_OK_ACK);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
		}
#else	 //#ifndef VDP_EMULATE_WRITING
		Sleep(500);
#endif	 //#ifndef VDP_EMULATE_WRITING

		//read the checksum of written data to flash
#ifndef VDP_EMULATE_WRITING
		checkSumReaded=0;
		len=CommRead(&checkSumReaded, VDP_CMDLDR_WRITE_OUT_CRC_LEN, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
		if(!canceling && len!=VDP_CMDLDR_WRITE_OUT_CRC_LEN)
		{
			str.Format(_T(" Unknown written flash CRC readed: length=0x%8.8X, but must be 0x%8.8X."), len, VDP_CMDLDR_WRITE_OUT_CRC_LEN);
			strError+=str;
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
				ERROR_INVALID_FUNCTION, m_ComPort+strError);
		}

		//read the answer after checksum.
		if(m_LdrOpt.m_IsExistWriteCmdSkipBytesAfterWittenCheckSum)
		{
			if(!LoaderSkipData(VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT, m_LdrOpt.m_WriteCmdSkipBytesAfterWittenCheckSum))
			{
				if(!canceling)  //this check is needed if we do not want see next error message when we clicked Cancel.
				{
					str.Format(_T(" Unknown result of writing flash readed, length must be 0x%8.8X."), m_LdrOpt.m_WriteCmdSkipBytesAfterWittenCheckSum);
					strError+=str;
					AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
						ERROR_INVALID_FUNCTION, m_ComPort+strError);
				}
			}
		}
		else
		{
			memset(buf, 0, sizeof(buf)); 
			len=CommRead(buf, vdpAnsLoaderWriteOkLen, VDP_MEMORY_WRITE_ALL_PAGE_TIMEOUT);
			if(!canceling && len < vdpAnsLoaderWriteOkLen)
			{
				str.Format(_T(" Unknown result of writing flash readed: length=0x%8.8X, but must be 0x%8.8X."), len, vdpAnsLoaderWriteOkLen);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
			else if(!canceling && memcmp(buf, vdpAnsLoaderWriteOk, vdpAnsLoaderWriteOkLen)!=0)
			{
				str.Format(_T(" Wrong result readed. Must be: %c%c, readed: %c%c."),
					(TCHAR)vdpAnsLoaderWriteOk[0], (TCHAR)vdpAnsLoaderWriteOk[1], (TCHAR)buf[0], (TCHAR)buf[1]);
				strError+=str;
				AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
					ERROR_INVALID_FUNCTION, m_ComPort+strError);
			}
		}
#endif	 //#ifndef VDP_EMULATE_WRITING
		if(!SetMayCancel(oldcancelstate))
			AfxThrowUserException();
		if(canceling)
			AfxThrowUserException();

		if(curSize&0x01)
		{
			strError+=_T(" Current block size is not aligned to WORD");
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_INVALID_FUNCTION),
				ERROR_INVALID_FUNCTION, m_ComPort+strError);
		}
#ifndef VDP_EMULATE_WRITING
		if(pBuf!=NULL)
		{
			for(checkSum=0, len=0; len<(DWORD)(curSize/2); len++)
				checkSum+=((WORD*)pBuf)[len];
			if(checkSumReaded!=checkSum)
			{
				if(pProgress)
				{
					strEvent=MSTR("Written data CRC error - writing again...");
					strEvent+=_T(' ');
					pProgress->SetStatus(strStat+strEvent);
				}
				DeviceSkipData(VDP_WRITE_AFTER_BAD_CRC_SKIP_TIMEOUT);
				ThrowExceptionIfLoaderNotReady();
				m_ErrorsCorrectedCount++;
				continue;
			}
		}
#endif	 //#ifndef VDP_EMULATE_WRITING
		if(pProgress)
		{
			pProgress->OffsetPos(curSize);
			pProgress->SetStatus(strStat+m_strWritingStatOk);
		}
		restSize-=curSize;
		Address+=curSize;
		pBuf=((BYTE*)pBuf)+curSize;
		strEvent.Empty();
	}

	return Size-restSize;
}

// //Reset the loader in the phone
BOOL VDevicePhone::Reset(void)
{
	OpenDevice(TRUE);
	TRY{LoaderStopLoader();}CATCH_ALL(e){}END_CATCH_ALL;
	CloseDevice();
	VDevice::Reset();
	return TRUE;
}

// Empty the combobox with phone names and destroy the associated item data
BOOL VDevicePhone::EmptyComboBox(CComboBox* pCB)
{
	int i;
	VPHONE_INFO* pi;
	for(i=0; i<pCB->GetCount(); i++)
	{
		pi=(VPHONE_INFO*)pCB->GetItemDataPtr(i);
		if(pi!=NULL)
			delete pi;
	}
	pCB->ResetContent();
	return TRUE;
}

// Search the laders and fill the combo box by file names of loaders
BOOL VDevicePhone::FillByLoadersComboBox(CComboBox* pCB)
{
	static BOOL alreadyFilled=FALSE;
	if(alreadyFilled)
		return TRUE;
	alreadyFilled=TRUE;
	EmptyComboBox(pCB);
	int i;
	VPHONE_INFO* pi;
	CString str, key, type, name;
	CFileFind finder;
	BOOL bWorking=finder.FindFile(((CKLayApp*)AfxGetApp())->m_LoadersPath+_T("*.vkd"));
	while(bWorking)
	{
		bWorking=finder.FindNextFile();

		name=VGetPrivateProfileString(drvsecPhoneCommonInfo, drvkeyName, finder.GetFileTitle(), finder.GetFilePath());
		type=VGetPrivateProfileString(drvsecPhoneCommonInfo, drvkeyType, NULL, finder.GetFilePath());
		if(type.GetLength()==1 && type[0]==_T('.'))
			type.Empty();
		for(i=0; ; i++)
		{
			key.Format(drvsecPhone, i+1);
			str=VGetPrivateProfileString(key, drvkeyName, NULL, finder.GetFilePath());
			if(str.IsEmpty())
				break;
			type=VGetPrivateProfileString(key, drvkeyType, type, finder.GetFilePath());
			if(type.GetLength()==1 && type[0]==_T('.'))
				type.Empty();
			pi=new VPHONE_INFO;
			pi->num=i+1;
			pi->fileName=finder.GetFilePath();
			if(!type.IsEmpty())
				str+=CString(_T(" ("))+type+_T(")");
			pCB->SetItemDataPtr(pCB->AddString(str), pi);
		}
		if(i==0)
		{
			pi=new VPHONE_INFO;
			pi->num=0;
			pi->fileName=finder.GetFilePath();
			if(!type.IsEmpty())
				name+=CString(_T(" ("))+type+_T(")");
			pCB->SetItemDataPtr(pCB->AddString(name), pi);
		}
	}
	pCB->InsertString(0, IdentStr(MSTR("Select phone:")));
	return TRUE;
}

// return the string with detailed info of parameters of device
CString VDevicePhone::ReadDeviceInfoString(void)
{
	CString str(MSTR("Flash Info: ERROR, can not read the flash info! Try to press Refresh again..."));
	TRY
	{
		LoaderReadFlashInfo();
		str=MakeFlashInfoString();
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL;
	return str;
}

// Authorize the loader
BOOL VDevicePhone::LoaderAuthorization(void)
{
	if(m_LdrOpt.m_Authorization == 1)
	{
		UINT64 imei;
		MakeFlashInfoString(NULL, NULL, NULL, NULL, &imei);
		int i;
		DWORD r = 0;
		for(i = 0; i < 14; i++)
			r = ((r << 1) + (r << 3)) + (i & 1 ? ((((BYTE*)(&imei))[(14 - i) / 2] >> 4) & 0x0f) : ((((BYTE*)(&imei))[(14 - i) / 2] >> 0) & 0x0f));
		for(i = 0; i < 0x23c; i++)
		{
			if(r & 0x0001)
			{
				r = (r >> 1) | 0x80000000;
			}
			else
			{
				r >>= 1;
				r ^= 0x93000;
			}
		}
		i=0;
		((BYTE*)(&imei))[i++] = vdpCmdLoaderAuthorization[0];
		while(i < 5)
			((BYTE*)(&imei))[i++] = ((BYTE*)(&r))[sizeof(r) - i];

		VDCSec CriticalSectionOfDevice(this);
		CommWrite(&imei, 5);
		if(!LoaderIsReady())
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_NOT_READY),
			ERROR_NOT_READY, m_ComPort+_T(" - Loader: Authorization #1: Loader not ready"));
	}
	return TRUE;
}

// read the flash info from phone
BOOL VDevicePhone::LoaderReadFlashInfo(void)
{
	VDCSec CriticalSectionOfDevice(this);

	if(!m_LdrOpt.m_IsInfoCmdDis)
		if(!LoaderIsReady())
			AfxThrowFileException(CFileException::OsErrorToException(ERROR_NOT_READY),
				ERROR_NOT_READY, m_ComPort+_T(" - Loader: Loader not ready while reading Flash Info"));

	memset(&m_PhoneInfo, 0, sizeof(m_PhoneInfo));
	m_PhoneInfoLen=0;
	if(m_LdrOpt.m_IsInfoCmdDis)
		return TRUE;
	BOOL oldc=IsCanCancel();
	for(int i=0; i < VDP_READ_INFO_DATA_TRY_COUNT; i++)
	{
		if(!SetMayCancel(FALSE))
			AfxThrowUserException();
		m_PhoneInfoLen=DeviceSendCommand(vdpCmdLoaderFlashInfo, &m_PhoneInfo, sizeof(m_PhoneInfo),
			VDP_READ_INFO_TIMEOUT, VDP_READ_INFO_INTERVAL_TIMEOUT);
		if(!SetMayCancel(oldc))
			AfxThrowUserException();
		if(m_PhoneInfoLen!=0)
			break;

		m_PhoneInfoLen=DeviceSendCommand(vdpCmdLoaderWithNoAnswer, &m_PhoneInfo, sizeof(m_PhoneInfo),
			VDP_READ_INFO_TIMEOUT, VDP_READ_INFO_INTERVAL_TIMEOUT);
		if(m_PhoneInfoLen!=0)
			break;

		ThrowExceptionIfLoaderNotReady();
	}

	return TRUE;
}

// Make a Flash info string from the data from phone
CString VDevicePhone::MakeFlashInfoString(CString* pManufac/*=NULL*/, CString* pModel/*=NULL*/, CString* pLangPack/*=NULL*/,
	int* pFwVer/*=NULL*/, UINT64* pbcdIMEI/*=NULL*/, DWORD* pPhoneID0/*=NULL*/, DWORD* pPhoneID1/*=NULL*/)
{
	if(pManufac!=NULL)
		pManufac->Empty();
	if(pModel!=NULL)
		pModel->Empty();
	if(pLangPack!=NULL)
		pLangPack->Empty();
	if(pFwVer!=NULL)
		*pFwVer=0;
	if(pbcdIMEI!=NULL)
		*pbcdIMEI=0;
	if(pPhoneID0!=NULL)
		*pPhoneID0=0;
	if(pPhoneID1!=NULL)
		*pPhoneID1=0;

	CString str, tmp, num, icformat, pidformat, divider;
	str=IdentStr(MSTR("Flash Info:"), 0, 1);
	icformat.Format(MSTR("ID%s: %s, Type%s: %s"), intformat, hex4digformat, intformat, hex4digformat);
	pidformat.Format(MSTR("PhoneID%s: %s"), intformat, _T("%s"));
	CString imeiformat=MSTR("IMEI: %s");
	divider=MSTR("; ");

	CString strModel, strManufac, strLangPack, strExtra;
	int fwVer=-1;
	DWORD pID0=-1, pID1=-1, ftyp0=-1, ftyp1=-1;
	UINT64 bcdIMEI=-1;
	if(m_PhoneInfoLen==sizeof(VDP_ANS_CMD_LOADER_INFO))
	{
		VDP_ANS_CMD_LOADER_INFO *pi=(VDP_ANS_CMD_LOADER_INFO *)m_PhoneInfo;
		pi->strModelName[sizeof(pi->strModelName)-1]=0;
		strModel=pi->strModelName;
		pi->strManufacturerName[sizeof(pi->strManufacturerName)-1]=0;
		strManufac=pi->strManufacturerName;
		pi->strLangpackName[sizeof(pi->strLangpackName)-1]=0;
		strLangPack=pi->strLangpackName;
		fwVer=pi->FWVersion;
		pID0=pi->phoneID0;
		pID1=pi->phoneID1;
		ftyp0=pi->flash0Type;
		ftyp1=pi->flash1Type;
	}
	else if(m_PhoneInfoLen==sizeof(VDP_ANS_CMD_LOADER_INFO_V2))
	{
		VDP_ANS_CMD_LOADER_INFO_V2 *pi=(VDP_ANS_CMD_LOADER_INFO_V2 *)m_PhoneInfo;
		pi->strModelName[sizeof(pi->strModelName)-1]=0;
		strModel=pi->strModelName;
		pi->strManufacturerName[sizeof(pi->strManufacturerName)-1]=0;
		strManufac=pi->strManufacturerName;
		pi->strLangpackName[sizeof(pi->strLangpackName)-1]=0;
		strLangPack=pi->strLangpackName;
		fwVer=pi->FWVersion;
		pID0=pi->phoneID0;
		pID1=pi->phoneID1;
		ftyp0=pi->flash0Type;
		ftyp1=pi->flash1Type;
	}
	else if(m_PhoneInfoLen==sizeof(VDP_ANS_CMD_LOADER_INFO_V3))
	{
		VDP_ANS_CMD_LOADER_INFO_V3 *pi=(VDP_ANS_CMD_LOADER_INFO_V3 *)m_PhoneInfo;
		pi->strModelName[sizeof(pi->strModelName)-1]=0;
		strModel=pi->strModelName;
		pi->strManufacturerName[sizeof(pi->strManufacturerName)-1]=0;
		strManufac=pi->strManufacturerName;
		pi->strIMEI[sizeof(pi->strIMEI)-1]=0;
		VStrAToBCD((char*)pi->strIMEI, &bcdIMEI, sizeof(bcdIMEI));
		ftyp0=pi->flash0Type;

{
strExtra.Format(_T("FlashBaseAddr: 0x%8.8lX; FlashSize: 0x%8.8lX; FlashRegionsNum: %i; "),
	(DWORD)pi->flashBaseAddr, ((DWORD)1)<<pi->flashSizePow, (int)pi->flashRegionsNum);
WORD *pRegionsInfo=&pi->flashRegion0BlocksNumMinus1;
CString stret;
for(int i=0; i<pi->flashRegionsNum; i++)
{
	stret.Format(_T("Region %i: blocks count=%i, size of each=0x%8.8lX; "),
		i+1, ((INT)(pRegionsInfo[i*2+0]+1)), (DWORD)(pRegionsInfo[i*2+1]*256));
	strExtra+=stret;
}
}

		if(!m_Cache.IsParametersValid())
		{
			m_Cache.Init();
			int i;
			INT64 startAddr=pi->flashBaseAddr;
			WORD *pRegionsInfo=&pi->flashRegion0BlocksNumMinus1;
			for(i=0; i<pi->flashRegionsNum; i++)
			{
				m_Cache.AddParameterMemGeometry(startAddr, pRegionsInfo[i*2+1]*256);
				startAddr+=((INT64)(pRegionsInfo[i*2+0]+1))*(pRegionsInfo[i*2+1]*256);
			}
		}
	}
	else
		return CString(MSTR("Read Info error: Unknown info answer"));

	str.Empty();
	str+=strManufac;
	if(!strModel.IsEmpty())
	{
		str+=_T(' ');
		str+=strModel;
	}
	if(!strLangPack.IsEmpty())
	{
		str+=_T(' ');
		str+=strLangPack;
	}
	if(fwVer!=-1)
	{
		str+=_T(' ');
		num.Format(_T("fw%x"), fwVer);
		str+=num;
	}

	if((ftyp0!=0 && ftyp0!=-1) || (ftyp1!=0 && ftyp1!=-1))
	{
		str+=divider;
		str+=MSTR("Flash:");
		str+=_T(' ');

		if(ftyp0!=0 && ftyp0!=-1)
		{
			tmp.Format(icformat, 1, LOWORD(ftyp0), 1, HIWORD(ftyp0));
			str+=tmp;
		}
		if(ftyp1!=0 && ftyp1!=-1)
		{
			if(ftyp0!=0 && ftyp0!=-1)
				str+=divider;
			tmp.Format(icformat, 2, LOWORD(ftyp1), 2, HIWORD(ftyp1));
			str+=tmp;
		}
	}

	if(pID0!=-1 || pID1!=-1)
	{
		tmp=m_Name;
		if(tmp.MakeLower().Find(_T("c30"))==-1)
		{
			if(pID0!=-1)
			{
				str+=divider;
				num.Format(hex8diglongformat, pID0);
				tmp.Format(pidformat, 1, num);
				str+=tmp;
			}
			if(pID1!=-1)
			{
				str+=divider;
				num.Format(hex8diglongformat, pID1);
				tmp.Format(pidformat, 2, num);
				str+=tmp;
			}
		}
		else
		{
			str+=divider;
			num.Format(hex16digi64format, ((((unsigned __int64)pID1)<<32)|pID0));
			tmp.Format(pidformat, 1, num);
			str+=tmp;
		}
	}

	if(bcdIMEI!=-1)
	{
		str+=divider;
		num.Format(_T("%15.15I64X"), bcdIMEI);
		tmp.Format(imeiformat, num);
		str+=tmp;
	}

	if(!strExtra.IsEmpty())
	{
		str+=divider;
		str+=strExtra;
	}

{
CString vt;
CString dbg(_T("   dbg: cache param: "));
int n=(int)m_Cache.m_MemGeometry.GetSize();
vt.Format(_T("No=%i,"),n);
dbg+=vt;
for(int i=0; i<n; i++)
{
	dbg+=VSignedHexToStr(m_Cache.m_MemGeometry[i].startAddr, addr64format);
	dbg+=_T(": ");
	dbg+=VSignedHexToStr(m_Cache.m_MemGeometry[i].pageSize, addr64format);
	dbg+=_T(", ");
}
str+=dbg;
}



	if(pManufac!=NULL)
		*pManufac=strManufac;
	if(pModel!=NULL)
		*pModel=strModel;
	if(pLangPack!=NULL)
		*pLangPack=strLangPack;
	if(pFwVer!=NULL)
		*pFwVer=fwVer;
	if(pbcdIMEI!=NULL)
		*pbcdIMEI=bcdIMEI;
	if(pPhoneID0!=NULL)
		*pPhoneID0=pID0;
	if(pPhoneID1!=NULL)
		*pPhoneID1=pID1;
	return str;
}

// Main program must call this function, when parameters of the device is changed
BOOL VDevicePhone::OnChangeParameters(void)
{
	if(!VDevice::OnChangeParameters())
		return FALSE;
	CDialog* dlg=(CDialog*)AfxGetMainWnd();
	if(dlg!=NULL)
	{
		CComboBox* lpcb=(CComboBox*)dlg->GetDlgItem(IDC_PHONES_LIST);
		if(lpcb!=NULL)
			lpcb->GetWindowText(strCurPhoneModelName);
	}
	CloseDevice();
	return OpenCurBootFile();
}



// // Check, if skipping of writing to some part of this block is needed
BOOL VDevicePhone::IsSkipWritingInBlock(INT64 Address, INT64 Size)
{
	if((m_bSkipWritingBootCore==FALSE && m_bEnableNoWrite==FALSE) || m_MemArea.IsEmpty())
		return FALSE;
	Size+=Address;
	int i, n;
	INT64 beg, end;
	n=(int)m_MemArea.GetCount();
	for(i=0; i<n; i++)
	{
		if(m_MemArea[i].Flags&(AreaInf::bootcore|AreaInf::nowrite))
		{
			beg=m_MemArea[i].Address-m_MemAreaStart;
			end=beg+m_MemArea[i].Size;
			if(Address < end && Size > beg)
			{
				if(m_MemArea[i].Flags&AreaInf::nowrite)
				{
					if(m_bEnableNoWrite==TRUE)
						return TRUE;
					if(!o_bDisableWarningsMsg)
					{
						CString msg, msgareas, str;
						msgareas+=_T("\n\n");
						int ia, nlenmax=0;
						for(ia=0; ia<m_MemArea.GetSize(); ia++)
							if(m_MemArea[ia].Flags&AreaInf::nowrite)
								if(nlenmax < m_MemArea[ia].Name.GetLength())
									nlenmax=m_MemArea[ia].Name.GetLength();
						for(ia=0; ia<m_MemArea.GetSize(); ia++)
						{
							if(m_MemArea[ia].Flags&AreaInf::nowrite)
							{
								CString sm(_T(' '), nlenmax);
								sm.CopyChars(sm.GetBuffer(), m_MemArea[ia].Name, m_MemArea[ia].Name.GetLength());
								str.Format(_T("%s\t(%s, size=%s)\n"), sm,
									VSignedHexToStr(m_MemArea[ia].Address-m_MemAreaStart, addr64format),
									VSignedHexToStr(m_MemArea[ia].Size, addr64format));
								msgareas+=str;
							}
						}
						msgareas+=_T("\n");
						msg.Format(
							MSTR("! ! ! WARNING ! ! !\n\nWriting to this memory areas of phone is DISABLED:%sLoader do not support writing any data to this areas.\nIf you continue, writing to this areas will be skipped.\n\nContinue operation?"),
							msgareas);
						if(AfxMessageBox(msg, MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON1)!=IDYES)
							AfxThrowUserException();
					}
					m_bEnableNoWrite=TRUE;
					return TRUE;
				}
				else if(m_MemArea[i].Flags&AreaInf::bootcore)
				{
					if(m_bSkipWritingBootCore!=TRUE && m_bSkipWritingBootCore!=FALSE)
					{
						CString strskipcmt, num1, num2, num3, tmp;
						INT64 pageSize=m_Cache.GetPageSizeAtAddr(Address);
						num1.Format(addr64format, pageSize);
						num2.Format(uint64format, pageSize);
						num3.Format(uint64format, pageSize/1024);
						strskipcmt.Format(
							MSTR("Rest data (which is not in BOOTCORE section) will be written. Note that in this case some patches, which is changing some data in BOOTCORE section of flash, may not work. Also note that this will skip writing not only in BOOTCORE section but in whole block of flash memory IC in which BOOTCORE lies. Current size of block is %s = %s bytes = %s KB"),
							num1, num2, num3);

						CString strbootcoreinfo;
						INT64 size=end-beg;
						num1.Format(addr64format, size);
						num2.Format(uint64format, size);
						num3.Format(uint64format, size/1024);
						strbootcoreinfo.Format(
							MSTR("Current size of BOOTCORE is %s = %s bytes = %s KB."),
							num1, num2, num3);
						strskipcmt+=CString(_T(". "))+strbootcoreinfo;

						if(o_bIsEnableBootcoreWritting)
						{
							int ret;
							tmp=CString(_T("\n\n\n  "))+
								MSTR("CANCEL - to CANCEL write operation.\n\n  YES - to write data in BOOTCORE. Press YES only if you SURE that you write to BOOTCORE a RIGHT DATA.\n\n  NO - to do not write data in BOOTCORE.")
								+_T(' ')+strskipcmt;
							ret=AfxMessageBox(CString(
								MSTR("! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !\nWARNING!               WARNING!               WARNING!\n! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !\n\nYou trying to write some data to the BOOTCORE section of phone flash!\nIf you write to BOOTCORE a WRONG DATA, you cannot repair a phone later!\n\n\nDo you want to write data in BOOTCORE section of the flash of phone?")
								)+tmp, MB_YESNOCANCEL|MB_DEFBUTTON2|MB_ICONSTOP);
							if(ret!=IDYES && ret!=IDNO)
								AfxThrowUserException();
							m_bSkipWritingBootCore= (ret==IDNO);
						}
						else
						{
							if(AfxMessageBox(
								CString(MSTR("! ! ! WARNING ! ! !\n\nYou trying to write some data to the BOOTCORE section of phone flash.\n\nWriting to BOOTCORE is DISABLED!"))
								+_T("\n\n")+strskipcmt+_T("\n\n")+
								MSTR("If you want to enable writing in BOOTCORE - go to the \"Options\" -> \"Advanced...\".\n\nContinue without writing to BOOTCORE?"),
								MB_ICONSTOP|MB_DEFBUTTON1|MB_YESNO)!=IDYES)
								AfxThrowUserException();
							m_bSkipWritingBootCore=TRUE;
						}
					}
					return m_bSkipWritingBootCore;
				}
			}
		}
	}
	return FALSE;
}

// Return the name of current working device
CString VDevicePhone::GetName(void)
{
	CString name, str;
	CWnd* dlg=AfxGetMainWnd();
	if(dlg!=NULL)
		dlg->GetDlgItemText(IDC_PHONES_LIST, name);
	else
		name=_T("phone");
	str=VDevice::GetName();
	if(!str.IsEmpty())
		str+=_T('_');
	return str+name;
}

// Return the unique name of current connected device
CString VDevicePhone::GetUniqueName(void)
{
	CString str, LangPack;
	int FwVer;
	DWORD PhoneID0, PhoneID1;
	UINT64 bcdIMEI;
	MakeFlashInfoString(NULL, &str, &LangPack, &FwVer, &bcdIMEI, &PhoneID0, &PhoneID1);
	if(str.IsEmpty() && LangPack.IsEmpty() && FwVer==0 && PhoneID0==0 && PhoneID1==0 && bcdIMEI==0)
		return GetName();
	if(bcdIMEI==0 || bcdIMEI==-1)
		bcdIMEI=((((unsigned __int64)PhoneID1)<<32)|PhoneID0);

	CString num;
	if(!LangPack.IsEmpty())
	{
		str+=_T(' ');
		str+=LangPack;
	}
	if(FwVer!=0 && FwVer!=-1)
	{
		str+=_T(' ');
		num.Format(_T("fw%x"), FwVer);
		str+=num;
	}
	if(bcdIMEI!=0 && bcdIMEI!=-1)
	{
		str+=_T(' ');
		num.Format(_T("%16.16I64X"), bcdIMEI);
		str+=num;
	}
	return str;
}

void VDevicePhone::SetLdrStateLoaded(BOOL isLoaded/*=TRUE*/)
{
	if(this==NULL)
		return;
	m_bLdrStateLoaded=isLoaded;
	if(!isLoaded)
		VDCSec::Reset(this);
	CKLayDlg *maindlg=(CKLayDlg*)AfxGetMainWnd();
	if(maindlg==NULL)
		return;
	if(maindlg->m_LdrStateCtrl.GetSafeHwnd()==NULL)
		return;
	maindlg->m_LdrStateCtrl.SetColor(isLoaded ? VKL_LDR_LOADED_COLOR : VKL_LDR_UNLOADED_COLOR);
	CString str;
	if(isLoaded!=VKL_LDR_STATE_HIDE)
	{
		if(isLoaded)
			str=MSTR("Loader is loaded. Do not disconnect the phone!");
		else
			str=MSTR("Loader is unloaded. Phone can be disconnected.");
	}
	if(maindlg->m_pToolTipCtrl->GetSafeHwnd()!=NULL)
		maindlg->m_pToolTipCtrl->UpdateTipText(str, maindlg, IDC_PHONE_LDRSTATE);
}

// Check is need to skip loader load/unload
BOOL VDevicePhone::IsSkipLoaderLoadUnload(void)
{
	if(!(GetKeyState(VK_SHIFT)&SHIFTED))
		return FALSE;
	if(!(GetKeyState(VK_CONTROL)&SHIFTED))
		return FALSE;
	if(!(GetKeyState(VK_MENU)&SHIFTED))
		return FALSE;
	return TRUE;
}

// Return bit mask of supported operations
DWORD VDevicePhone::GetSupportedOperations(DWORD checkMask/*=-1*/)
{
	DWORD res=0;
	if(checkMask&VDevice::read)
		res|=VDevice::read;
	if(checkMask&VDevice::write)
		res|=VDevice::write;
	if(checkMask&VDevice::restorebootcore && m_LdrOpt.m_IsRstBootcoreCmdEn)
		res|=VDevice::restorebootcore;
	if(checkMask&VDevice::getmemorymap)
		res|=VDevice::getmemorymap;
	return res;
}

// Restore bootcore of phone to original state
BOOL VDevicePhone::RestoreBootcore(void)
{
	LoaderWriteMemory(-1, -1, NULL);
	return TRUE;
}

// Get memory map of flash IC of phone
BOOL VDevicePhone::GetMemoryMap(VDevMemMapArray &arrayMemMap)
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
			if(m_LdrOpt.m_IsTstEmptyCmdEn)
				mmb.isEmpty=LoaderTestIsEmpty(mmb.addr, mmb.size);
			else
			{
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

// called for sending keep alive packets for loader
int VDevicePhone_KeepAlive_Divider = 1;
BOOL VDevicePhone::KeepAlive(void)
{
	if(m_LdrOpt.m_IsKeepAliveCmdEnAndInterval != 0)
	{
		if(VDevicePhone_KeepAlive_Divider > m_LdrOpt.m_IsKeepAliveCmdEnAndInterval)
			VDevicePhone_KeepAlive_Divider = m_LdrOpt.m_IsKeepAliveCmdEnAndInterval;
		if(--VDevicePhone_KeepAlive_Divider <= 0)
		{
			VDevicePhone_KeepAlive_Divider = m_LdrOpt.m_IsKeepAliveCmdEnAndInterval;
			if(m_hCom != INVALID_HANDLE_VALUE && m_bLdrStateLoaded && !VDCSec::IsLocked(this))
			{
				TRY
				{
#ifdef VDP_CMD_LOADER_KEEPALIVE_ANS_TIMEOUT
					DeviceSendCommand(vdpCmdLoaderKeepAlive, INGORE_ANSWER, -1, VDP_CMD_LOADER_KEEPALIVE_ANS_TIMEOUT);
#else
					DeviceSendCommand(vdpCmdLoaderKeepAlive, NULL);
#endif
				}
				CATCH_ALL(e)
				{
				}
				END_CATCH_ALL;
			}
		}
	}
	return VDevice::KeepAlive();
}


// Test, if loader is ready, and, if not - throw exception
void VDevicePhone::ThrowExceptionIfLoaderNotReady(void)
{
	for(int i=0; i<4; i++)
	{
		CommReset();
		DeviceSkipData(10);
		if(LoaderIsReady())
		{
			CommReset();
			if(LoaderIsReady())
			{
				DeviceSkipData(10);
				return;
			}
		}
	}
	ThrowCommException(_T("Loader not ready!"));
}







//////////////////////////////////////////////////////////////////////////

VDevicePhoneCmdBaudsCodes::VDevicePhoneCmdBaudsCodes(void)
{
}

// Init members of class
void VDevicePhoneCmdBaudsCodes::Init(void)
{
	RemoveAll();
}


// Initialising the class by parameters from string
BOOL VDevicePhoneCmdBaudsCodes::ParseParameters(LPCTSTR strParam)
{
	CString str, par(strParam);
	par.Trim();
	if(par.IsEmpty())
		return TRUE;
	int i, pos;
	VDEVICEPHONE_BAUDCODE bc;
	for(pos=0, i=0; ; i++)
	{
		str=VGetParameterFromList(par, pos);
		if(str.IsEmpty())
			break;
		bc.baud=(DWORD)VStrToInt(str);

		str=VGetParameterFromList(par, pos);
		if(str.IsEmpty())
		{
			if(i==0 && bc.baud==0)
			{
				return TRUE;
			}
			else
			{
				Init();
				if(strParam==vdpCmdLoaderBaudListDefault)
					AfxMessageBox(_T("VDevicePhoneCmdBaudsCodes::ParseParameters: error in default list of bauds codes."));
				return FALSE;
			}
		}
		bc.code=(DWORD)VStrToInt(str);
		if(i==0)
			Init();
		Add(bc);
	}
	return TRUE;
}

// Return code for specified baud rate or -1 if code is not found
DWORD VDevicePhoneCmdBaudsCodes::GetCode(DWORD baud)
{
	DWORD c=-1;
	for(int i=0; i<GetSize(); i++)
	{
		if(GetAt(i).baud==baud)
		{
			c=GetAt(i).code;
			break;
		}
	}
	return c;
}



