// KLayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "KLayDlg.h"
#include "LayoutPage.h"
#include "PatchPage.h"
#include "VDevice.h"
#include "VDeviceFile.h"
#include "VDevicePhone.h"
#include "FlasherPage.h"
#include "OptionsDlg.h"

#include "jrtslinkctrl.h"
#include ".\klaydlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


_TCHAR strDefTool1Name[]=_T("#1");
_TCHAR strDefTool2Name[]=_T("#2");
_TCHAR strDefTool3Name[]=_T("#3");



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedHelpwnd();
	CJRTSLinkCtrl m_WebCtrl;
	CJRTSLinkCtrl m_EmailCtrl;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WEB, m_WebCtrl);
	DDX_Control(pDX, IDC_EMAIL, m_EmailCtrl);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_HELPWND, OnBnClickedHelpwnd)
END_MESSAGE_MAP()


// CKLayDlg dialog



CKLayDlg::CKLayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKLayDlg::IDD, pParent)
	, m_MinSize(0)
{
	m_DynSize.AddControl(IDCANCEL, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(ID_ABOUT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(ID_OPTIONS, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_WORKWITH_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SELECT_FILE, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONES_LIST, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_PORTS_LIST, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_FNAME, EL(00), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_BROWSE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_FROM, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_SIZE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_FILE_TEXT, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_FROM_TEXT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_FLASH_SIZE_TEXT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_PORTS_LIST_TEXT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_BAUDS_LIST, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_BAUDS_LIST_TEXT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_RESET, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_PHONE_LDRSTATE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TAB, EL(0), EL(0), EL(100), EL(100));
	m_DynSize.AddControl(IDC_RESIZE_MARKER_STATIC, EL(100), EL(100), EL(100), EL(100));
	m_DynSize.AddControl(IDC_RESIZE_MARKER2_STATIC, EL(100), EL(100), EL(100), EL(100));
	m_DynSize.AddControl(IDC_PHONE_FLASH_INFO, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOLS_OPTIONS, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXTLAY, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXTPIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXTFNT, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXT1, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXT2, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_TOOL_EXT3, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_DONATE_LINK, EL(100), EL(100), EL(100), EL(100));
	m_DynSize.AddControl(IDC_DONATE_DETAILS, EL(0), EL(100), EL(100), EL(100));

	m_hIcon = AfxGetApp()->LoadIcon(IDI_VKLAY);
	m_DeviceType=0;
	m_iUpdateTimer=0;
	m_pToolTipCtrl=NULL;
	m_hAccelerator=NULL;
	m_hTimerKeepAlive=0;
	memset(pPage, NULL, sizeof(pPage));
}

CKLayDlg::~CKLayDlg()
{
	if(m_pToolTipCtrl!=NULL)
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
}

void CKLayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_TabCtrl);
	DDX_Control(pDX, IDC_PHONES_LIST, m_PhonesListCtrl);
	DDX_Control(pDX, IDC_PHONE_LDRSTATE, m_LdrStateCtrl);
	DDX_Control(pDX, IDC_DONATE_LINK, m_DonateLinkCtrl);
}

BEGIN_MESSAGE_MAP(CKLayDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_ABOUT, OnBnClickedAbout)
	ON_BN_CLICKED(IDC_FLASH_BROWSE, OnBnClickedFlashBrowse)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnTcnSelchangingTab)
//	ON_EN_CHANGE(IDC_FLASH_FROM, OnEnChangeFlashFrom)
	ON_BN_CLICKED(IDC_SELECT_PHONE, OnBnClickedSelectPhone)
	ON_BN_CLICKED(IDC_SELECT_FILE, OnBnClickedSelectFile)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PHONE_RESET, OnBnClickedPhoneReset)
	ON_CBN_SELCHANGE(IDC_PHONES_LIST, OnCbnSelchangePhonesList)
	ON_BN_CLICKED(ID_OPTIONS, OnBnClickedOptions)
	ON_BN_CLICKED(IDC_PHONE_QUERY_INFO, OnBnClickedPhoneQueryInfo)
//	ON_WM_TIMER()
	ON_MESSAGE(VM_INITIAL_UPDATE, OnInitialUpdate)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
//	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_EN_KILLFOCUS(IDC_FLASH_FROM, OnEnKillfocusFlashFrom)
	ON_EN_KILLFOCUS(IDC_FLASH_SIZE, OnEnKillfocusFlashSize)
//	ON_EN_CHANGE(IDC_FLASH_FROM, OnEnChangeFlashFrom)
//	ON_EN_CHANGE(IDC_FLASH_SIZE, OnEnChangeFlashSize)
	ON_BN_CLICKED(IDC_TOOLS_OPTIONS, OnBnClickedToolsOptions)
	ON_BN_CLICKED(IDC_TOOL_EXTLAY, OnBnClickedToolExtlay)
	ON_BN_CLICKED(IDC_TOOL_EXTPIC, OnBnClickedToolExtpic)
	ON_BN_CLICKED(IDC_TOOL_EXT1, OnBnClickedToolExt1)
	ON_BN_CLICKED(IDC_TOOL_EXT2, OnBnClickedToolExt2)
	ON_BN_CLICKED(IDC_TOOL_EXT3, OnBnClickedToolExt3)
	ON_BN_CLICKED(IDC_TOOL_EXTFNT, OnBnClickedToolExtfnt)
	ON_COMMAND(ID_SWITCH_PANEL, OnSwitchPanel)
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
//	ON_WM_ENTERIDLE()

	// for updating dialog box controls
	ON_MESSAGE(WM_KICKIDLE,OnKickIdle)
	ON_MESSAGE(WM_IDLEUPDATECMDUI,OnIdleUpdateCmdUI)

	ON_BN_CLICKED(IDC_DONATE_LINK, OnBnClickedDonateLink)
	ON_NOTIFY(NMLC_ACTIVATE, IDC_DONATE_LINK, OnActivatedDonateLink)
END_MESSAGE_MAP()


// CKLayDlg message handlers

BOOL CALLBACK EnumResNameProc(
  HMODULE hModule,   // module handle
  LPCTSTR lpszType,  // resource type
  LPTSTR lpszName,   // resource name
  LONG_PTR lParam    // application-defined parameter
)
{
	if(*(LPTSTR*)lParam==NULL)
		*(LPTSTR*)lParam=lpszName;
	return TRUE;
}

