//---------------------------------------------------------

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   mdichild.c
//
//  PURPOSE:
//    To implement the basic mdi child commands.
//
//  FUNCTIONS:
//
//  COMMENTS:
//
//
//  SPECIAL INSTRUCTIONS: N/A
//

#include "stdafx.h"

//#include <windows.h>            // required for all Windows applications
//#include <windowsx.h>
//#include <oledlg.h>
//#include <richedit.h>
// Shouldn't need to #include <initguid.h> here, but IID_IRichEditOleCallback
// (defined in richole.h) has gone AWOL from riched32.dll, so just do this
// for now.
//#include <initguid.h>
//#include <richole.h>
//#include <string.h>
//#include <malloc.h>
#include "REDoc.h"
#include "V_KLay.h"

namespace VKLayRichEditDoc
{


static TCHAR szAppName[1024]=_T("V_KLay");   // Caption of error message boxes

// Used by Ansi/Unicode conversion macros in writepad.inl
static int _temp;
#include "WRITEPAD.INL"


// Functions to export from DLL
/*extern "C" {

// Set the apliation name (used as caption in error messageboxes)
void _export SetAppName(char *Name)
{
    strncpy(szAppName,Name,1023);
}

// Initializes RichEdit ole interface.
// Return value: Handle 
DWORD _export InitRichEditOleInterface(HWND MainWndHandle, HWND RichEditHandle)
{
    CRichEditDoc *RD;
    RD=new CRichEditDoc("Untitled");
    RD->Create(MainWndHandle,RichEditHandle);
    return (DWORD)RD;
}

// Frees memory. Should be called before destruction of RichEdit control
// or in TForm.OnDestroy
void _export CloseRichEditOleInterface(DWORD Handle)
{
    if(Handle)
    {
        CRichEditDoc *RD=(CRichEditDoc*)Handle;
        RD->Close();
        delete RD;
    }
}

// Shows dialog to insert object into RichEdit.
// Return: TRUE==succeeded
BOOL _export InsertObject(DWORD Handle)
{
    if(Handle)
    {
        CRichEditDoc *RD=(CRichEditDoc*)Handle;
        return RD->InsertObject();
    }
    return FALSE;
}

// Sets the file name of document. Used in caption of OLE server
// Default name is "Untitled"
void _export SetFileName(DWORD Handle,char *FileName)
{
    if(Handle)
    {
        CRichEditDoc *RD=(CRichEditDoc*)Handle;
        RD->SetFileName(FileName);
    }
}

} // extern "C"
// End of exported functions
*/

/*
 * OleUIMetafilePictIconFree    [stolen from OLE2UI]
 *
 * Purpose:
 *  Deletes the metafile contained in a METAFILEPICT structure and
 *  frees the memory for the structure itself.
 *
 * Parameters:
 *  hMetaPict       HGLOBAL metafilepict structure created in
 *                  OleUIMetafilePictFromIconAndLabel
 *
 * Return Value:
 *  None
 */

STDAPI_(void) OleUIMetafilePictIconFree(HGLOBAL hMetaPict)
{
    LPMETAFILEPICT pMF;
    
    if (NULL == hMetaPict)
        return;
    
    pMF = (LPMETAFILEPICT)GlobalLock(hMetaPict);
    
    if (NULL != pMF)
    {
        if (NULL != pMF->hMF)
            DeleteMetaFile(pMF->hMF);
    }
    
    GlobalUnlock(hMetaPict);
    GlobalFree(hMetaPict);
    return;
}

/* OleStdSwitchDisplayAspect    [stolen from OLE2UI]
** -------------------------
**    Switch the currently cached display aspect between DVASPECT_ICON
**    and DVASPECT_CONTENT.
**
**    NOTE: when setting up icon aspect, any currently cached content
**    cache is discarded and any advise connections for content aspect
**    are broken.
**
**    RETURNS:
**      S_OK -- new display aspect setup successfully
**      E_INVALIDARG -- IOleCache interface is NOT supported (this is
**                  required).
**      <other SCODE> -- any SCODE that can be returned by
**                  IOleCache::Cache method.
**      NOTE: if an error occurs then the current display aspect and
**            cache contents unchanged.
*/

STDAPI OleStdSwitchDisplayAspect(LPOLEOBJECT  lpOleObj,
                                 LPDWORD      lpdwCurAspect,
                                 DWORD        dwNewAspect,
                                 HGLOBAL      hMetaPict,
                                 BOOL         fDeleteOldAspect,
                                 BOOL         fSetupViewAdvise,
                                 LPADVISESINK lpAdviseSink,
                                 LPBOOL       lpfMustUpdate)
{
    LPOLECACHE      lpOleCache = NULL;
    LPVIEWOBJECT    lpViewObj = NULL;
    LPENUMSTATDATA  lpEnumStatData = NULL;
    STATDATA        StatData;
    FORMATETC       FmtEtc;
    STGMEDIUM       Medium;
    DWORD           dwAdvf;
    DWORD           dwNewConnection;
    DWORD           dwOldAspect = *lpdwCurAspect;
    HRESULT         hrErr;

    if (lpfMustUpdate)
        *lpfMustUpdate = FALSE;

    lpOleObj->QueryInterface(IID_IOleCache, (LPVOID*)&lpOleCache);

    // if IOleCache* is NOT available, do nothing
    if (!lpOleCache)
        return E_INVALIDARG;

    // Setup new cache with the new aspect
    FmtEtc.cfFormat = 0;     // whatever is needed to draw
    FmtEtc.ptd      = NULL;
    FmtEtc.dwAspect = dwNewAspect;
    FmtEtc.lindex   = -1;
    FmtEtc.tymed    = TYMED_NULL;

    /* OLE2NOTE: if we are setting up Icon aspect with a custom icon
    **    then we do not want DataAdvise notifications to ever change
    **    the contents of the data cache. thus we set up a NODATA
    **    advise connection. otherwise we set up a standard DataAdvise
    **    connection.
    */
    if (dwNewAspect == DVASPECT_ICON && hMetaPict)
        dwAdvf = ADVF_NODATA;
    else
        dwAdvf = ADVF_PRIMEFIRST;

    hrErr = lpOleCache->Cache(&FmtEtc, dwAdvf, &dwNewConnection);
    if (!SUCCEEDED(hrErr))
    {
        lpOleCache->Release();
        return hrErr;
    }

    *lpdwCurAspect = dwNewAspect;

    /* OLE2NOTE: if we are setting up Icon aspect with a custom icon,
    **    then stuff the icon into the cache. otherwise the cache must
    **    be forced to be updated. set the *lpfMustUpdate flag to tell
    **    caller to force the object to Run so that the cache will be
    **    updated.
    */
    if (dwNewAspect == DVASPECT_ICON && hMetaPict)
    {
       FmtEtc.cfFormat = CF_METAFILEPICT;
       FmtEtc.ptd      = NULL;
       FmtEtc.dwAspect = DVASPECT_ICON;
       FmtEtc.lindex   = -1;
       FmtEtc.tymed    = TYMED_MFPICT;

       Medium.tymed          = TYMED_MFPICT;
       Medium.hGlobal        = hMetaPict;
       Medium.pUnkForRelease = NULL;

       /*hrErr =*/ lpOleCache->SetData(&FmtEtc, &Medium, FALSE /* fRelease */);
    }
    else
    {
        if (lpfMustUpdate)
            *lpfMustUpdate = TRUE;
    }

    if (fSetupViewAdvise && lpAdviseSink)
    {
        /* OLE2NOTE: re-establish the ViewAdvise connection */
        lpOleObj->QueryInterface(IID_IViewObject, (LPVOID*)&lpViewObj);

        if (lpViewObj)
        {
            lpViewObj->SetAdvise(dwNewAspect, 0, lpAdviseSink);
            lpViewObj->Release();
        }
    }

    /* OLE2NOTE: remove any existing caches that are set up for the old
    **    display aspect. It WOULD be possible to retain the caches set
    **    up for the old aspect, but this would increase the storage
    **    space required for the object and possibly require additional
    **    overhead to maintain the unused cachaes. For these reasons the
    **    strategy to delete the previous caches is prefered. If it is a
    **    requirement to quickly switch between Icon and Content
    **    display, then it would be better to keep both aspect caches.
    */

    if (fDeleteOldAspect)
    {
        hrErr = lpOleCache->EnumCache(&lpEnumStatData);

        while (hrErr == NOERROR)
        {
            hrErr = lpEnumStatData->Next(1, &StatData, NULL);
            if (hrErr != NOERROR)
                break;              // DONE! no more caches.

            if (StatData.formatetc.dwAspect == dwOldAspect)
            {
                // Remove previous cache with old aspect
                lpOleCache->Uncache(StatData. dwConnection);
            }
        }

        if (lpEnumStatData)
        {
            if (lpEnumStatData->Release() != 0)
                MessageBox(NULL,
                           TEXT("OleStdSwitchDisplayAspect: Cache enumerator NOT released"),
                           NULL,
                           MB_ICONEXCLAMATION | MB_OK);
        }
    }

    if (lpOleCache)
        lpOleCache->Release();

    return NOERROR;
}

//
//  FUNCTION: CRichEditDoc::CRichEditDoc(LPTSTR)
//
//  PURPOSE:  CRichEditDoc constructor
//
//  PARAMETERS:
//    LPTSTR    - initial window title (filename or "untitled")
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

CRichEditDoc::CRichEditDoc(LPCTSTR pszFileName)
    : m_cRef(0), m_hwndDoc(NULL), m_hwndEdit(NULL), m_hwndRuler(NULL),
      m_hdcTarget(NULL), m_fInitialized(FALSE), m_fFileNameSet(FALSE),
      m_fCSHMode(FALSE), m_pRECallback(NULL), m_pReObj(NULL), m_pActiveObject(NULL)
{
    if (pszFileName)
        lstrcpy(m_szFileName, pszFileName);
    else
        m_szFileName[0] = 0;
}


//
//  FUNCTION: CRichEditDoc::~CRichEditDoc()
//
//  PURPOSE:  CRichEditDoc destructor
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    None
//
//  COMMENTS:
//

CRichEditDoc::~CRichEditDoc()
{
    if (m_hdcTarget)
        DeleteDC(m_hdcTarget);

    if (m_pActiveObject)
        m_pActiveObject->Release();

    if (m_pRECallback)
        delete m_pRECallback;

    if (m_pReObj)
        m_pReObj->Release();
}


//
//  FUNCTION: CRichEditDoc::Create()
//
//  PURPOSE: Create an MDI child window associated with the CRichEditDoc object
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    BOOLEAN indicating success/failure
//
//  COMMENTS:
//

BOOL CRichEditDoc::Create(HWND MainWnd, HWND RichEdit)
{
    // First make sure we haven't already been called for this object
    if (m_hwndDoc)
        return FALSE;

    // Create the MDI child window
    m_hwndDoc=MainWnd;
    CreateEditControl(RichEdit);

    if (m_hwndDoc)
    {
        ShowWindow(m_hwndDoc, SW_SHOW);
        SetFocus(m_hwndDoc);

        m_fInitialized = TRUE;
        return TRUE;
    }
    else
        return FALSE;
}


/*
 *	CRichEditDoc::QueryInterface
 *
 *	Purpose:
 *		Returns a pointer to the specified interface.
 *
 *	Arguments:
 *		REFIID			Interface desired.
 *		LPUNKNOWN *		Interface we return.
 *
 *	Returns:
 *		HRESULT		 Error status.
 */

STDMETHODIMP CRichEditDoc::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    *ppvObj = NULL;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_IOleWindow) ||
		IsEqualIID(riid, IID_IOleInPlaceUIWindow))
	{
        *ppvObj = this;
	}
    else if (IsEqualIID(riid, IID_IRichEditOleCallback))
    {
        *ppvObj = m_pRECallback;
    }
	else
		return E_NOINTERFACE;

    if (*ppvObj)
        ((LPUNKNOWN)*ppvObj)->AddRef();

	return S_OK;
}


