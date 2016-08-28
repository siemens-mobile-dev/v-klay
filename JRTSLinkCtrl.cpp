/*|*\
|*|  File:      JRTSLinkCtrl.cpp
|*|  
|*|  By:        James R. Twine, JRTwine Software, LLC.
|*|             Copyright 2002, JRTwine Software, LLC.
|*|  Date:      Tuesday, May 2, 2002
|*|             
|*|  Notes:     This Is The Implementation Of A "Correctly Behaving" Link
|*|             Control a'la Internet Explorer.  It Correctly Acts Like
|*|             A Button And Can Be Focused And Activated Via Keyboard.
|*|             It Can Also Be Dragged Onto An Instance Of Internet
|*|             Explorer To Have The Link Opened In That Instance.
|*|             
|*|             May Be Freely Incorporated Into Projects Of Any Type
|*|             **EXCEPT** For "Ad-Ware", "Spy-Ware", Or "Mass-Emailing"
|*|				(Spamming) Applications, Subject To The Following Conditions:
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
#include	"stdafx.h"
#include	"JRTSLinkCtrl.h"
#include	<AFXOLE.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HCURSOR			CJRTSLinkCtrl::ms_hCursor = NULL;
const	UINT	CF_SHELLURL = ::RegisterClipboardFormat( CFSTR_SHELLURL );

/////////////////////////////////////////////////////////////////////////////
// CJRTSLinkCtrl

/**/	CJRTSLinkCtrl::CJRTSLinkCtrl()
{
	//
	//	Initialize Dfaults...
	//
	m_crBackground = ::GetSysColor( COLOR_BTNFACE );
	m_crDisabled = ::GetSysColor( COLOR_GRAYTEXT );
//	m_crActive = ::GetSysColor( RGB(0x00, 0x00, 0xFF) );
//	m_crVisited = ::GetSysColor( COLOR_BTNTEXT );
//	m_crHotTrack = ::GetSysColor( RGB(0x00, 0x00, 0xFF) );
	//m_crActive = ::GetSysColor( COLOR_HIGHLIGHT );
	//m_crVisited = ::GetSysColor( COLOR_HIGHLIGHT );
	//m_crHotTrack = ::GetSysColor( COLOR_HOTLIGHT );
	m_crActive=RGB(0x00, 0x00, 0xFF);
	m_crVisited=::GetSysColor( COLOR_HIGHLIGHT );
	m_crHotTrack=::GetSysColor( COLOR_HOTLIGHT );

	m_dwLastError = ERROR_SUCCESS;
	m_dwStyle = ( LCS_HOTTRACK | LCS_DRAGDROP | 
			LCS_TOOLTIPS );
	return;													// Done!
}


/**/	CJRTSLinkCtrl::~CJRTSLinkCtrl()
{
	return;													// Done!
}



BEGIN_MESSAGE_MAP(CJRTSLinkCtrl, CWnd)
	//{{AFX_MSG_MAP(CJRTSLinkCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_ENABLE()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )
	ON_MESSAGE( WM_MOUSEHOVER, OnMouseHover )
END_MESSAGE_MAP()


DWORD	CJRTSLinkCtrl::ModifyLCStyle( DWORD dwRemove, DWORD dwAdd )
{
	DWORD	dwOldStyle = m_dwStyle;							// Store Old Style For Later

	if( dwRemove )											// If Removing Some Styles
	{
		m_dwStyle &= ~( dwRemove & LCS_PUBLICMASK );		// Remove Specified Style(s)
	}
	if( dwAdd )												// If Adding Some Styles
	{
		m_dwStyle |= ( dwAdd & LCS_PUBLICMASK );			// Add Specified Style(s)
	}
	RedrawWindow();											// Redraw

	return( dwOldStyle );									// Done!
}


void	CJRTSLinkCtrl::SetTarget( LPCTSTR cpTarget )
{
	m_sTarget = cpTarget ? cpTarget : _T( "" );				// Set Target As Specified
	m_dwStyle |= LCS_I_USEURL;								// Add In Use URL Flag
	if( IsWindow( m_hWnd ) )								// If We Are Created
	{
		RedrawWindow();										// Trigger Redraw
	}
	return;													// Done!
}