// Set the icon and tooltip for Tool button
BOOL CKLayDlg::SetToolButton(UINT idToolButton, LPCTSTR strPathRegKey, UINT idDefaultIcon, LPCTSTR strTTipFormat, LPCTSTR strDefaultTTipToolName/*=NULL*/)
{
	CWinApp* app=AfxGetApp();
	CString str, strtt, tpath;
	CButton *btn=(CButton*)GetDlgItem(idToolButton);
	if(app==NULL || btn==NULL)
		return FALSE;
	if(strPathRegKey!=NULL)
		tpath=app->GetProfileString(secOptions, strPathRegKey, NULL);
	else
		tpath.Empty();

	HICON hIcon1=NULL;							// icon handle 
	if(!tpath.IsEmpty())
	{
		HICON hIconLarge = NULL;
		// try to get small icon from file.
		if(ExtractIconEx(tpath, 0, &hIconLarge, &hIcon1, 1) == 0)
			hIcon1 = NULL;
		if(hIconLarge != NULL)
			DestroyIcon(hIconLarge);
		hIconLarge = NULL;

		if(hIcon1 == NULL)
		{
			//try to get small associated icon from registry
            CString ext;
			str = GetFileExt(tpath);
			if(!str.IsEmpty())
			{
				ext = CString(_T('.')) + str;
				HKEY key = NULL;
				DWORD len, type;
				if(RegOpenKeyEx(HKEY_CLASSES_ROOT, ext, 0, KEY_READ, &key) == ERROR_SUCCESS)
				{
					len = 1024;
					if(RegQueryValueEx(key, NULL, NULL, &type, (LPBYTE)str.GetBuffer(len / sizeof(_TCHAR)), &len) == ERROR_SUCCESS)
					{
						str.ReleaseBuffer();
						RegCloseKey(key);
						key = NULL;
						int i, ic=0;
						if(RegOpenKeyEx(HKEY_CLASSES_ROOT, str + _T("\\DefaultIcon"), 0, KEY_READ, &key) == ERROR_SUCCESS)
						{
							len = 1024;
							if(RegQueryValueEx(key, NULL, NULL, &type, (LPBYTE)str.GetBuffer(len / sizeof(_TCHAR)), &len) == ERROR_SUCCESS)
							{
								str.ReleaseBuffer();
								i = str.ReverseFind(_T(','));
								ic = 0;
								if(i != -1)
								{
									ic = _tstoi(((LPCTSTR)(str)) + i + 1);
									str = str.Left(i);
								}
							}
						}
						else if(RegOpenKeyEx(HKEY_CLASSES_ROOT, str + _T("\\shell\\open\\command"), 0, KEY_READ, &key) == ERROR_SUCCESS)
						{
							len = 1024;
							if(RegQueryValueEx(key, NULL, NULL, &type, (LPBYTE)str.GetBuffer(len / sizeof(_TCHAR)), &len) == ERROR_SUCCESS)
							{
								str.ReleaseBuffer();
								str.Trim();
								i = 0;
								if(str[0] == _T('\"'))
								{
									i = str.Find(_T('\"'), 1);
									if(i == -1)
										i=0;
									else
										i++;
								}
								for(; i < str.GetLength() && !_istspace(str[i]); i++);
								str = str.Left(i);
								str.Trim(_T("\" "));
								ic = 0;
							}
						}
						else
							str.Empty();

						if(!str.IsEmpty())
						{
							if(ExtractIconEx(str, ic, &hIconLarge, &hIcon1, 1) == 0)
								hIcon1 = NULL;
							if(hIconLarge != NULL)
								DestroyIcon(hIconLarge);
							hIconLarge = NULL;
						}
					}
				}
				if(key != NULL)
					RegCloseKey(key);
			}
			str.ReleaseBuffer();
		}

		if(hIcon1 == NULL)
		{
			//get default small icon for some known extensions.
			CString ext;
			int ic;
			str = GetFileExt(tpath);
			if(!str.IsEmpty())
				ext = CString(_T('.')) + str.MakeLower();

			GetSystemDirectory(str.GetBuffer(1024), 1024);
			str.ReleaseBuffer();
			if(str[str.GetLength() - 1] != _T('\\'))
				str += _T('\\');
			str += _T("shell32.dll");

			// Determine the default icon for the file extension
			if     (ext == _T(".doc")) ic = 1;
			else if(ext == _T(".exe")
				 || ext == _T(".com")) ic = 2;
			else if(ext == _T(".hlp")) ic = 23;
			else if(ext == _T(".ini")
				 || ext == _T(".inf")) ic = 63;
			else if(ext == _T(".txt")) ic = 64;
			else if(ext == _T(".txt")) ic = 65;
			else if(ext == _T(".dll")
				 || ext == _T(".sys")
				 || ext == _T(".vbx")
				 || ext == _T(".ocx")
				 || ext == _T(".vxd")) ic = 66;
			else if(ext == _T(".fon")) ic = 67;
			else if(ext == _T(".ttf")) ic = 68;
			else if(ext == _T(".fot")) ic = 69;
			else ic = 0;

			if(ExtractIconEx(str, ic, &hIconLarge, &hIcon1, 1) == 0)
				hIcon1 = NULL;
			if(hIconLarge != NULL)
				DestroyIcon(hIconLarge);
			hIconLarge = NULL;
		}

		if(hIcon1 != NULL)
			btn->SetIcon(hIcon1);
	}

	if(hIcon1==NULL)
	{
		btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(idDefaultIcon),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	}

	if(strTTipFormat!=NULL)
	{
		if(strPathRegKey!=NULL)
		{
			str=GetFileTitle(tpath);
			if(str.IsEmpty())
				str=strDefaultTTipToolName;
			str.Trim();
			if(!str.IsEmpty())
				str.SetAt(0, toupper(str[0]));
			strtt.Format(strTTipFormat, str);
		}
		else
		{
			strtt=strTTipFormat;
		}
		if(m_pToolTipCtrl)
		{
			CToolInfo ti;
			if(m_pToolTipCtrl->GetToolInfo(ti, btn))
				m_pToolTipCtrl->UpdateTipText(strtt, btn);
			else
				m_pToolTipCtrl->AddTool(btn, strtt);
		}
	}
	return TRUE;
}