/*
 *	CRichEditDoc::AddRef
 *
 *	Purpose:
 *		Increments reference count on the specified object.
 *
 *	Arguments:
 *		None
 *
 *	Returns:
 *		ULONG			New value of reference count.
 */

STDMETHODIMP_(ULONG) CRichEditDoc::AddRef()
{
	return ++m_cRef;
}


/*
 *	CRichEditDoc::Release
 *
 *	Purpose:
 *		Decrements reference count on the specified object.  If count is
 *		decremented to zero, the object is freed.
 *
 *	Arguments:
 *		None
 *
 *	Returns:
 *		ULONG			New value of reference count.
 */

STDMETHODIMP_(ULONG) CRichEditDoc::Release()
{
    ULONG cRef = --m_cRef;

//    if(!cRef)         // CRichEditDoc destruction happens during WM_DESTROY
//        delete this;

    return cRef;
}


/*
 *	CRichEditDoc::GetWindow
 *
 *	Purpose:
 *		Return the window handle of the doc-level window for in place use.
 *
 *	Arguments:
 *		HWND*               Out param for window handle.
 *
 *	Returns:
 *		HRESULT				Error status.
 */
STDMETHODIMP CRichEditDoc::GetWindow(HWND* phwnd)
{
    *phwnd = m_hwndDoc;
	return S_OK;
}