void	CJRTSLinkCtrl::SetTarget( const SHELLEXECUTEINFO &seiSEI )
{
	m_seiSEI = seiSEI;										// Copy Structure
	m_dwStyle &= ~LCS_I_USEURL;								// Remove Use URL Flag
	if( IsWindow( m_hWnd ) )								// If We Are Created
	{
		RedrawWindow();										// Trigger Redraw
	}
	return;													// Done!
}


void	CJRTSLinkCtrl::SetDisplay( LPCTSTR cpDisplay )
{
	m_sDisplay = cpDisplay ? cpDisplay : _T( "" );			// Set Target As Specified
	if( IsWindow( m_hWnd ) )								// If We Are Created
	{
		RedrawWindow();										// Trigger Redraw
	}
	return;													// Done!
}


void	CJRTSLinkCtrl::SetNormalFont( const LOGFONT &lfNormal )
{
	if( (HFONT)m_fFont )									// If Already Created
	{
		m_fFont.DeleteObject();								// Free It
	}
	m_fFont.CreateFontIndirect( &lfNormal );				// Create "Normal" Font

	return;													// Done!
}


void	CJRTSLinkCtrl::SetULFont( const LOGFONT &lfUL )
{
	if( (HFONT)m_fULFont )									// If Already Created
	{
		m_fULFont.DeleteObject();							// Free It
	}
	m_fULFont.CreateFontIndirect( &lfUL );					// Create "UL" Font

	return;													// Done!
}


void	CJRTSLinkCtrl::Initialize( void )
{
	RECT	rClient;

	//
	//	If You Get An Error About "OCR_HAND", You Did Not Read
	//	The Documentation... 
	//
	ms_hCursor = (HCURSOR)::LoadImage( 0, 
#if(WINVER >= 0x0500)
			MAKEINTRESOURCE( OCR_HAND ), IMAGE_CURSOR, 
#else
			MAKEINTRESOURCE( OCR_NORMAL ), IMAGE_CURSOR, 
#endif 
			0, 0, ( LR_DEFAULTSIZE | LR_SHARED ) );			// Try To Load System Hand Cursor

	GetClientRect( &rClient );								// Get Client Rect
	m_ttToolTips.Create( this );							// Create The TT Control
	m_ttToolTips.SetDelayTime(TTDT_AUTOPOP, 32000);
	CRect r;
	if(SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0))
		m_ttToolTips.SetMaxTipWidth(r.Size().cx-10);

	m_ttToolTips.AddTool( this, m_sToolTip.GetLength() ? 
		m_sToolTip : m_sTarget.GetLength() ? m_sTarget : 
		m_sDisplay, &rClient, 1 );							// Add Default Tool
	m_ttToolTips.Activate( TRUE );							// Activate TT

	return;													// Done!
}


void	CJRTSLinkCtrl::SetToolTip( LPCTSTR cpToolTip )
{
	m_sToolTip = cpToolTip ? cpToolTip : _T( "" );			// Store Tooltip Text
	if( IsWindow( m_hWnd ) )								// If We Are Created
	{
		m_ttToolTips.UpdateTipText( m_sToolTip, this, 1 );	// Update ToolTip's Tip Text
	}
	return;													// Done!
}


DWORD	CJRTSLinkCtrl::BuildFlags( void )
{
	DWORD	dwFlags = 0;

	//
	//	Build Flags...  (Maybe Using ::GetKeyboardState Would Be Faster...?)
	//
	dwFlags |= ( ::GetAsyncKeyState( VK_LCONTROL ) & 0x8000 ) ? LCF_LCTRL : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_RCONTROL ) & 0x8000 ) ? LCF_RCTRL : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_LSHIFT ) & 0x8000 ) ? LCF_LSHIFT : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_RSHIFT ) & 0x8000 ) ? LCF_RSHIFT : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_LMENU ) & 0x8000 ) ? LCF_LALT : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_RMENU ) & 0x8000 ) ? LCF_RALT : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) ? LCF_CTRL : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) ? LCF_SHIFT : 0;
	dwFlags |= ( ::GetAsyncKeyState( VK_MENU ) & 0x8000 ) ? LCF_ALT : 0;

	return( dwFlags );										// Return Flags
}


