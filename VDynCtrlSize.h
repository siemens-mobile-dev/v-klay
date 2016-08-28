// VDynCtrlSize.h: interface for the VDynCtrlSize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VDYNCTRLSIZE_H__7B014AA8_989C_11D5_B686_93C0FDBA4911__INCLUDED_)
#define AFX_VDYNCTRLSIZE_H__7B014AA8_989C_11D5_B686_93C0FDBA4911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define DS_ELASTANCE_MAX	254
#define EL(percent) ((DWORD)((((double)(percent))*254.0/100.0)+0.5))

class VDynCtrlSizeInf
{
public:
	UINT m_ID;				//ИД элемента управления
	BYTE m_ElTop;			//степень привязки верхней границы элемента управления с низу диалога(0-254)
	BYTE m_ElLeft;			//степень привязки левой границы элемента управления с правой стороне диалога(0-254)
	BYTE m_ElBottom;		//степень привязки нижней границы элемента управления с низу диалога(0-254)
	BYTE m_ElRight;			//степень привязки правой границы элемента управления с правой стороне диалога(0-254)
	CRect m_OrigPos;		//первоначальное положение элемента управления
};

class VDynCtrlSize : public CArray<VDynCtrlSizeInf, VDynCtrlSizeInf&>  
{
public:
	BOOL RemoveControl(UINT ID);
	void OnSize(int newcx, int newcy);
	CWnd *m_pDlg;			//Указатель на окно диалога
	int AddControl(UINT ID, BYTE ElLeft, BYTE ElTop, BYTE ElRight, BYTE ElBottom);
	void AttachDialog(CWnd *pDialog);
	CSize m_OrigSize;		//Первоначальный размер диалога
	VDynCtrlSize();
	virtual ~VDynCtrlSize();

};

#endif // !defined(AFX_VDYNCTRLSIZE_H__7B014AA8_989C_11D5_B686_93C0FDBA4911__INCLUDED_)