/*
 *	CRichEditDoc::ContextSensitiveHelp
 *
 *	Purpose:
 *		Notifies the frame that the object has entered Context Help mode.
 *
 *	Arguments:
 *		BOOL                CSH mode.
 *
 *	Returns:
 *		HRESULT				Error status.
 */
STDMETHODIMP CRichEditDoc::ContextSensitiveHelp(BOOL fEnterMode)
{
    if (m_fCSHMode != fEnterMode)
    {
        m_fCSHMode = fEnterMode;

        // this code "trickles" the context sensitive help via shift+f1
        // to the inplace active object.  See the technotes for implementation
        // details.
        if (m_pActiveObject)
        {
            LPOLEINPLACEOBJECT pipo;
            m_pActiveObject->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&pipo);
            if (pipo)
            {
                pipo->ContextSensitiveHelp(fEnterMode);
                pipo->Release();
            }
        }
    }
    return S_OK;
}


/*
 *	CRichEditDoc::GetBorder
 *
 *	Purpose:
 *		Returns a RECT structure in which the object can put frame
 *		adornments while an object is active in place.
 *
 *	Arguments:
 *      LPRECT lprectBorder - out param to contain the outermost
 *                            rect for frame adornments
 *
 *	Returns:
 *		HRESULT				Error status.
 */

