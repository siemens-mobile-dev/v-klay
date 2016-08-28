// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "OptionsDlg.h"
#include "OptionsAdvancedDlg.h"
#include "HexViewDlg.h"




LPCTSTR strDefault=NULL;


// COptionsDlg dialog

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)
COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
	, m_bDisableWarningsMsg(FALSE)
	, m_strLocale(_T(""))
	, m_strLanguage(_T(""))
{
	m_strSystemDefault=MSTR("(system default)");
	m_PatcherFontSize=8;
	m_bIsPatcherSwapPasteKeys = FALSE;
}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_WARNING_MSG_DISABLE, m_bDisableWarningsMsg);
	DDX_Check(pDX, IDC_RTF_WARNING_MSG_DISABLE, m_bDisableRTFSaveWarningsMsg);
	DDX_Check(pDX, IDC_PASTE_KEYS_SWAP, m_bIsPatcherSwapPasteKeys);

	if(!pDX->m_bSaveAndValidate && m_strLocale==_T(""))
		DDX_CBStringExact(pDX, IDC_LOCALE, m_strSystemDefault);
	else
		DDX_CBStringExact(pDX, IDC_LOCALE, m_strLocale);
	if(pDX->m_bSaveAndValidate)
		if(m_strLocale==m_strSystemDefault)
			m_strLocale=_T("");
	DDX_Control(pDX, IDC_LANG, m_LangCtrl);
	if(pDX->m_bSaveAndValidate)
	{
		int i=m_LangCtrl.GetCurSel();
		if(i!=-1)
			m_strLanguage=*((CString*)m_LangCtrl.GetItemDataPtr(i));
	}
	else
	{
		int sel=-1, i, n=m_LangCtrl.GetCount();
		for(i=0; i<n; i++)
			if(m_strLanguage.CompareNoCase(*((CString*)m_LangCtrl.GetItemDataPtr(i)))==0)
				sel=i;
		m_LangCtrl.SetCurSel(sel);
	}

	CString str, strtt;
	CWinApp *app=AfxGetApp();
	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXTLAY_PATH, str);
		app->WriteProfileString(secOptions, keyToolExtLayPath, str);
		DDX_Text(pDX, IDC_TOOL_EXTLAY_ARG, str);
		app->WriteProfileString(secOptions, keyToolExtLayArg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExtLayPath, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTLAY_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXTLAY_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExtLayArg, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTLAY_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXTPIC_PATH, str);
		app->WriteProfileString(secOptions, keyToolExtPicPath, str);
		DDX_Text(pDX, IDC_TOOL_EXTPIC_ARG, str);
		app->WriteProfileString(secOptions, keyToolExtPicArg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExtPicPath, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTPIC_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXTPIC_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExtPicArg, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTPIC_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXTFNT_PATH, str);
		app->WriteProfileString(secOptions, keyToolExtFntPath, str);
		DDX_Text(pDX, IDC_TOOL_EXTFNT_ARG, str);
		app->WriteProfileString(secOptions, keyToolExtFntArg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExtFntPath, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTFNT_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXTFNT_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExtFntArg, NULL);
		DDX_Text(pDX, IDC_TOOL_EXTFNT_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXT1_PATH, str);
		app->WriteProfileString(secOptions, keyToolExt1Path, str);
		DDX_Text(pDX, IDC_TOOL_EXT1_ARG, str);
		app->WriteProfileString(secOptions, keyToolExt1Arg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExt1Path, NULL);
		DDX_Text(pDX, IDC_TOOL_EXT1_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXT1_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExt1Arg, NULL);
		DDX_Text(pDX, IDC_TOOL_EXT1_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXT2_PATH, str);
		app->WriteProfileString(secOptions, keyToolExt2Path, str);
		DDX_Text(pDX, IDC_TOOL_EXT2_ARG, str);
		app->WriteProfileString(secOptions, keyToolExt2Arg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExt2Path, NULL);
		DDX_Text(pDX, IDC_TOOL_EXT2_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXT2_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExt2Arg, NULL);
		DDX_Text(pDX, IDC_TOOL_EXT2_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_EXT3_PATH, str);
		app->WriteProfileString(secOptions, keyToolExt3Path, str);
		DDX_Text(pDX, IDC_TOOL_EXT3_ARG, str);
		app->WriteProfileString(secOptions, keyToolExt3Arg, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyToolExt3Path, NULL);
		DDX_Text(pDX, IDC_TOOL_EXT3_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_EXT3_PATH))->SetSel(0,-1);
		str=app->GetProfileString(secOptions, keyToolExt3Arg, NULL);
