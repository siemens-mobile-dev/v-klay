#pragma once


// VRichEditCtrl

class VRichEditCtrl : public CRichEditCtrl
{
	DECLARE_DYNAMIC(VRichEditCtrl)

public:
	VRichEditCtrl();
	virtual ~VRichEditCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnCopy(WPARAM , LPARAM);
};