STDMETHODIMP CRichEditDoc::GetBorder(LPRECT prcBorder)
{
	return E_NOTIMPL;
}


/*
 *	CRichEditDoc::RequestBorderSpace
 *
 *	Purpose:
 *		Determines whether tools can be installed around the objects
 *		window frame while the object is active in place.
 *
 *	Arguments:
 *		LPCBORDERWIDTHS     Requested border space
 *
 *	Returns:
 *		HRESULT				Error status.
 */

STDMETHODIMP CRichEditDoc::RequestBorderSpace(LPCBORDERWIDTHS pbw)
{
    // Deny all requests
    return (pbw ? INPLACE_E_NOTOOLSPACE : S_OK);
}


/*
 *	CRichEditDoc::SetBorderSpace
 *
 *	Purpose:
 *		Allocates space for the border requested in the call to the
 *		RequestBorderSpace member function.
 *
 *	Arguments:
 *		LPCBORDERWIDTHS     Requested border space
 *
 *	Returns:
 *		HRESULT				Error status.
 */

STDMETHODIMP CRichEditDoc::SetBorderSpace(LPCBORDERWIDTHS pbw)
{
    // Deny all requests
    return (pbw ? INPLACE_E_NOTOOLSPACE : S_OK);
}


/*
 *	CRichEditDoc::SetActiveObject
 *
 *	Purpose:
 *		Called by the object to provide the document window a direct channel
 *		of communication with the active in-place object.
 *
 *	Arguments:
 *		LPOLEINPLACEACTIVEOBJECT    New active object (or NULL, if none)
 *      LPCOLESTR                   Object name
 *
 *	Returns:
 *		HRESULT				Error status.
 */

STDMETHODIMP CRichEditDoc::SetActiveObject(LPOLEINPLACEACTIVEOBJECT pipaobj,
                                           LPCOLESTR szObjName)
{
    if (szObjName)
    {
        TCHAR szNewTitle[MAX_PATH];
#ifdef UNICODE
        wsprintf(szNewTitle, TEXT("%s in %s"), szObjName, m_szFileName);
#else
        wsprintf(szNewTitle, TEXT("%s in %s"), WtoA(szObjName), m_szFileName);
#endif
        SetWindowText(m_hwndDoc, szNewTitle);
    }
    else
        SetWindowText(m_hwndDoc, m_szFileName);

	// Free any existing active object
	if(m_pActiveObject)
		m_pActiveObject->Release();

	// If we're given an object, AddRef it; update our remembered ipaobj
	if(pipaobj)
		pipaobj->AddRef();

	m_pActiveObject = pipaobj;

	return S_OK;
}


//
//  FUNCTION: CRichEditDoc::CreateEditControl(HWND)
//
//  PURPOSE: Create the RichEdit control with the MDI child as the parent
//
//  PARAMETERS:
//    None
//
//  RETURN VALUE:
//    TRUE - If initialization was successful.
//    FALSE - If initialization failed.
//
//  COMMENTS:
//