BOOL CKLayDlg::OnInitDialog()
{
	ModifyStyleEx(0, WS_EX_CONTROLPARENT);

	OleInitialize(NULL);

	m_hAccelerator=LoadAccelerators(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	SetDlgItemText(IDC_WORKWITH_STATIC, IdentStr(MSTR("Work with:")));
	SetDlgItemText(IDC_SELECT_PHONE, MSTR("Phone"));
	SetDlgItemText(IDC_SELECT_FILE, MSTR("File with part of Flash"));
	SetDlgItemText(IDC_FLASH_FILE_TEXT, MSTR("WARNING!  Ensure, that the begining of loaded Flash part is in the \"From address\" box!"));
	SetDlgItemText(IDC_FLASH_FROM_TEXT, MSTR("From address:"));
	SetDlgItemText(IDC_FLASH_SIZE_TEXT, MSTR("Size:"));
	SetDlgItemText(IDC_PHONE_PORTS_LIST_TEXT, MSTR("Port:"));
	SetDlgItemText(IDC_PHONE_BAUDS_LIST_TEXT, MSTR("Baud:"));
	SetDlgItemText(IDC_PHONE_QUERY_INFO, MSTR("Refresh ->"));
	SetDlgItemText(ID_OPTIONS, MSTR("Options"));
	SetDlgItemText(ID_ABOUT, MSTR("?"));
	SetDlgItemText(IDCANCEL, MSTR("Exit"));


	int i;
	CString str, strtt;
	CWnd *w;

	CDialog::OnInitDialog();

	CComboBox *pcb = (CComboBox *)GetDlgItem(IDC_PHONE_PORTS_LIST);
	if(pcb != NULL)
	{
		pcb->ResetContent();
		for(i = 1; i < 256; i++)
		{
			str.Format(_T("COM%i"), i);
			pcb->AddString(str);
		}
	}

	m_DonateLinkCtrl.SetDisplay(_CXR(cryptstrDonateLinkName));
	m_DonateLinkCtrl.SetTarget (_CXR(cryptstrDonateLinkURL));
	m_DonateLinkCtrl.SetToolTip(_CXR(cryptstrDonateLinkTip));
	w=GetDlgItem(IDC_DONATE_DETAILS);
	if(w!=NULL)
		w->SetWindowText(_CXR(cryptstrDonateDescr));

	CRect r;
	m_pToolTipCtrl=new CToolTipCtrl;
	if(!m_pToolTipCtrl->Create(this))
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
	else
	{
		m_pToolTipCtrl->SetDelayTime(TTDT_AUTOPOP, 32000);
		if(SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0))
			m_pToolTipCtrl->SetMaxTipWidth(r.Size().cx-10);
	}

	CKLayApp* app=(CKLayApp*) AfxGetApp();
	CButton *btn;
	strToolsToolTipFormat=
		MSTR("Open the fullflash of phone in the external tool %s\r\nAlso you may found something useful for you at:");
	strToolsToolTipFormat+=_T("\r\nhttp://www.siemens-club.ru");

	btn=(CButton*)GetDlgItem(IDC_TOOLS_OPTIONS);
	btn->GetWindowRect(r); ScreenToClient(r); r.left--; btn->MoveWindow(r);
	SetToolButton(IDC_TOOLS_OPTIONS, NULL, IDI_TOOLS_OPTIONS, MSTR("Options for external tools"));

	btn=(CButton*)GetDlgItem(IDC_TOOL_EXTLAY);
	btn->GetWindowRect(r); ScreenToClient(r); r.OffsetRect(5, 0); btn->MoveWindow(r);
	str=MSTR("Open the fullflash of phone in the external editor of keyboard layouts LayoutMe\r\nDownload the LayoutMe from");
	str+=_T(" http://www.avkiev.kiev.ua");
	SetToolButton(IDC_TOOL_EXTLAY, keyToolExtLayPath, IDI_TOOL_EXTLAY, str);

	btn=(CButton*)GetDlgItem(IDC_TOOL_EXTPIC);
	btn->GetWindowRect(r); ScreenToClient(r); r.OffsetRect(4, 0); btn->MoveWindow(r);
	str=MSTR("Open the fullflash of phone in the external editor of phone pictures SPC3\r\nDownload the Siemens Picture Change 3 from");
	str+=_T(" http://www.gsmdev.de");
	SetToolButton(IDC_TOOL_EXTPIC, keyToolExtPicPath, IDI_TOOL_EXTPIC, str);

	btn=(CButton*)GetDlgItem(IDC_TOOL_EXTFNT);
	btn->GetWindowRect(r); ScreenToClient(r); r.OffsetRect(3, 0); btn->MoveWindow(r);
	str=MSTR("Open the fullflash of phone in the external editor of phone fonts Siemens CE\r\nDownload the Siemens Character Editor from");
	str+=_T(" http://v3.allsiemens.com");
	SetToolButton(IDC_TOOL_EXTFNT, keyToolExtFntPath, IDI_TOOL_EXTFNT, str);

	btn=(CButton*)GetDlgItem(IDC_TOOL_EXT1);
	btn->GetWindowRect(r); ScreenToClient(r); r.OffsetRect(2, 0); btn->MoveWindow(r);
	str=MSTR("Open the fullflash of phone in the external editor of fullflash Smelter\r\nDownload the Smelter from");
	str+=_T(" http://www.avkiev.kiev.ua");
	SetToolButton(IDC_TOOL_EXT1, keyToolExt1Path, IDI_TOOL_EXTFFED, str);

	btn=(CButton*)GetDlgItem(IDC_TOOL_EXT2);
	btn->GetWindowRect(r); ScreenToClient(r); r.OffsetRect(1, 0); btn->MoveWindow(r);
	SetToolButton(IDC_TOOL_EXT2, keyToolExt2Path, IDI_TOOL_EXT1, strToolsToolTipFormat, strDefTool1Name);

	if(app->GetProfileString(secOptions, keyToolExt3Path, NULL).IsEmpty())
	{
		str = app->m_AppPath + _T("utils\\papuas_key\\V_Code2vkd.bat");
		app->WriteProfileString(secOptions, keyToolExt3Path, str);
		app->WriteProfileString(secOptions, keyToolExt3Arg, ExtToolArgs[0]);
	}
	btn=(CButton*)GetDlgItem(IDC_TOOL_EXT3);
	SetToolButton(IDC_TOOL_EXT3, keyToolExt3Path, IDI_TOOL_EXT2, strToolsToolTipFormat, strDefTool2Name);


	btn=(CButton*)GetDlgItem(IDC_PHONE_RESET);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_RESET),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Unload loader (prepare for Turn ON)"));

	m_LdrStateCtrl.SetColor(VKL_LDR_UNLOADED_COLOR);
	m_LdrStateCtrl.GetWindowRect(r);
	ScreenToClient(r);
	m_pToolTipCtrl->AddTool(this, _T("Loader state"), r, IDC_PHONE_LDRSTATE);


	i=0;
	pPage[i++]=new CPatchPage(&m_TabCtrl);
	pPage[i++]=new CLayoutPage(&m_TabCtrl);
	pPage[i++]=new CFlasherPage(&m_TabCtrl);
	m_TabCtrl.SetCurSel(-1);
	SetCurPage(VKL_PATCH_PAGE);

	w=GetDlgItem(IDC_RESIZE_MARKER_STATIC);
	if(w!=NULL)
	{
		w->GetWindowRect(r);
		ScreenToClient(r);
		r.OffsetRect(1, 1);
		w->MoveWindow(r);
	}
	w=GetDlgItem(IDC_RESIZE_MARKER2_STATIC);
	if(w!=NULL)
	{
		w->GetWindowRect(r);
		ScreenToClient(r);
		r.OffsetRect(1, 1);
		w->MoveWindow(r);
	}
	GetWindowRect(&r);
	m_MinSize.SetPoint(r.Size().cx, r.Size().cy);
	m_DynSize.AttachDialog(this);
	

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		pSysMenu->AppendMenu(MF_SEPARATOR);
		strAboutMenu=MSTR("Help");
		if (!strAboutMenu.IsEmpty())
			pSysMenu->AppendMenu(MF_STRING, IDM_HELPWND, strAboutMenu);
		strAboutMenu=MSTR("&About V_KLay...");
		if (!strAboutMenu.IsEmpty())
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	if(o_cmdExist_PartFName)
		GetFlashFileName(o_cmdPartFName);
	if(o_cmdExist_PartFrom)
	{
		GetDlgItemText(IDC_FLASH_FROM, str);
		if(str.IsEmpty())
		{
			str.Format(addr64format, o_cmdPartFrom);
			SetDlgItemText(IDC_FLASH_FROM, str);
		}
	}


	i=AfxGetApp()->GetProfileInt(secOptions, keyCurDevTyp, m_DeviceType);
	if(o_cmdExist_WorkWith)
		if(o_cmdWorkWith)
			i=o_cmdWorkWith-1;
	m_DeviceType=-1;
	SetCurDeviceType(0);
	switch(i)
	{
	case 1:
		((CButton*)GetDlgItem(IDC_SELECT_FILE))->SetCheck(TRUE);
		break;
	default:
		((CButton*)GetDlgItem(IDC_SELECT_PHONE))->SetCheck(TRUE);
	}
	SetCurDeviceType(i);


	o_MainWndMaximize=app->GetProfileInt(secMainWnd, keyIsMaximized, FALSE);
	r.left=app->GetProfileInt(secMainWnd, keyXPos, 0);
	r.top=app->GetProfileInt(secMainWnd, keyYPos, 0);
	r.right=r.left+app->GetProfileInt(secMainWnd, keyXSize, m_MinSize.x);
	r.bottom=r.top+app->GetProfileInt(secMainWnd, keyYSize, m_MinSize.y);
	MoveWindow(r);

	PostMessage(VM_INITIAL_UPDATE);
	RegisterHotKey(this->GetSafeHwnd(), 23, MOD_CONTROL | MOD_ALT, VK_TAB);
	m_hTimerKeepAlive=SetTimer(41108, VD_KEEPALIVE_TICK, KeepAliveTimerProc);
	return FALSE;  // return TRUE  unless you set the focus to a control
}

