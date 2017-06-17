// XButton.h
//

#ifndef  XBUTTON_H 
#define  XBUTTON_H 

#include "uxtheme.h"
#include "XControl.h"

extern UINT WM_XBUTTON_CLICKED;

/////////////////////////////////////////////////////////////////////////////
// CXButton 

class CXButton : public CXControl
{
// Construction
public:
	CXButton(CWnd *pParent, LPCTSTR lpszText, int nFixedWidth = 0);
	virtual ~CXButton();

// Attributes
public:
	int GetFixedWidth() { return m_nFixedWidth; }
	CString GetWindowText() { return m_strText; }
	void SetFixedWidth(int nFixedWidth) { m_nFixedWidth = nFixedWidth; }
	void SetWindowText(LPCTSTR lpszString) { m_strText = lpszString; }

// Operations
public:
	void Draw(CDC *pDC, CRect& rect);

// Implementation
protected:
	CString			m_strText;
	static HTHEME	m_hTheme;
	int				m_nFixedWidth;			// for fixed width
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // XBUTTON_H 
