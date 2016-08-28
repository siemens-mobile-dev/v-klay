#pragma once
#include <oledlg.h>
#include <richedit.h>
#include <richole.h>

/*
 *  REDoc.h
 *
 *  Purpose:
 *      Definition of the CRichEditDoc object, which provides the
 *      IRichEditOleCallback and IOleInPlaceUIWindow interfaces for
 *      RichEdit OLE support.
 *
 */

namespace VKLayRichEditDoc
{

class CRichEditOleCallback;             // IRichEditOleCallback interface

class CRichEditDoc : public IOleInPlaceUIWindow
{
public:
    UINT m_cRef;                        // Reference count
    HWND m_hwndDoc;                     // MDI child (parent of RichEdit & Ruler)
    HWND m_hwndEdit;                    // RichEdit control
    HWND m_hwndRuler;                   // Ruler window
    HDC  m_hdcTarget;                   // Target DC for WYSIWYG
    BOOL m_fFileNameSet;                // TRUE if filename has been set
    BOOL m_fInitialized;                // TRUE if object was successfully initialized
    BOOL m_fCSHMode;                    // Context-sensitive help mode flag
    CRichEditOleCallback* m_pRECallback;// IRichEditOleCallback interface
    IRichEditOle* m_pReObj;
    LPOLEINPLACEACTIVEOBJECT m_pActiveObject;
    TCHAR m_szFileName[MAX_PATH];

    // construction/destruction
    CRichEditDoc(LPCTSTR pszFileName=0);
    virtual ~CRichEditDoc();
    BOOL Create(HWND MainWnd, HWND RichEdit);
    BOOL Close();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IOleWindow methods
    STDMETHODIMP GetWindow(HWND* phwnd);
    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);

    // IOleInPlaceUIWindow methods
    STDMETHODIMP GetBorder(LPRECT prcBorder);
    STDMETHODIMP RequestBorderSpace(LPCBORDERWIDTHS pbw);
    STDMETHODIMP SetBorderSpace(LPCBORDERWIDTHS pbw);
    STDMETHODIMP SetActiveObject(LPOLEINPLACEACTIVEOBJECT pipaobj, LPCOLESTR szObjName);

    // Document methods
public:
    HRESULT GetNewStorage(LPSTORAGE* ppstg);
    HRESULT DoVerb(LONG iVerb);
    BOOL InsertObject();
    BOOL PasteSpecial();
    BOOL ConvertObject();
    BOOL IsFileNameSet()
        { return m_fFileNameSet; }
    BOOL SetFileName(LPTSTR pszFileName);
    BOOL GetFileName(LPTSTR pszFileName, UINT cbFileName);

    // Implementation
private:
    BOOL CreateEditControl(HWND RichEdit);
};


class CRichEditOleCallback : public IRichEditOleCallback
{
public:
    UINT m_cRef;                        // Reference count
    CRichEditDoc* m_pDoc;               // Pointer to document object

    // construction/destruction
    CRichEditOleCallback(CRichEditDoc* pDoc)
        : m_cRef(0), m_pDoc(pDoc) {}
    virtual ~CRichEditOleCallback() {}
private:
    CRichEditOleCallback();

public:
    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppvObj);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    // IRichEditOleCallback methods
    STDMETHODIMP GetNewStorage(LPSTORAGE* ppstg);
    STDMETHODIMP GetInPlaceContext(LPOLEINPLACEFRAME* ppipframe,
	    						   LPOLEINPLACEUIWINDOW* ppipuiDoc,
		    					   LPOLEINPLACEFRAMEINFO pipfinfo);
    STDMETHODIMP ShowContainerUI(BOOL fShowContainerUI);
    STDMETHODIMP QueryInsertObject(LPCLSID pclsid,
                                   LPSTORAGE pstg,
                                   LONG cp);
    STDMETHODIMP DeleteObject(LPOLEOBJECT poleobj);
    STDMETHODIMP QueryAcceptData(LPDATAOBJECT pdataobj,
                                 CLIPFORMAT* pcfFormat,
                                 DWORD reco,
                                 BOOL fReally,
                                 HGLOBAL hMetaPict);
    STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
    STDMETHODIMP GetClipboardData(CHARRANGE* pchrg,
                                  DWORD reco,
                                  LPDATAOBJECT *ppdataobj);
    STDMETHODIMP GetDragDropEffect(BOOL fDrag,
                                   DWORD grfKeyState,
                                   LPDWORD pdwEffect);
    STDMETHODIMP GetContextMenu(WORD seltype,
                                LPOLEOBJECT poleobj,
                                CHARRANGE* pchrg,
                                HMENU* phmenu);
};

#define MAKEBOOL(i) ((BOOL) !!(i))                          // General
#define DIM(a) (sizeof(a)/sizeof(a[0]))
#define RECTWIDTH(pr)  ((pr)->right - (pr)->left)
#define RECTHEIGHT(pr) ((pr)->bottom - (pr)->top)
}