DWORD	CJRTSLinkCtrl::SendNotify( UINT uiCode, DWORD dwRetVal )
{
	NMLINKCTRL	nmlcNMLC;
	const MSG	*pMsg = GetCurrentMessage();
	CWnd		*pParent = GetParent();
	
	::memset( &nmlcNMLC, 0, sizeof( NMLINKCTRL ) );			// Init Structore
	if( pParent )											// If Parent Window Available
	{
		UINT	uiCtrlID = GetDlgCtrlID();		

		_ASSERTE( pMsg );									// (In)Sanity Check
		nmlcNMLC.m_hdr.code = uiCode;						// Set Code As Required
		nmlcNMLC.m_hdr.hwndFrom = m_hWnd;					// Set Originating HWND
		nmlcNMLC.m_hdr.idFrom = uiCtrlID;					// Set Originating ID
		nmlcNMLC.m_dwRetVal = dwRetVal;						// Set Default Return Value As Required
		nmlcNMLC.m_dwFlags = BuildFlags();					// Set Flags
		nmlcNMLC.m_ptWhere = pMsg -> pt;					// Set Cursor Location

		pParent -> SendMessage( WM_NOTIFY, (WPARAM)uiCtrlID,
				(LPARAM)&nmlcNMLC );						// Send Notification Message
	}
	return( nmlcNMLC.m_dwRetVal );							// Return RetVal
}



void	CJRTSLinkCtrl::Activate( void )
{
	if( !SendNotify( NMLC_ACTIVATE, TRUE ) )				// Send Notification Message
	{														// If Return Back With FALSE
		return;												// Do Not Activate
	}
	m_dwStyle |= LCS_VISITED;								// Set Visited Style (Success Or Failure, We Activated The Link)

	if( m_dwStyle & LCS_I_USEURL )							// If Using The URL
	{														// (We Build The SEI Structire)
		::memset( &m_seiSEI, 0, sizeof( 
				SHELLEXECUTEINFO ) );						// Init The Structure
		m_seiSEI.cbSize = sizeof( SHELLEXECUTEINFO );		// Set Size
		m_seiSEI.hwnd = m_hWnd;								// Set HWND
		m_seiSEI.lpVerb = _T( "open" );						// Set Verb
		m_seiSEI.lpFile = m_sTarget.GetLength() ? 
				m_sTarget : m_sDisplay;						// Set Target To Open
		m_seiSEI.nShow = SW_SHOWNORMAL;						// Show Normal
	}
	//
	//	If LCS_I_USEURL Not Set, The Developer Has Setup
	//	The SEI Structure Him/HerSelf...
	//
	if( ( !::ShellExecuteEx( &m_seiSEI ) ) || 				// Try To Activate The Link
			( __int64( m_seiSEI.hInstApp ) <= 32 ) )				
	{														// If Failed
		m_dwLastError = ::GetLastError();					// Store Last Error Code
	}
	else													// If Successful
	{
		m_dwLastError = ERROR_SUCCESS;						// Set Success Code
	}
	return;													// Done!
}


/////////////////////////////////////////////////////////////////////////////
// CJRTSLinkCtrl message handlers

LRESULT	CJRTSLinkCtrl::OnMouseLeave( WPARAM, LPARAM )
{
	TRACKMOUSEEVENT	tmeTME;

	tmeTME.cbSize = sizeof( TRACKMOUSEEVENT );				// Init Size
	tmeTME.dwFlags = TME_CANCEL;							// Init Flags
	tmeTME.hwndTrack = m_hWnd;								// Init Tracking Window
	tmeTME.dwHoverTime = 0;									// Init Hover Time

	_TrackMouseEvent( &tmeTME );							// Cancel Tracking
	m_dwStyle &= ~LCS_I_TRACKING;							// Unset Tracking Flag
	SendNotify( NMLC_MOUSELEAVE, 0 );						// Send Notification Message
	RedrawWindow();											// Redraw Entire Control

	return( 0 );											// Done!
}


LRESULT	CJRTSLinkCtrl::OnMouseHover( WPARAM, LPARAM )
{
	SendNotify( NMLC_MOUSEHOVER, 0 );						// Send Notification Message

	return( 0 );											// Done!
}


