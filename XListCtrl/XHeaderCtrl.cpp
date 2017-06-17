// XHeaderCtrl.cpp  Version 1.5
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// This code is based on "Outlook 98-Style FlatHeader Control" 
// by Maarten Hoeben.
//
// See http://www.codeguru.com/listview/FlatHeader.shtml
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XHeaderCtrl.h"
#include "XThemeHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXHeaderCtrl

IMPLEMENT_DYNCREATE(CXHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CXHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CXHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_ERASEBKGND()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(HDM_SETIMAGELIST, OnSetImageList)
	ON_MESSAGE(HDM_LAYOUT, OnLayout)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

HTHEME CXHeaderCtrl::m_hThemeHeader = NULL;

///////////////////////////////////////////////////////////////////////////////
// ctor
CXHeaderCtrl::CXHeaderCtrl()
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::CXHeaderCtrl\n"));
	m_cr3DHighLight        = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow           = ::GetSysColor(COLOR_3DSHADOW);
	m_cr3DFace             = ::GetSysColor(COLOR_3DFACE);
	m_crBtnText            = ::GetSysColor(COLOR_BTNTEXT);

	m_pListCtrl            = NULL;
	m_nFormat              = DT_DEFAULT;
	m_crText               = m_crBtnText;
	m_bDividerLines        = TRUE;
	m_bStaticBorder        = FALSE;
	m_bAllowUserInput      = TRUE;
	m_bMouseInWindow       = FALSE;
	m_nSpacing             = 5;
	m_sizeArrow.cx         = 8;
	m_sizeArrow.cy         = 8;
	m_sizeImage.cx         = 0;
	m_sizeImage.cy         = 0;
	m_nDontDropCursor      = 0;
	m_nClickFlags          = 0;
	m_nCurrentHot          = -1;
	m_nCurrentCheckboxItem = -1;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXHeaderCtrl::~CXHeaderCtrl()
{
	if (m_hThemeHeader)
		ThemeHelper.CloseThemeData(m_hThemeHeader);
	m_hThemeHeader = NULL;

	for (int i = 0; i < m_ColumnData.GetSize(); i++)
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(i);
		if (p)
			delete p;
		m_ColumnData[i] = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// ModifyProperty
BOOL CXHeaderCtrl::ModifyProperty(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
		case FH_PROPERTY_SPACING:
			m_nSpacing = (int)lParam;
			break;

		case FH_PROPERTY_ARROW:
			m_sizeArrow.cx = LOWORD(lParam);
			m_sizeArrow.cy = HIWORD(lParam);
			break;

		case FH_PROPERTY_STATICBORDER:
			m_bStaticBorder = (BOOL)lParam;
			break;

		case FH_PROPERTY_DONTDROPCURSOR:
			m_nDontDropCursor = (UINT)lParam;
			break;

		default:
			return FALSE;
	}

	Invalidate();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// OnCustomDraw
void CXHeaderCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	//XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnCustomDraw =====\n"));

	if (m_hThemeHeader == NULL)
	{
		// open theme for header

		if (ThemeHelper.IsThemeLibAvailable())
		{
			XLISTCTRL_TRACE(_T("opening theme data ~~~~~\n"));
			m_hThemeHeader = ThemeHelper.OpenThemeData(m_hWnd, _T("Header"));
		}
	}

	// Get pointer to  NMCUSTOMDRAW structure that contains information about 
	// the drawing operation. The dwItemSpec member of this structure contains 
	// the index of the item being drawn and the lItemlParam member of this 
	// structure contains the item's lParam.
	NMCUSTOMDRAW* pCD = reinterpret_cast<NMCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if (pCD->dwDrawStage == CDDS_PREPAINT)
	{
		//XLISTCTRL_TRACE(_T("header CDDS_PREPAINT\n"));
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pCD->dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_ITEM))
	{
		// This is the notification message for an item.

		int nItem = static_cast<int> (pCD->dwItemSpec);
		//XLISTCTRL_TRACE(_T("header CDDS_ITEMPREPAINT | CDDS_ITEM - nItem %d\n"), nItem);

		int iItem = OrderToIndex(nItem);

		TCHAR szText[FLATHEADER_TEXT_MAX];

		HDITEM hditem;
		hditem.mask = HDI_WIDTH|HDI_FORMAT|HDI_TEXT|HDI_IMAGE|HDI_BITMAP|HDI_LPARAM;
		hditem.pszText = szText;
		hditem.cchTextMax = sizeof(szText)/sizeof(TCHAR);
		VERIFY(GetItem(iItem, &hditem));

		CDC* pDC = CDC::FromHandle(pCD->hdc);

		CRect rectItem;
		VERIFY(GetItemRect(iItem, rectItem));

		CRect rectClip;
		if (pDC->GetClipBox(&rectClip) != ERROR)
		{
			if (rectItem.right >= rectClip.left || rectItem.left <= rectClip.right)
			{
				DrawItem(iItem, pDC, m_crText, m_cr3DFace, rectItem, hditem);
			}
		}

		*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
	}
}

