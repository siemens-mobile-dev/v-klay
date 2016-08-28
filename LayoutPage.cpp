// LayoutPage.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "LayoutPage.h"
#include "KLayDlg.h"


CString vklaydocfilter;

_TCHAR secIniInfo[]=_T("Info");
_TCHAR keyIniFWVer[]=_T("FirmwareVersion");
_TCHAR keyIniName[]=_T("Name");
_TCHAR keyIniAuthor[]=_T("Author");
_TCHAR keyIniComments[]=_T("Comments");
_TCHAR keyIniPhone[]=_T("Phone");


CINIFileInfo& CINIFileInfo::operator=(const CINIFileInfo& src)
{
	m_Name=src.m_Name;
	m_Author=src.m_Author;
	m_Comments=src.m_Comments;
	m_Phone=src.m_Phone;
	m_FWVersion=src.m_FWVersion;
	m_FilePath=src.m_FilePath;
	return *this;
}

CINIPhoneFiles& CINIPhoneFiles::operator=(const CINIPhoneFiles& src)
{
	m_Name=src.m_Name;
	m_Files.RemoveAll();
	m_Files.Copy(src.m_Files);
	return *this;
}




// CLayoutPage dialog

IMPLEMENT_DYNAMIC(CLayoutPage, CDialog)
CLayoutPage::CLayoutPage(CWnd* pParent /*=NULL*/)
	: CDialog(CLayoutPage::IDD, pParent)
{
	m_DynSize.AddControl(IDC_KEYS_SAVE, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_KEYS_LOAD, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG1_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG2_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG3_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG4_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL1_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL2_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL3_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL4_STATIC, EL(100), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_INI_TITLE_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_INI_COMMENTS_STATIC, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_INI_COMMENTS, EL(0), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_INI_NAME, EL(0), EL(0), EL(100), EL(0));

	m_DynSize.AddControl(IDC_CODES3_STATIC, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY3, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY3, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY6, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY6, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY9, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY9, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEYG, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEYG, EL(50.+50.*2./3.), EL(0), EL(100), EL(0));

	m_DynSize.AddControl(IDC_SYMBOLS3_STATIC, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY3, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY3, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY6, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY6, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY9, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY9, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEYG, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEYG, EL(50.+50.*1./3.), EL(0), EL(50.+50.*2./3.), EL(0));

	m_DynSize.AddControl(IDC_TEXT_KEY3, EL(50.+50.*1./3.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEY6, EL(50.+50.*1./3.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEY9, EL(50.+50.*1./3.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEYG, EL(50.+50.*1./3.), EL(0), EL(50.+50.*1./3.), EL(0));

	m_DynSize.AddControl(IDC_CODES2_STATIC, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY2, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY2, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY5, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY5, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY8, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY8, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY0, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY0, EL(50.), EL(0), EL(50.+50.*1./3.), EL(0));

	m_DynSize.AddControl(IDC_SYMBOLS2_STATIC, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY2, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY2, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY5, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY5, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY8, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY8, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY0, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY0, EL(50.-50.*1./3.), EL(0), EL(50.-50.*0./3.), EL(0));

	m_DynSize.AddControl(IDC_TEXT_KEY2, EL(50.-50.*1./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEY5, EL(50.-50.*1./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEY8, EL(50.-50.*1./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_TEXT_KEY0, EL(50.-50.*1./3.), EL(0), EL(50.-50.*1./3.), EL(0));

	m_DynSize.AddControl(IDC_CODES1_STATIC, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY1, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY1, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY4, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY4, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEY7, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEY7, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_CODE_KEYS, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_CODE_KEYS, EL(50.-50.*2./3.), EL(0), EL(50.-50.*1./3.), EL(0));

	m_DynSize.AddControl(IDC_SYMBOLS1_STATIC, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY1, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY1, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY4, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY4, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEY7, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEY7, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_BIG_KEYS, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));
	m_DynSize.AddControl(IDC_SMALL_KEYS, EL(0), EL(0), EL(50.-50.*2./3.), EL(0));


	m_IsModified=FALSE;
	m_pToolTipCtrl=NULL;


	Create(CLayoutPage::IDD, pParent);		//вставлять всякие инициализации преременных класса только ДО вызова
											// этой функции, иначе они проинициализируются 2 раза !!!!!!
											// (?????????? и хер его знает чего оно так ????????????)
	CRect r;
	((CTabCtrl*)pParent)->InsertItem(((CTabCtrl*)pParent)->GetItemCount(), MSTR("Keyboard Layout"));
	((CTabCtrl*)pParent)->GetClientRect(&r);
	((CTabCtrl*)pParent)->AdjustRect(FALSE, &r);
	r.left-=2;
	MoveWindow(&r);

}

CLayoutPage::~CLayoutPage()
{
	if(m_pToolTipCtrl!=NULL)
	{
		delete m_pToolTipCtrl;
		m_pToolTipCtrl=NULL;
	}
}

void CLayoutPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INI_NAME, m_ININameCtrl);
	DDX_Control(pDX, IDC_INI_PHONE, m_INIPhoneCtrl);
}


BEGIN_MESSAGE_MAP(CLayoutPage, CDialog)
	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY1,        IDC_BIG_KEY1, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY1,   IDC_BIG_CODE_KEY1, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY1,      IDC_SMALL_KEY1, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY1, IDC_SMALL_CODE_KEY1, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY2,        IDC_BIG_KEY2, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY2,   IDC_BIG_CODE_KEY2, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY2,      IDC_SMALL_KEY2, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY2, IDC_SMALL_CODE_KEY2, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY3,        IDC_BIG_KEY3, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY3,   IDC_BIG_CODE_KEY3, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY3,      IDC_SMALL_KEY3, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY3, IDC_SMALL_CODE_KEY3, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY4,        IDC_BIG_KEY4, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY4,   IDC_BIG_CODE_KEY4, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY4,      IDC_SMALL_KEY4, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY4, IDC_SMALL_CODE_KEY4, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY5,        IDC_BIG_KEY5, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY5,   IDC_BIG_CODE_KEY5, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY5,      IDC_SMALL_KEY5, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY5, IDC_SMALL_CODE_KEY5, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY6,        IDC_BIG_KEY6, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY6,   IDC_BIG_CODE_KEY6, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY6,      IDC_SMALL_KEY6, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY6, IDC_SMALL_CODE_KEY6, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY7,        IDC_BIG_KEY7, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY7,   IDC_BIG_CODE_KEY7, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY7,      IDC_SMALL_KEY7, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY7, IDC_SMALL_CODE_KEY7, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY8,        IDC_BIG_KEY8, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY8,   IDC_BIG_CODE_KEY8, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY8,      IDC_SMALL_KEY8, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY8, IDC_SMALL_CODE_KEY8, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY9,        IDC_BIG_KEY9, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY9,   IDC_BIG_CODE_KEY9, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY9,      IDC_SMALL_KEY9, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY9, IDC_SMALL_CODE_KEY9, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEY0,        IDC_BIG_KEY0, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEY0,   IDC_BIG_CODE_KEY0, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEY0,      IDC_SMALL_KEY0, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEY0, IDC_SMALL_CODE_KEY0, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEYS,        IDC_BIG_KEYS, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEYS,   IDC_BIG_CODE_KEYS, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEYS,      IDC_SMALL_KEYS, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEYS, IDC_SMALL_CODE_KEYS, OnEnChangeKeySymCode)

	ON_CONTROL_RANGE(EN_CHANGE,        IDC_BIG_KEYG,        IDC_BIG_KEYG, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE,   IDC_BIG_CODE_KEYG,   IDC_BIG_CODE_KEYG, OnEnChangeKeySymCode)
	ON_CONTROL_RANGE(EN_CHANGE,      IDC_SMALL_KEYG,      IDC_SMALL_KEYG, OnEnChangeKeySym)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_SMALL_CODE_KEYG, IDC_SMALL_CODE_KEYG, OnEnChangeKeySymCode)

	ON_BN_CLICKED(IDC_KEYS_LOAD, OnBnClickedKeysLoad)
	ON_BN_CLICKED(IDC_KEYS_SAVE, OnBnClickedKeysSave)
	ON_BN_CLICKED(ID_UPDATE, OnBnClickedUpdate)
	ON_BN_CLICKED(ID_FLASH_LOAD, OnBnClickedFlashLoad)
	ON_WM_CLOSE()

	ON_MESSAGE(VM_IS_CAN_CLOSE, OnIsCanClose)
	ON_MESSAGE(VM_OPTIONS_CHANGED, OnOptionsChanged)
	ON_MESSAGE(VM_INITIAL_UPDATE, OnInitialUpdate)
	ON_MESSAGE(VM_DEVICE_CHANGED, OnDeviceChanged)

	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_INI_BROWSE, OnBnClickedIniBrowse)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_INI_PHONE, OnCbnSelchangeIniPhone)
	ON_CBN_SELCHANGE(IDC_INI_NAME, OnCbnSelchangeIniName)