// Keep alive timer handler
void CALLBACK EXPORT CKLayDlg::KeepAliveTimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
	if(nIDEvent==41108)
	{
		if(pDevice!=NULL)
			pDevice->KeepAlive();
	}
}

afx_msg LRESULT CKLayDlg::OnInitialUpdate(WPARAM wParam, LPARAM lParam)
{
	int i;
	for(i=0; i<VKL_PAGES_COUNT; i++)
		if(i!=VKL_PATCH_PAGE && pPage[i])
			if(!pPage[i]->SendMessage(VM_INITIAL_UPDATE, i, (LPARAM)this))
				break;
	if(i==VKL_PAGES_COUNT && pPage[VKL_PATCH_PAGE])
		pPage[VKL_PATCH_PAGE]->SendMessage(VM_INITIAL_UPDATE, VKL_PATCH_PAGE, (LPARAM)this);
	if(o_cmdQuit)
		PostQuitMessage(0);
	return 0;
}

void CKLayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if((nID & 0xFFF0) == IDM_HELPWND)
	{
		((CKLayApp*)AfxGetApp())->ShowHelp();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKLayDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKLayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKLayDlg::OnBnClickedAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
//m_DonateLinkCtrl.EnableWindow();
}

_TCHAR addr64format[]=_T("0x%8.8I64X");
_TCHAR addrformat[]=_T("0x%8.8X");
_TCHAR dataformat[]=_T("%2.2X");
_TCHAR intformat[]=_T("%i");
_TCHAR uintformat[]=_T("%u");
_TCHAR int64format[]=_T("%I64i");
_TCHAR uint64format[]=_T("%I64u");
_TCHAR hex4digformat[]=_T("0x%4.4X");
_TCHAR hex8diglongformat[]=_T("0x%8.8lX");
_TCHAR hex16digi64format[]=_T("0x%16.16I64X");

CString flashfilter;
CString openDialogTitle;
CString saveDialogTitle;

BOOL CKLayDlg::GetFlashFileName(LPCTSTR strFNameToOpen/*=NULL*/)
{
	CString strFlashFileName;
	if(strFNameToOpen==NULL)
	{
		GetDlgItem(IDC_FLASH_FNAME)->GetWindowText(strFlashFileName);
		CFileDialog fdlg(TRUE, NULL, strFlashFileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, flashfilter, AfxGetMainWnd());
		fdlg.m_ofn.lpstrTitle=openDialogTitle;
		fdlg.m_ofn.lpstrInitialDir=o_strFlashPartDir;
		if(fdlg.DoModal()!=IDOK)
			return FALSE;
		o_strFlashPartDir=GetFileDir(fdlg.GetPathName());
		strFlashFileName=fdlg.GetPathName();
	}
	else
		strFlashFileName=strFNameToOpen;

	GetDlgItem(IDC_FLASH_FNAME)->SetWindowText(strFlashFileName);
	int i=strFlashFileName.GetLength();
	((CEdit*)GetDlgItem(IDC_FLASH_FNAME))->SetSel(i, i, TRUE);

	CFile *f=NULL;
	INT64 PartialFlashSize=-1;
	INT64 PartialFlashAddress=0;
	TRY
	{
		f= new CFile(strFlashFileName, CFile::modeRead|CFile::typeBinary);
		PartialFlashSize=(INT64)f->GetLength();
	}
	CATCH_ALL(e)
	{
		e->ReportError();
		PartialFlashSize=-1;
	}
	END_CATCH_ALL;
	if(f!=NULL)
	{
		f->Close();
		delete f;
	}

	INT64 a=GetAddrFromFileName(strFlashFileName);
	if(a!=-1)
		PartialFlashAddress=a;

	CString str;
	if(PartialFlashAddress!=-1)
		str=VSignedHexToStr(PartialFlashAddress, addr64format);
	else
		str.Empty();
	GetDlgItem(IDC_FLASH_FROM)->SetWindowText(str);
	if(PartialFlashSize!=-1)
		str.Format(addr64format, PartialFlashSize);
	else
		str.Empty();
	GetDlgItem(IDC_FLASH_SIZE)->SetWindowText(str);
	return TRUE;
}

void CKLayDlg::OnBnClickedFlashBrowse()
{
	GetFlashFileName();
	if(pDevice)
		pDevice->OnChangeParameters();
	SendMessageToAllPages(VM_DEVICE_CHANGED);
}

void CKLayDlg::OnCancel()
{
	int i;
	for(i=0; i<VKL_PAGES_COUNT; i++)
	{
		if(pPage[i])
		{
			if(!pPage[i]->SendMessage(VM_IS_CAN_CLOSE, i, (LPARAM)this))
				return;
		}
	}
	for(i=0; i<VKL_PAGES_COUNT; i++)
	{
		if(pPage[i])
		{
			pPage[i]->DestroyWindow();
			delete pPage[i];
			pPage[i]=NULL;
		}
	}
	CDialog::OnCancel();
}

void CKLayDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur=m_TabCtrl.GetCurSel();
	if(cur!=-1)
	{
		if(pPage[cur])
		{
			pPage[cur]->EnableWindow(TRUE);
			pPage[cur]->ShowWindow(SW_SHOW);
			pPage[cur]->SetFocus();
		}
	}
	*pResult = 0;
}

void CKLayDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur=m_TabCtrl.GetCurSel();
	if(cur!=-1)
	{
		if(pPage[cur])
		{
			pPage[cur]->EnableWindow(FALSE);
			pPage[cur]->ShowWindow(SW_HIDE);
		}
	}
	*pResult = FALSE;
}