//{CHexViewDlg dlg;
//dlg.m_pData = (BYTE*)(LPCTSTR)str;
//dlg.m_DataSize = str.GetLength()*sizeof(_TCHAR);
//dlg.DoModal();}
		DDX_Text(pDX, IDC_TOOL_EXT3_ARG, str);
	}

	if(pDX->m_bSaveAndValidate)
	{
		DDX_Text(pDX, IDC_TOOL_TMP_PATH, str);
		if(str==strDefault)
			str.Empty();
		if(!str.IsEmpty())
		{
			if(!CreateDirectory(str, NULL))
			{
				if(GetLastError()!=ERROR_ALREADY_EXISTS)
				{
					AfxMessageBox(MSTR("Invalid folder path!"), MB_OK|MB_ICONSTOP);
					pDX->Fail();
				}
			}
		}
		app->WriteProfileString(secOptions, keyTmpPath, str);
	}
	else
	{
		str=app->GetProfileString(secOptions, keyTmpPath, NULL);
		if(str.IsEmpty())
			str=strDefault;
		DDX_Text(pDX, IDC_TOOL_TMP_PATH, str);
		((CEdit*)GetDlgItem(IDC_TOOL_TMP_PATH))->SetSel(0,-1);
	}

	DDX_Text(pDX, IDC_PATCHER_FONT_SIZE, m_PatcherFontSize);
	DDV_MinMaxInt(pDX, m_PatcherFontSize, 3, 100);

    //{{AFX_DATA_MAP(COptionsDlg)
    //}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_LANG, OnCbnSelchangeLang)
//	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_TOOL_EXTLAY_PATH_BROWSE, OnBnClickedLayoutExtlayPathBrowse)
	ON_BN_CLICKED(IDC_TOOL_EXTPIC_PATH_BROWSE, OnBnClickedToolExtpicPathBrowse)
	ON_BN_CLICKED(IDC_TOOL_EXT1_PATH_BROWSE, OnBnClickedToolExt1PathBrowse)
	ON_BN_CLICKED(IDC_TOOL_EXT2_PATH_BROWSE, OnBnClickedToolExt2PathBrowse)
	ON_BN_CLICKED(IDC_TOOL_EXT3_PATH_BROWSE, OnBnClickedToolExt3PathBrowse)
	ON_BN_CLICKED(IDC_TOOL_EXTFNT_PATH_BROWSE, OnBnClickedToolExtfntPathBrowse)
	ON_BN_CLICKED(IDC_OPTIONS_ADVANCED, OnBnClickedOptionsAdvanced)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TOOL_TMP_PATH_BROWSE, OnBnClickedToolTmpPathBrowse)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_TOOL_EXTLAY_ARG_INS, IDC_TOOL_EXT3_ARG_INS, OnBnClickedSetToolArg)
END_MESSAGE_MAP()


// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	int i;

	strDefault=MSTR("(default)");

	SetWindowText(MSTR("Options..."));
	SetDlgItemText(IDC_GENERAL_STATIC, IdentStr(MSTR("General:")));
	SetDlgItemText(IDC_WARNING_MSG_DISABLE, MSTR("Disable warning and success messages"));
	SetDlgItemText(IDC_LANG_STATIC, MSTR("Language of interface:"));
	SetDlgItemText(IDC_LANG_AUTHOR_STATIC, MSTR("Author of translation:"));
	SetDlgItemText(IDC_PATCHER_TAB_STATIC, IdentStr(MSTR("Patcher Tab:")));
	SetDlgItemText(IDC_PATCHER_FONT_SIZE_STATIC, MSTR("Default font size of patch text:"));
	SetDlgItemText(IDC_PATCHER_CODEPAGE_STATIC, MSTR("Codepage of patch text:"));
	SetDlgItemText(IDOK, MSTR("OK"));
	SetDlgItemText(IDCANCEL, MSTR("Cancel"));
	SetDlgItemText(IDC_TOOL_EXTLAY_PATH_STATIC, MSTR("External editor of keyboard latouts:"));
	SetDlgItemText(IDC_TOOL_EXTPIC_PATH_STATIC, MSTR("External editor of phone pictures:"));
	SetDlgItemText(IDC_TOOL_EXTFNT_PATH_STATIC, MSTR("External editor of phone fonts:"));
	SetDlgItemText(IDC_TOOL_EXT1_PATH_STATIC, MSTR("External editor of phone fullflash:"));
	SetDlgItemText(IDC_TOOL_EXT2_PATH_STATIC, MSTR("External tool #1:"));
	SetDlgItemText(IDC_TOOL_EXT3_PATH_STATIC, MSTR("External tool #2:"));
	SetDlgItemText(IDC_OPTIONS_ADVANCED, MSTR("Advanced..."));
	SetDlgItemText(IDC_TOOL_TMP_PATH_STATIC, MSTR("Temporary folder for fullflash and patches:"));
	SetDlgItemText(IDC_RTF_WARNING_MSG_DISABLE, MSTR("Disable warning about saving in RTF"));
	SetDlgItemText(IDC_PASTE_KEYS_SWAP, MSTR("Swap key combinations for \"Paste\" and \"Paste Text\""));

	CRect r;
	GetDlgItem(IDC_OPTIONS_ADVANCED)->GetWindowRect(r);
	ScreenToClient(r);
	r.top--;
	GetDlgItem(IDC_OPTIONS_ADVANCED)->MoveWindow(r);

	CWinApp *app = AfxGetApp();

	for(i = IDC_TOOL_EXTLAY_ARG_INS; i <= IDC_TOOL_EXT3_ARG_INS; i++)
	{
		((CButton*)GetDlgItem(i))->
			SetIcon((HICON)LoadImage(app->m_hInstance, MAKEINTRESOURCE(IDI_TOOLS_OPTIONS),
			IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	}

	((CComboBox*)GetDlgItem(IDC_LOCALE))->AddString(m_strSystemDefault);

	CDialog::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_PATCHER_FONT_SIZE_SPIN))->SetRange(5, 30);
	m_LangCtrl.ResetContent();
	CFileFind finder;
	BOOL bWorking=finder.FindFile(((CKLayApp*)AfxGetApp())->m_LangPath+_T("*.h"));
	int sel=-1;
	CString str;
	while(bWorking)
	{
		bWorking=finder.FindNextFile();
		str=((CKLayApp*)AfxGetApp())->GetLangFileInfo(finder.GetFilePath(), _T("LANGUAGE_NAME"));
		if(str.IsEmpty())
			str=finder.GetFileTitle();
		i=m_LangCtrl.AddString(str);
		m_LangCtrl.SetItemDataPtr(i, new CString(finder.GetFileTitle()));
		if(sel==-1)
			if(m_strLanguage.CompareNoCase(finder.GetFileTitle())==0)
				sel=i;
	}
	m_LangCtrl.SetCurSel(sel);
	UpdateData(FALSE);
	OnCbnSelchangeLang();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnCbnSelchangeLang()
{
	CString author;
	int i=m_LangCtrl.GetCurSel();
	if(i!=-1)
	{
		CString lang=*((CString*)m_LangCtrl.GetItemDataPtr(i));
		author=((CKLayApp*)AfxGetApp())->GetLangFileInfo(
			((CKLayApp*)AfxGetApp())->m_LangPath+lang+_T(".h"), _T("AUTHOR"));
	}
	SetDlgItemText(IDC_LANG_AUTHOR, author);
	GetDlgItem(IDC_LANG_AUTHOR_STATIC)->ShowWindow(author.IsEmpty() ? SW_HIDE : SW_SHOW);
	GetDlgItem(IDC_LANG_AUTHOR)->ShowWindow(author.IsEmpty() ? SW_HIDE : SW_SHOW);
}

void COptionsDlg::OnDestroy()
{
	int i, n=m_LangCtrl.GetCount();
	CString *pstr;
	for(i=0; i<n; i++)
	{
		pstr=(CString*)m_LangCtrl.GetItemDataPtr(i);
		if(pstr!=NULL)
		{
			delete pstr;
			m_LangCtrl.SetItemDataPtr(i, NULL);
		}
	}
	CDialog::OnDestroy();
}

