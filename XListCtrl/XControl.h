#ifndef XCONTROL_H
#define XCONTROL_H

#include "uxtheme.h"

/////////////////////////////////////////////////////////////////////////////
// CXControl

class CXControl
{
// Construction
public:
	CXControl(CWnd *pParent);
	virtual ~CXControl();

// Attributes
public:
	void EnableTheming(BOOL bEnable) 
	{ 
		m_bEnableTheming = bEnable; 
		if (m_bEnableTheming)
		{
			TRACE(_T("Theming will be enabled only on XP and only ")
				  _T("if the app is themed with a manifest file.\n"));
		}
	}
	BOOL Enable(BOOL bEnable) 
	{ 
		BOOL bOldState = m_bEnabled;
		m_bEnabled = bEnable;
		return bOldState; 
	}
	BOOL GetMouseOver() { return m_bMouseOver; }
	BOOL GetPressed() { return m_bPressed; }
	void GetRect(CRect& rect) { rect.CopyRect(m_rect); }
	BOOL IsThemed(HTHEME hTheme);
	void SetMouseOver(BOOL bMouseOver) { m_bMouseOver = bMouseOver; }
	void SetPressed(BOOL bPressed) { m_bPressed = bPressed; }

// Operations
public:

// Implementation
protected:
	CWnd *			m_pParent;
	BOOL			m_bMouseOver;
	BOOL			m_bEnableTheming;
	BOOL			m_bPressed;
	BOOL			m_bEnabled;				// TRUE = button is enabled
	CRect			m_rect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XCONTROL_H