void	CJRTSLinkCtrl::OnPaint() 
{
	CPaintDC	dcDC( this ); // device context for painting
	CRect		rClient;
	bool		bIsOn = false;
	int			iDCState = dcDC.SaveDC();					// Save DC State;
	int			iSpaceX = ( ::GetSystemMetrics( SM_CXEDGE ) );
	int			iSpaceY = ( ::GetSystemMetrics( SM_CYEDGE ) );

	if( !(HFONT)m_fFont )									// If "Normal" Font Not Yet Created
	{
		LOGFONT	lfFont;

		GetFont() -> GetLogFont( &lfFont );					// Get Normal Font
		m_fFont.CreateFontIndirect( &lfFont );				// Create "Normal" Font
	}
	if( !(HFONT)m_fULFont )									// If "UL" Font Not Yet Created
	{
		LOGFONT	lfFont;

		GetFont() -> GetLogFont( &lfFont );					// Get Normal Font
		lfFont.lfUnderline = TRUE;							// Set UL Flag
		m_fULFont.CreateFontIndirect( &lfFont );			// Create "UL" Font
	}
	GetClientRect( rClient );								// Get Client Area
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		dcDC.SetTextColor( m_crDisabled );					// Set Text To Disabled Color
	}
	else													// If Not Disabled		
	{
		if( m_dwStyle & LCS_VISITED )						// If Visited
		{
			dcDC.SetTextColor( m_crVisited );				// Set "Visited" Color
		}
		else												// If Not Yet "Visited"
		{
			dcDC.SetTextColor( m_crActive );				// Set "Normal" Color
		}
	}
	if( ( m_dwStyle & LCS_HOTTRACK ) && ( m_dwStyle & 		// If HotTracking, And Currently 
			LCS_I_TRACKING ) )								// Tracking The Mouse
	{														
		POINT	ptWhere = GetCurrentMessage() -> pt;		// Store Mouse Location At Time Of Message
		
		ScreenToClient( &ptWhere );							// Convert Location
		if( rClient.PtInRect( ptWhere ) )					// If In Our DRAWING Area
		{
			dcDC.SelectObject( m_fULFont );					// Select In "UL" Font
			dcDC.SetTextColor( m_crHotTrack );				// Set HotTrack Color
			bIsOn = true;									// Set "On" State
		}
		else												// If Not
		{
			dcDC.SelectObject( m_fFont );					// Select In "Normal" Font
		}
	}
	else													// If Not
	{
		dcDC.SelectObject( m_fULFont );						// Select In UL Font
	}
	SIZE	szExtent = dcDC.GetTextExtent( m_sDisplay );	// Get Text Extent (Drawn Size Area)

	m_rLinkArea.left = iSpaceX;								// Adjust Left Of Drawing Rect
	m_rLinkArea.top = iSpaceY;								// Adjust Top Of Drawing Rect
	m_rLinkArea.bottom = ( szExtent.cy + ( iSpaceX * 2 ) );	// Adjust Bottom Of Drawing Rect
	m_rLinkArea.right = ( szExtent.cx + ( iSpaceY * 2 ) );	// Adjust Right Of Drawing Rect
	m_rLinkArea.InflateRect( iSpaceX / 2 , iSpaceY / 2 );	// Inflate For Focus Rectangle

	dcDC.SetBkColor( m_crBackground );						// Set Background Color
	if(rClient.bottom-rClient.top <= 15)
		iSpaceY=0;
	dcDC.ExtTextOut( iSpaceX, iSpaceY, ETO_OPAQUE, &rClient, 
			m_sDisplay, NULL );								// Draw Control's Text
	if(iSpaceY!=0 && GetFocus() == this )					// If We Are Focused
	{
		dcDC.DrawFocusRect( &m_rLinkArea );					// Draw The Focus Rectangle
	}
	dcDC.RestoreDC( iDCState );								// Restore DC State
	
	ValidateRect( &rClient );								// Mark Drawing Area As Valid

	return;													// Done!
}