BOOL COptionsDlg::BrowseForToolPath(CEdit* pathEditBox)
{
	if(pathEditBox==NULL)
		return FALSE;
	CString str;
	pathEditBox->GetWindowText(str);
	CFileDialog fdlg(TRUE, _T("exe"), str, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, exefilter, this);
	fdlg.m_ofn.lpstrTitle=openDialogTitle;
	if(fdlg.DoModal()==IDOK)
	{
		pathEditBox->SetWindowText(fdlg.GetPathName());
		pathEditBox->SetSel(0,-1);
	}
	return TRUE;
}

void COptionsDlg::OnBnClickedLayoutExtlayPathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXTLAY_PATH));
}

void COptionsDlg::OnBnClickedToolExtpicPathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXTPIC_PATH));
}

void COptionsDlg::OnBnClickedToolExtfntPathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXTFNT_PATH));
}

void COptionsDlg::OnBnClickedToolExt1PathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXT1_PATH));
}

void COptionsDlg::OnBnClickedToolExt2PathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXT2_PATH));
}

void COptionsDlg::OnBnClickedToolExt3PathBrowse()
{
	BrowseForToolPath((CEdit*)GetDlgItem(IDC_TOOL_EXT3_PATH));
}


void COptionsDlg::OnBnClickedOptionsAdvanced()
{
	COptionsAdvancedDlg dlg;
	dlg.m_AutoignitionType=o_AutoignitionType;
	dlg.m_bCommSimple=o_bCommSimple;
	dlg.m_bIsRepairPatchCanSkip=o_bIsRepairPatchCanSkip;
	dlg.m_bIsPatchIgnoreNoOld=o_bIsPatchIgnogeNoOld;
	dlg.m_bIsRepairPatchTextSimple=o_bIsRepairPatchTextSimple;
	dlg.m_bCommRTS=o_bCommRTS;
	dlg.m_bCommDTR=o_bCommDTR;
	dlg.m_bIsEnableBootcoreWritting=o_bIsEnableBootcoreWritting;
	if(dlg.DoModal()==IDOK)
	{
		int at=o_AutoignitionType;
		BOOL cs=o_bCommSimple;
		BOOL crts=o_bCommRTS;
		BOOL cdtr=o_bCommDTR;

		o_AutoignitionType=dlg.m_AutoignitionType;
		o_bCommSimple=dlg.m_bCommSimple;
		o_bCommRTS=dlg.m_bCommRTS;
		o_bCommDTR=dlg.m_bCommDTR;

		o_bIsRepairPatchCanSkip=dlg.m_bIsRepairPatchCanSkip;
		o_bIsPatchIgnogeNoOld=dlg.m_bIsPatchIgnoreNoOld;
		o_bIsRepairPatchTextSimple=dlg.m_bIsRepairPatchTextSimple;
		o_bIsEnableBootcoreWritting=dlg.m_bIsEnableBootcoreWritting;
		((CKLayApp*)AfxGetApp())->SaveOptions();

		if(o_AutoignitionType != at
			|| o_bCommSimple != cs
			|| o_bCommRTS != crts
			|| o_bCommDTR != cdtr
			)
		{
			o_AutoignitionType=at;			//we need to reset device with old options!
			o_bCommSimple=cs;
			o_bCommRTS=crts;
			o_bCommDTR=cdtr;
			if(pDevice!=NULL)
				pDevice->Reset();
			o_AutoignitionType=dlg.m_AutoignitionType;
			o_bCommSimple=dlg.m_bCommSimple;
			o_bCommRTS=dlg.m_bCommRTS;
			o_bCommDTR=dlg.m_bCommDTR;
		}
	}
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg==BFFM_INITIALIZED)
		::SendMessage(hwnd, BFFM_SETSELECTION, FALSE, lpData);
	return 0;
}