END_MESSAGE_MAP()


// CLayoutPage message handlers
BOOL CLayoutPage::OpenDocument(LPCTSTR strDocFileName)
{
	m_strDocFileName=strDocFileName;
	m_IsModified=FALSE;
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(TRUE, m_strDocFileName, m_IsModified);
	for(int i=0; i<SYM_LISTS_COUNT; i++)
	{
		if(!m_SymList[i].LoadDocument(m_strDocFileName))
			return FALSE;
	}
	return TRUE;
}


void CLayoutPage::OnBnClickedKeysLoad()
{
	if(!SaveModifiedDocument())
		return;
	CFileDialog fdlg(TRUE, NULL, m_strDocFileName, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, vklaydocfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=openDialogTitle;
	if(m_strDocFileName.IsEmpty())
		fdlg.m_ofn.lpstrInitialDir=o_strLayoutDir;
	if(fdlg.DoModal()!=IDOK)
		return;
	o_strLayoutDir=GetFileDir(fdlg.GetPathName());
	OpenDocument(fdlg.GetPathName());
}

BOOL CLayoutPage::UpdateMainTitle(BOOL pUpdate /*=TRUE*/)
{
	CWnd* pm=AfxGetMainWnd();
	if(pm==NULL)
		return FALSE;
	if(pUpdate)
	{
		int i=m_strDocFileName.ReverseFind(_TCHAR('\\'));
		CString str;
		if(i!=-1)
			str=m_strDocFileName.Right(m_strDocFileName.GetLength()-i-1);
		else
			str=m_strDocFileName;
		pm->SetWindowText(AfxGetAppName()+CString(" - ")+str);
	}
	else
		pm->SetWindowText(AfxGetAppName());
	return TRUE;
}

void CLayoutPage::OnBnClickedKeysSave()
{
	SaveDocument();
}

BOOL CLayoutPage::SaveModifiedDocument(void)
{
	if(!m_IsModified)
		return TRUE;
	UINT ret=AfxMessageBox(MSTR("Save changes in current keyboard layout?"), MB_YESNOCANCEL|MB_DEFBUTTON1);
	if(ret==IDCANCEL)
		return FALSE;
	if(ret==IDNO)
		return TRUE;
	return SaveDocument();
}

BOOL CLayoutPage::SaveDocument(void)
{
	CFileDialog fdlg(FALSE, _T("vkl"), m_strDocFileName, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		vklaydocfilter, AfxGetMainWnd());
	fdlg.m_ofn.lpstrTitle=saveDialogTitle;
	if(m_strDocFileName.IsEmpty())
		fdlg.m_ofn.lpstrInitialDir=o_strLayoutDir;
	if(fdlg.DoModal()!=IDOK)
		return FALSE;
	o_strLayoutDir=GetFileDir(fdlg.GetPathName());

	m_strDocFileName=fdlg.GetPathName();
	CString strtemp=m_strDocFileName+_T(".tmp");
	TRY{CFile::Remove(strtemp);}CATCH_ALL(e){}END_CATCH_ALL;
	for(int i=0; i<SYM_LISTS_COUNT; i++)
	{
		if(!m_SymList[i].SaveDocument(strtemp))
		{
			CFile::Remove(strtemp);
			return FALSE;
		}
	}
	TRY{CFile::Remove(m_strDocFileName);}CATCH_ALL(e){}END_CATCH_ALL;
	TRY
	{
		CFile::Rename(strtemp, m_strDocFileName);
	}
	CATCH_ALL(e)
	{
		e->ReportError();
	}
	END_CATCH_ALL;
	m_IsModified=FALSE;
	return TRUE;
}


void CLayoutPage::OnBnClickedFlashLoad()
{
	if(!Options.IsValid)
	{
		AfxMessageBox(MSTR("For reading layout you must load the INI file first!"));
		return;
	}
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For reading layout you must select the device first!"));
		return;
	}
	if(!SaveModifiedDocument())
		return;
	CProgressDlg Progress(0, this);
	if(!pDevice->Open())
		return;
	for(int i=0; i<SYM_LISTS_COUNT; i++)
	{
		if(m_SymList[i].LoadFlash())
			m_IsModified=TRUE;
		else
			break;
	}
	pDevice->Close();
}