int CKLayDlg::SetCurPage(int PageIdxToSet)
{
	int cur=m_TabCtrl.GetCurSel();
	if(PageIdxToSet!=cur)
	{
		LRESULT res=FALSE;
		if(cur!=-1)
			OnTcnSelchangingTab(NULL, &res);
		m_TabCtrl.SetCurSel(PageIdxToSet);
		if(res==FALSE && PageIdxToSet!=-1)
			OnTcnSelchangeTab(NULL, &res);
	}
	return cur;
}

//BOOL CKLayDlg::UpdateFlashPartInf(BOOL bSaveAndValidate/*=FALSE*/)
/*{
	if(this==NULL)
		return FALSE;
	static inupdate=FALSE;
	if(inupdate)
		return TRUE;
	inupdate=TRUE;
	CString str;
	BOOL ret=TRUE;
	if(bSaveAndValidate)
	{
		GetDlgItem(IDC_FLASH_FROM)->GetWindowText(str);
		_TCHAR *end;
		Options.PartialFlashAddress=_tcstol(str, &end, 0);
		if(end-((LPCTSTR)str)!=str.GetLength())
		{
			GetDlgItem(IDC_FLASH_FROM)->SetFocus();
			((CEdit*)GetDlgItem(IDC_FLASH_FROM))->SetSel(0,-1,TRUE);
			AfxMessageBox(IDS_ERR_NOT_VALID_HEX_NUMBER);
			ret=false;
		}
	}
	else
	{
		str.Format(addrformat, Options.PartialFlashAddress);
		GetDlgItem(IDC_FLASH_FROM)->SetWindowText(str);
		str.Format(addrformat, Options.PartialFlashSize);
		GetDlgItem(IDC_FLASH_SIZE)->SetWindowText(str);
	}
	inupdate=FALSE;
	return ret;
}*/

BOOL CKLayDlg::UpdateMainTitle(BOOL bUpdate/*=FALSE*/, LPCTSTR lpstrFilePath/*=NULL*/, BOOL bModified/*=FLASE*/)
{
	if(this==NULL)
		return FALSE;
	if(bUpdate && lpstrFilePath && _tcslen(lpstrFilePath))
		SetWindowText(AfxGetAppName()+CString(_T(" - "))+GetFileNameFromPath(lpstrFilePath)
		+ (bModified ? _T("*") : NULL));
	else
		SetWindowText(AfxGetAppName());
	return TRUE;
}

void CKLayDlg::OnBnClickedSelectPhone()
{
	SetCurDeviceType(0);
}

void CKLayDlg::OnBnClickedSelectFile()
{
	SetCurDeviceType(1);
}

// Set current device type, that is selected in dialog
BOOL CKLayDlg::SetCurDeviceType(int devType)
{
	if(m_DeviceType==devType)
		return TRUE;
	m_DeviceType=-1;
	if(pDevice)
	{
		delete pDevice;
		pDevice=NULL;
	}
	switch(devType)
	{
	case 0:
		pDevice=new VDevicePhone();
		break;
	case 1:
		pDevice=new VDeviceFile();
		break;
	}
	if(pDevice)
	{
		if(!pDevice->Init())
		{
			pDevice=NULL;
			return FALSE;
		}
		m_DeviceType=devType;
		AfxGetApp()->WriteProfileInt(secOptions, keyCurDevTyp, m_DeviceType);
		pDevice->OnChangeParameters();
		SendMessageToAllPages(VM_DEVICE_CHANGED);
	}
	return TRUE;
}

void CKLayDlg::OnOK()
{
//	CDialog::OnOK();
}

void CKLayDlg::OnDestroy()
{
	if(m_hTimerKeepAlive!=0)
	{
		KillTimer(m_hTimerKeepAlive);
		m_hTimerKeepAlive=0;
	}
	UnregisterHotKey(this->GetSafeHwnd(), 23);
	SetCurDeviceType(-1);
	((CKLayApp*)AfxGetApp())->SaveOptions();
	VDevicePhone::EmptyComboBox((CComboBox*)GetDlgItem(IDC_PHONES_LIST));
	CDialog::OnDestroy();
}

void CKLayDlg::OnBnClickedPhoneReset()
{
	if(pDevice)
		pDevice->Reset();
}

TCHAR strAbout2[]=_T(".%i");
BOOL CAboutDlg::OnInitDialog()
{
	SetWindowText(MSTR("About V_KLay"));
	SetDlgItemText(IDOK, MSTR("OK"));
	SetDlgItemText(IDC_HELPWND, MSTR("Help"));
	SetDlgItemText(IDC_DESCRIPTION_TEXT, IdentStr(MSTR("Program for patching Flash and changing the keyboard layouts in mobile phones.\n\n   English localization:\n ValeraVi\t\t\t\tIgs\n http://www.vi-soft.com.ua\t\thttp://mobile-sl45.narod.ru\n ValeraVi@mail.ru\t\t\tsl45@hotbox.ru")));

	m_WebCtrl.SetDisplay  (_CXR(cryptstrSiteName));
	m_WebCtrl.SetTarget   (_CXR(cryptstrSiteURL));
	m_WebCtrl.SetToolTip  (_CXR(cryptstrSiteTip));
	m_EmailCtrl.SetDisplay(_CXR(cryptstrMailName));
	m_EmailCtrl.SetTarget (_CXR(cryptstrMailURL));
	m_EmailCtrl.SetToolTip(_CXR(cryptstrMailTip));

//e-mail: ValeraVi@mail.ru\nweb: www.vi-soft.com.ua\n
	CDialog::OnInitDialog();
//Getting the Version
	UINT64 ver = GetAppVersion();
	DWORD VersionHight = (DWORD)((ver >> 32) & 0xffffffff);
	DWORD VersionLow   = (DWORD)((ver >>  0) & 0xffffffff);
	CString str, strt;
	str.Format(_CXR(cryptstrAbout1), HIWORD(VersionHight), LOWORD(VersionHight));
	if(HIWORD(VersionLow) || LOWORD(VersionLow))
	{
		strt.Format(strAbout2, HIWORD(VersionLow));
		str+=strt;
		if(LOWORD(VersionLow))
		{
			strt.Format(strAbout2, LOWORD(VersionLow));
			str+=strt;
		}
	}
	str+=_CXR(cryptstrAbout3);
//Setting the text
	SetDlgItemText(IDC_ABOUT_TEXT, str);
	CWnd *wnd=GetWindow(GW_CHILD);
	CWnd *next;
	int id;
	while(wnd)
	{
		next=wnd->GetWindow(GW_HWNDNEXT);
		id=wnd->GetDlgCtrlID();
		if(id!=IDC_V_ICON && id!=IDOK && id!=IDC_DESCRIPTION_TEXT && id!=IDC_ABOUT_TEXT && id!=IDC_HELPWND
			&& id!=IDC_WEB_TEXT && id!=IDC_WEB && id!=IDC_EMAIL_TEXT && id!=IDC_EMAIL)
			wnd->DestroyWindow();
		wnd=next;
	}
	SendMessageToDescendants(WM_SHOWWINDOW, (WPARAM)TRUE, (LPARAM)0, TRUE);
	return TRUE;
}