BOOL CRichEditDoc::CreateEditControl(HWND RichEdit)
{
    // First make sure we have a doc window but _not_ an edit control
    if (!m_hwndDoc || m_hwndEdit)
        return FALSE;

    RECT rect;
    GetBorder(&rect);

    m_hwndEdit = RichEdit;

    if (!m_hwndEdit)
        return FALSE;
/*
    // Set the RichEdit control to send the EN_SELCHANGE notification
    // via the WM_NOTIFY message.
    SendMessage(m_hwndEdit, EM_SETEVENTMASK, 0, ENM_SELCHANGE | EN_DROPFILES );

    // Set up the RichEdit control to act as WSYWIG as possible,
    // here we force a 7" wide page size.
    m_hdcTarget = CreateCompatibleDC(NULL);
//    m_hdcTarget = GetCurrentPrinterDC();
    if (!SendMessage(m_hwndEdit, EM_SETTARGETDEVICE, (WPARAM)m_hdcTarget, (LPARAM)(1440*7)))
    {
        DeleteDC(m_hdcTarget);
        m_hdcTarget = NULL;
    }

    // Set default character format...  The point here is to avoid having
    // the system font as default since it generally doesn't scale well.

    // Set up CHARFORMAT for default 10 point Times New Roman with no
    // extra effects (bold, italic, etc.)
    CHARFORMAT cf;
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_STRIKEOUT
                | CFM_UNDERLINE | CFM_COLOR | CFM_OFFSET | CFM_PROTECTED;
    cf.dwEffects = CFE_AUTOCOLOR;   // use COLOR_WINDOWTEXT
    cf.yHeight = 200; // 200 twips == 10 points
    cf.yOffset = 0;
    cf.crTextColor = 0;
    cf.bPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
    lstrcpy(cf.szFaceName, TEXT("Times New Roman"));

    // Set the default character format.
    SendMessage(m_hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
*/
    // Get the control's IRichEditOle interface
    SendMessage(m_hwndEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&m_pReObj);
    if (m_pReObj)
    {
        m_pReObj->AddRef();
#ifdef _UNICODE
		LPSTR asciiApp, asciiFile;
		int asciiAppLen=(int)_tcsclen(szAppName);
		int asciiFileLen=(int)_tcsclen(m_szFileName);
		asciiApp=new char[asciiAppLen+16];
		asciiFile=new char[asciiFileLen+16];
		WideCharToMultiByte(CP_ACP, 0, szAppName, -1, asciiApp, asciiAppLen+16, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, m_szFileName, -1, asciiFile, asciiFileLen+16, NULL, NULL);
        m_pReObj->SetHostNames(asciiApp, asciiFile);
		delete asciiApp;
		delete asciiFile;
#else
        m_pReObj->SetHostNames(szAppName, m_szFileName);
#endif
    }

    // Create IRichEditOleCallback interface and give to RichEdit control
    m_pRECallback = new CRichEditOleCallback(this);
    if (m_pRECallback)
        SendMessage(m_hwndEdit, EM_SETOLECALLBACK, 0, (LPARAM)m_pRECallback);
        // the control should AddRef this

    // Accept dropped files
//    DragAcceptFiles(m_hwndEdit, TRUE);
                                                
    return TRUE;
}                                   


//
//  FUNCTION: CRichEditDoc::Close()
//
//  PURPOSE:  Query user to save file before closing window
//
//  PARAMETERS:
//    wparam - Message specific data (unused).
//    lparam - Message specific data (unused).
//
//  RETURN VALUE:
//    Return 0 if user hits "Cancel" from the query save dialog.  This
//    prevents the MDI child from closing.
//
//  COMMENTS:
//    Call DefMDIChildProc so that the window gets destroyed.
//

BOOL CRichEditDoc::Close()
{
    if (m_pReObj)
    {
        // Deactivate any inplace object
        m_pReObj->InPlaceDeactivate();

        // Make sure all objects are shut down
        REOBJECT reobj = {0};
        reobj.cbStruct = sizeof(reobj);

        int cObj = m_pReObj->GetObjectCount();
        for (int i = 0; i < cObj; i++)
        {
            m_pReObj->GetObject(i, &reobj, REO_GETOBJ_POLEOBJ);
            if (reobj.poleobj)
            {
                reobj.poleobj->Close(OLECLOSE_NOSAVE);
                reobj.poleobj->Release();
            }
        }
    }
    return TRUE;
}

//
//	CRichEditDoc::GetNewStorage
//
//	Purpose:
//		Gets storage for a new object.
//
//	Arguments:
//		LPSTORAGE*          Where to return storage
//
//	Returns:
//		HRESULT	            Error status.
//

HRESULT CRichEditDoc::GetNewStorage(LPSTORAGE* ppstg)
{
    if (!ppstg)
        return E_INVALIDARG;

    // first, NULL the out pointer
    *ppstg = NULL;

    // Create a temporary storage object on HGLOBAL.  First create an
    // ILockBytes object on HGLOBAL with delete-on-release, then create
    // storage on it, then release the ILockBytes so its only remaining
    // reference is from the storage.  Releasing the storage will free
    // everything.

    LPLOCKBYTES pLockBytes;
    HRESULT hr = CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);

    if (FAILED(hr))
        return hr;

    hr = StgCreateDocfileOnILockBytes(pLockBytes,
                                      STGM_SHARE_EXCLUSIVE | STGM_CREATE
                                       | STGM_READWRITE,
                                      0,
                                      ppstg);
    pLockBytes->Release();

    return hr;
}


//
//	CRichEditDoc::DoVerb
//
//	Purpose:
//		Executes OLE verb
//
//	Arguments:
//		LONG                Verb #
//
//	Returns:
//		HRESULT	            Error status.
//

