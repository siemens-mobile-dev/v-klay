// VRichEditCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "V_KLay.h"
#include "VRichEditCtrl.h"


// VRichEditCtrl

IMPLEMENT_DYNAMIC(VRichEditCtrl, CRichEditCtrl)
VRichEditCtrl::VRichEditCtrl()
{
}

VRichEditCtrl::~VRichEditCtrl()
{
}


BEGIN_MESSAGE_MAP(VRichEditCtrl, CRichEditCtrl)
	ON_MESSAGE(WM_COPY, OnCopy)
END_MESSAGE_MAP()



// VRichEditCtrl message handlers

afx_msg LRESULT VRichEditCtrl::OnCopy(WPARAM , LPARAM)
{
	return 1;
}
