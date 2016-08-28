#if !defined(AFX_JRTSLINKCTRL_H__9B9DEAEA_96B8_4D54_8717_ACB9B992A4C1__INCLUDED_)
#define AFX_JRTSLINKCTRL_H__9B9DEAEA_96B8_4D54_8717_ACB9B992A4C1__INCLUDED_
/*|*\
|*|  File:      JRTSLinkCtrl.h
|*|  
|*|  By:        James R. Twine, JRTwine Software, LLC.
|*|             Copyright 2002, JRTwine Software, LLC.
|*|  Date:      Tuesday, May 2, 2002
|*|             
|*|  Notes:     This Is The Implementation Of A "Correctly Behaving" Link
|*|             Control a'la Internet Explorer.  It Correctly Acts Like
|*|             A Button And Can Be Focused And Activated Via The Keyboard.
|*|             It Can Also Be Dragged Onto An Instance Of Internet
|*|             Explorer To Have The Link Opened In That Instance.
|*|             
|*|             May Be Freely Incorporated Into Projects Of Any Type
|*|             **EXCEPT** For "Ad-Ware", "Spy-Ware", Or "Mass-Emailing"
|*|             (Spamming) Applications, Subject To The Following Conditions:
|*|             
|*|             o This Header Must Remain In This File, And Any
|*|               Files Derived From It
|*|             o Do Not Misrepresent The Origin Of This Code
|*|               (IOW, Do Not Claim You Wrote It)
|*|             
|*|             A "Mention In The Credits", Or Similar Acknowledgement,
|*|             Is *NOT* Required.  It Would Be Nice, Though! :)
|*|             
|*|             Tab-Size For This File Is "4".  If The Following 2 Boxes
|*|             Do Not Line Up Correctly, Your Tab Settings Are Not Correct
|*|				For This File, Or Someone Else Messed The File Up! :)
|*|             +---+---+
|*| 			|	|   |
|*|             +---+---+
\*|*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//  You Must Place The Following #define Before ANY #includes 
//  In Your STDAFX.H File...
//
//  #define     OEMRESOURCE
//

//
//  Link Control Notification Messages...
//
const   UINT    NMLC_DBLCLICK       =   NM_DBLCLK;          // Double-Click Notification Message
const   UINT    NMLC_SETFOCUS       =   NM_SETFOCUS;        // Control Has Gained The Input Focus
const   UINT    NMLC_KILLFOCUS      =   NM_KILLFOCUS;       // Control Has Lost The Input Focus
const   UINT    NMLC_RCLICK         =   NM_RCLICK;          // Right-Click In Control
const   UINT    NMLC_RDBLCLICK      =   NM_RDBLCLK;         // Double-Right-Click In Control
const   UINT    NMLC_ACTIVATE       =   ( NM_FIRST - 90 );  // Link Activate Notification Message (LClick/<Enter>)
const   UINT    NMLC_MOUSEENTER     =   ( NM_FIRST - 89 );  // MouseOver Link Notification
const   UINT    NMLC_MOUSEHOVER     =   ( NM_FIRST - 88 );  // Mouse Hover Notification
const   UINT    NMLC_MOUSELEAVE     =   ( NM_FIRST - 87 );  // Mouse Leave Notification
const   UINT    NMLC_BEGINDRAG      =   ( NM_FIRST - 86 );  // Begin Drag Operation Notification
const   UINT    NMLC_ENDDRAG        =   ( NM_FIRST - 85 );  // End Drag Operation Notification
const   UINT    NMLC_BEGINRDRAG     =   ( NM_FIRST - 84 );  // Begin RDrag Operation Notification
const   UINT    NMLC_ENDRDRAG       =   ( NM_FIRST - 83 );  // End RDrag Operation Notification
const   UINT    NMLC_MCLICK         =   ( NM_FIRST - 82 );  // MButton Click
const   UINT    NMLC_MDBLCLICK      =   ( NM_FIRST - 81 );  // MButton Double-Click


//
//  "Public" Link Control Styles...
//
const   DWORD   LCS_HOTTRACK        =   0x00000001;         // HotTracking Style
const   DWORD   LCS_DRAGDROP        =   0x00000002;         // Drag-N-Drop Style
const   DWORD   LCS_VISITED         =   0x00000004;         // Visited Style
const   DWORD   LCS_TOOLTIPS        =   0x00000010;         // ToolTips Style
const   DWORD   LCS_RDRAGDROP       =   0x00000020;         // RDrag-N-Drop Style
const   DWORD   LCS_PUBLICMASK      =   0x00FFFFFF;         // Public Style Bits Mask


//
//  "Private" (Internal) Link Control Styles...  (Masked Off From External Modification)
//
const   DWORD   LCS_I_USEURL        =   0x10000000;         // Use Target/Display As URL
const   DWORD   LCS_I_TRACKING      =   0x20000000;         // Currently Tracking The Mouse
const   DWORD   LCS_I_PREDRAG       =   0x40000000;         // In Pre-Drag State
const   DWORD   LCS_I_MDOWNONCLIENT =   0x80000000;         // Mouse Is Down In Client Area
const   DWORD   LCS_I_RDRAG         =   0x01000000;         // RDrag Flag
const   DWORD   LCS_I_DBLCLICK      =   0x02000000;         // Double-Click State Flag


//
//  Notification Structure Used With NMLC_* Notification Messages...
//
struct  NMLINKCTRL                                          // Link Control's Notification Object
{
    NMHDR   m_hdr;                                          // Base NMHDR Object
    DWORD   m_dwRetVal;                                     // Return Value
    DWORD   m_dwFlags;                                      // Message Flags
    POINT   m_ptWhere;                                      // Cursor Position (Screen)
};


//
//  Modifiers/Flags Used With Notification Messages...
//
const   DWORD   LCF_RCTRL           =   0x00000001;         // Right-CTRL Key Flag
const   DWORD   LCF_LCTRL           =   0x00000002;         // Left-CTRL Key Flag
const   DWORD   LCF_RSHIFT          =   0x00000004;         // Right-SHIFT Key Flag
const   DWORD   LCF_LSHIFT          =   0x00000008;         // Left-SHIFT Key Flag
const   DWORD   LCF_RALT            =   0x00000010;         // Right-ALT Key Flag
const   DWORD   LCF_LALT            =   0x00000020;         // Left-ALT Key Flag
const   DWORD   LCF_CTRL            =   0x00000040;         // The/A CTRL Key Flag
const   DWORD   LCF_SHIFT           =   0x00000080;         // The/A SHIFT Key Flag
const   DWORD   LCF_ALT             =   0x00000100;         // The/A ALT Key Flag


//
//  The Actual Control...
//
class CJRTSLinkCtrl : public CWnd
{
public:
    /**/    CJRTSLinkCtrl();								// Constructor