HRESULT CRichEditDoc::DoVerb(LONG iVerb)
{
    HRESULT hr = S_FALSE;

    if (!m_hwndEdit || !m_pReObj)
        return E_FAIL;

    REOBJECT reobj = {0};
    reobj.cbStruct = sizeof(reobj);

    // Get IOleObject and IOleClientSite interfaces for the selected object
    if (S_OK == m_pReObj->GetObject(REO_IOB_SELECTION,
                                    &reobj,
                                    REO_GETOBJ_POLEOBJ | REO_GETOBJ_POLESITE))
    {
        // Get object position
        POINT pt;
        SendMessage(m_hwndEdit, EM_POSFROMCHAR, (WPARAM)&pt, REO_IOB_SELECTION);

        // Calculate object rect in screen units for DoVerb
        RECT rect = {0};
        HDC hdc = GetDC(NULL);  // screen DC
        rect.right  = MulDiv(reobj.sizel.cx,
                             GetDeviceCaps(hdc, LOGPIXELSX),
                             2540);
        rect.bottom = MulDiv(reobj.sizel.cy,
                             GetDeviceCaps(hdc, LOGPIXELSY),
                             2540);
        ReleaseDC(NULL, hdc);
        OffsetRect(&rect, pt.x, pt.y);
 
        // Call the object
        hr = reobj.poleobj->DoVerb(iVerb,
                                   NULL,
                                   reobj.polesite,
                                   0,
                                   m_hwndEdit,
                                   &rect);

        reobj.poleobj->Release();
        reobj.polesite->Release();
    }

    return hr;
}


//
//	CRichEditDoc::InsertObject
//
//	Purpose:
//		Insert new object into RichEdit control
//
//	Arguments:
//		None
//
//	Returns:
//		BOOL                Success/failure
//

BOOL CRichEditDoc::InsertObject()
{
    OLEUIINSERTOBJECT   io = {0};
    LPOLEOBJECT         poleobj = NULL;
    LPOLECLIENTSITE     polesite = NULL;
    LPSTORAGE           pstgItem = NULL;
    TCHAR               szFile[MAX_PATH];
    DWORD               dwRet;
    REOBJECT            reobj;
    BOOL                bResult = FALSE;

    if (!m_hwndEdit || !m_pReObj)
        return FALSE;

    if (m_pReObj->GetClientSite(&polesite))
    {
        MessageBox(m_hwndDoc, TEXT("No client site!"), NULL, MB_OK);
        goto error;
    }

    if (FAILED(GetNewStorage(&pstgItem)))
    {
        MessageBox(m_hwndDoc, TEXT("Can't create item storage!"), NULL, MB_OK);
        goto error;
    }

    szFile[0] = 0;
    io.cbStruct         = sizeof(io);
    io.dwFlags          = IOF_SHOWHELP | IOF_CREATENEWOBJECT | IOF_CREATEFILEOBJECT |
                            IOF_CREATELINKOBJECT | IOF_SELECTCREATENEW;
//                            IOF_VERIFYSERVERSEXIST;
    io.hWndOwner        = m_hwndDoc;
    io.lpszCaption      = NULL;
    io.lpszFile         = szFile;
    io.cchFile          = MAX_PATH;
    io.iid              = IID_IOleObject;
    io.oleRender        = OLERENDER_DRAW;
    io.lpIOleClientSite = polesite;
    io.lpIStorage       = pstgItem;
    io.ppvObj           = (LPVOID*)&poleobj;
    io.clsid            = CLSID_NULL;

    if ((dwRet = OleUIInsertObject(&io)) != OLEUI_SUCCESS)
    {
        if (dwRet == OLEUI_CANCEL)
            goto error;

        else if (dwRet == OLEUI_IOERR_SCODEHASERROR)
            wsprintf(szFile, TEXT("OleUIInsertObject scode is %lx"), io.sc);

        else
            wsprintf(szFile, TEXT("OleUIInsertObject returned %ld"), dwRet);

        MessageBox(m_hwndDoc, szFile, szAppName, MB_ICONEXCLAMATION | MB_OK);
        goto error;
    }

    reobj.cbStruct  = sizeof(REOBJECT);
    reobj.cp        = REO_CP_SELECTION;
    reobj.clsid     = io.clsid;
    reobj.poleobj   = poleobj;
    reobj.pstg      = pstgItem;
    reobj.polesite  = polesite;
    reobj.sizel.cx  = 0;
    reobj.sizel.cy  = 0;
    reobj.dvaspect  = DVASPECT_CONTENT;
    reobj.dwFlags   = REO_RESIZABLE;
    reobj.dwUser    = 0;

    if (io.dwFlags & IOF_SELECTCREATENEW)
        reobj.dwFlags |= REO_BLANK;

    // RichEdit doesn't setup advises if reobj.clsid == CLSID_NULL
    // Try our darndest to get a CLSID

    if (IsEqualCLSID(reobj.clsid, CLSID_NULL))
    {
#ifdef UNICODE
        if (!SUCCEEDED(GetClassFile(szFile, &reobj.clsid)))
#else
        if (!SUCCEEDED(GetClassFile(AtoW(szFile), &reobj.clsid)))
#endif
            MessageBox(m_hwndDoc,
                       TEXT("No CLSID, but forging on"),
                       szAppName,
                       MB_ICONINFORMATION | MB_OK);
    }

    // Do we want an iconized version ?
    if (io.dwFlags & IOF_CHECKDISPLAYASICON)
    {
        BOOL fUpdate;               // Can't pass in NULL instead of &this

        // This will update dvaspect on success
        if(OleStdSwitchDisplayAspect(poleobj,
                                     &reobj.dvaspect,
                                     DVASPECT_ICON,
                                     io.hMetaPict,
                                     TRUE,
                                     FALSE,
                                     NULL,
                                     &fUpdate))
        {
            MessageBox(m_hwndDoc,
                       TEXT("Object couldn't be displayed as an icon."),
                       szAppName,
                       MB_ICONEXCLAMATION | MB_OK);
        }
    }

    // Put the thing in the edit control
    if (m_pReObj->InsertObject(&reobj))
    {
        MessageBox(m_hwndDoc,
                   TEXT("Object couldn't be inserted"),
                   szAppName,
                   MB_ICONEXCLAMATION | MB_OK);
        goto error;
    }

    bResult = TRUE;     // Success!

    // Do show verb only on new objects
    if (io.dwFlags & IOF_SELECTCREATENEW)
    {
        // Get object position
        POINT pt;
        SendMessage(m_hwndEdit, EM_POSFROMCHAR, (WPARAM)&pt, REO_IOB_SELECTION);

        RECT rect = {0, 0, 50, 50};
        OffsetRect(&rect, pt.x, pt.y);

        poleobj->DoVerb(OLEIVERB_SHOW,
                        NULL,
                        polesite,
                        0,
                        m_hwndEdit,
                        &rect);
    }

error:
    if (io.hMetaPict)
        OleUIMetafilePictIconFree(io.hMetaPict);
    if (polesite)
        polesite->Release();
    if (pstgItem)
        pstgItem->Release();
    if (poleobj)
        poleobj->Release();

    return bResult;
}