char strCopyright[]="made by ValeraVi";
void CLayoutPage::OnBnClickedUpdate()
{
	if(!Options.IsValid)
	{
		AfxMessageBox(MSTR("For writing layout you must load the INI file first!"));
		return;
	}
	if(pDevice==NULL)
	{
		AfxMessageBox(MSTR("For writing layout you must select the device first!"));
		return;
	}
	if(!o_bDisableWarningsMsg)
		if(AfxMessageBox(MSTR("Update the current Flash file with this keyboard layout?"), MB_YESNO)==IDNO)
			return;
	CProgressDlg Progress(0, this);
	if(!pDevice->Open())
		return;
	INT64 BytesUsedInFlash=0;
	BOOL bOk=TRUE;
	for(int i=0; i<SYM_LISTS_COUNT; i++)
	{
		if(!m_SymList[i].SaveFlash(BytesUsedInFlash))
			break;
	}
	if(i==SYM_LISTS_COUNT)
	{
		TRY
		{
			VKey copyright;
			memcpy(copyright.Symbols, strCopyright, sizeof(strCopyright));
			copyright.SymbolsCount=sizeof(strCopyright)/2;
			INT64 storeaddr=copyright.GetStoreAddress(BytesUsedInFlash);
			if(storeaddr!=-1)
				copyright.WriteSymInStoreAddress(storeaddr);
		}
		CATCH_ALL(e)
		{
		}
		END_CATCH_ALL;
	}
	else
	{
		pDevice->Abort();
		bOk=FALSE;
	}
	if(!pDevice->Close())
		bOk=FALSE;
	Progress.DestroyWindow();
	if(bOk && !o_bDisableWarningsMsg)
		AfxMessageBox(MSTR("Keyboard layout has been written successfully!"), MB_OK|MB_ICONINFORMATION);
}