///////////////////////////////////////////////////////////////////////////////
// ColumnHitTest
int CXHeaderCtrl::ColumnHitTest(CPoint point)
{
	int nCol = -1;

	CRect rectHeader;
	GetClientRect(&rectHeader);

	if (rectHeader.PtInRect(point))
	{
		for (int i = 0; i < GetItemCount(); i++)
		{
			CRect rectItem;
			GetItemRect(i, &rectItem);
			if (rectItem.PtInRect(point))
			{
				nCol = i;
				break;
			}
		}
	}

	return nCol;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateItem
void CXHeaderCtrl::UpdateItem(int nItem)
{
	if ((nItem < 0) || nItem >= GetItemCount())
		return;

	CRect rect;
	GetItemRect(nItem, &rect);

	InvalidateRect(&rect);
	UpdateWindow();
}

///////////////////////////////////////////////////////////////////////////////
// ResetCurrentCheckbox
void CXHeaderCtrl::ResetCurrentCheckbox()
{
	if ((m_nCurrentCheckboxItem < 0) || (m_nCurrentCheckboxItem >= GetItemCount()))
		return;

	XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(m_nCurrentCheckboxItem);
	if (p && p->pCheckbox)
	{
		p->pCheckbox->SetMouseOver(FALSE);
		p->pCheckbox->SetPressed(FALSE);
	}
	UpdateItem(m_nCurrentCheckboxItem);

	m_nCurrentCheckboxItem = -1;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessMouseOverCheckbox
void CXHeaderCtrl::ProcessMouseOverCheckbox(int nItem, CPoint point)
{
	//XLISTCTRL_TRACE(_T("in CXHeaderCtrl::ProcessMouseOverCheckbox\n"));

	XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nItem);

	if ((p == NULL) || (p->pCheckbox == NULL))
	{
		// there is no checkbox
		if (m_nCurrentCheckboxItem != nItem)
		{
			ResetCurrentCheckbox();
		}
		m_nCurrentCheckboxItem = -1;
	}
	else
	{
		// save current state flags
		BOOL bMouseOver = p->pCheckbox->GetMouseOver();
		BOOL bIsPressed = p->pCheckbox->GetPressed();

		if (PtInCheckboxRect(nItem, point))
		{
			p->pCheckbox->SetMouseOver(TRUE);

			if (m_nCurrentCheckboxItem != nItem)
				ResetCurrentCheckbox();

			// set new active checkbox
			m_nCurrentCheckboxItem = nItem;
		}
		else
		{
			ResetCurrentCheckbox();
		}

		if ((bMouseOver != p->pCheckbox->GetMouseOver()) || 
			(bIsPressed != p->pCheckbox->GetPressed()))
		{
			// state has changed, repaint checkbox
			UpdateItem(nItem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// PtInCheckboxRect
BOOL CXHeaderCtrl::PtInCheckboxRect(int nItem, CPoint point)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;

	BOOL bRet = FALSE;

	XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nItem);
	if (p && p->pCheckbox)
	{
		CRect rectCheckbox(0,0,0,0);
		p->pCheckbox->GetRect(rectCheckbox);

		CRect rectItem;
		GetItemRect(nItem, &rectItem);

		int h = XLISTCTRL_CHECKBOX_SIZE;			// height and width are the same
		rectCheckbox.top = rectItem.top + (rectItem.Height() - h) / 2;
		rectCheckbox.bottom = rectCheckbox.top + h;

		bRet = rectCheckbox.PtInRect(point);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CXHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	int nCol = ColumnHitTest(point);

	if ((nCol >= 0))// && !m_bMouseInWindow)
	{
		m_bMouseInWindow = TRUE;

		XLISTCTRL_TRACE(_T("mouse over header +++++\n"));

		SetTimer(1, 100, NULL);
	}

	if (ThemeHelper.IsAppThemed() && m_hThemeHeader)
	{
		XLISTCTRL_TRACE(_T("OnMouseMove:  nCol=%d  m_nCurrentHot=%d ~~~~~\n"), nCol, m_nCurrentHot);

		int nPreviousHot = m_nCurrentHot;
		m_nCurrentHot = nCol;
		if (nPreviousHot != m_nCurrentHot)
			RedrawWindow();

		// is cursor over checkbox?
		if ((nCol == -1) || (nCol >= m_ColumnData.GetSize()))
		{
			ResetCurrentCheckbox();
		}
		else
		{
			ProcessMouseOverCheckbox(nCol, point);
		}
	}

	CHeaderCtrl::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseLeave
LRESULT CXHeaderCtrl::OnMouseLeave(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnMouseLeave +++++\n"));

	m_bMouseInWindow = FALSE;
	ResetCurrentCheckbox();
	m_nCurrentHot = -1;
	RedrawWindow();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CXHeaderCtrl::OnTimer(UINT nIDEvent)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	int nCol = ColumnHitTest(point);
	XLISTCTRL_TRACE(_T("OnTimer:  nCol=%d\n"), nCol);

	if (nCol == -1)
	{
		// mouse not over window
		KillTimer(nIDEvent);
		SendMessage(WM_MOUSELEAVE);
	}

	CHeaderCtrl::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// DrawItem
void CXHeaderCtrl::DrawItem(int nCol,
							CDC *pDC,
							COLORREF crText,
							COLORREF crBkgnd,
							CRect& rect,
							HDITEM& hditem)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::DrawItem:  nCol=%d  m_nCurrentHot=%d\n"), nCol, m_nCurrentHot);

	ASSERT(hditem.mask & HDI_FORMAT);

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rectClient;
	GetClientRect(&rectClient);

	DWORD state = HIS_NORMAL;

	if (ThemeHelper.IsAppThemed() && m_hThemeHeader)	// user can disable theming
														// for app by right-clicking
														// on exe, then clicking on
														// Properties | Compatibility |
														// Disable visual themes
	{
		//TRACE("THEMED ~~~~~\n");
		DWORD part  = HP_HEADERITEM;

		if (rect.PtInRect(point) && rectClient.PtInRect(point))
		{
			state = HIS_HOT;
			m_nCurrentHot = nCol;
		}
		else
		{
			// cursor not over this item -
			// reset if marked as hot
			if (m_nCurrentHot == nCol)
			{
				m_nCurrentHot = -1;
			}
		}
		ThemeHelper.DrawThemeBackground(m_hThemeHeader, pDC->m_hDC, part, state, &rect, NULL);

		rect.bottom -= 3;	// make room for highlight on bottom of header tab
	}
	else
	{
		//TRACE("NOT THEMED ~~~~~\n");
		CBrush brBackground(crBkgnd);
		pDC->FillRect(&rect, &brBackground);
	}

	int nWidth = 0;

	CBitmap* pBitmap = NULL;
	BITMAP BitmapInfo;

	if (hditem.fmt & HDF_BITMAP)
	{
		ASSERT(hditem.mask & HDI_BITMAP);
		ASSERT(hditem.hbm);

		pBitmap = CBitmap::FromHandle(hditem.hbm);
		if (pBitmap)
			VERIFY(pBitmap->GetObject(sizeof(BITMAP), &BitmapInfo));
	}

	rect.left += m_nSpacing;

	if (nCol < m_ColumnData.GetSize())
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p && p->pCheckbox)
			rect.left += p->pCheckbox->Draw(pDC, rect);
	}

	rect.right -= ((nWidth = DrawBitmap(pDC, rect, hditem, pBitmap, &BitmapInfo, TRUE)) != 0) ? 
		nWidth : 0;

	DrawHeaderText(nCol, pDC, crText, crBkgnd, rect, hditem);
}

///////////////////////////////////////////////////////////////////////////////
// DrawBitmap
int CXHeaderCtrl::DrawBitmap(CDC* pDC, 
								CRect rect, 
								HDITEM hditem, 
								CBitmap* pBitmap, 
								BITMAP* pBitmapInfo, 
								BOOL bRight)
{
	UNUSED_ALWAYS(hditem);

	int nWidth = 0;

	if (pBitmap)
	{
		nWidth = pBitmapInfo->bmWidth;
		if ((nWidth <= rect.Width()) && (rect.Width() > 0))
		{
			POINT point;

			point.y = rect.CenterPoint().y - (pBitmapInfo->bmHeight >> 1);

			if (bRight)
				point.x = rect.right - nWidth;
			else
				point.x = rect.left;

			CDC dc;
			if (dc.CreateCompatibleDC(pDC) == TRUE) 
			{
				CBitmap * pOldBitmap = dc.SelectObject(pBitmap);
				nWidth = pDC->BitBlt(
					point.x, point.y, 
					pBitmapInfo->bmWidth, pBitmapInfo->bmHeight, 
					&dc, 
					0, 0, 
					SRCCOPY
				) ? nWidth:0;
				dc.SelectObject(pOldBitmap);
			}
			else 
				nWidth = 0;
		}
		else
			nWidth = 0;
	}

	return nWidth;
}

///////////////////////////////////////////////////////////////////////////////
// DrawHeaderText
int CXHeaderCtrl::DrawHeaderText(int nCol,
								 CDC* pDC, 
								 COLORREF crText,
								 COLORREF crBkgnd, 
								 CRect rect, 
								 HDITEM hditem)
{
	//XLISTCTRL_TRACE(_T("in CXHeaderCtrl::DrawHeaderText: crText=0x%X\n"), crText);

	CSize size = pDC->GetTextExtent(hditem.pszText);

	if ((rect.Width() > m_nSpacing) && (hditem.mask & HDI_TEXT) && (hditem.fmt & HDF_STRING))
	{
		rect.right -= m_nSpacing;

		UINT nFormat = m_nFormat;

		if (nFormat == DT_DEFAULT)
		{
			// default to whatever alignment the column is set to

			if (hditem.fmt & LVCFMT_CENTER)
				nFormat = DT_CENTER;
			else if (hditem.fmt & LVCFMT_RIGHT)
				nFormat = DT_RIGHT;
			else
				nFormat = DT_LEFT;
		}

		nFormat |= DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;

		COLORREF textcolor = crText;		// assume we are using global text color
		COLORREF tc = (DWORD)-1;

		if (nCol < m_ColumnData.GetSize())
		{
			XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
			if (p)
				tc = p->crText;				// get text color for this column
		}

		if (tc != (DWORD)-1)				// use it if it was supplied
			textcolor = tc;

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(textcolor);
		pDC->SetBkColor(crBkgnd);
		pDC->DrawText(hditem.pszText, -1, rect, nFormat);
	}

	size.cx = __max(rect.Width(), size.cx);

	return (size.cx > 0) ? size.cx : 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnSetImageList
LRESULT CXHeaderCtrl::OnSetImageList(WPARAM, LPARAM lParam)
{
	CImageList* pImageList;
	pImageList = CImageList::FromHandle((HIMAGELIST)lParam);

	IMAGEINFO info;
	if (pImageList->GetImageInfo(0, &info))
	{
		m_sizeImage.cx = info.rcImage.right - info.rcImage.left;
		m_sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
	}

	return Default();
}

///////////////////////////////////////////////////////////////////////////////
// OnLayout
LRESULT CXHeaderCtrl::OnLayout(WPARAM, LPARAM lParam)
{
	LPHDLAYOUT lphdlayout = (LPHDLAYOUT)lParam;

	if (m_bStaticBorder)
		lphdlayout->prc->right += GetSystemMetrics(SM_CXBORDER)*2;

	return CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// OnSysColorChange
void CXHeaderCtrl::OnSysColorChange() 
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnSysColorChange\n"));

	CHeaderCtrl::OnSysColorChange();
	
	m_cr3DHighLight = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow    = ::GetSysColor(COLOR_3DSHADOW);
	m_cr3DFace      = ::GetSysColor(COLOR_3DFACE);
	m_crBtnText     = ::GetSysColor(COLOR_BTNTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CXHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetCheck
int CXHeaderCtrl::GetCheck(int nCol)
{
	int rc = XLISTCTRL_NO_CHECKBOX;

	ASSERT(nCol >= 0);
	ASSERT(nCol < GetItemCount());
	if ((nCol < 0) || (nCol >= GetItemCount()))
		return rc;

	if (nCol < m_ColumnData.GetSize())
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p && p->pCheckbox)
			rc = p->pCheckbox->GetCheck();
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetCheck
void CXHeaderCtrl::SetCheck(int nCol, int nCheckedState)
{
	ASSERT(nCol >= 0);
	ASSERT(nCol < GetItemCount());
	if ((nCol < 0) || (nCol >= GetItemCount()))
		return;
	ASSERT((nCheckedState == XLISTCTRL_NO_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_CHECKED_CHECKBOX));

	// use the image index to indicate the checked status
	HDITEM hditem;
	hditem.mask = HDI_IMAGE;
	hditem.iImage = nCheckedState;
	SetItem(nCol, &hditem);

	if (nCol < m_ColumnData.GetSize())
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p && p->pCheckbox)
			p->pCheckbox->SetCheck(nCheckedState);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetItemColor
COLORREF CXHeaderCtrl::GetItemColor(int nCol)
{
	COLORREF rgb = RGB(0,0,0);

	if (nCol < m_ColumnData.GetSize())
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p)
			rgb = p->crText;
	}

	return rgb;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemColor - if nCol = -1, all columns are set
BOOL CXHeaderCtrl::SetItemColor(int nCol, COLORREF crText)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::SetItemColor:  %d  0x%X\n"), nCol, crText);

	BOOL bRet = FALSE;
	int nSize = m_ColumnData.GetSize();
	int nNewSize = nCol + 10;

	if (nCol >= nSize)
	{
		// grow column data list
		m_ColumnData.SetSize(nNewSize);

		// create new entries
		for (int i = nSize; i < nNewSize; i++)
			m_ColumnData[i] = new XHEADERCTRLDATA;
	}
	else if (nCol == -1)
	{
		// set color for all columns
		m_crText = crText; 
		Invalidate();
		bRet = TRUE;
	}

	if (nCol >= 0)
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p)
		{
			p->crText = crText;
			bRet = TRUE;
		}
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// InsertColumn
void CXHeaderCtrl::InsertColumn(int nCol, int nCheckboxState)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::InsertColumn:  %d\n"), nCol);

	int nSize = m_ColumnData.GetSize();
	int nNewSize = nCol + 10;

	if (nCol >= nSize)
	{
		// grow column data list
		m_ColumnData.SetSize(nNewSize);

		// create new entries
		for (int i = nSize; i < nNewSize; i++)
			m_ColumnData[i] = new XHEADERCTRLDATA;
	}

	if (nCheckboxState != XLISTCTRL_NO_CHECKBOX)
	{
		// a checkbox was specified
		CXCheckbox * pCheckbox = new CXCheckbox(this);
		ASSERT(pCheckbox);
		if (pCheckbox)
		{
			pCheckbox->SetCheck(nCheckboxState);

			XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
			if (p)
			{
				p->pCheckbox = pCheckbox;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXHeaderCtrl::InsertItem(int nCol, HDITEM* phdi)
{
	TRACE(_T("in CXHeaderCtrl::InsertItem\n"));

	ASSERT(nCol >= 0);
	if (nCol < 0)
		return FALSE;

	InsertColumn(nCol, (phdi->mask & HDI_IMAGE) ? 
					phdi->iImage : XLISTCTRL_NO_CHECKBOX);

	return CHeaderCtrl::InsertItem(nCol, phdi);
}

///////////////////////////////////////////////////////////////////////////////
// DeleteColumn
BOOL CXHeaderCtrl::DeleteColumn(int nCol)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::DeleteColumn\n"));

	ASSERT(nCol >= 0);
	ASSERT(nCol < GetItemCount());
	if ((nCol < 0) || (nCol >= GetItemCount()))
		return FALSE;

	BOOL bRet = FALSE;

	if (nCol < m_ColumnData.GetSize())
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p)
			delete p;
		m_ColumnData.RemoveAt(nCol);
		bRet = TRUE;
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteItem
BOOL CXHeaderCtrl::DeleteItem(int nCol)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::DeleteItem\n"));
	
	ASSERT(nCol >= 0);
	ASSERT(nCol < GetItemCount());
	if ((nCol < 0) || (nCol >= GetItemCount()))
		return FALSE;

	DeleteColumn(nCol);

	return CHeaderCtrl::DeleteItem(nCol);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDblClk
void CXHeaderCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnLButtonDblClk\n"));

	SendMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));

	//CHeaderCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnLButtonDown\n"));

	int nCol = ColumnHitTest(point);

	if ((nCol >= 0) && (nCol < GetItemCount()))
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p)
		{
			if (m_bAllowUserInput && p->bAllowUserInput)
			{
				if (p->pCheckbox)
				{
					if (PtInCheckboxRect(nCol, point))
					{
						p->pCheckbox->SetPressed(TRUE);
						UpdateItem(nCol);
					}
				}
			}
			else
			{
				return;
			}
		}
	}

	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CXHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	XLISTCTRL_TRACE(_T("in CXHeaderCtrl::OnLButtonUp\n"));

	int nCol = ColumnHitTest(point);
	XLISTCTRL_TRACE(_T("ColumnHitTest returned %d\n"), nCol);

	if ((nCol >= 0) && (nCol < GetItemCount()))
	{
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nCol);
		if (p && p->pCheckbox)
		{
			XLISTCTRL_TRACE(_T("column has checkbox\n"));
			if (PtInCheckboxRect(nCol, point))
			{
				XLISTCTRL_TRACE(_T("point is in checkbox\n"));
				BOOL bPressed = p->pCheckbox->GetPressed();
				p->pCheckbox->SetPressed(FALSE);

				if (bPressed)
				{
					// only send message if this was the checkbox that was initially pressed

					// flip the checkmark

					int nCheckedState = p->pCheckbox->GetCheck();

					nCheckedState = (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) ? 
													  XLISTCTRL_CHECKED_CHECKBOX : 
													  XLISTCTRL_UNCHECKED_CHECKBOX;

					p->pCheckbox->SetCheck(nCheckedState);

					if (m_pListCtrl && ::IsWindow(m_pListCtrl->m_hWnd))
						m_pListCtrl->SendMessage(WM_XHEADERCTRL_CHECKBOX_CLICKED, (WPARAM)-1, nCol);
				}
				else
				{
					XLISTCTRL_TRACE(_T("NOT pressed\n"));
				}
				UpdateItem(nCol);
			}
		}
	}

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// GetAllowUserInput
BOOL CXHeaderCtrl::GetAllowUserInput(int nItem)
{
	ASSERT((nItem == -1) || (nItem >= 0));
	ASSERT(nItem < GetItemCount());
	if ((nItem < -1) || (nItem >= GetItemCount()))
		return FALSE;

	BOOL bFlag = FALSE;

	if (nItem == -1)
	{
		// return global flag
		bFlag = m_bAllowUserInput;
	}
	else
	{
		// return column flag
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nItem);
		if (p)
			bFlag = p->bAllowUserInput;
	}

	return bFlag;
}

///////////////////////////////////////////////////////////////////////////////
// SetAllowUserInput
BOOL CXHeaderCtrl::SetAllowUserInput(int nItem, BOOL bAllowUserInput)
{
	ASSERT((nItem == -1) || (nItem >= 0));
	ASSERT(nItem < GetItemCount());
	if ((nItem < -1) || (nItem >= GetItemCount()))
		return FALSE;

	if (nItem == -1)
	{
		// set global flag
		m_bAllowUserInput = bAllowUserInput;
	}
	else
	{
		// set column flag
		XHEADERCTRLDATA * p = (XHEADERCTRLDATA *) m_ColumnData.GetAt(nItem);
		if (p)
			p->bAllowUserInput = bAllowUserInput;
		else
			return FALSE;
	}

	return TRUE;
}