//
//	CRichEditDoc::PasteSpecial
//
//	Purpose:
//		Paste object from clipboard into RichEdit control
//
//	Arguments:
//		None
//
//	Returns:
//		BOOL                Success/failure
//

BOOL CRichEditDoc::PasteSpecial()
{
    MessageBox(m_hwndDoc, _T("Not yet implemented."), szAppName, MB_ICONSTOP | MB_OK);
    return FALSE;
}


//
//	CRichEditDoc::ConvertObject
//
//	Purpose:
//		Convert object to new type
//
//	Arguments:
//		None
//
//	Returns:
//		BOOL                Success/failure
//

BOOL CRichEditDoc::ConvertObject()
{
    MessageBox(m_hwndDoc, _T("Not yet implemented."), szAppName, MB_ICONSTOP | MB_OK);
    return FALSE;
}


//
//	CRichEditDoc::SetFileName
//
//	Purpose:
//		Set new Document name
//
//	Arguments:
//		LPTSTR              New name
//
//	Returns:
//		BOOL                Success/failure
//

BOOL CRichEditDoc::SetFileName(LPTSTR pszFileName)
{
    if (pszFileName)
    {
        lstrcpyn(m_szFileName, pszFileName, DIM(m_szFileName));
        m_fFileNameSet = TRUE;

        if (m_hwndDoc)
            SetWindowText(m_hwndDoc, m_szFileName);

        if (m_pReObj)
		{
#ifdef _UNICODE
			LPSTR asciiApp, asciiFile;
			int asciiAppLen=(int)_tcsclen(szAppName);
			int asciiFileLen=(int)_tcsclen(m_szFileName);
			asciiApp=new char[asciiAppLen+16];
			asciiFile=new char[asciiFileLen+16];
			WideCharToMultiByte(CP_ACP, 0, szAppName, -1, asciiApp, asciiAppLen+16, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, m_szFileName, -1, asciiFile, asciiFileLen+16, NULL, NULL);
			m_pReObj->SetHostNames(asciiApp, asciiFile);
			delete asciiApp;
			delete asciiFile;
#else
	        m_pReObj->SetHostNames(szAppName, m_szFileName);
#endif
		}

        return TRUE;
    }

    return FALSE;
}


//
//	CRichEditDoc::GetFileName
//
//	Purpose:
//		Return Document name
//
//	Arguments:
//		LPTSTR              Buffer to return name
//      UINT                Size of buffer (chars for Unicode, bytes for MBCS)
//
//	Returns:
//		BOOL                Success/failure
//

BOOL CRichEditDoc::GetFileName(LPTSTR pszFileName, UINT cbFileName)
{
    lstrcpyn(pszFileName, m_szFileName, cbFileName);
    return m_fFileNameSet;
}


//
//  CRichEditOleCallback implementation
//