BOOL	CJRTSLinkCtrl::PreTranslateMessage( MSG *pMsg ) 
{
	if( !( GetStyle() & WS_DISABLED ) )						// If Control Is NOT Disabled
	{
		m_ttToolTips.Activate( ( m_dwStyle & 
				LCS_TOOLTIPS ) ? TRUE : FALSE );			// Enable/Disable ToolTip As Required
		m_ttToolTips.RelayEvent( pMsg );					// Relay Message
	}
	return( CWnd::PreTranslateMessage( pMsg ) );			// Do Default
}


void CJRTSLinkCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( m_rLinkArea.PtInRect( point ) )						// If Point Inside Of Link Proper
	{
		m_dwStyle |= LCS_I_PREDRAG;							// Turn On PreDrag Flag
		m_ptPreDrag = point;								// Store Drag Origin
	}
	else													// If Not
	{
		m_dwStyle &= ~LCS_I_PREDRAG;						// Turn Off PreDrag Flag
	}
	m_dwStyle |= LCS_I_MDOWNONCLIENT;						// Set Mouse-Down-In-Client-Area Flag

	SetFocus();												// Focus The Control
	SetCapture();											// Capture Mouse Mesages

	return;													// Done!
}


void CJRTSLinkCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	CRect	rClient;

	GetClientRect( &rClient );								// Get Client Rect
	if( ( m_dwStyle & LCS_I_MDOWNONCLIENT ) &&				// If Over Client Area (Captured)
			( rClient.PtInRect( point ) ) )					// And Mouse Is Still Over Client Area
	{
		Activate();											// Activate The Link
	}
	m_dwStyle &= ~LCS_I_PREDRAG;							// Turn Off PreDrag Flag
	m_dwStyle &= ~LCS_I_TRACKING;							// Turn Off Tracking Flag
	m_dwStyle &= ~LCS_I_MDOWNONCLIENT;						// Turn Off MDownOnClient Flag

	ReleaseCapture();										// Release Capture

	return;													// Done!
}


void CJRTSLinkCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( GetFocus() == this )
	{
		if( nChar == VK_RETURN || nChar == VK_SPACE )		// If <RETURN> Hit
		{
			Activate();										// Trigger Activation Function
		}
	}
	return;													// Done!
}


void CJRTSLinkCtrl::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	Initialize();											// Initialize The Control

	return;													// Done!
}


BOOL	CJRTSLinkCtrl::Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
				DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, 
				CCreateContext* pContext ) 
{
	BOOL	bStatus = CWnd::Create( lpszClassName, 
					lpszWindowName, dwStyle, rect, 
					pParentWnd, nID, pContext );			// Do Default

	Initialize();											// Initialize The Control

	return( bStatus );
}


void CJRTSLinkCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( !( m_dwStyle & LCS_I_TRACKING ) )					// If Not Currently Tracking
	{
		TRACKMOUSEEVENT	tmeTME;

		tmeTME.cbSize = sizeof( TRACKMOUSEEVENT );			// Init Size
		tmeTME.dwFlags = ( TME_LEAVE | TME_HOVER );			// Init Flags
		tmeTME.hwndTrack = m_hWnd;							// Init Tracking Window
		tmeTME.dwHoverTime = HOVER_DEFAULT;					// Set Hover Time To System Default

		_TrackMouseEvent( &tmeTME );						// Start Tracking
		m_dwStyle |= LCS_I_TRACKING;						// Set Tracking Flag
		SendNotify( NMLC_MOUSEENTER, 0 );					// Send Notification Message
	}
	if( m_dwStyle & LCS_I_TRACKING )						// If Tracking
	{
		RedrawWindow( NULL, NULL, ( RDW_INVALIDATE | 
				RDW_UPDATENOW ) );							// Trigger A Non-Erasing Redraw
	}
	if( m_dwStyle & LCS_I_PREDRAG )							// If In Pre-Drag Mode
	{
		int	iDragX = ::GetSystemMetrics( SM_CXDRAG );		// X-Delta To Begin A Drag Operation
		int	iDragY = ::GetSystemMetrics( SM_CYDRAG );		// Y-Delta To Begin A Drag Operation
		int	iXDiff = ::abs( ( m_ptPreDrag.x - point.x ) );	// X Delta Movement
		int	iYDiff = ::abs( ( m_ptPreDrag.y - point.y ) );	// Y Delta Movement

		if( ( iXDiff >= iDragX ) || ( iYDiff >= iDragY ) )	// If We Moved Far Enough For A Drag To Begin
		{
			BOOL	bContinue = FALSE;

			if( m_dwStyle & LCS_I_RDRAG )					// If A RDrag
			{
				bContinue = SendNotify( NMLC_BEGINRDRAG, 
						TRUE );								// Send RDrag Notification Message
			}
			else											// If A Normal (Left) Drag
			{
				bContinue = SendNotify( NMLC_BEGINDRAG, 
						TRUE );								// Send Drab Notification Message
			}
			if( !bContinue )								// If Returned With FALSE
			{												
				m_dwStyle &= ~LCS_I_PREDRAG;				// Turn Off PreDrag Flag
				m_dwStyle &= ~LCS_I_RDRAG;					// Turn Off RDrag Style
				return;										// Cancel Drag Operation
			}
			COleDataSource	odsData;

			if( m_dwStyle & LCS_I_RDRAG )					// If A RDrag
			{
				SendNotify( NMLC_ENDRDRAG, 
						odsData.DoDragDrop(
						( DROPEFFECT_COPY | 
						DROPEFFECT_MOVE | DROPEFFECT_LINK | // Execute The D&D, 
						DROPEFFECT_SCROLL ) ) );			// Notify Parent Of Results
			}
			else											// If A Normal (Left) Drag
			{
				HGLOBAL	hGlobal = ::GlobalAlloc( 
								( GMEM_SHARE | 
								GMEM_ZEROINIT ), 4096 + 1);// Allocate Memory For Target
				LPSTR	cpURL = (LPSTR)::GlobalLock( 
								hGlobal );
				
				if( !cpURL )								// If Allocation Failed
				{
					MessageBeep( MB_ICONEXCLAMATION );		// Beep At User
					m_dwStyle &= ~LCS_I_PREDRAG;			// Turn Off PreDrag Flag
					m_dwStyle &= ~LCS_I_TRACKING;			// Turn Off Tracking Flag
					m_dwStyle &= ~LCS_I_MDOWNONCLIENT;		// Turn Off MDownOnClient Flag
					m_dwStyle &= ~LCS_I_RDRAG;				// Turn Off RDrag Style
			
					return;									// Stop Here
				}
				ReleaseCapture();							// Release Mouse Capture
#ifdef UNICODE
				::wcstombs
#else
				::_tcsncpy
#endif
					( cpURL, m_sTarget.GetLength() ? 
						m_sTarget : m_sDisplay, 4096 );		// Copy Target Over
				cpURL[ 4096 ] = _T( '\0' );					// PTerminate Target
				::GlobalUnlock( hGlobal );					// Unlock GMemory
				odsData.CacheGlobalData( 
						CLIPFORMAT( CF_SHELLURL ), 
						hGlobal );							// Prepare ODS Object
				SendNotify( NMLC_ENDDRAG, 
						odsData.DoDragDrop( 				// Execute The D&D, 
						( DROPEFFECT_LINK | 
						DROPEFFECT_SCROLL ) ) );			// Notify Parent Of Results
			}
			m_dwStyle &= ~LCS_I_PREDRAG;					// Turn Off PreDrag Flag
			m_dwStyle &= ~LCS_I_TRACKING;					// Turn Off Tracking Flag
			m_dwStyle &= ~LCS_I_MDOWNONCLIENT;				// Turn Off MDownOnClient Flag
			m_dwStyle &= ~LCS_I_RDRAG;						// Turn Off RDrag Style
		}
	}
	return;													// Done!
}


void CJRTSLinkCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	if( m_dwStyle & LCS_I_TRACKING )						// If Tracking
	{
		TRACKMOUSEEVENT	tmeTME;

		ReleaseCapture();									// Release Mouse Capture
		tmeTME.cbSize = sizeof( TRACKMOUSEEVENT );
		tmeTME.dwFlags = TME_CANCEL;
		tmeTME.hwndTrack = m_hWnd;
		tmeTME.dwHoverTime = 0;

		_TrackMouseEvent( &tmeTME );						// Cancel Tracking
		m_dwStyle &= ~LCS_I_PREDRAG;						// Turn Off PreDrag Flag
		m_dwStyle &= ~LCS_I_TRACKING;						// Turn Off Tracking Flag
		m_dwStyle &= ~LCS_I_MDOWNONCLIENT;					// Turn Off MDownOnClient Flag

		RedrawWindow();										// Redraw Entire Control
	}
	SendNotify( NMLC_KILLFOCUS, 0 );						// Send Notification Message

	return;													// Done!
}


