// XCheckbox.h
//

#ifndef XCHECKBOX_H
#define XCHECKBOX_H

#include "uxtheme.h"
#include "XControl.h"

extern UINT WM_XHEADERCTRL_CHECKBOX_CLICKED;

///////////////////////////////////////////////////////////////////////////////
// these definitions are used for checkboxes in both the header 
// and the list control
#define XLISTCTRL_NO_CHECKBOX			0
#define XLISTCTRL_UNCHECKED_CHECKBOX	1
#define XLISTCTRL_CHECKED_CHECKBOX		2

#define XLISTCTRL_CHECKBOX_SIZE	13

///////////////////////////////////////////////////////////////////////////////
// CXCheckbox 

class CXCheckbox : public CXControl
{
// Construction
public:
	CXCheckbox(CWnd *pParent);
	virtual ~CXCheckbox();

// Attributes
public:
	int		GetCheck() { return m_nCheckedState; }
	void	SetCheck(int nCheckedState);

// Operations
public:
	int Draw(CDC* pDC, CRect& rect);

// Implementation
protected:
	int				m_nCheckedState;
	static HTHEME	m_hTheme;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XCHECKBOX_H