//
//  CRichEditOleCallback IUnknown implementation
//
//  CRichEditOleCallback::QueryInterface
//  CRichEditOleCallback::AddRef
//  CRichEditOleCallback::Release
//
//  COMMENTS:
//    Delegates back to the Document object
//

STDMETHODIMP CRichEditOleCallback::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    return m_pDoc->QueryInterface(riid, ppvObj);
}

STDMETHODIMP_(ULONG) CRichEditOleCallback::AddRef()
{
    return m_pDoc->AddRef();
}

STDMETHODIMP_(ULONG) CRichEditOleCallback::Release()
{
    return m_pDoc->Release();
}


//
//  CRichEditOleCallback::GetNewStorage
//
//  Purpose:
//      Gets storage for a new object.
//
//  Arguments:
//      LPSTORAGE*          Where to return storage.
//
//  Returns:
//      HRESULT	            Error status.
//
//  Comments:
//    Implemented in the Document object
//

STDMETHODIMP CRichEditOleCallback::GetNewStorage(LPSTORAGE* ppstg)
{
    return m_pDoc->GetNewStorage(ppstg);
}


//
//  CRichEditOleCallback::GetInPlaceContext
//
//  Purpose:
//      Gets context information for an in place object.
//
//  Arguments:
//      LPOLEINPLACEFRAME*      Frame window object.
//      LPOLEINPLACEUIWINDOW*   Document window object.
//      LPOLEINPLACEFRAMEINFO   Frame window information.
//
//  Returns:
//      HRESULT                 Error status.
//

STDMETHODIMP CRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME* ppFrame,
                                                     LPOLEINPLACEUIWINDOW* ppDoc,
                                                     LPOLEINPLACEFRAMEINFO pFrameInfo)
{
	return E_NOTIMPL;
}


//
//  CRichEditOleCallback::ShowContainerUI
//
//  Purpose:
//      Displays or hides container UI.
//
//  Arguments:
//      BOOL
//
//  Returns:
//      HRESULT                 Error status.
//

STDMETHODIMP CRichEditOleCallback::ShowContainerUI(BOOL fShowContainerUI)
{
	return E_NOTIMPL;
}


STDMETHODIMP CRichEditOleCallback::QueryInsertObject(LPCLSID /* pclsid */,
                                                     LPSTORAGE /* pstg */,
                                                     LONG /* cp */)
{
    // Let RichEdit handle this
	return S_OK;
}


STDMETHODIMP CRichEditOleCallback::DeleteObject(LPOLEOBJECT /* poleobj */)
{
    // This call is a notification only
	return S_OK;
}


STDMETHODIMP CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT /* pdataobj */,
                                                   CLIPFORMAT* /* pcfFormat */,
                                                   DWORD /* reco */,
                                                   BOOL /* fReally */,
                                                   HGLOBAL /* hMetaPict */)
{
    // accept everything
	return S_OK;
}


STDMETHODIMP CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
    return E_NOTIMPL;
}


STDMETHODIMP CRichEditOleCallback::GetClipboardData(CHARRANGE* /* pchrg */,
                                                    DWORD /* reco */,
                                                    LPDATAOBJECT* ppdataobj)
{
    *ppdataobj = NULL;  // NULL out pointer

	// delegate back to RichEdit
	return E_NOTIMPL;
}


STDMETHODIMP CRichEditOleCallback::GetDragDropEffect(BOOL fDrag,
                                                     DWORD grfKeyState,
                                                     LPDWORD pdwEffect)
{
	// delegate back to RichEdit
	return E_NOTIMPL;
}


//
//  FUNCTION: CRichEditOleCallback::GetContextMenu(WORD, LPOLEOBJECT, CHARRANGE*, HMENU*)
//
//  PURPOSE: Creates context menu for alternate mouse clicks in RichEdit
//
//  PARAMETERS:
//    seltype   - Selection type
//    poleobj   - IOleObject interface of selected object, if any
//    pchrg     - Selection range
//    phmenu    - Out param for returning menu
//
//  RETURN VALUE:
//
//  HRESULT     - Error status
//
//  COMMENTS:
//
//

STDMETHODIMP CRichEditOleCallback::GetContextMenu(WORD seltype,
                                                  LPOLEOBJECT poleobj,
                                                  CHARRANGE* pchrg,
                                                  HMENU* phmenu)
{
	CWnd *mwnd=CWnd::FromHandle(m_pDoc->m_hwndDoc);
	OLECALLBACK_MENU_DATA pm;
	pm.seltype=seltype;
	pm.poleobj=poleobj;
	pm.pchrg=pchrg;
	pm.phmenu=phmenu;
	mwnd->SendMessage(VM_GET_PATCH_MENU, 0, (LPARAM)&pm);
	*phmenu=*pm.phmenu;
	return S_OK;
}


}