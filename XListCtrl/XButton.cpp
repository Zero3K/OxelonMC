// XButton.cpp

#include "stdafx.h"
#include "XButton.h"
#include "XThemeHelper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT WM_XBUTTON_CLICKED  = ::RegisterWindowMessage(_T("WM_XBUTTON_CLICKED"));

HTHEME CXButton::m_hTheme = NULL;

#define DFCS_CAPTIONCLOSE       0x0000
#define DFCS_CAPTIONMIN         0x0001
#define DFCS_CAPTIONMAX         0x0002
#define DFCS_CAPTIONRESTORE     0x0003
#define DFCS_CAPTIONHELP        0x0004

#define DFCS_MENUARROW          0x0000
#define DFCS_MENUCHECK          0x0001
#define DFCS_MENUBULLET         0x0002
#define DFCS_MENUARROWRIGHT     0x0004
#define DFCS_SCROLLUP           0x0000
#define DFCS_SCROLLDOWN         0x0001
#define DFCS_SCROLLLEFT         0x0002
#define DFCS_SCROLLRIGHT        0x0003
#define DFCS_SCROLLCOMBOBOX     0x0005
#define DFCS_SCROLLSIZEGRIP     0x0008
#define DFCS_SCROLLSIZEGRIPRIGHT 0x0010

#define DFCS_BUTTONCHECK        0x0000
#define DFCS_BUTTONRADIOIMAGE   0x0001
#define DFCS_BUTTONRADIOMASK    0x0002
#define DFCS_BUTTONRADIO        0x0004
#define DFCS_BUTTON3STATE       0x0008
#define DFCS_BUTTONPUSH         0x0010

#define DFCS_INACTIVE           0x0100
#define DFCS_PUSHED             0x0200
#define DFCS_CHECKED            0x0400

#define DFCS_TRANSPARENT        0x0800
#define DFCS_HOT                0x1000

#define DFCS_ADJUSTRECT         0x2000
#define DFCS_FLAT               0x4000
#define DFCS_MONO               0x8000


///////////////////////////////////////////////////////////////////////////////
// ctor
CXButton::CXButton(CWnd *pParent, /*int nItem, int nSubItem,*/ LPCTSTR lpszText, int nFixedWidth /*= 0*/) :
	CXControl(pParent),
	m_strText(lpszText),
	m_nFixedWidth(nFixedWidth)
{
	//XLISTCTRL_TRACE(_T("in CXButton::CXButton\n"));
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXButton::~CXButton()
{
	if (m_hTheme)
		ThemeHelper.CloseThemeData(m_hTheme);
	m_hTheme = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Draw
void CXButton::Draw(CDC *pDC, CRect& rect)
{
	//XLISTCTRL_TRACE(_T("in CXButton::Draw:  <%s>\n"), m_strText);
	//TRACERECT(rect);

	if (m_hTheme == NULL)
	{
		// open theme for Button

		if (ThemeHelper.IsThemeLibAvailable())
		{
			XLISTCTRL_TRACE(_T("opening theme data ~~~~~\n"));
			m_hTheme = ThemeHelper.OpenThemeData(m_pParent->m_hWnd, _T("Button"));
		}
	}

	CFont *pFont = m_pParent->GetFont();
	CFont *pOldFont = NULL;

	if (pFont)
		pOldFont = pDC->SelectObject(pFont);

	CRect rectButton(rect);

	m_rect.CopyRect(rectButton);

	//dc.SetBkMode(TRANSPARENT);
	pDC->SetBkMode(OPAQUE);

	DWORD part = BP_PUSHBUTTON;

	BOOL bIsThemed = IsThemed(m_hTheme);

	if (bIsThemed)
	{
		DWORD state = m_bPressed ? PBS_PRESSED : PBS_NORMAL;

		if (state == PBS_NORMAL)
		{
			if (!m_bEnabled)
				state = PBS_DISABLED;
			if (m_bMouseOver)
				state = PBS_HOT;
		}
		ThemeHelper.DrawThemeBackground(m_hTheme, pDC->m_hDC, part, state, &rectButton, NULL);
	}
	else
	{
		CBrush brBackground(GetSysColor(COLOR_BTNFACE));

		pDC->FillRect(&rectButton, &brBackground);

		// draw pressed button
		if (m_bPressed)
		{
			CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(&rectButton, &brBtnShadow);
		}
		else	// ...else draw non pressed button
		{
			UINT uState = DFCS_BUTTONPUSH |
				  (m_bMouseOver ? DFCS_HOT : 0) |
				  ((m_bPressed) ? DFCS_PUSHED : 0);

			pDC->DrawFrameControl(&rectButton, DFC_BUTTON, uState);
		}
	}

	BOOL bHasText = !m_strText.IsEmpty();

	UINT uTextAlignment = DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_WORDBREAK;

	// write the button title (if any)
	if (bHasText)
	{
		// draw the button's title -
		// if button is pressed then "press" title also
		if (m_bPressed && !bIsThemed)
			rectButton.OffsetRect(1, 1);

		if (bIsThemed)
		{
			ThemeHelper.DrawThemeText(m_hTheme, pDC->m_hDC, part, m_bEnabled ? PBS_NORMAL : PBS_DISABLED,
							m_strText, uTextAlignment, 0, &rectButton);
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);

			if (!m_bEnabled)
			{
				rectButton.OffsetRect(1, 1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
				pDC->DrawText(m_strText, -1, &rectButton, uTextAlignment);
				rectButton.OffsetRect(-1, -1);
				pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
				pDC->DrawText(m_strText, -1, &rectButton, uTextAlignment);
			}
			else
			{
				pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
				pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
				pDC->DrawText(m_strText, -1, &rectButton, uTextAlignment);
			}
		}
	}

	if (pOldFont)
		pDC->SelectObject(pOldFont);
}