void CLayoutPage::OnEnChangeKeySym(UINT nID)
{
	CWnd *wnd=GetDlgItem(nID);
	if(wnd)
	{
		VKey *key=NULL;
		key=(VKey*)(LONG_PTR)GetWindowLongPtr(wnd->GetSafeHwnd(), GWL_USERDATA);
		if(key)
		{
			m_IsModified=TRUE;
			key->UpdateSymEdit(TRUE);
		}
	}
}

void CLayoutPage::OnEnChangeKeySymCode(UINT nID)
{
	CWnd *wnd=GetDlgItem(nID);
	if(wnd)
	{
		VKey *key=NULL;
		key=(VKey*)(LONG_PTR)GetWindowLongPtr(wnd->GetSafeHwnd(), GWL_USERDATA);
		if(key)
		{
			m_IsModified=TRUE;
			key->UpdateSymCodeEdit(TRUE);
		}
	}
}

_TCHAR lBig[]=_T("Big");
_TCHAR lSml[]=_T("Small");
_TCHAR k0[]=_T("0");
_TCHAR k1[]=_T("1");
_TCHAR k2[]=_T("2");
_TCHAR k3[]=_T("3");
_TCHAR k4[]=_T("4");
_TCHAR k5[]=_T("5");
_TCHAR k6[]=_T("6");
_TCHAR k7[]=_T("7");
_TCHAR k8[]=_T("8");
_TCHAR k9[]=_T("9");
_TCHAR kS[]=_T("S");
_TCHAR kG[]=_T("G");