// Attributes
public:
    void    SetTarget( LPCTSTR cpTarget );                  // Set Target As A URL
    void    SetTarget( const SHELLEXECUTEINFO &seiSEI );    // Set Target As SEI Information
    void    SetDisplay( LPCTSTR cpDisplay );                // Set Display Text
    void    SetToolTip( LPCTSTR cpToolTip );                // Set ToolTip Text, If Any
    void    SetNormalFont( const LOGFONT &lfNormalFont );   // Set "Normal" Font
    void    SetULFont( const LOGFONT &lfULFont );           // Set "Underline"/"HotTracked" Font
    DWORD   GetLastActivateError( void );                   // Return Error Code (If Any) From Last Activate

// Operations
public:
    void    Activate( void );                               // Externally Activate The Link
    DWORD   ModifyLCStyle( DWORD dwRemove, DWORD dwAdd );   // Modify Control's Styles

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CJRTSLinkCtrl)
    public:
    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CJRTSLinkCtrl();

    // Generated message map functions
protected:
    //{{AFX_MSG(CJRTSLinkCtrl)
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LRESULT OnMouseLeave( WPARAM, LPARAM );
    afx_msg LRESULT OnMouseHover( WPARAM, LPARAM );

    DECLARE_MESSAGE_MAP()

    void    Initialize( void );                             // Initialize The Control
	DWORD	SendNotify( UINT uiCode, DWORD dwRetVal );		// Send Our Notification Messages
	DWORD	BuildFlags( void );								// Build And Return Flags Value

    SHELLEXECUTEINFO    m_seiSEI;                           // Used To Set/Build The "Activate" Action
    static  HCURSOR     ms_hCursor;     
    CToolTipCtrl        m_ttToolTips;
    COLORREF            m_crBackground;
    COLORREF            m_crActive;
    COLORREF            m_crDisabled;
    COLORREF            m_crVisited;
    COLORREF            m_crHotTrack;
    CString             m_sTarget;
    CString             m_sDisplay;
    CString             m_sToolTip;
    CPoint              m_ptPreDrag;
    DWORD               m_dwLastError;
    DWORD               m_dwStyle;
    CRect               m_rLinkArea;
    CFont               m_fFont;
    CFont               m_fULFont;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JRTSLINKCTRL_H__9B9DEAEA_96B8_4D54_8717_ACB9B992A4C1__INCLUDED_)