void COptionsDlg::OnBnClickedToolTmpPathBrowse()
{
	CEdit* pathEditBox=(CEdit*)GetDlgItem(IDC_TOOL_TMP_PATH);
	if(pathEditBox==NULL)
		return;
	LPITEMIDLIST pidl, curpidl=NULL;
	CString name, path;
	pathEditBox->GetWindowText(path);
	if(path.IsEmpty() || path==strDefault)
	{
		path=((CKLayApp*)AfxGetApp())->m_AppPath+defaultTmpPathRelativeApp;
		CreateDirectory(path, NULL);
	}
	IShellFolder *pshf;
	if(SHGetDesktopFolder(&pshf)==NOERROR)
	{
		LPOLESTR ppath;
#ifdef UNICODE
		ppath=(LPOLESTR)(LPCTSTR)path;
#else
		ppath=new WCHAR[path.GetLength()+16];
		MultiByteToWideChar(CP_ACP, 0, path, -1, ppath, path.GetLength()+16);
#endif
		if(pshf->ParseDisplayName(this->GetSafeHwnd(), NULL, ppath, NULL, &curpidl, NULL)!=S_OK)
			curpidl=NULL;
#ifndef UNICODE
		delete ppath;
#endif
		pshf->Release();
	}
	IMalloc *im;
	if(SHGetMalloc(&im)!=NOERROR)
		im=NULL;
	BROWSEINFO bi;
	bi.hwndOwner=this->GetSafeHwnd();
	bi.pidlRoot=NULL; //curpidl;
	bi.pszDisplayName=name.GetBuffer(MAX_PATH*10);
	bi.lpszTitle=MSTR("Select the folder for storing fullflash and patches, while working with external tools:");
	bi.lpfn=BrowseCallbackProc;
	bi.lParam=(LPARAM)curpidl;
	bi.ulFlags=BIF_NEWDIALOGSTYLE;
	pidl=SHBrowseForFolder(&bi);
	name.ReleaseBuffer();
	if(im!=NULL && curpidl!=NULL)
		im->Free(curpidl);
	if(pidl!=NULL)
	{
		if(SHGetPathFromIDList(pidl, path.GetBuffer(MAX_PATH*10)))
		{
			path.ReleaseBuffer();
			IncludeTrailingSlash(path);
			pathEditBox->SetWindowText(path);
			pathEditBox->SetSel(0,-1);
		}
		if(im!=NULL)
			im->Free(pidl);
	}
	if(im!=NULL)
		im->Release();
}

void COptionsDlg::OnBnClickedSetToolArg(UINT nId)
{
	CString str;
	CRect r;
	CMenu menu;
	menu.CreatePopupMenu();
	CMenu* pPopup = &menu;
	ASSERT(pPopup != NULL);
	pPopup->AppendMenu(MF_STRING, 0,
		CString(_T("\t")) + MSTR("Add argument:") + _T("                      "));
	pPopup->AppendMenu(MF_SEPARATOR);
	pPopup->SetDefaultItem(0, TRUE);
	pPopup->AppendMenu(MF_STRING, 1,
		CString(_T("\t")) + MSTR("Default (automatic)"));
	pPopup->AppendMenu(MF_STRING, 2,
		CString(ExtToolArgs[0]) + _T("\t") + MSTR("Without arguments"));
	//pPopup->AppendMenu(MF_STRING, 3,
	//	CString(ExtToolArgs[1]) + _T("\t") + MSTR("Current opened patch file name"));
	//pPopup->AppendMenu(MF_STRING, 4,
	//	CString(ExtToolArgs[2]) + _T("\t") + MSTR("Current opened fullflash file name"));
	//pPopup->AppendMenu(MF_STRING, 5,
	//	CString(ExtToolArgs[3]) + _T("\t") + MSTR("Current opened keyboard layout file name"));
	CWinApp *app=AfxGetApp();
	if(app==NULL)
		return;
	GetDlgItem(nId)->GetWindowRect(r);
	int ret=pPopup->TrackPopupMenuEx(TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON, r.right, r.bottom, this, NULL);
	CEdit *ed;
	ed = (CEdit*) GetDlgItem(IDC_TOOL_EXTLAY_ARG + (nId - IDC_TOOL_EXTLAY_ARG_INS));
	if(ed == NULL)
		return;
	if(ret == 1)
	{
		ed->SetWindowText(NULL);
	}
	else if(ret >= 2)
	{
		ed->GetWindowText(str);
		if(!str.IsEmpty())
			str += _T(' ');
		str += ExtToolArgs[ret - 2];
		ed->SetWindowText(str);
		ed->SetSel(0,-1);
	}
}
