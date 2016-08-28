// StaticColor.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "StaticColor.h"


// CStaticColor

IMPLEMENT_DYNAMIC(CStaticColor, CStatic)
CStaticColor::CStaticColor()
{
	m_Color=RGB(0,255,0);
}

CStaticColor::~CStaticColor()
{
}


BEGIN_MESSAGE_MAP(CStaticColor, CStatic)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CStaticColor message handlers


BOOL CStaticColor::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(r);
	pDC->FillSolidRect(r, m_Color);
	return TRUE;
}

void CStaticColor::SetColor(COLORREF color)
{
	m_Color=color;
	RedrawWindow();
}

void CStaticColor::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
}