void CAboutDlg::OnBnClickedHelpwnd()
{
	((CKLayApp*)AfxGetApp())->ShowHelp();
}

void CKLayDlg::OnCbnSelchangePhonesList()
{
	if(SelectPhone())
		pDevice->SaveOptionsPhoneName();
}

int cklaydlg_oldphonelistitem=-2;
BOOL CKLayDlg::SelectPhone(int phoneNum/*=-2*/)
{
	if(phoneNum!=-2)
		phoneNum=m_PhonesListCtrl.SetCurSel(phoneNum);
	else
		phoneNum=m_PhonesListCtrl.GetCurSel();
	if(phoneNum==0)
	{
		if(cklaydlg_oldphonelistitem!=-2)
		{
			if(cklaydlg_oldphonelistitem >= m_PhonesListCtrl.GetCount())
			{
				if(m_PhonesListCtrl.GetCount() > 1)
					cklaydlg_oldphonelistitem=m_PhonesListCtrl.GetCount()-1;
				else
					cklaydlg_oldphonelistitem=0;
			}
		}
		m_PhonesListCtrl.SetCurSel(cklaydlg_oldphonelistitem);
	}
	else
		cklaydlg_oldphonelistitem=phoneNum;

	if(pDevice)
		pDevice->OnChangeParameters();
	SendMessageToAllPages(VM_DEVICE_CHANGED);
	return TRUE;
}

void CKLayDlg::OnBnClickedOptions()
{
	COptionsDlg dlg;
	dlg.m_bDisableWarningsMsg=o_bDisableWarningsMsg;
	dlg.m_bDisableRTFSaveWarningsMsg=o_bDisableRTFSaveWarningsMsg;
	dlg.m_strLocale=o_strLocale;
	dlg.m_strLanguage=o_strLanguage;
	dlg.m_PatcherFontSize=o_PatcherFontSize;
	dlg.m_bIsPatcherSwapPasteKeys = o_bIsPatcherSwapPasteKeys;
	if(dlg.DoModal()==IDOK)
	{
		o_bDisableWarningsMsg=dlg.m_bDisableWarningsMsg;
		o_bDisableRTFSaveWarningsMsg=dlg.m_bDisableRTFSaveWarningsMsg;
		o_strLocale=dlg.m_strLocale;
		o_PatcherFontSize=dlg.m_PatcherFontSize;
		o_bIsPatcherSwapPasteKeys = dlg.m_bIsPatcherSwapPasteKeys;
		BOOL langchanged=o_strLanguage.CompareNoCase(dlg.m_strLanguage);
		o_strLanguage=dlg.m_strLanguage;
		((CKLayApp*)AfxGetApp())->SaveOptions();

		SetToolButton(IDC_TOOL_EXTLAY, keyToolExtLayPath, IDI_TOOL_EXTLAY, NULL);
		SetToolButton(IDC_TOOL_EXTPIC, keyToolExtPicPath, IDI_TOOL_EXTPIC, NULL);
		SetToolButton(IDC_TOOL_EXTFNT, keyToolExtFntPath, IDI_TOOL_EXTFNT, NULL);
		SetToolButton(IDC_TOOL_EXT1, keyToolExt1Path, IDI_TOOL_EXTFFED, NULL, NULL);
		SetToolButton(IDC_TOOL_EXT2, keyToolExt2Path, IDI_TOOL_EXT1, strToolsToolTipFormat, strDefTool1Name);
		SetToolButton(IDC_TOOL_EXT3, keyToolExt3Path, IDI_TOOL_EXT2, strToolsToolTipFormat, strDefTool2Name);

		SendMessageToAllPages(VM_APP_OPTIONS_CHANGED);

		if(langchanged)
			AfxMessageBox(MSTR("The language of the user interface is changed!\nYou must restart the program for the changes take effect."));
	}
}

BOOL CKLayDlg::SendMessageToAllPages(UINT msg)
{
	for(int i=0; i<VKL_PAGES_COUNT; i++)
		if(pPage[i])
			pPage[i]->SendMessage(msg, i, (LPARAM)this);
	return TRUE;
}

void CKLayDlg::OnBnClickedPhoneQueryInfo()
{
	if(pDevice)
	{
		CProgressDlg Progress(0, this);
		if(pDevice->Open())
		{
			SetDlgItemText(IDC_PHONE_FLASH_INFO, pDevice->ReadDeviceInfoString());
			pDevice->Close();
		}
		Progress.DestroyWindow();
	}
}

void CKLayDlg::OnSize(UINT nType, int cx, int cy)
{
	m_DynSize.OnSize(cx, cy);
	if(m_TabCtrl.GetSafeHwnd()!=NULL)
	{
		CRect r;
		m_TabCtrl.GetClientRect(r);
		m_TabCtrl.AdjustRect(FALSE, r);
		r.left-=2;
		for(int i=0; i<VKL_PAGES_COUNT; i++)
			pPage[i]->MoveWindow(r);
	}
	CDialog::OnSize(nType, cx, cy);
}

void CKLayDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize=m_MinSize;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CKLayDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	if(!CDialog::Create(lpszTemplateName, pParentWnd))
		return FALSE;
	return TRUE;
}

void CKLayDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if(bShow && nStatus==0 && o_MainWndMaximize!=-1)
	{
		if(o_MainWndMaximize)
		{
			o_MainWndMaximize=-1;
			ShowWindow(SW_MAXIMIZE);
		}
		o_MainWndMaximize=-1;
	}
	CDialog::OnShowWindow(bShow, nStatus);
}

void CKLayDlg::OnEnKillfocusFlashFrom()
{
	if(pDevice)
		pDevice->OnChangeParameters();
	SendMessageToAllPages(VM_DEVICE_CHANGED);
}

void CKLayDlg::OnEnKillfocusFlashSize()
{
	if(pDevice)
		pDevice->OnChangeParameters();
	SendMessageToAllPages(VM_DEVICE_CHANGED);
}

