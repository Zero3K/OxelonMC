// XCheckbox.cpp
//
// Implements the class CXCheckbox.  Note that this is a drawing class,
// not a window class.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XCheckbox.h"
#include "XThemeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT WM_XHEADERCTRL_CHECKBOX_CLICKED = ::RegisterWindowMessage(_T("WM_XHEADERCTRL_CHECKBOX_CLICKED"));

HTHEME CXCheckbox::m_hTheme = NULL;

///////////////////////////////////////////////////////////////////////////////
// ctor
CXCheckbox::CXCheckbox(CWnd *pParent) :
	CXControl(pParent),
	m_nCheckedState(XLISTCTRL_UNCHECKED_CHECKBOX)
{
	XLISTCTRL_TRACE(_T("in CXCheckbox::CXCheckbox +++++\n"));
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXCheckbox::~CXCheckbox()
{
	XLISTCTRL_TRACE(_T("in CXCheckbox::~CXCheckbox +++++\n"));
	if (m_hTheme)
		ThemeHelper.CloseThemeData(m_hTheme);
	m_hTheme = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// SetCheck
void CXCheckbox::SetCheck(int nCheckedState) 
{ 
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::SetCheck:  %d\n"), nCheckedState);
	m_nCheckedState = nCheckedState; 
}

///////////////////////////////////////////////////////////////////////////////
// Draw
//
// nState values are same as XLISTCTRL_NO_CHECKBOX, etc.:
//     nState = 0 : no checkbox
//              1 : unchecked checkbox
//              2 : checked checkbox
//
int CXCheckbox::Draw(CDC* pDC, CRect& rect)
{
	XLISTCTRL_TRACE(_T("in CXCheckbox::Draw\n"));

	int nWidth = 0;

	ASSERT(m_pParent && ::IsWindow(m_pParent->m_hWnd));

	if (m_pParent && ::IsWindow(m_pParent->m_hWnd))
	{
		if (m_nCheckedState != XLISTCTRL_NO_CHECKBOX)
		{
			m_rect.CopyRect(rect);

			if (m_rect.Width() > 0)
			{
				// center the checkbox

				int h = XLISTCTRL_CHECKBOX_SIZE;			// height and width are the same
				m_rect.right  = m_rect.left + h;
				m_rect.top    = m_rect.top + (m_rect.Height() - h) / 2;
				m_rect.bottom = m_rect.top + h;

				if (m_hTheme == NULL)
				{
					// open theme for checkbox

					if (ThemeHelper.IsThemeLibAvailable())
					{
						XLISTCTRL_TRACE(_T("opening theme data ~~~~~\n"));
						m_hTheme = ThemeHelper.OpenThemeData(m_pParent->m_hWnd, _T("Button"));
					}
				}

				BOOL bIsThemed = IsThemed(m_hTheme);

				if (bIsThemed)			// user can disable theming for app by 
										// right-clicking on exe, then clicking 
										// on Properties | Compatibility | Disable 
										// visual themes
				{
					//XLISTCTRL_TRACE(_T("THEMED ~~~~~\n"));


					DWORD part  = BP_CHECKBOX;

					DWORD state = (m_nCheckedState == XLISTCTRL_CHECKED_CHECKBOX) ? CBS_CHECKEDNORMAL : CBS_UNCHECKEDNORMAL;

					if (state == CBS_CHECKEDNORMAL)
					{
						if (!m_bEnabled)
							state = CBS_CHECKEDDISABLED;
						else if (m_bPressed)
							state = CBS_CHECKEDPRESSED;
						else if (m_bMouseOver)
							state = CBS_CHECKEDHOT;
					}
					else
					{
						if (!m_bEnabled)
							state = CBS_UNCHECKEDDISABLED;
						else if (m_bPressed)
							state = CBS_UNCHECKEDPRESSED;
						else if (m_bMouseOver)
							state = CBS_UNCHECKEDHOT;
					}

					ThemeHelper.DrawThemeBackground(m_hTheme, pDC->m_hDC, 
						part, state, &m_rect, NULL);
				}
				else
				{
					//XLISTCTRL_TRACE(_T("NOT THEMED ~~~~~\n"));

					// fill rect inside checkbox with white
					COLORREF oldBackground = pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
					pDC->FillSolidRect(&m_rect, ::GetSysColor(COLOR_WINDOW));

					// draw border
					CBrush brush(RGB(51,102,153));
					pDC->FrameRect(&m_rect, &brush);

					if (m_nCheckedState == XLISTCTRL_CHECKED_CHECKBOX)
					{
						CPen pen(PS_SOLID, 1, RGB(51,153,51));
						CPen * pOldPen = pDC->SelectObject(&pen);

						// draw the checkmark
						int x = m_rect.left + 9;
						ASSERT(x < m_rect.right);
						int y = m_rect.top + 3;
						int i = 0;
						for (i = 0; i < 4; i++)
						{
							pDC->MoveTo(x, y);
							pDC->LineTo(x, y+3);
							x--;
							y++;
						}
						for (i = 0; i < 3; i++)
						{
							pDC->MoveTo(x, y);
							pDC->LineTo(x, y+3);
							x--;
							y--;
						}

						if (pOldPen)
							pDC->SelectObject(pOldPen);
					}

					pDC->SetBkColor(oldBackground);
				}

				nWidth = m_rect.Width() + 4;		// same as list control
			}
			else
			{
				// width = 0, do nothing
			}
		}
	}

	return nWidth;
}
