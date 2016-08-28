#pragma once


// CStaticColor

class CStaticColor : public CStatic
{
	DECLARE_DYNAMIC(CStaticColor)

public:
	CStaticColor();
	virtual ~CStaticColor();

protected:
	COLORREF m_Color;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetColor(COLORREF color);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