BOOL CKLayDlg::PreTranslateMessage(MSG* pMsg)
{
	if(m_pToolTipCtrl!=NULL)
		m_pToolTipCtrl->RelayEvent(pMsg);
	if(m_hAccelerator!=NULL)
		TranslateAccelerator(GetSafeHwnd(), m_hAccelerator, pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CKLayDlg::OnBnClickedToolsOptions()
{
	CString str;
	CRect r;
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_TOOLS_OPTIONS_MENU));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	pPopup->ModifyMenu(ID_TOOLS_READFF, MF_BYCOMMAND|MF_STRING, ID_TOOLS_READFF,
		MSTR("Read fullflash and open it in the external tool"));
	pPopup->ModifyMenu(ID_TOOLS_USEFF, MF_BYCOMMAND|MF_STRING, ID_TOOLS_USEFF,
		MSTR("Open previously readed fullflash in external tool"));
	pPopup->ModifyMenu(ID_TOOLS_ONLY_RUN, MF_BYCOMMAND|MF_STRING, ID_TOOLS_ONLY_RUN,
		MSTR("Just run external tool without parameters"));
	pPopup->ModifyMenu(ID_TOOLS_SHOW_OPTIONS, MF_BYCOMMAND|MF_STRING, ID_TOOLS_SHOW_OPTIONS,
		MSTR("Change path to the external tools..."));
	CWinApp *app=AfxGetApp();
	if(app==NULL)
		return;
	pPopup->CheckMenuRadioItem(0, 2, app->GetProfileInt(secOptions, keyToolsRunMethod, 0), MF_BYPOSITION);
	GetDlgItem(IDC_TOOLS_OPTIONS)->GetWindowRect(r);
	int ret=pPopup->TrackPopupMenuEx(TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON, r.right, r.bottom, this, NULL);
	switch(ret)
	{
	case ID_TOOLS_READFF:
		app->WriteProfileInt(secOptions, keyToolsRunMethod, 0);
		break;
	case ID_TOOLS_USEFF:
		app->WriteProfileInt(secOptions, keyToolsRunMethod, 1);
		break;
	case ID_TOOLS_ONLY_RUN:
		app->WriteProfileInt(secOptions, keyToolsRunMethod, 2);
		break;
	case ID_TOOLS_SHOW_OPTIONS:
		OnBnClickedOptions();
		break;
	}
}

void CKLayDlg::OnBnClickedToolExtlay()
{
	RunExternalTool(keyToolExtLayPath, keyToolExtLayArg);
}

void CKLayDlg::OnBnClickedToolExtpic()
{
	RunExternalTool(keyToolExtPicPath, keyToolExtPicArg);
}

void CKLayDlg::OnBnClickedToolExtfnt()
{
	RunExternalTool(keyToolExtFntPath, keyToolExtFntArg);
}

void CKLayDlg::OnBnClickedToolExt1()
{
	RunExternalTool(keyToolExt1Path, keyToolExt1Arg);
}

void CKLayDlg::OnBnClickedToolExt2()
{
	RunExternalTool(keyToolExt2Path, keyToolExt2Arg);
}

void CKLayDlg::OnBnClickedToolExt3()
{
	RunExternalTool(keyToolExt3Path, keyToolExt3Arg);
}