BOOL CJRTSLinkCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if( nHitTest == HTCLIENT )								// If in Our Client Area
	{
		if( ms_hCursor )									// If Cursor Obtained
		{
			::SetCursor( ms_hCursor );						// Set The Cursor
			return( TRUE );									// Done!
		}
	}
	return( FALSE );										// Did Not Set The Cursor
}


BOOL CJRTSLinkCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return( FALSE );										// All Erasing Handled In Painting...
}


void CJRTSLinkCtrl::OnEnable(BOOL bEnable) 
{
	if( bEnable )											// If We Are Enabling
	{
		ModifyStyle( WS_DISABLED, 0 );						// Remove Disabled Style
	}
	else													// If Disabling
	{
		ModifyStyle( 0, WS_DISABLED );						// Add Disabled Style
	}
	RedrawWindow();											// Redraw Control

	return;													// Done!
}


void CJRTSLinkCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	SendNotify( NMLC_SETFOCUS, 0 );							// Send Notification Message
	
	return;													// Done!
}


void CJRTSLinkCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	SendNotify( NMLC_DBLCLICK, 0 );							// Send Notification Message

	return;													// Done!
}


void CJRTSLinkCtrl::OnMButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	SendNotify( NMLC_MDBLCLICK, 0 );						// Send Notification Message
	m_dwStyle |= LCS_I_DBLCLICK;							// Add Double-Click Style
	
	return;													// Done!
}


void CJRTSLinkCtrl::OnMButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();												// Focus The Control
	SetCapture();											// Capture Mouse Mesages

	return;													// Done!
}


void CJRTSLinkCtrl::OnMButtonUp(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( m_dwStyle & LCS_I_DBLCLICK )						// If Just Sent A Double-Click
	{
		m_dwStyle &= ~LCS_I_DBLCLICK;						// Remove Double-Click Style
	}
	else													// If Not
	{
		SendNotify( NMLC_MCLICK, 0 );						// Send Notification Message
	}
	ReleaseCapture();										// Release Capture

	return;													// Done!
}


void CJRTSLinkCtrl::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	SendNotify( NMLC_RDBLCLICK, 0 );						// Send Notification Message
	m_dwStyle |= LCS_I_DBLCLICK;							// Add Double-Click Style

	ReleaseCapture();										// Release Capture

	return;													// Done!
}


void CJRTSLinkCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( m_rLinkArea.PtInRect( point ) )						// If Point Inside Of Link Proper
	{
		m_dwStyle |= LCS_I_PREDRAG;							// Turn On PreDrag Flag
		m_dwStyle |= LCS_I_RDRAG;							// Turn On RDrag Flag

		m_ptPreDrag = point;								// Store Drag Origin
	}
	else													// If Not
	{
		m_dwStyle &= ~LCS_I_PREDRAG;						// Turn Off PreDrag Flag
		m_dwStyle &= ~LCS_I_RDRAG;							// Turn Off RDrag Flag
	}
	m_dwStyle |= LCS_I_MDOWNONCLIENT;						// Set Mouse-Down-In-Client-Area Flag

	SetFocus();												// Focus The Control
	SetCapture();											// Capture Mouse Mesages

	return;													// Done!
}


void CJRTSLinkCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if( GetStyle() & WS_DISABLED )							// If Control Is Disabled
	{
		return;												// Stop Here
	}
	if( m_dwStyle & LCS_I_DBLCLICK )						// If Just Sent A Double-Click
	{
		m_dwStyle &= ~LCS_I_DBLCLICK;						// Remove Double-Click Style
	}
	else													// If Not
	{
		SendNotify( NMLC_RCLICK, 0 );						// Send Notification Message
	}
	ReleaseCapture();										// Release Capture
	
	return;													// Done!
}