BOOL CLayoutPage::OnInitDialog()
{
	CString str;
	SetDlgItemText(IDC_INI_TITLE_STATIC, IdentStr(MSTR("INI File:")));
	SetDlgItemText(IDC_INI_AUTHOR_STATIC, MSTR("Author:"));
	SetDlgItemText(IDC_INI_COMMENTS_STATIC, MSTR("Comments:"));
	SetDlgItemText(ID_FLASH_LOAD, MSTR("Load layout from Flash"));
	SetDlgItemText(ID_UPDATE, MSTR("Write Layout to Flash"));
	str=MSTR("Symbols");
	SetDlgItemText(IDC_SYMBOLS1_STATIC, str);
	SetDlgItemText(IDC_SYMBOLS2_STATIC, str);
	SetDlgItemText(IDC_SYMBOLS3_STATIC, str);
	str=MSTR("Codes");
	SetDlgItemText(IDC_CODES1_STATIC, str);
	SetDlgItemText(IDC_CODES2_STATIC, str);
	SetDlgItemText(IDC_CODES3_STATIC, str);
	str=MSTR("Big");
	SetDlgItemText(IDC_BIG1_STATIC, str);
	SetDlgItemText(IDC_BIG2_STATIC, str);
	SetDlgItemText(IDC_BIG3_STATIC, str);
	SetDlgItemText(IDC_BIG4_STATIC, str);
	str=MSTR("Small");
	SetDlgItemText(IDC_SMALL1_STATIC, str);
	SetDlgItemText(IDC_SMALL2_STATIC, str);
	SetDlgItemText(IDC_SMALL3_STATIC, str);
	SetDlgItemText(IDC_SMALL4_STATIC, str);


	CDialog::OnInitDialog();
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

	CWinApp* app=AfxGetApp();
	CButton* btn;

	btn=(CButton*)GetDlgItem(IDC_KEYS_LOAD);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_OPEN),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Open Layout File..."));

	btn=(CButton*)GetDlgItem(IDC_KEYS_SAVE);
	btn->SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_SAVE_AS),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	m_pToolTipCtrl->AddTool(btn, MSTR("Save Layout File..."));