// Run the external tool
BOOL CKLayDlg::RunExternalTool(LPCTSTR strToolPathKey, LPCTSTR strToolArgKey)
{
	CKLayApp *app=(CKLayApp*)AfxGetApp();
	if(app==NULL)
		return FALSE;

	app->SaveOptions();
	if(pDevice != NULL)
		pDevice->SaveOptions();

	BOOL RunMethod=app->GetProfileInt(secOptions, keyToolsRunMethod, 0);
	CString strEXEArg = app->GetProfileString(secOptions, strToolArgKey);
	strEXEArg.Trim();
	if(!strEXEArg.IsEmpty())
	{
		strEXEArg.Replace(ExtToolArgs[0], _T(""));
		strEXEArg.Trim();
		if(strEXEArg.IsEmpty())
			RunMethod = 2;
	}
	DWORD size=0;
	BYTE *pBuffer=NULL;
	CFile f;
	CString strFFName, strPatchName, strFFDir, str;
	PROCESS_INFORMATION ExeInf;
	ExeInf.hProcess=NULL;
	ExeInf.hThread=NULL;
	CFileException e;
	if(RunMethod!=2)
	{
		if(pDevice==NULL)
		{
			AfxMessageBox(MSTR("For Reading select the device first!"));
			return FALSE;
		}
		BOOL bNothingToRead= (pDevice->m_MemArea.GetSize()==0);
		if(!bNothingToRead)
			bNothingToRead= (pDevice->m_MemArea[VDA_FULLFLASH].Size==0);
		if(bNothingToRead)
		{
			AfxMessageBox(MSTR("There is nothing to read."));
			return FALSE;
		}
		if(pDevice->m_MemArea[VDA_FULLFLASH].Address-pDevice->m_MemAreaStart != 0)
		{
			AfxMessageBox(MSTR("The external tool accept only a fullflash."));
			return FALSE;
		}
		size=(DWORD)pDevice->m_MemArea[VDA_FULLFLASH].Size;
		//strFFDir.ReleaseBuffer(GetTempPath(1024, strFFDir.GetBuffer(1024)));
		strFFDir=app->GetProfileString(secOptions, keyTmpPath, NULL);
		if(strFFDir.IsEmpty())
			strFFDir=app->m_AppPath+defaultTmpPathRelativeApp;
		IncludeTrailingSlash(strFFDir);
		if(!CreateDirectory(strFFDir, NULL))
		{
			if(GetLastError()!=ERROR_ALREADY_EXISTS)
			{
				strFFDir.ReleaseBuffer(GetSystemDirectory(strFFDir.GetBuffer(1024), 1204));
				strFFDir=GetFileDriveRootPath(strFFDir);
			}
		}
		str=pDevice->GetName();
		//int i=str.Find(_T(' '));
		//if(i!=-1)
		//	str=str.Left(i);
		CheckFileNameString(str, _T(" ."));
		strPatchName=strFFDir+_T("V_KLay_Temp_FullFlash_from_")+str;
		strFFName=strPatchName+_T(".bin");
		strPatchName+=_T(".vkp");
	}
	CString strEXEName=app->GetProfileString(secOptions, strToolPathKey);
	if(strEXEName.IsEmpty())
	{
		AfxMessageBox(MSTR("The path to the external tool is not specified.\nSet it in the Options."));
		return FALSE;
	}

	TRY
	{
		CProgressDlg Progress(0, this);
		Progress.SetUpper(size);

		if(RunMethod!=2)
		{
			//Open temporary file for saving fullflash in it
			Progress.SetStatus(MSTR("Opening a temporary file for saving the fullflash in it..."));
			if(RunMethod==1)
			{
				if(!f.Open(strFFName, CFile::modeRead))
				{
					str.Format(MSTR("Can not open fullflash file:\n%s\nSelect the reading fullflash before opening it in the external tool."),
						(LPCTSTR)strFFName);
					AfxMessageBox(str);
					AfxThrowUserException();
				}
			}
			else
			{
				if(!o_bDisableWarningsMsg)
				{
					if(f.Open(strFFName, CFile::modeWrite | CFile::typeBinary))
					{
						f.Close();
						if(AfxMessageBox(
							MSTR("Temporary file with fullflash for this phone already exist.\nIf you do not modify flash of this phone after you download this temporary fullflash you may use Open external tool with previously readed fullflash.\n\nDo you want to read new fullflash and overwrtite current file with fullflash?"),
							MB_ICONQUESTION | MB_YESNO)!=IDYES)
							AfxThrowUserException();
					}
				}

				if(!f.Open(strFFName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, &e))
					THROW(&e);

				//read fullflash
				pBuffer=new BYTE[size];
				memset(pBuffer, 0, size);
				f.Write(pBuffer, size);
				f.SeekToBegin();
				Progress.ShowPerformanceInfo();
				if(!pDevice->Open())
					AfxThrowUserException();
				if(!pDevice->Read(0, size, pBuffer))
					AfxThrowUserException();
				if(!pDevice->Close())
					AfxThrowUserException();
				if(!pDevice->Reset())
					AfxThrowUserException();
				Progress.ShowPerformanceInfo(FALSE);

				//write fullflash to the file
				Progress.SetStatus(MSTR("Saving fullflash to the temporary file..."));
				f.Write(pBuffer, size);
				delete pBuffer;
				pBuffer=NULL;
			}
			f.Close();
			str=CString(_T(" \""))+strFFName+_T('\"');
		}
		else
		{
			str.Empty();
		}

		//run External tool
		Progress.SetStatus(MSTR("Running external tool..."));
	    STARTUPINFO si;
	    ZeroMemory(&si, sizeof(si));
		si.cb=sizeof(si);
		if(!CreateProcess(NULL, (LPTSTR)(LPCTSTR)(CString(_T('\"'))+strEXEName+_T('\"')+str),
			NULL, NULL, FALSE, 0, NULL, GetFileDir(strEXEName), &si, &ExeInf))
//		if((__int64)ShellExecute(GetSafeHwnd(), _T("open"), strEXEName,
//			strFFName, strFFDir, SW_SHOWNORMAL) <= 32)
		{
			str.Format(MSTR("Can not run external tool:\n%s\nSpecify new path to it in the Options."),
				(LPCTSTR)strEXEName);
			AfxMessageBox(str);
			AfxThrowUserException();
		}

		if(RunMethod!=2)
		{
			BOOL bOldPatchExist;
			CFileStatus OldPatchStatus, NewPatchStatus;
			bOldPatchExist=CFile::GetStatus(strPatchName, OldPatchStatus);

			Progress.SetStatus(MSTR("Waiting for terminating the external tool..."));

			DWORD ret;
			BOOL iscancelled;
			while(!(iscancelled=Progress.CheckCancelButton()) &&
				(ret=WaitForSingleObject(ExeInf.hProcess, 100))==WAIT_TIMEOUT)
				Sleep(1);
			if(iscancelled)
				AfxThrowUserException();
			if(ret!=WAIT_OBJECT_0)
			{
				AfxMessageBox(_T("Error while waiting for terminating."));
				AfxThrowUserException();
			}

			Progress.DestroyWindow();

			SetForeground(AfxGetMainWnd());

			//chechikng if new patch from external tool is exist.
			if(CFile::GetStatus(strPatchName, NewPatchStatus))
			{
				if(bOldPatchExist)
					if(NewPatchStatus.m_ctime > OldPatchStatus.m_ctime
						|| NewPatchStatus.m_mtime > OldPatchStatus.m_mtime
						|| NewPatchStatus.m_size != OldPatchStatus.m_size)
						bOldPatchExist=FALSE;
				if(!bOldPatchExist)
				{
					if(!o_bDisableWarningsMsg)
						if(AfxMessageBox(
							MSTR("External tool has make a patch on phone fullflash.\nYou may apply it to the phone and also it will applied on the temporary fullflash file and you can Open external tool with previously readed fullflash.\n\nDo you want to apply a patch from external tool?"),
							MB_ICONQUESTION | MB_YESNO)!=IDYES)
							AfxThrowUserException();
					//apply patch to the phone
					SetCurPage(VKL_PATCH_PAGE);
					CPatchPage *pPatchPage=(CPatchPage *)pPage[VKL_PATCH_PAGE];
					if(pPatchPage->SaveModifiedDocument())
					{
						if(pPatchPage->OpenDocument(strPatchName))
						{
							pPatchPage->m_PatchDataCtrl.SetModify();
							pPatchPage->m_strDocFileName=pPatchPage->GetDefaultFileName();
							pPatchPage->m_IsDocNew=TRUE;
							UpdateMainTitle(TRUE, pPatchPage->m_strDocFileName, TRUE);
							if(pPatchPage->UpdateData(TRUE))
							{
								if(pPatchPage->DoPatchApply(pPatchPage->m_PatchData, pDevice))
								{
									//apply patch to the temporary fullflash file
									VDeviceFile *pffd=new VDeviceFile();
									if(pffd!=NULL)
									{
										pffd->m_bDetachFromMainWnd=TRUE;
										if(pffd->OpenDevice(strFFName, 0, size))
										{
											pffd->Close();
											pPatchPage->DoPatchApply(pPatchPage->m_PatchData, pffd, TRUE);
										}
										delete pffd;
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
	        Progress.DestroyWindow();
		}
	}
	CATCH(CUserException, e)
	{
	}
	AND_CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
	if(pBuffer!=NULL)
		delete pBuffer;
	pBuffer=NULL;
	if(ExeInf.hThread!=NULL)
		CloseHandle(ExeInf.hThread);
	ExeInf.hThread=NULL;
	if(ExeInf.hProcess!=NULL)
		CloseHandle(ExeInf.hProcess);
	ExeInf.hProcess=NULL;
	return TRUE;
}

afx_msg void CKLayDlg::OnSwitchPanel(void)
{
	int cur=m_TabCtrl.GetCurSel();
	if(cur!=-1)
	{
		CWnd *wnd=GetFocus();
		if(wnd!=NULL)
		{
			if(wnd->GetParent()->GetSafeHwnd()==GetSafeHwnd())
				wnd=pPage[cur]->GetNextDlgTabItem(NULL);
			else
				wnd=GetNextDlgTabItem(NULL);
			if(wnd!=NULL)
				wnd->SetFocus();
		}
	}
}

afx_msg LRESULT CKLayDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	if(wParam==23)
	{
		int cur=m_TabCtrl.GetCurSel();
		CWnd *wnd=GetFocus();
		if(cur!=-1 && wnd!=NULL)
		{
			for(int n=0; wnd!=NULL; n++)
				if((wnd=wnd->GetParent())->GetSafeHwnd()==m_TabCtrl.GetSafeHwnd())
					break;
			if(wnd==NULL)
			{
				if(pPage[cur]!=NULL)
					wnd=pPage[cur]->GetNextDlgTabItem(NULL);
			}
			else
				wnd=GetNextDlgTabItem(NULL);
			if(wnd!=NULL)
				wnd->SetFocus();
		}
	}
	return 0;
}

// for updating dialog box controls
// this 2 functions are taken from Visual C++ for dummies Quick
// Reference book --- Dialog Command Enablers --- page 185 and 186
LRESULT CKLayDlg::OnKickIdle(WPARAM w, LPARAM l)
{
	if(AfxGetApp()!=NULL)
		AfxGetApp()->OnIdle(-1);
	OnIdleUpdateCmdUI(w,l);
	return 0L;
}
LRESULT CKLayDlg::OnIdleUpdateCmdUI(WPARAM w, LPARAM l)
{
	UpdateDialogControls((CCmdTarget*)this,true);
	return 0L;
}

void CKLayDlg::OnBnClickedDonateLink()
{
}

void CKLayDlg::OnActivatedDonateLink(NMHDR *pNotifyStruct, LRESULT *result)
{
	CWnd *w=GetDlgItem(IDC_DONATE_DETAILS);
	if(w!=NULL)
	{
		w->ShowWindow(SW_SHOW);
		w->EnableWindow(TRUE);
		w->RedrawWindow();
	}
	*result = 0;
}