#define di(id) ((CEdit*)GetDlgItem(id))

	int i=0;
	m_SymList[i++].Init(lBig, k0, di(IDC_BIG_KEY0), di(IDC_BIG_CODE_KEY0));
	m_SymList[i++].Init(lBig, k1, di(IDC_BIG_KEY1), di(IDC_BIG_CODE_KEY1));
	m_SymList[i++].Init(lBig, k2, di(IDC_BIG_KEY2), di(IDC_BIG_CODE_KEY2));
	m_SymList[i++].Init(lBig, k3, di(IDC_BIG_KEY3), di(IDC_BIG_CODE_KEY3));
	m_SymList[i++].Init(lBig, k4, di(IDC_BIG_KEY4), di(IDC_BIG_CODE_KEY4));
	m_SymList[i++].Init(lBig, k5, di(IDC_BIG_KEY5), di(IDC_BIG_CODE_KEY5));
	m_SymList[i++].Init(lBig, k6, di(IDC_BIG_KEY6), di(IDC_BIG_CODE_KEY6));
	m_SymList[i++].Init(lBig, k7, di(IDC_BIG_KEY7), di(IDC_BIG_CODE_KEY7));
	m_SymList[i++].Init(lBig, k8, di(IDC_BIG_KEY8), di(IDC_BIG_CODE_KEY8));
	m_SymList[i++].Init(lBig, k9, di(IDC_BIG_KEY9), di(IDC_BIG_CODE_KEY9));
	m_SymList[i++].Init(lBig, kS, di(IDC_BIG_KEYS), di(IDC_BIG_CODE_KEYS));
	m_SymList[i++].Init(lBig, kG, di(IDC_BIG_KEYG), di(IDC_BIG_CODE_KEYG));

	m_SymList[i++].Init(lSml, k0, di(IDC_SMALL_KEY0), di(IDC_SMALL_CODE_KEY0));
	m_SymList[i++].Init(lSml, k1, di(IDC_SMALL_KEY1), di(IDC_SMALL_CODE_KEY1));
	m_SymList[i++].Init(lSml, k2, di(IDC_SMALL_KEY2), di(IDC_SMALL_CODE_KEY2));
	m_SymList[i++].Init(lSml, k3, di(IDC_SMALL_KEY3), di(IDC_SMALL_CODE_KEY3));
	m_SymList[i++].Init(lSml, k4, di(IDC_SMALL_KEY4), di(IDC_SMALL_CODE_KEY4));
	m_SymList[i++].Init(lSml, k5, di(IDC_SMALL_KEY5), di(IDC_SMALL_CODE_KEY5));
	m_SymList[i++].Init(lSml, k6, di(IDC_SMALL_KEY6), di(IDC_SMALL_CODE_KEY6));
	m_SymList[i++].Init(lSml, k7, di(IDC_SMALL_KEY7), di(IDC_SMALL_CODE_KEY7));
	m_SymList[i++].Init(lSml, k8, di(IDC_SMALL_KEY8), di(IDC_SMALL_CODE_KEY8));
	m_SymList[i++].Init(lSml, k9, di(IDC_SMALL_KEY9), di(IDC_SMALL_CODE_KEY9));
	m_SymList[i++].Init(lSml, kS, di(IDC_SMALL_KEYS), di(IDC_SMALL_CODE_KEYS));
	m_SymList[i++].Init(lSml, kG, di(IDC_SMALL_KEYG), di(IDC_SMALL_CODE_KEYG));

	m_ININameCtrl.GetWindowRect(r);
	ScreenToClient(r);
	r.OffsetRect(0, 1);
	m_ININameCtrl.MoveWindow(r);
	m_INIPhoneCtrl.GetWindowRect(r);
	ScreenToClient(r);
	r.OffsetRect(0, 1);
	m_INIPhoneCtrl.MoveWindow(r);

	m_DynSize.AttachDialog(this);

	m_ININameCtrl.ResetContent();
	m_INIPhoneCtrl.ResetContent();
	m_INIPhones.RemoveAll();
	m_INIPhones.Add(CINIPhoneFiles());
	m_INIPhones.Add(CINIPhoneFiles());
	m_INIPhones[0].m_Name=MSTR("(All phones)");
	m_INIPhones[1].m_Name=MSTR("(Unknown)");
	CFileFind finder;
	int p;
	BOOL bWorking=finder.FindFile(((CKLayApp*)AfxGetApp())->m_INIPath+_T("*.ini"));
	while(bWorking)
	{
		bWorking=finder.FindNextFile();
		i=(int)m_INIPhones[0].m_Files.Add(CINIFileInfo());
		m_INIPhones[0].m_Files[i].m_FilePath=finder.GetFilePath();
		m_INIPhones[0].m_Files[i].m_FWVersion=VGetPrivateProfileString(secIniInfo, keyIniFWVer, NULL, finder.GetFilePath());
		m_INIPhones[0].m_Files[i].m_Name=VGetPrivateProfileString(secIniInfo, keyIniName, NULL, finder.GetFilePath());
		m_INIPhones[0].m_Files[i].m_Author=VGetPrivateProfileString(secIniInfo, keyIniAuthor, NULL, finder.GetFilePath());
		m_INIPhones[0].m_Files[i].m_Comments=VGetPrivateProfileString(secIniInfo, keyIniComments, NULL, finder.GetFilePath());
		m_INIPhones[0].m_Files[i].m_Phone=VGetPrivateProfileString(secIniInfo, keyIniPhone, NULL, finder.GetFilePath());

		if(m_INIPhones[0].m_Files[i].m_Name.IsEmpty())
			m_INIPhones[0].m_Files[i].m_Name=finder.GetFileTitle();
		if(!m_INIPhones[0].m_Files[i].m_FWVersion.IsEmpty())
			m_INIPhones[0].m_Files[i].m_Name=CString(_T("fw"))+m_INIPhones[0].m_Files[i].m_FWVersion+CString(_T(' '))+m_INIPhones[0].m_Files[i].m_Name;

		str=m_INIPhones[0].m_Files[i].m_Phone;
		for(p=2; p<m_INIPhones.GetSize(); p++)
		{
			if(m_INIPhones[p].m_Name==str)
			{
				m_INIPhones[p].m_Files.Add(m_INIPhones[0].m_Files[i]);
				break;
			}
		}
		if(p==m_INIPhones.GetSize())
		{
			if(m_INIPhones[0].m_Files[i].m_Phone.IsEmpty())
				m_INIPhones[1].m_Files.Add(m_INIPhones[0].m_Files[i]);
			else
			{
				p=(int)m_INIPhones.Add(CINIPhoneFiles());
				m_INIPhones[p].m_Name=m_INIPhones[0].m_Files[i].m_Phone;
				m_INIPhones[p].m_Files.Add(m_INIPhones[0].m_Files[i]);
				m_INIPhoneCtrl.SetItemData(m_INIPhoneCtrl.AddString(m_INIPhones[p].m_Name), p);
			}
		}

		if(!m_INIPhones[0].m_Files[i].m_Phone.IsEmpty())
			m_INIPhones[0].m_Files[i].m_Name=m_INIPhones[0].m_Files[i].m_Phone+CString(_T(' '))+m_INIPhones[0].m_Files[i].m_Name;
	}
	m_INIPhoneCtrl.SetItemData(m_INIPhoneCtrl.InsertString(m_INIPhoneCtrl.GetCount(), m_INIPhones[0].m_Name), 0);
	m_INIPhoneCtrl.SetItemData(m_INIPhoneCtrl.InsertString(m_INIPhoneCtrl.GetCount(), m_INIPhones[1].m_Name), 1);
	m_INIPhoneCtrl.SetItemData(m_INIPhoneCtrl.InsertString(0, IdentStr(MSTR("Select phone:"))), -1);
	m_INIPhoneCtrl.SetCurSel(0);
	OnCbnSelchangeIniPhone();


	if(m_pToolTipCtrl)
		m_pToolTipCtrl->Activate(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

afx_msg LRESULT CLayoutPage::OnInitialUpdate(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	CString fname=((CKLayApp*)AfxGetApp())->GetCmdLineFileName();
	if(!fname.IsEmpty())
	{
		if(fname.GetLength()>=4)
		{
			CString ext=fname.Right(4);
			if(ext.CompareNoCase(_T(".vkl"))==0)
				if(OpenDocument(fname))
				{
					((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
					if(o_cmdDoWrite)
						OnBnClickedUpdate();
					return 0;
				}
		}
	}
	return 1;
}

afx_msg LRESULT CLayoutPage::OnIsCanClose(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	if(m_IsModified && pCKLayDlg && wTabIdx!=-1)
		((CKLayDlg*)pCKLayDlg)->SetCurPage((int)wTabIdx);
	return SaveModifiedDocument();
}

afx_msg LRESULT CLayoutPage::OnOptionsChanged(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	for(int i=0; i<SYM_LISTS_COUNT; i++)
	{
		if(!m_SymList[i].LoadOptions())
		{
			m_INIPhoneCtrl.SetCurSel(0);
			OnCbnSelchangeIniPhone();
			return FALSE;
		}
	}

	int p, f;
	p=m_INIPhoneCtrl.GetCurSel();
	f=m_ININameCtrl.GetCurSel();
	if(p!=-1 && f!=-1)
	{
		p=(int)m_INIPhoneCtrl.GetItemData(p);
		f=(int)m_ININameCtrl.GetItemData(f);
		if(p!=-1 && f!=-1)
		{
			if(Options.strINIFileName.CompareNoCase(m_INIPhones[p].m_Files[f].m_FilePath)!=0)
			{
				BOOL old=Options.IsValid;
				m_INIPhoneCtrl.SetCurSel(0);
				OnCbnSelchangeIniPhone();
				Options.IsValid=old;
			}
		}
	}

	SetDlgItemText(IDC_INI_AUTHOR,
		VGetPrivateProfileString(secIniInfo, keyIniAuthor, NULL, Options.strINIFileName));
	SetDlgItemText(IDC_INI_COMMENTS,
		VGetPrivateProfileString(secIniInfo, keyIniComments, NULL, Options.strINIFileName));
	return TRUE;
}

void CLayoutPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	((CKLayDlg*)AfxGetMainWnd())->UpdateMainTitle(bShow, m_strDocFileName, m_IsModified);
}

void CLayoutPage::OnCancel()
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void CLayoutPage::OnBnClickedIniBrowse()
{
	if(Options.Load())
	{
		GetDlgItem(IDC_INI_FNAME)->SetWindowText(Options.strINIFileName);
		int len=Options.strINIFileName.GetLength();
		((CEdit*)GetDlgItem(IDC_INI_FNAME))->SetSel(len, len, TRUE);
	}
}

void CLayoutPage::OnOK()
{
}


void CLayoutPage::OnSize(UINT nType, int cx, int cy)
{
	m_DynSize.OnSize(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

void CLayoutPage::OnCbnSelchangeIniPhone()
{
	m_ININameCtrl.ResetContent();
	int i=m_INIPhoneCtrl.GetCurSel();
	Options.IsValid=FALSE;
	if(i!=-1)
	{
		i=(int)m_INIPhoneCtrl.GetItemData(i);
		if(i!=-1)
		{
			CINIFileInfos &files=m_INIPhones[i].m_Files;
			for(i=0; i<files.GetSize(); i++)
				m_ININameCtrl.SetItemData(m_ININameCtrl.AddString(files[i].m_Name), i);
		}
		m_ININameCtrl.SetItemData(m_ININameCtrl.InsertString(0, IdentStr(MSTR("Select layout type:"))), -1);
		m_ININameCtrl.SetCurSel(0);
		OnCbnSelchangeIniName();
	}
}

void CLayoutPage::OnCbnSelchangeIniName()
{
	Options.IsValid=FALSE;
	int p, f;
	p=m_INIPhoneCtrl.GetCurSel();
	f=m_ININameCtrl.GetCurSel();
	SetDlgItemText(IDC_INI_AUTHOR, NULL);
	SetDlgItemText(IDC_INI_COMMENTS, NULL);
	if(p!=-1 && f!=-1)
	{
		p=(int)m_INIPhoneCtrl.GetItemData(p);
		f=(int)m_ININameCtrl.GetItemData(f);
		if(p!=-1 && f!=-1)
		{
			if(!Options.Load(m_INIPhones[p].m_Files[f].m_FilePath))
			{
				m_INIPhoneCtrl.SetCurSel(0);
				OnCbnSelchangeIniPhone();
			}
		}
	}
}

afx_msg LRESULT CLayoutPage::OnDeviceChanged(WPARAM wTabIdx, LPARAM pCKLayDlg)
{
	m_INIPhoneCtrl.SetCurSel(0);
	OnCbnSelchangeIniPhone();
	return TRUE;
}

BOOL CLayoutPage::PreTranslateMessage(MSG* pMsg)
{
	if(m_pToolTipCtrl!=NULL)
		m_pToolTipCtrl->RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

