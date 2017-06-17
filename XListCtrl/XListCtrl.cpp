// XListCtrl.cpp  Version 1.5 beta 2 - article available at 
//     http://www.codeproject.com/listctrl/xlistctrl.asp
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.5 - 2006 November 14
//     - See article for changes
//
//     Version 1.4 - 2006 September 1
//     - See article for changes
//
//     Version 1.3 - 2005 February 9
//     - See article for changes
//
//     Version 1.0 - 2002 February 4
//     - Initial public release
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
#include "XListCtrl.h"
#include "SortCStringArray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_COMBO_SELECTION  = ::RegisterWindowMessage(_T("WM_XLISTCTRL_COMBO_SELECTION"));
XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_EDIT_END         = ::RegisterWindowMessage(_T("WM_XLISTCTRL_EDIT_END"));
XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_CHECKBOX_CLICKED = ::RegisterWindowMessage(_T("WM_XLISTCTRL_CHECKBOX_CLICKED"));
XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_BUTTON_CLICKED   = ::RegisterWindowMessage(_T("WM_XLISTCTRL_BUTTON_CLICKED"));

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl

BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	ON_MESSAGE(LVM_CANCELEDITLABEL, OnCancelEditLabel)
	ON_MESSAGE(LVM_GETEDITCONTROL, OnGetEditControl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_VK_ESCAPE, OnComboEscape)
	ON_REGISTERED_MESSAGE(WM_XCOMBOLIST_COMPLETE, OnComboComplete)
#endif
#ifndef NO_XLISTCTRL_TOOL_TIPS
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
#endif
	ON_REGISTERED_MESSAGE(WM_XEDIT_KILL_FOCUS, OnXEditKillFocus)
	ON_REGISTERED_MESSAGE(WM_XEDIT_VK_ESCAPE, OnXEditEscape)
	ON_REGISTERED_MESSAGE(WM_XBUTTON_CLICKED, OnXButtonClicked)
	ON_REGISTERED_MESSAGE(WM_XHEADERCTRL_CHECKBOX_CLICKED, OnHeaderCheckboxClicked)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXListCtrl::CXListCtrl()
{

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	m_bComboIsClicked         = FALSE;
	m_nComboItem              = 0;
	m_nComboSubItem           = 0;
	m_pCombo                  = NULL;
	m_bFontIsCreated          = FALSE;
#endif

	m_dwExtendedStyleX        = 0;
	m_bHeaderIsSubclassed     = FALSE;
	m_bUseEllipsis            = TRUE;
	m_bListModified           = FALSE;
	m_bInitialCheck           = FALSE;
	m_strInitialString        = _T("");
	m_nPadding                = 5;
	m_pEdit                   = NULL;
	m_nEditItem               = 0;
	m_nEditSubItem            = 0;
	m_bCancelEditLabel        = FALSE;
	m_nItemHeight             = -2;
	m_nHeaderHeight           = 0;
	m_bAllowUserInput         = TRUE;

	m_nCurrentButtonItem      = -1;
	m_nCurrentButtonSubItem   = -1;

	m_nCurrentCheckboxItem    = -1;
	m_nCurrentCheckboxSubItem = -1;

	m_strEmptyMessage         = _T("");
	m_crEmptyTextColor        = RGB(0,0,0);

	GetColors();
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXListCtrl::~CXListCtrl()
{
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	if (m_pCombo)
		delete m_pCombo;
#endif
	if (m_pEdit)
		delete m_pEdit;
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	// for Dialog based applications, this is a good place
	// to subclass the header control because the OnCreate()
	// function does not get called.

	SubclassHeaderControl();
}

///////////////////////////////////////////////////////////////////////////////
// OnCreate
int CXListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
	{
		ASSERT(FALSE);
		return -1;
	}

	// When the CXListCtrl object is created via a call to Create(), instead
	// of via a dialog box template, we must subclass the header control
	// window here because it does not exist when the PreSubclassWindow()
	// function is called.

	SubclassHeaderControl();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SubclassHeaderControl
void CXListCtrl::SubclassHeaderControl()
{
	if (m_bHeaderIsSubclassed)
		return;

	// if the list control has a header control window, then
	// subclass it

	// Thanks to Alberto Gattegno and Alon Peleg  and their article
	// "A Multiline Header Control Inside a CListCtrl" for easy way
	// to determine if the header control exists.

	CHeaderCtrl* pHeader = CListCtrl::GetHeaderCtrl();
	if (pHeader)
	{
		VERIFY(m_HeaderCtrl.SubclassWindow(pHeader->m_hWnd));
		m_bHeaderIsSubclassed = TRUE;
		m_HeaderCtrl.SetListCtrl(this);
		CRect rectHeader;
		m_HeaderCtrl.GetWindowRect(&rectHeader);
		m_nHeaderHeight = rectHeader.Height();
	}
}

///////////////////////////////////////////////////////////////////////////////
// SendRegisteredMessage
void CXListCtrl::SendRegisteredMessage(UINT nMessage, int nItem, int nSubItem)
{
	CWnd *pWnd = GetParent();
	if (!pWnd)
		pWnd = GetOwner();
	if (pWnd && ::IsWindow(pWnd->m_hWnd))
	{
		int nID = GetDlgCtrlID();
		LPARAM lParam = MAKELPARAM(nSubItem, nID);
		pWnd->SendMessage(nMessage, nItem, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnClick
BOOL CXListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	pNMHDR   = pNMHDR;
	*pResult = 0;

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	int nItem = -1;
	int nSubItem = -1;

	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("OnClick: lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
		nSubItem = lvhit.iSubItem;
	}

	if ((nItem < 0) || (nItem >= GetItemCount()) ||
		(nSubItem < 0) || (nSubItem >= GetColumns()))
		return TRUE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);

	if (pXLCD && pXLCD[0].bEnabled)
	{
		if (pXLCD[nSubItem].pButton)
		{
			if (PtInButtonRect(nItem, nSubItem, point))
			{
				BOOL bPressed = pXLCD[nSubItem].pButton->GetPressed();
				pXLCD[nSubItem].pButton->SetPressed(FALSE);
				UpdateSubItem(nItem, nSubItem);

				if (bPressed)
				{
					// only send message if this was the button that was initially pressed
					OnXButtonClicked(nItem, nSubItem);
				}
				//return TRUE;
			}
		}
		else if (pXLCD[nSubItem].pCheckbox)
		{
			if (PtInCheckboxRect(nItem, nSubItem, point))
			{
				BOOL bPressed = pXLCD[nSubItem].pCheckbox->GetPressed();
				pXLCD[nSubItem].pCheckbox->SetPressed(FALSE);

				if (bPressed)
				{
					// only send message if this was the checkbox that was initially pressed

					// flip the checkmark

					int nCheckedState = pXLCD[nSubItem].pCheckbox->GetCheck();

					nCheckedState = (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) ? 
													  XLISTCTRL_CHECKED_CHECKBOX : 
													  XLISTCTRL_UNCHECKED_CHECKBOX;

					pXLCD[nSubItem].pCheckbox->SetCheck(nCheckedState);
					pXLCD[nSubItem].bModified = TRUE;
					m_bListModified = TRUE;

					SendRegisteredMessage(WM_XLISTCTRL_CHECKBOX_CLICKED, nItem, nSubItem);

					// now update checkbox in header

					// 0 = no checkbox in column header
					if (GetHeaderCheckedState(nSubItem) != XLISTCTRL_NO_CHECKBOX)
					{
						int nCheckedCount = CountCheckedItems(nSubItem);

						if (nCheckedCount == GetItemCount())
							SetHeaderCheckedState(nSubItem, XLISTCTRL_CHECKED_CHECKBOX);
						else
							SetHeaderCheckedState(nSubItem, XLISTCTRL_UNCHECKED_CHECKBOX);
					}
				}
				UpdateSubItem(nItem, nSubItem);
				//return TRUE;
			}
		}
	}

	return FALSE;		// return FALSE to send message to parent also -
						// NOTE:  MSDN documentation is incorrect
}

///////////////////////////////////////////////////////////////////////////////
// OnCustomDraw
void CXListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// This is the notification message for an item.  We'll request
		// notifications before each subitem's prepaint stage.

		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// This is the prepaint stage for a subitem. Here's where we set the
		// item's text and background colors. Our return value will tell
		// Windows to draw the subitem itself, but it will use the new colors
		// we set here.

		int nItem = static_cast<int> (pLVCD->nmcd.dwItemSpec);
		int nSubItem = pLVCD->iSubItem;

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) pLVCD->nmcd.lItemlParam;
		ASSERT(pXLCD);

		COLORREF crText  = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;

		if (pXLCD)
		{
			crText  = pXLCD[nSubItem].crText;
			crBkgnd = pXLCD[nSubItem].crBackground;

			if (!pXLCD[0].bEnabled)
				crText = m_crGrayText;
		}

		// store the colors back in the NMLVCUSTOMDRAW struct
		pLVCD->clrText = crText;
		pLVCD->clrTextBk = crBkgnd;

		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		CRect rect;
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

		//XLISTCTRL_TRACE(_T("OnCustomDraw: (%d,%d)  -----\n"), nItem, nSubItem);
		//TRACERECT(rect);

		if (rect.top < 0)
		{
			*pResult = CDRF_SKIPDEFAULT;	// rect is not in the true client area
		}
		else if (pXLCD && (pXLCD[nSubItem].bShowProgress))
		{
			DrawProgress(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else if (pXLCD && pXLCD[nSubItem].pCheckbox)
		{
			DrawCheckbox(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
		else
		{
			rect.left += DrawImage(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			rect.right -= DrawButton(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			if (rect.right <= rect.left)
				rect.right = rect.left + 1;

			DrawSubItemText(nItem, nSubItem, pDC, crText, crBkgnd, rect, pXLCD);

			*pResult = CDRF_SKIPDEFAULT;	// We've painted everything.
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawProgress
void CXListCtrl::DrawProgress(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF /*crBkgnd*/,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	UNUSED_ALWAYS(nItem);

	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return;
	}

	// fill interior with light gray
	pDC->FillSolidRect(rect, RGB(224,224,224));

	rect.bottom -= 1;
	rect.left += 1;		// leave margin in case row is highlighted

	// draw border
	pDC->Draw3dRect(&rect, RGB(0,0,0), m_crBtnShadow);

	if (pXLCD[nSubItem].nProgressPercent > 0)
	{
		// draw progress bar and text

		CRect LeftRect, RightRect;
		LeftRect = rect;
		LeftRect.left += 1;
		LeftRect.top += 1;
		LeftRect.bottom -= 1;
		RightRect = LeftRect;
		int w = (LeftRect.Width() * pXLCD[nSubItem].nProgressPercent) / 100;
		LeftRect.right = LeftRect.left + w - 1;
		RightRect.left = LeftRect.right;
		pDC->FillSolidRect(LeftRect, m_crHighLight);

		if (pXLCD[nSubItem].bShowProgressMessage)
		{
			CString str, format;
			format = pXLCD[nSubItem].strProgressMessage;
			if (format.IsEmpty())
				str.Format(_T("%d%%"), pXLCD[nSubItem].nProgressPercent);
			else
				str.Format(format, pXLCD[nSubItem].nProgressPercent);

			pDC->SetBkMode(TRANSPARENT);

			CRect TextRect;
			TextRect = rect;
			TextRect.DeflateRect(1, 1);

			CRgn rgn;
			rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, 
					LeftRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(m_crHighLightText);//crBkgnd);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rgn.DeleteObject();
			rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, 
					RightRect.bottom);
			pDC->SelectClipRgn(&rgn);
			pDC->SetTextColor(crText);
			pDC->DrawText(str, &TextRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			rgn.DeleteObject();
			pDC->SelectClipRgn(NULL);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DrawButton
int CXListCtrl::DrawButton(int nItem, 
						   int nSubItem, 
						   CDC *pDC,
						   COLORREF crText,
						   COLORREF crBkgnd,
						   CRect& rect,
						   XLISTCTRLDATA *pXLCD)
{
	//XLISTCTRL_TRACE(_T("in CXListCtrl::DrawButton:  nItem=%d  nSubItem=%d\n"), nItem, nSubItem);

	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return 0;
	}

	if (pXLCD[nSubItem].pButton == NULL)
		return 0;

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	//pDC->FillSolidRect(&rect, crBkgnd);

	CString strItem = GetItemText(nItem, nSubItem);
	CString strButton = GetButtonText(nItem, nSubItem);

	CRect rectButton(rect);

	pDC->DrawText(strButton, &rectButton, DT_LEFT | DT_SINGLELINE | DT_CALCRECT);

	rectButton.top = rect.top;
	rectButton.bottom = rect.bottom;
	rectButton.right += 11;

	if (rectButton.Width() > rect.Width())
		rectButton.CopyRect(rect);

	int w = rectButton.Width();

	if (pXLCD[nSubItem].pButton->GetFixedWidth() > 0)
		w = pXLCD[nSubItem].pButton->GetFixedWidth();

	if (strItem.IsEmpty())
	{
		// no string, so center the button
		rectButton.left = rect.left + (rect.Width() - w) / 2;
		rectButton.right = rectButton.left + w;
		w = rect.right - rectButton.left + 2;
	}
	else
	{
		// draw button on right
		rectButton.right = rect.right - 2;
		rectButton.left = rectButton.right - w;
		w += 2;
	}

	pXLCD[nSubItem].pButton->Draw(pDC, rectButton);

	return w;
}

///////////////////////////////////////////////////////////////////////////////
// DrawCheckbox
void CXListCtrl::DrawCheckbox(int nItem,
							  int nSubItem,
							  CDC *pDC,
							  COLORREF crText,
							  COLORREF crBkgnd,
							  CRect& rect,
							  XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	CRect chkboxrect(rect);

	// center the checkbox

	int h = XLISTCTRL_CHECKBOX_SIZE;			// height and width are the same
	chkboxrect.left  += 5;
	chkboxrect.right  = chkboxrect.left + h;
	chkboxrect.top    = rect.top + (rect.Height() - h) / 2;
	chkboxrect.bottom = chkboxrect.top + h;

	CString str = GetItemText(nItem, nSubItem);

	if (str.IsEmpty())
	{
		// center the checkbox

		chkboxrect.left = rect.left + (rect.Width() - h) / 2 - 1;
		chkboxrect.right = chkboxrect.left + h;
	}

	if (pXLCD[nSubItem].pCheckbox)
	{
		pXLCD[nSubItem].pCheckbox->Enable(pXLCD[0].bEnabled);
		pXLCD[nSubItem].pCheckbox->Draw(pDC, chkboxrect);
	}

	if (!str.IsEmpty())
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		CRect textrect;
		textrect = rect;
		textrect.left = chkboxrect.right + 4;

		UINT nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		if (m_bUseEllipsis)
			nFormat |= DT_END_ELLIPSIS;

		pDC->DrawText(str, &textrect, nFormat);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetDrawColors
void CXListCtrl::GetDrawColors(int nItem,
							   int nSubItem,
							   COLORREF& colorText,
							   COLORREF& colorBkgnd)
{
	DWORD dwStyle    = GetStyle();
	DWORD dwExStyle  = GetExtendedStyle();

	COLORREF crText  = colorText;
	COLORREF crBkgnd = colorBkgnd;

	if (GetItemState(nItem, LVIS_SELECTED))
	{
		if (dwExStyle & LVS_EX_FULLROWSELECT)
		{
			// selected?  if so, draw highlight background
			crText  = m_crHighLightText;
			crBkgnd = m_crHighLight;

			// has focus?  if not, draw gray background
			if (m_hWnd != ::GetFocus())
			{
				if (dwStyle & LVS_SHOWSELALWAYS)
				{
					crText  = m_crWindowText;
					crBkgnd = m_crBtnFace;
				}
				else
				{
					crText  = colorText;
					crBkgnd = colorBkgnd;
				}
			}
		}
		else	// not full row select
		{
			if (nSubItem == 0)
			{
				// selected?  if so, draw highlight background
				crText  = m_crHighLightText;
				crBkgnd = m_crHighLight;

				// has focus?  if not, draw gray background
				if (m_hWnd != ::GetFocus())
				{
					if (dwStyle & LVS_SHOWSELALWAYS)
					{
						crText  = m_crWindowText;
						crBkgnd = m_crBtnFace;
					}
					else
					{
						crText  = colorText;
						crBkgnd = colorBkgnd;
					}
				}
			}
		}
	}

	colorText = crText;
	colorBkgnd = crBkgnd;
}

///////////////////////////////////////////////////////////////////////////////
// DrawImage
int CXListCtrl::DrawImage(int nItem,
						  int nSubItem,
						  CDC* pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect rect,
  						  XLISTCTRLDATA *pXLCD)
{
	if (rect.IsRectEmpty())
	{
		return 0;
	}

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rect, crBkgnd);

	int nWidth = 0;
	rect.left += m_HeaderCtrl.GetSpacing();

	CImageList* pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		SIZE sizeImage;
		sizeImage.cx = sizeImage.cy = 0;
		IMAGEINFO info;

		int nImage = -1;
		if (pXLCD)
			nImage = pXLCD[nSubItem].nImage;

		if (nImage == -1)
			return 0;

		if (pImageList->GetImageInfo(nImage, &info))
		{
			sizeImage.cx = info.rcImage.right - info.rcImage.left;
			sizeImage.cy = info.rcImage.bottom - info.rcImage.top;
		}

		if (nImage >= 0)
		{
			if (rect.Width() > 0)
			{
				POINT point;

				point.y = rect.CenterPoint().y - (sizeImage.cy >> 1);
				point.x = rect.left;

				SIZE size;
				size.cx = rect.Width() < sizeImage.cx ? rect.Width() : sizeImage.cx;
				size.cy = rect.Height() < sizeImage.cy ? rect.Height() : sizeImage.cy;

				// save image list background color
				COLORREF rgb = pImageList->GetBkColor();

				// set image list background color
				pImageList->SetBkColor(crBkgnd);
				pImageList->DrawIndirect(pDC, nImage, point, size, CPoint(0, 0));
				pImageList->SetBkColor(rgb);

				nWidth = sizeImage.cx + m_HeaderCtrl.GetSpacing();
			}
		}
	}

	return nWidth;
}

///////////////////////////////////////////////////////////////////////////////
// DrawSubItemText
int CXListCtrl::DrawSubItemText(int nItem,
								int nSubItem,
								CDC *pDC,
								COLORREF crText,
								COLORREF crBkgnd,
								CRect& rect,
								XLISTCTRLDATA *pXLCD)
{
	ASSERT(pDC);
	ASSERT(pXLCD);

	if (rect.IsRectEmpty())
	{
		return 0;
	}

	int nWidth = 0;

	CRect rectText(rect);

	GetDrawColors(nItem, nSubItem, crText, crBkgnd);

	pDC->FillSolidRect(&rectText, crBkgnd);

	CString str = GetItemText(nItem, nSubItem);

	if (!str.IsEmpty())
	{
		// get text justification
		HDITEM hditem;
		hditem.mask = HDI_FORMAT;
		m_HeaderCtrl.GetItem(nSubItem, &hditem);
		int nFmt = hditem.fmt & HDF_JUSTIFYMASK;
		UINT nFormat = DT_VCENTER | DT_SINGLELINE;
		if (m_bUseEllipsis)
			nFormat |= DT_END_ELLIPSIS;

		if (pXLCD[nSubItem].nFormat == DT_DEFAULT)
		{
			// default to same alignment as header
			if (nFmt == HDF_CENTER)
				nFormat |= DT_CENTER;
			else if (nFmt == HDF_LEFT)
				nFormat |= DT_LEFT;
			else
				nFormat |= DT_RIGHT;
		}
		else
		{
			nFormat |= pXLCD[nSubItem].nFormat;
		}

		CFont *pOldFont = NULL;
		CFont boldfont;

		// check if bold specified for subitem
		if (pXLCD && pXLCD[nSubItem].bBold)
		{
			CFont *font = pDC->GetCurrentFont();
			if (font)
			{
				LOGFONT lf;
				font->GetLogFont(&lf);
				lf.lfWeight = FW_BOLD;
				boldfont.CreateFontIndirect(&lf);
				pOldFont = pDC->SelectObject(&boldfont);
			}
		}
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		rectText.DeflateRect(m_nPadding, 0);
		CRect rectOut(rectText);
		pDC->DrawText(str, &rectOut, nFormat | DT_CALCRECT);
		pDC->DrawText(str, &rectText, nFormat);
		rectText.InflateRect(m_nPadding, 0);
		rectOut.InflateRect(m_nPadding, 0);
		nWidth = rectOut.right;
		if (pOldFont)
			pDC->SelectObject(pOldFont);
	}
	return nWidth;
}

///////////////////////////////////////////////////////////////////////////////
// GetSubItemRect
BOOL CXListCtrl::GetSubItemRect(int nItem,
								int nSubItem,
								int nArea,
								CRect& rect)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	BOOL bRC = CListCtrl::GetSubItemRect(nItem, nSubItem, nArea, rect);

	// if nSubItem == 0, the rect returned by CListCtrl::GetSubItemRect
	// is the entire row, so use left edge of second subitem

	if (nSubItem == 0)
	{
		if (GetColumns() > 1)
		{
			CRect rect1;
			bRC = GetSubItemRect(nItem, 1, LVIR_BOUNDS, rect1);
			rect.right = rect1.left;
		}
	}

	if (nSubItem == 0)
	{
		if (GetColumns() > 1)
		{
			CRect rect1;
			// in case 2nd col width = 0
			for (int i = 1; i < GetColumns(); i++)
			{
				bRC = GetSubItemRect(nItem, i, LVIR_BOUNDS, rect1);
				if (rect1.Width() > 0)
				{
					rect.right = rect1.left;
					break;
				}
			}
		}
	}

	return bRC;
}

///////////////////////////////////////////////////////////////////////////////
// GetSubItemAlignment
UINT CXListCtrl::GetSubItemAlignment(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return DT_DEFAULT;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return DT_DEFAULT;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return DT_DEFAULT;
	}

	return pXLCD[nSubItem].nFormat;
}

///////////////////////////////////////////////////////////////////////////////
// SetSubItemAlignment
void CXListCtrl::SetSubItemAlignment(int nItem, int nSubItem, UINT nFormat)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].nFormat = nFormat;
	}

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDown
void CXListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnLButtonDown\n"));

	int nItem = -1;
	int nSubItem = -1;

	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("OnLButtonDown: lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
		nSubItem = lvhit.iSubItem;
	}

	XLISTCTRLDATA *pXLCD = NULL;

	if ((nItem < 0) || (nItem >= GetItemCount()) ||
		(nSubItem < 0) || (nSubItem >= GetColumns()))
	{

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
		if (m_pCombo)
			OnComboEscape(0, 0);
#endif

	}
	else
	{
		pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			return;
		}

		if (!pXLCD[0].bEnabled)
			return;

		if (m_bAllowUserInput && pXLCD[nSubItem].bAllowUserInput)
		{
			if (pXLCD[nSubItem].pCheckbox && PtInCheckboxRect(nItem, nSubItem, point))
			{
				pXLCD[nSubItem].pCheckbox->SetPressed(TRUE);
				UpdateSubItem(nItem, nSubItem);
				CListCtrl::OnLButtonDown(nFlags, point);
				return;
			}
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
			else if (pXLCD[nSubItem].bCombo)
			{
				CListCtrl::OnLButtonDown(nFlags, point);
				DrawComboBox(nItem, nSubItem);
				return;
			}
#endif
			else if (pXLCD[nSubItem].bEdit)
			{
				CListCtrl::OnLButtonDown(nFlags, point);
				DrawEdit(nItem, nSubItem);
				return;
			}
			else if (pXLCD[nSubItem].pButton && PtInButtonRect(nItem, nSubItem, point))
			{
				pXLCD[nSubItem].pButton->SetPressed(TRUE);
				UpdateSubItem(nItem, nSubItem);
				return;
			}
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonUp
void CXListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	TRACE(_T("in CXListCtrl::OnLButtonUp\n"));

	int nItem = -1;
	int nSubItem = -1;

	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("OnLButtonUp: lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
		nSubItem = lvhit.iSubItem;
	}

	XLISTCTRLDATA *pXLCD = NULL;

	if ((nItem < 0) || (nItem >= GetItemCount()) ||
		(nSubItem < 0) || (nSubItem >= GetColumns()))
	{

	}
	else
	{
		pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD && pXLCD[0].bEnabled)
		{
			if (pXLCD[nSubItem].pButton)
			{
				if (PtInButtonRect(nItem, nSubItem, point))
				{
					BOOL bPressed = pXLCD[nSubItem].pButton->GetPressed();
					pXLCD[nSubItem].pButton->SetPressed(FALSE);
					UpdateSubItem(nItem, nSubItem);

					if (bPressed)
					{
						// only send message if this was the button that was initially pressed
						OnXButtonClicked(nItem, nSubItem);
					}
					return;
				}
			}
			else if (pXLCD[nSubItem].pCheckbox)
			{
				if (PtInCheckboxRect(nItem, nSubItem, point))
				{
					BOOL bPressed = pXLCD[nSubItem].pCheckbox->GetPressed();
					pXLCD[nSubItem].pCheckbox->SetPressed(FALSE);

					if (bPressed)
					{
						// only send message if this was the checkbox that was initially pressed

						// flip the checkmark

						int nCheckedState = pXLCD[nSubItem].pCheckbox->GetCheck();

						nCheckedState = (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) ? 
														  XLISTCTRL_CHECKED_CHECKBOX : 
														  XLISTCTRL_UNCHECKED_CHECKBOX;

						pXLCD[nSubItem].pCheckbox->SetCheck(nCheckedState);
						pXLCD[nSubItem].bModified = TRUE;
						m_bListModified = TRUE;

						SendRegisteredMessage(WM_XLISTCTRL_CHECKBOX_CLICKED, nItem, nSubItem);

						// now update checkbox in header

						// 0 = no checkbox in column header
						if (GetHeaderCheckedState(nSubItem) != XLISTCTRL_NO_CHECKBOX)
						{
							int nCheckedCount = CountCheckedItems(nSubItem);

							if (nCheckedCount == GetItemCount())
								SetHeaderCheckedState(nSubItem, XLISTCTRL_CHECKED_CHECKBOX);
							else
								SetHeaderCheckedState(nSubItem, XLISTCTRL_UNCHECKED_CHECKBOX);
						}
					}
					UpdateSubItem(nItem, nSubItem);
					return;
				}
			}
		}
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnLButtonDblClk
void CXListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnRButtonDown - added so we can ignore disabled items
void CXListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnRButtonDown\n"));

	int nItem = -1;

	LVHITTESTINFO lvhit;
	lvhit.pt = point;
	SubItemHitTest(&lvhit);
	if (lvhit.flags & LVHT_ONITEMLABEL)
	{
		XLISTCTRL_TRACE(_T("OnRButtonDown: lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
	}

	if (nItem != -1)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD)
		{
			return;
		}

		if (!pXLCD[0].bEnabled)
			return;
	}

	CListCtrl::OnRButtonDown(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnNcLButtonDown
void CXListCtrl::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	TRACE(_T("in CXListCtrl::OnNcLButtonDown\n"));
	
	if (m_pEdit)
	{
		OnXEditKillFocus(0, 0);
	}

	CListCtrl::OnNcLButtonDown(nHitTest, point);
}

///////////////////////////////////////////////////////////////////////////////
// OnPaint
void CXListCtrl::OnPaint()
{
    Default();
	if (GetItemCount() <= 0)
	{
		CDC* pDC = GetDC();
		int nSavedDC = pDC->SaveDC();

		CRect rc;
		GetWindowRect(&rc);
		ScreenToClient(&rc);
		CHeaderCtrl* pHC = CListCtrl::GetHeaderCtrl();
		if (pHC != NULL)
		{
			CRect rcH;
			pHC->GetItemRect(0, &rcH);
			rc.top += rcH.bottom;
		}
		rc.top += 10;

		COLORREF crText = m_crWindowText;
		COLORREF crBkgnd = m_crWindow;

		CString strText = m_strEmptyMessage;
		if (strText.IsEmpty())
		{
			strText = _T("There are no items to show in this view.");
		}
		else
		{
			crText = m_crEmptyTextColor;
		}

		CBrush brush(crBkgnd);
		pDC->FillRect(rc, &brush);

		pDC->SetTextColor(crText);
		pDC->SetBkColor(crBkgnd);
		pDC->SelectStockObject(ANSI_VAR_FONT);
		pDC->DrawText(strText, -1, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
		pDC->RestoreDC(nSavedDC);
		ReleaseDC(pDC);
	}
}

///////////////////////////////////////////////////////////////////////////////
// DeleteColumn
BOOL CXListCtrl::DeleteColumn(int nCol)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::DeleteColumn\n"));

	ASSERT(nCol >= 0);
	ASSERT(nCol < GetColumns());
	if ((nCol < 0) || (nCol >= GetColumns()))
		return FALSE;

	m_HeaderCtrl.DeleteColumn(nCol);

	return CListCtrl::DeleteColumn(nCol);
}

///////////////////////////////////////////////////////////////////////////////
// InsertColumn
int CXListCtrl::InsertColumn(int nCol, const LVCOLUMN* pColumn)
{
	ASSERT(nCol >= 0);
	ASSERT(pColumn);
	if (!pColumn)
		return -1;

	m_HeaderCtrl.InsertColumn(nCol, (pColumn->mask & LVCF_IMAGE) ? 
					pColumn->iImage : XLISTCTRL_NO_CHECKBOX);

	return CListCtrl::InsertColumn(nCol, pColumn);
}

///////////////////////////////////////////////////////////////////////////////
// InsertColumn
int CXListCtrl::InsertColumn(int nCol, 
							 LPCTSTR lpszColumnHeading, 
							 int nFormat /*= LVCFMT_LEFT*/, 
							 int nWidth /*= -1*/, 
							 int nSubItem /*= -1*/,
							 int nCheckboxState /*= XLISTCTRL_NO_CHECKBOX*/)
{
	LVCOLUMN lvcolumn;
	memset(&lvcolumn, 0, sizeof(lvcolumn));
	lvcolumn.mask     = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
	lvcolumn.fmt      = nFormat;
	lvcolumn.iSubItem = nSubItem;
	lvcolumn.pszText  = (LPTSTR) lpszColumnHeading;
	lvcolumn.cx       = nWidth;
	lvcolumn.iImage   = nCheckboxState;	// XListCtrl uses iImage as a checkbox flag

	return InsertColumn(nCol, &lvcolumn);
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(const LVITEM* pItem)
{
	ASSERT(pItem->iItem >= 0);
	if (pItem->iItem < 0)
		return -1;

	int index = CListCtrl::InsertItem(pItem);

	if (index < 0)
		return index;

	XLISTCTRLDATA *pXLCD = new XLISTCTRLDATA [GetColumns()];
	ASSERT(pXLCD);
	if (!pXLCD)
		return -1;

	pXLCD[0].crText       = m_crWindowText;
	pXLCD[0].crBackground = m_crWindow;
	pXLCD[0].nImage       = pItem->iImage;
	pXLCD[0].dwItemData   = pItem->lParam;

	CListCtrl::SetItemData(index, (DWORD) pXLCD);

	return index;
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem, LPCTSTR lpszItem)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	return InsertItem(nItem,
					  lpszItem,
					  m_crWindowText,
					  m_crWindow);
}

///////////////////////////////////////////////////////////////////////////////
// InsertItem
int CXListCtrl::InsertItem(int nItem,
						   LPCTSTR lpszItem,
						   COLORREF crText,
						   COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	if (nItem < 0)
		return -1;

	int index = CListCtrl::InsertItem(nItem, lpszItem);

	if (index < 0)
		return index;

	XLISTCTRLDATA *pXLCD = new XLISTCTRLDATA [GetColumns()];
	ASSERT(pXLCD);
	if (!pXLCD)
		return -1;

	pXLCD[0].crText       = crText;
	pXLCD[0].crBackground = crBackground;
	pXLCD[0].nImage       = -1;

	CListCtrl::SetItemData(index, (DWORD) pXLCD);

	return index;
}

///////////////////////////////////////////////////////////////////////////////
// SetItem
int CXListCtrl::SetItem(const LVITEM* pItem)
{
	ASSERT(pItem->iItem >= 0);
	if (pItem->iItem < 0)
		return -1;

	BOOL rc = CListCtrl::SetItem(pItem);

	if (!rc)
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(pItem->iItem);
	if (pXLCD)
	{
		pXLCD[pItem->iSubItem].nImage = pItem->iImage;
		UpdateSubItem(pItem->iItem, pItem->iSubItem);
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemImage
BOOL CXListCtrl::SetItemImage(int nItem, int nSubItem, int nImage)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	BOOL rc = TRUE;

	if (nItem < 0)
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].nImage = nImage;
	}

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	BOOL rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);
   
	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// SetButtonText
BOOL CXListCtrl::SetButtonText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD && pXLCD[nSubItem].pButton)
	{
		pXLCD[nSubItem].pButton->SetWindowText(lpszText);
	}
	else
	{
		return FALSE;
	}

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetButtonText
CString CXListCtrl::GetButtonText(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return _T("");
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return _T("");

	CString str = _T("");
   
	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD && pXLCD[nSubItem].pButton)
	{
		str = pXLCD[nSubItem].pButton->GetWindowText();
	}

	return str;
}

///////////////////////////////////////////////////////////////////////////////
// SetButtonWidth
BOOL CXListCtrl::SetButtonWidth(int nItem, int nSubItem, int nFixedWidth)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD && pXLCD[nSubItem].pButton)
	{
		pXLCD[nSubItem].pButton->SetFixedWidth(nFixedWidth);
	}
	else
	{
		return FALSE;
	}

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetButtonWidth
int CXListCtrl::GetButtonWidth(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return 0;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD && pXLCD[nSubItem].pButton)
	{
		return pXLCD[nSubItem].pButton->GetFixedWidth();
	}
	else
	{
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetItemText
//
// This function will set the text and colors for a subitem.  If lpszText
// is NULL, only the colors will be set.  If a color value is -1, the display
// color will be set to the default Windows color.
//
BOOL CXListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText,
					COLORREF crText, COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	BOOL rc = TRUE;

	if (nItem < 0)
		return FALSE;

	if (lpszText)
		rc = CListCtrl::SetItemText(nItem, nSubItem, lpszText);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		pXLCD[nSubItem].crText       = (crText == -1) ? m_crWindowText : crText;
		pXLCD[nSubItem].crBackground = (crBackground == -1) ? m_crWindow : crBackground;
	}

	UpdateSubItem(nItem, nSubItem);

	return rc;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteItem
BOOL CXListCtrl::DeleteItem(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
		delete [] pXLCD;

	CListCtrl::SetItemData(nItem, 0);
	return CListCtrl::DeleteItem(nItem);
}

///////////////////////////////////////////////////////////////////////////////
// DeleteAllItems
BOOL CXListCtrl::DeleteAllItems()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		if (pXLCD)
			delete [] pXLCD;
		CListCtrl::SetItemData(i, 0);
	}

	return CListCtrl::DeleteAllItems();
}

///////////////////////////////////////////////////////////////////////////////
// OnDestroy
void CXListCtrl::OnDestroy()
{
	int n = GetItemCount();
	for (int i = 0; i < n; i++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(i);
		
		if (pXLCD)
			delete [] pXLCD;

		CListCtrl::SetItemData(i, 0);
	}

	m_bHeaderIsSubclassed = FALSE;

	CListCtrl::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
// SetEdit
BOOL CXListCtrl::SetEdit(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].bEdit = TRUE;

	UpdateSubItem(nItem, nSubItem);

#ifdef _DEBUG

	DWORD dwStyle = GetStyle();
	if (dwStyle & LVS_EDITLABELS)
	{
		TRACE(_T("LVS_EDITLABELS is not necessary\n"));
	}

#endif

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetProgress
//
// This function creates a progress bar in the specified subitem.  The
// UpdateProgress function may then be called to update the progress
// percent.  If bShowProgressText is TRUE, either the default text
// of "n%" or the custom percent text (lpszProgressText) will be
// displayed.  If bShowProgressText is FALSE, only the progress bar
// will be displayed, with no text.
//
// Note that the lpszProgressText string should include the format
// specifier "%d":  e.g., "Pct %d%%"
//
BOOL CXListCtrl::SetProgress(int nItem,
							 int nSubItem,
							 BOOL bShowProgressText /*= TRUE*/,
							 LPCTSTR lpszProgressText /*= NULL*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].bShowProgress        = TRUE;
	pXLCD[nSubItem].nProgressPercent     = 0;
	pXLCD[nSubItem].bShowProgressMessage = bShowProgressText;
	pXLCD[nSubItem].strProgressMessage   = lpszProgressText;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteProgress
void CXListCtrl::DeleteProgress(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].bShowProgress = FALSE;
	pXLCD[nSubItem].nProgressPercent = 0;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// UpdateProgress
void CXListCtrl::UpdateProgress(int nItem, int nSubItem, int nPercent)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	ASSERT(nPercent >= 0 && nPercent <= 100);

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return;
	}

	pXLCD[nSubItem].nProgressPercent = nPercent;

	UpdateSubItem(nItem, nSubItem);
}

///////////////////////////////////////////////////////////////////////////////
// SetCheckbox
BOOL CXListCtrl::SetCheckbox(int nItem, int nSubItem, int nCheckedState)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;
	ASSERT((nCheckedState == XLISTCTRL_NO_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_CHECKED_CHECKBOX));

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update checkbox in subitem

	CXCheckbox* pCtrl = new CXCheckbox(this);
	ASSERT(pCtrl);
	if (pCtrl)
	{
		ASSERT(pXLCD[nSubItem].pCheckbox == NULL);
		pCtrl->SetCheck(nCheckedState);
		pXLCD[nSubItem].pCheckbox = pCtrl;
		pXLCD[0].bItemHasCheckbox = TRUE;
	}

	//pXLCD[nSubItem].nCheckedState = nCheckedState;

	UpdateSubItem(nItem, nSubItem);

	// now update checkbox in column header

	// -1 = no checkbox in column header
	if (GetHeaderCheckedState(nSubItem) != XLISTCTRL_NO_CHECKBOX)
	{
		int nCheckedCount = CountCheckedItems(nSubItem);

		if (nCheckedCount == GetItemCount())
			SetHeaderCheckedState(nSubItem, XLISTCTRL_CHECKED_CHECKBOX);
		else
			SetHeaderCheckedState(nSubItem, XLISTCTRL_UNCHECKED_CHECKBOX);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetEnabled
//
// Note that GetEnabled and SetEnabled only Get/Set the enabled flag from
// subitem 0, since this is a per-row flag.
//
BOOL CXListCtrl::GetEnabled(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	return pXLCD[0].bEnabled;
}

///////////////////////////////////////////////////////////////////////////////
// SetEnabled
BOOL CXListCtrl::SetEnabled(int nItem, BOOL bEnable)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[0].bEnabled = bEnable;

	if (pXLCD[0].bItemHasButton)
	{
		// item has button(s) - enable/disable them

		for (int i = 0; i < m_HeaderCtrl.GetItemCount(); i++)
		{
			if (pXLCD[i].pButton)
				pXLCD[i].pButton->Enable(bEnable);
		}
	}

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);
	InvalidateRect(&rect);
	UpdateWindow();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetAllowUserInput
BOOL CXListCtrl::GetAllowUserInput(int nItem, int nSubItem)
{
	ASSERT((nItem == -1) || (nItem >= 0));
	ASSERT(nItem < GetItemCount());
	if ((nItem < -1) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	BOOL bFlag = FALSE;

	if (nItem == -1)
	{
		// return global flag
		bFlag = m_bAllowUserInput;
	}
	else
	{
		// return subitem flag
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
			bFlag = pXLCD[nSubItem].bAllowUserInput;
	}

	return bFlag;
}

///////////////////////////////////////////////////////////////////////////////
// SetAllowUserInput
BOOL CXListCtrl::SetAllowUserInput(int nItem, int nSubItem, BOOL bAllowUserInput)
{
	ASSERT((nItem == -1) || (nItem >= 0));
	ASSERT(nItem < GetItemCount());
	if ((nItem < -1) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	if (nItem == -1)
	{
		// set global flag
		m_bAllowUserInput = bAllowUserInput;
	}
	else
	{
		// set subitem flag
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
			pXLCD[nSubItem].bAllowUserInput = bAllowUserInput;
		else
			return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetBold
BOOL CXListCtrl::SetBold(int nItem, int nSubItem, BOOL bBold)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update bold flag
	pXLCD[nSubItem].bBold = bBold;

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetBold
BOOL CXListCtrl::GetBold(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// return bold flag
	return pXLCD[nSubItem].bBold;
}

///////////////////////////////////////////////////////////////////////////////
// GetModified
BOOL CXListCtrl::GetModified(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// return modified flag
	return pXLCD[nSubItem].bModified;
}

///////////////////////////////////////////////////////////////////////////////
// SetModified
void CXListCtrl::SetModified(int nItem, int nSubItem, BOOL bModified)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		// set modified flag
		pXLCD[nSubItem].bModified = bModified;
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetCheckedState - Get checked state of a subitem
//
// Return values:
//     XLISTCTRL_NO_CHECKBOX			0
//     XLISTCTRL_UNCHECKED_CHECKBOX		1
//     XLISTCTRL_CHECKED_CHECKBOX		2
//
int CXListCtrl::GetCheckedState(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return XLISTCTRL_NO_CHECKBOX;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return XLISTCTRL_NO_CHECKBOX;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD || (pXLCD[nSubItem].pCheckbox == NULL))
	{
		return XLISTCTRL_NO_CHECKBOX;
	}

	// return checked state
	return pXLCD[nSubItem].pCheckbox->GetCheck();
}

///////////////////////////////////////////////////////////////////////////////
// SetCheckedState - see GetCheckedState() for values of nCheckedState
void CXListCtrl::SetCheckedState(int nItem, int nSubItem, int nCheckedState)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;
	ASSERT((nCheckedState == XLISTCTRL_NO_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_CHECKED_CHECKBOX));

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD && pXLCD[nSubItem].pCheckbox)
	{
		// update checked state
		pXLCD[nSubItem].pCheckbox->SetCheck(nCheckedState);

		UpdateSubItem(nItem, nSubItem);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetItemColors
BOOL CXListCtrl::GetItemColors(int nItem,
							   int nSubItem, 
							   COLORREF& crText, 
							   COLORREF& crBackground)
{
	crText = RGB(0,0,0);
	crBackground = RGB(0,0,0);
	
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	crText = pXLCD[nSubItem].crText;
	crBackground = pXLCD[nSubItem].crBackground;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemColors
void CXListCtrl::SetItemColors(int nItem,
							   int nSubItem,		// -1 = all subitems
							   COLORREF crText, 
							   COLORREF crBackground)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT((nSubItem >= 0) || (nSubItem == -1));
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < -1) || (nSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		if (nSubItem == -1)
		{
			// set all columns
			for (int i = 0; i < m_HeaderCtrl.GetItemCount(); i++)
			{
				pXLCD[i].crText = crText;
				pXLCD[i].crBackground = crBackground;
			}
		}
		else
		{
			// set only specified column
			pXLCD[nSubItem].crText = crText;
			pXLCD[nSubItem].crBackground = crBackground;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetComboBox
//
// Note:  SetItemText may also be used to set the initial combo selection.
//
BOOL CXListCtrl::SetComboBox(int nItem,
							 int nSubItem,
							 BOOL bEnableCombo,
							 CStringArray *psa,		// should not be allocated on stack
							 int nComboListHeight,
							 int nInitialComboSel,
							 BOOL bSort /*= FALSE*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;
	ASSERT(psa);
	if (!psa)
		return FALSE;
	ASSERT(nComboListHeight > 0);
	ASSERT(nInitialComboSel >= 0 && nInitialComboSel < psa->GetSize());
	if ((nInitialComboSel < 0) || (nInitialComboSel >= psa->GetSize()))
		nInitialComboSel = 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	// update flag
	pXLCD[nSubItem].bCombo = bEnableCombo;

	if (bEnableCombo)
	{
		// sort CStringArray before setting initial selection
		if (bSort)
			CSortCStringArray::SortCStringArray(psa);

		pXLCD[nSubItem].psa = psa;
		pXLCD[nSubItem].nComboListHeight = nComboListHeight;
		pXLCD[nSubItem].nInitialComboSel = nInitialComboSel;
		pXLCD[nSubItem].bSort = bSort;

		CString str = _T("");

		if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
			(pXLCD[nSubItem].psa->GetSize() > pXLCD[nSubItem].nInitialComboSel))
		{
			int index = pXLCD[nSubItem].nInitialComboSel;
			str = pXLCD[nSubItem].psa->GetAt(index);
		}

		SetItemText(nItem, nSubItem, str);
	}

	UpdateSubItem(nItem, nSubItem);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetComboText
//
// Actually this does nothing more than GetItemText()
//
CString	CXListCtrl::GetComboText(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return _T("");
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return _T("");

	CString str;
	str = _T("");

	str = GetItemText(nItem, nSubItem);

	return str;
}

///////////////////////////////////////////////////////////////////////////////
// SetCurSel
BOOL CXListCtrl::SetCurSel(int nItem, BOOL bEnsureVisible /*= FALSE*/)
{
	BOOL bRet = SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED,
		LVIS_FOCUSED | LVIS_SELECTED);

	if (bEnsureVisible)
		EnsureVisible(nItem, FALSE);

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// GetCurSel - returns selected item number, or -1 if no item selected
//
// Note:  for single-selection lists only
//
int CXListCtrl::GetCurSel()
{
	POSITION pos = GetFirstSelectedItemPosition();
	int nSelectedItem = -1;
	if (pos != NULL)
		nSelectedItem = GetNextSelectedItem(pos);
	return nSelectedItem;
}

///////////////////////////////////////////////////////////////////////////////
// UpdateSubItem
void CXListCtrl::UpdateSubItem(int nItem, int nSubItem)
{
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;

	CRect rect;
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
	{
		GetItemRect(nItem, &rect, LVIR_BOUNDS);
	}
	else
	{
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
	}

	// do not remove this line
	rect.InflateRect(2, 2);

	InvalidateRect(&rect);
	UpdateWindow();
}

///////////////////////////////////////////////////////////////////////////////
// GetColumns
int CXListCtrl::GetColumns()
{
	return CListCtrl::GetHeaderCtrl()->GetItemCount();
}

///////////////////////////////////////////////////////////////////////////////
// GetItemData
//
// The GetItemData and SetItemData functions allow for app-specific data
// to be stored, by using an extra field in the XLISTCTRLDATA struct.
//
DWORD CXListCtrl::GetItemData(int nItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return 0;
	}

	return pXLCD->dwItemData;
}

///////////////////////////////////////////////////////////////////////////////
// SetItemData
BOOL CXListCtrl::SetItemData(int nItem, DWORD dwData)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD->dwItemData = dwData;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetHeaderCheckedState
//
// The GetHeaderCheckedState and SetHeaderCheckedState may be used to toggle
// the checkbox in a column header.
//
// Return values:
//     XLISTCTRL_NO_CHECKBOX			0
//     XLISTCTRL_UNCHECKED_CHECKBOX		1
//     XLISTCTRL_CHECKED_CHECKBOX		2
//
int CXListCtrl::GetHeaderCheckedState(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return XLISTCTRL_NO_CHECKBOX;

	// use the image index to indicate the checked status
	HDITEM hditem;
	hditem.mask = HDI_IMAGE;
	m_HeaderCtrl.GetItem(nSubItem, &hditem);

	return hditem.iImage;
}

///////////////////////////////////////////////////////////////////////////////
// SetHeaderCheckedState - see GetHeaderCheckedState() for values 
//                         of nCheckedState
BOOL CXListCtrl::SetHeaderCheckedState(int nSubItem, int nCheckedState)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;
	ASSERT((nCheckedState == XLISTCTRL_NO_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) || 
		   (nCheckedState == XLISTCTRL_CHECKED_CHECKBOX));

	// use the image index to indicate the checked status
	HDITEM hditem;
	hditem.mask = HDI_IMAGE;
	hditem.iImage = nCheckedState;
	m_HeaderCtrl.SetItem(nSubItem, &hditem);
	m_HeaderCtrl.SetCheck(nSubItem, nCheckedState);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// OnColumnClick
BOOL CXListCtrl::OnColumnClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnColumnClick\n"));

	*pResult = 0;
	return FALSE;		// return FALSE to send message to parent also -
						// NOTE:  MSDN documentation is incorrect
}

///////////////////////////////////////////////////////////////////////////////
// CountCheckedItems
int CXListCtrl::CountCheckedItems(int nSubItem)
{
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return 0;

	int nCount = 0;

	for (int nItem = 0; nItem < GetItemCount(); nItem++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (!pXLCD || !pXLCD[nSubItem].pCheckbox)
		{
			continue;
		}

		if (pXLCD[nSubItem].pCheckbox->GetCheck() == XLISTCTRL_CHECKED_CHECKBOX)
			nCount++;
	}

	return nCount;
}

///////////////////////////////////////////////////////////////////////////////
// GetColors
void CXListCtrl::GetColors()
{
	m_cr3DFace              = ::GetSysColor(COLOR_3DFACE);
	m_cr3DHighLight         = ::GetSysColor(COLOR_3DHIGHLIGHT);
	m_cr3DShadow            = ::GetSysColor(COLOR_3DSHADOW);
	m_crActiveCaption       = ::GetSysColor(COLOR_ACTIVECAPTION);
	m_crBtnFace             = ::GetSysColor(COLOR_BTNFACE);
	m_crBtnShadow           = ::GetSysColor(COLOR_BTNSHADOW);
	m_crBtnText             = ::GetSysColor(COLOR_BTNTEXT);
	m_crGrayText            = ::GetSysColor(COLOR_GRAYTEXT);
	m_crHighLight           = ::GetSysColor(COLOR_HIGHLIGHT);
	m_crHighLightText       = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
	m_crInactiveCaption     = ::GetSysColor(COLOR_INACTIVECAPTION);
	m_crInactiveCaptionText = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);
	m_crWindow              = ::GetSysColor(COLOR_WINDOW);
	m_crWindowText          = ::GetSysColor(COLOR_WINDOWTEXT);
}

///////////////////////////////////////////////////////////////////////////////
// OnSysColorChange
void CXListCtrl::OnSysColorChange()
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnSysColorChange\n"));

	CListCtrl::OnSysColorChange();

	GetColors();
}

#ifndef DO_NOT_INCLUDE_XCOMBOLIST

///////////////////////////////////////////////////////////////////////////////
// OnTimer
//
// Timer usage:
//    1 - Unlock window updates - used to avoid flashing after combo is created
//        used to check if combo button needs to be unpressed,set in
//        OnLButtonDown (when combo button is clicked)
//    2 - used to close combo listbox, set in OnComboEscape (user hits Escape
//        or listbox loses focus)
//    3 - used to get combo listbox selection, then close combo listbox,
//        set in OnComboReturn and OnComboLButtonUp (user hits Enter
//        or clicks on item in listbox)
//    4 - used to get combo listbox selection, set in OnComboKeydown (for
//        example, user hits arrow key in listbox)
//
void CXListCtrl::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 1)			// timer set when combo is created
	{
		KillTimer(nIDEvent);
		UnlockWindowUpdate();
	}
	else if (nIDEvent == 2)		// close combo listbox
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 2 ~~~~~\n"));

		if (m_pCombo)
		{
			UpdateSubItem(m_nComboItem, m_nComboSubItem);
			m_pCombo->DestroyWindow();
			delete m_pCombo;
		}
		m_pCombo = NULL;
	}
	else if (nIDEvent == 3)		// get combo listbox selection, then close combo listbox
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 3 ~~~~~\n"));

		if (m_pCombo)
		{
			CString str;
			int i = m_pCombo->GetCurSel();
			if (i != LB_ERR)
			{
				m_pCombo->GetLBText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);

					if (str != m_strInitialString)
					{
						// string is not the same, mark item as modified

						XLISTCTRLDATA *pXLCD = 
							(XLISTCTRLDATA *) CListCtrl::GetItemData(m_nComboItem);

						if (pXLCD)
						{
							pXLCD[m_nComboSubItem].bModified = TRUE;
							m_bListModified = TRUE;
						}
					}

					SendRegisteredMessage(WM_XLISTCTRL_COMBO_SELECTION, m_nComboItem, m_nComboSubItem);
				}
			}

			m_pCombo->DestroyWindow();
			delete m_pCombo;
		}
		m_pCombo = NULL;
	}
	else if (nIDEvent == 4)		// get combo listbox selection
	{
		KillTimer(nIDEvent);

		XLISTCTRL_TRACE(_T("timer 4 ~~~~~\n"));

		if (m_pCombo)
		{
			CString str;
			int i = m_pCombo->GetCurSel();
			if (i != LB_ERR)
			{
				m_pCombo->GetLBText(i, str);

				if ((m_nComboItem >= 0 && m_nComboItem < GetItemCount()) &&
					(m_nComboSubItem >= 0 && m_nComboSubItem < GetColumns()))
				{
					SetItemText(m_nComboItem, m_nComboSubItem, str);

					UpdateSubItem(m_nComboItem, m_nComboSubItem);
				}
			}
		}
	}

	CListCtrl::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
// OnComboEscape
LRESULT CXListCtrl::OnComboEscape(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnComboEscape\n"));
	SetTimer(2, 50, NULL);
	//UpdateSubItem(m_nComboItem, m_nComboSubItem);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnComboComplete
LRESULT CXListCtrl::OnComboComplete(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnComboComplete\n"));
	SetTimer(3, 50, NULL);
	return 0;
}

#endif		// #ifndef DO_NOT_INCLUDE_XCOMBOLIST

#ifndef NO_XLISTCTRL_TOOL_TIPS

///////////////////////////////////////////////////////////////////////////////
// OnToolHitTest
int CXListCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = point;
	
	int nItem = ListView_SubItemHitTest(this->m_hWnd, &lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	//XLISTCTRL_TRACE(_T("in CToolTipListCtrl::OnToolHitTest: %d,%d\n"), nItem, nSubItem);

	UINT nFlags = lvhitTestInfo.flags;

	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tool tips for
		
		// get the client (area occupied by this control
		RECT rcClient;
		GetClientRect(&rcClient);
		
		// fill in the TOOLINFO structure
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT) (nItem * 1000 + nSubItem + 1);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = rcClient;
		
		return pTI->uId;	// By returning a unique value per listItem,
							// we ensure that when the mouse moves over another
							// list item, the tooltip will change
	}
	else
	{
		//Otherwise, we aren't interested, so let the message propagate
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnToolTipText
BOOL CXListCtrl::OnToolTipText(UINT /*id*/, NMHDR * pNMHDR, LRESULT * pResult)
{
	UINT nID = pNMHDR->idFrom;
	//XLISTCTRL_TRACE(_T("in CXListCtrl::OnToolTipText: id=%d\n"), nID);
	
	// check if this is the automatic tooltip of the control
	if (nID == 0) 
		return TRUE;	// do not allow display of automatic tooltip,
						// or our tooltip will disappear
	
	// handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	
	*pResult = 0;
	
	// get the mouse position
	const MSG* pMessage;
	pMessage = GetCurrentMessage();
	ASSERT(pMessage);
	CPoint pt;
	pt = pMessage->pt;		// get the point from the message
	ScreenToClient(&pt);	// convert the point's coords to be relative to this control
	
	// see if the point falls onto a list item
	
	LVHITTESTINFO lvhitTestInfo;
	
	lvhitTestInfo.pt = pt;
	
	int nItem = SubItemHitTest(&lvhitTestInfo);
	int nSubItem = lvhitTestInfo.iSubItem;
	
	UINT nFlags = lvhitTestInfo.flags;
	
	// nFlags is 0 if the SubItemHitTest fails
	// Therefore, 0 & <anything> will equal false
	if (nFlags & LVHT_ONITEMLABEL)
	{
		// If it did fall on a list item,
		// and it was also hit one of the
		// item specific subitems we wish to show tooltips for
		
		CString strToolTip;
		strToolTip = _T("");

		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
		{
			strToolTip = pXLCD[nSubItem].strToolTip;
		}

		if (!strToolTip.IsEmpty())
		{
			// If there was a CString associated with the list item,
			// copy it's text (up to 80 characters worth, limitation 
			// of the TOOLTIPTEXT structure) into the TOOLTIPTEXT 
			// structure's szText member
			
#ifndef _UNICODE
			if (pNMHDR->code == TTN_NEEDTEXTA)
				lstrcpyn(pTTTA->szText, strToolTip, 80);
			else
				_mbstowcsz(pTTTW->szText, strToolTip, 80);
#else
			if (pNMHDR->code == TTN_NEEDTEXTA)
				_wcstombsz(pTTTA->szText, strToolTip, 80);
			else
				lstrcpyn(pTTTW->szText, strToolTip, 80);
#endif
			return FALSE;	 // we found a tool tip,
		}
	}
	
	return FALSE;	// we didn't handle the message, let the 
					// framework continue propagating the message
}

///////////////////////////////////////////////////////////////////////////////
// SetItemToolTipText
BOOL CXListCtrl::SetItemToolTipText(int nItem, int nSubItem, LPCTSTR lpszToolTipText)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	pXLCD[nSubItem].strToolTip = lpszToolTipText;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// GetItemToolTipText
CString CXListCtrl::GetItemToolTipText(int nItem, int nSubItem)
{
	CString strToolTip;
	strToolTip = _T("");

	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return strToolTip;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return strToolTip;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (pXLCD)
	{
		strToolTip = pXLCD[nSubItem].strToolTip;
	}

	return strToolTip;
}

///////////////////////////////////////////////////////////////////////////////
// DeleteAllToolTips
void CXListCtrl::DeleteAllToolTips()
{
	int nRow = GetItemCount();
	int nCol = GetColumns();

	for (int nItem = 0; nItem < nRow; nItem++)
	{
		XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
		if (pXLCD)
			for (int nSubItem = 0; nSubItem < nCol; nSubItem++)
				pXLCD[nSubItem].strToolTip = _T("");
	}
}

#endif

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
///////////////////////////////////////////////////////////////////////////////
// DrawComboBox
void CXListCtrl::DrawComboBox(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	// Make sure that nSubItem is valid
	ASSERT(GetColumnWidth(nSubItem) >= 5);
	if (GetColumnWidth(nSubItem) < 5)
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pXLCD[0].bEnabled)
		return;

#ifdef _DEBUG
	DWORD dwExStyle = GetExtendedStyle();
	if ((dwExStyle & LVS_EX_FULLROWSELECT) == 0)
	{
		XLISTCTRL_TRACE(_T("XListCtrl: combo boxes require LVS_EX_FULLROWSELECT style\n"));
		ASSERT(FALSE);
	}
#endif

	// Get the column offset
	int offset = 0;
	for (int i = 0; i < nSubItem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

	CRect rectClient;
	GetClientRect(&rectClient);

	m_pCombo = new CXCombo(this);
	ASSERT(m_pCombo);

	if (m_pCombo)
	{
		m_nComboItem = nItem;
		m_nComboSubItem = nSubItem;

		rect.top -= 1;
		m_rectComboList = rect;
		ClientToScreen(&m_rectComboList);
		m_rectComboList.left += 1;

		DWORD dwStyle = CBS_DROPDOWNLIST | WS_POPUP | WS_VISIBLE ;

		BOOL bSuccess = m_pCombo->CreateEx(WS_EX_CONTROLPARENT, 
										   ADVCOMBOBOXCTRL_CLASSNAME, 
										   _T(""),
										   dwStyle,
										   m_rectComboList,
										   this, 
										   0, 
										   NULL);

		if (bSuccess)
		{
			LockWindowUpdate();
			if (pXLCD[nSubItem].psa)
			{
				// add strings to combo

				CString s = _T("");
				try
				{
					for (int i = 0; i < pXLCD[nSubItem].psa->GetSize(); i++)
					{
						s = pXLCD[nSubItem].psa->GetAt(i);
						if (!s.IsEmpty())
							m_pCombo->AddString(s);
					}
				}
				catch(...)
				{
					TRACE(_T("ERROR - exception in CXListCtrl::DrawComboBox\n"));
					TRACE(_T("==>  Attempting to access psa pointer;  string array must be a class\n"));
					TRACE(_T("==>  variable, a global variable, or allocated on the heap.\n"));
				}
			}

			m_pCombo->SetDefaultVisibleItems(pXLCD[nSubItem].nComboListHeight);

			int index = 0;

			CString str = _T("");

			// Note that strings in combo are sorted in CXListCtrl::SetComboBox()

			if (pXLCD[nSubItem].psa)
			{
				try
				{
					if ((pXLCD[nSubItem].nInitialComboSel >= 0) &&
						(m_pCombo->GetCount() > pXLCD[nSubItem].nInitialComboSel))
					{
						index = pXLCD[nSubItem].nInitialComboSel;
						m_pCombo->GetLBText(index, str);
						XLISTCTRL_TRACE(_T("nInitialComboSel=%d  str=<%s>\n"), index, str);
						SetItemText(nItem, nSubItem, str);
						pXLCD[nSubItem].nInitialComboSel = -1; // default after first time
					}
				}
				catch(...)
				{
					TRACE(_T("ERROR - exception in CXListCtrl::DrawComboBox\n"));
					TRACE(_T("==>  Attempting to access psa pointer;  string array must be a class\n"));
					TRACE(_T("==>  variable, a global variable, or allocated on the heap.\n"));
				}
			}

			if (str.IsEmpty())
				str = GetItemText(nItem, nSubItem);

			if (str.IsEmpty())
			{
				// str is empty, try to get from first listbox string
				if (m_pCombo->GetCount() > 0)
				{
					m_pCombo->GetLBText(0, str);
					index = 0;
				}

				SetItemText(nItem, nSubItem, str);
			}
			else
			{
				// set listbox selection from subitem text
				index = m_pCombo->FindStringExact(-1, str);
				XLISTCTRL_TRACE(_T("FindStringExact returned %d\n"), index);

				if (index == LB_ERR)
					index = 0;
			}
			m_pCombo->SetCurSel(index);
			m_pCombo->GetLBText(index, m_strInitialString);

			SetTimer(1, 50, NULL);
		}

		m_pCombo->Invalidate();
		m_pCombo->RedrawWindow();
		m_pCombo->BringWindowToTop();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
// DrawEdit - Start edit of a sub item label
//     nItem    - The row index of the item to edit
//     nSubItem - The column of the sub item.
void CXListCtrl::DrawEdit(int nItem, int nSubItem)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::DrawEdit\n"));

	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return;

	// Make sure that nSubItem is valid
	ASSERT(GetColumnWidth(nSubItem) >= 5);
	if (GetColumnWidth(nSubItem) < 5)
		return;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		ASSERT(FALSE);
		return;
	}

	if (!pXLCD[0].bEnabled)
		return;

#ifdef _DEBUG
	DWORD dwExStyle = GetExtendedStyle();
	if ((dwExStyle & LVS_EX_FULLROWSELECT) == 0)
	{
		XLISTCTRL_TRACE(_T("XListCtrl: edit boxes require LVS_EX_FULLROWSELECT style\n"));
		ASSERT(FALSE);
	}
#endif

	// make sure that the item is visible
	if (!EnsureVisible(nItem, TRUE)) 
		return;

	// get the column offset
	int offset = 0;
	for (int i = 0; i < nSubItem; i++)
		offset += GetColumnWidth(i);

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// now scroll if we need to expose the column
	CRect rectClient;
	GetClientRect(&rectClient);
	if (offset + rect.left < 0 || offset + rect.left > rectClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nSubItem, &lvcol);
	DWORD dwStyle = 0;
	if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if ((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	dwStyle |= WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT /*| WS_BORDER*/ ;

	//rect.top -= 2;
	rect.bottom += 1;
	rect.left += offset + 2;
	rect.right = rect.left + GetColumnWidth(nSubItem) - 4;

	if (rect.right > rectClient.right) 
		rect.right = rectClient.right;

	m_strInitialString = GetItemText(nItem, nSubItem);

	m_bCancelEditLabel = FALSE;

	ASSERT(m_pEdit == NULL);

	// need to create edit control before sending the LVN_BEGINLABELEDIT
	// notification message, since parent may try to use LVM_GETEDITCONTROL
	// in his LVN_BEGINLABELEDIT handler
	BOOL bCreateOk = FALSE;
	m_pEdit = new CXEdit(this, m_strInitialString);		
	if (m_pEdit)
	{
		bCreateOk = m_pEdit->Create(dwStyle, rect, this, 99);
#ifdef _DEBUG
		if (bCreateOk)
		{
			XLISTCTRL_TRACE(_T("edit hwnd=0x%X\n"), m_pEdit->m_hWnd);
		}
		else
		{
			TRACE(_T("ERROR - edit Create() failed\n"));
		}
#endif
	}

	// send LVN_BEGINLABELEDIT to parent
	BOOL bRet = SendLabelEditMessage(LVN_BEGINLABELEDIT, nItem, nSubItem, m_strInitialString, FALSE);

	// if parent returns TRUE, don't allow edit

	if (bRet)
	{
		XLISTCTRL_TRACE(_T("parent disallowed edit for %d,%d\n"), nItem, nSubItem);
		if (bCreateOk)
			m_pEdit->DestroyWindow();
		delete m_pEdit;
		m_pEdit = NULL;
	}
	else
	{
		// parent allowed edit
		if (m_pEdit && bCreateOk)
		{
			m_nEditItem = nItem;
			m_nEditSubItem = nSubItem;
			m_pEdit->SetFocus();
		}
		else
		{
			// something failed
			if (bCreateOk)
				m_pEdit->DestroyWindow();
			delete m_pEdit;
			m_pEdit = NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnXEditEscape
LRESULT CXListCtrl::OnXEditEscape(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnXEditEscape\n"));

	// send LVN_ENDLABELEDIT to parent
	SendLabelEditMessage(LVN_ENDLABELEDIT, m_nEditItem, m_nEditSubItem, NULL, TRUE);

	if (m_pEdit && ::IsWindow(m_pEdit->m_hWnd))
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
	m_pEdit = NULL;

	// restore original string
	SetItemText(m_nEditItem, m_nEditSubItem, m_strInitialString);

	UpdateSubItem(m_nEditItem, m_nEditSubItem);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnXEditKillFocus
LRESULT CXListCtrl::OnXEditKillFocus(WPARAM, LPARAM)
{
	static BOOL bInOnXEditKillFocus = FALSE;

	if (bInOnXEditKillFocus || m_bCancelEditLabel)
		return 0;

	bInOnXEditKillFocus = TRUE;

	XLISTCTRL_TRACE(_T("in CXListCtrl::OnXEditKillFocus\n"));

	CString str = _T("");

	if (m_pEdit && ::IsWindow(m_pEdit->m_hWnd))
	{
		m_pEdit->GetWindowText(str);

		// send LVN_ENDLABELEDIT to parent
		BOOL bRet = SendLabelEditMessage(LVN_ENDLABELEDIT, m_nEditItem, m_nEditSubItem, str, FALSE);

		m_pEdit->DestroyWindow();		//-----
		delete m_pEdit;

		m_pEdit = NULL;

		if (bRet)
		{
			// set new string in subitem
			SetItemText(m_nEditItem, m_nEditSubItem, str);

			if (str != m_strInitialString)
			{
				XLISTCTRL_TRACE(_T("m_strInitialString=<%s>\n"), m_strInitialString);

				// string is not the same, mark item as modified

				XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(m_nEditItem);
				if (pXLCD)
				{
					pXLCD[m_nEditSubItem].bModified = TRUE;
					m_bListModified = TRUE;
				}
			}
		}
		else
		{
			XLISTCTRL_TRACE(_T("parent disallowed new text for %d,%d\n"), m_nEditItem, m_nEditSubItem);
		}

		UpdateSubItem(m_nEditItem, m_nEditSubItem);

		SendRegisteredMessage(WM_XLISTCTRL_EDIT_END, m_nEditItem, m_nEditSubItem);
	}

	bInOnXEditKillFocus = FALSE;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SendLabelEditMessage
BOOL CXListCtrl::SendLabelEditMessage(UINT nCode, 
									  int nItem, 
									  int nSubItem, 
									  LPCTSTR lpszText, 
									  BOOL bCancel)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::SendLabelEditMessage\n"));

	CString str(lpszText);

	BOOL bRet = TRUE;

	CWnd *pWnd = GetParent();
	if (!pWnd)
		pWnd = GetOwner();
	if (pWnd && ::IsWindow(pWnd->m_hWnd))
	{
		NMLVDISPINFO dispinfo;
		dispinfo.hdr.hwndFrom    = m_hWnd;
		dispinfo.hdr.idFrom      = GetDlgCtrlID();
		dispinfo.hdr.code        = nCode;
		dispinfo.item.mask       = LVIF_TEXT;
		dispinfo.item.iItem      = nItem;
		dispinfo.item.iSubItem   = nSubItem;
		dispinfo.item.pszText    = bCancel ? NULL : (LPTSTR)((LPCTSTR)str);
		dispinfo.item.cchTextMax = bCancel ? 0 : str.GetLength();

		XLISTCTRL_TRACE(_T("sending WM_NOTIFY\n"));
		bRet = pWnd->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// OnCancelEditLabel - this message is sent by the parent if it wants to 
//                     cancel the ongoing edit
LRESULT CXListCtrl::OnCancelEditLabel(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnCancelEditLabel\n"));
	m_bCancelEditLabel = TRUE;
	OnXEditEscape(0, 0);
	return CListCtrl::Default();
}

///////////////////////////////////////////////////////////////////////////////
// OnGetEditControl - this message is sent by the parent if it wants the 
//                    handle for the edit control
LRESULT CXListCtrl::OnGetEditControl(WPARAM, LPARAM)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnGetEditControl\n"));
	HWND hwndEdit = NULL;
	if (m_pEdit)
		hwndEdit = m_pEdit->m_hWnd;
	return (LRESULT)hwndEdit;
}

///////////////////////////////////////////////////////////////////////////////
// OnEraseBkgnd
BOOL CXListCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClip, rectTop, rectBottom, rectRight;
	int nItemCount = GetItemCount();

	if (!nItemCount) // Empty XListCtrl, nothing to do, CListCtrl will
		return CListCtrl::OnEraseBkgnd(pDC); // erase the Background

	if (pDC->GetClipBox(&rectClip) == ERROR)
	{
		ASSERT(false);
		return CListCtrl::OnEraseBkgnd(pDC);
	}

	int nFirstRow = GetTopIndex();
	int nLastRow = nFirstRow + GetCountPerPage();
	nLastRow = min (nLastRow, nItemCount - 1); // Last Item displayed in Ctrl

	CListCtrl::GetSubItemRect(nFirstRow, 0, LVIR_BOUNDS, rectTop);
	CListCtrl::GetSubItemRect(nLastRow, 0, LVIR_BOUNDS, rectBottom);

	CRect rectEraseTop = rectClip;
	rectEraseTop.bottom = rectTop.top;
	pDC->FillSolidRect(rectEraseTop, m_crWindow);

	CRect rectEraseBottom = rectClip;
	rectEraseBottom.top = rectBottom.bottom;
	pDC->FillSolidRect(rectEraseBottom, m_crWindow);

	CRect rectEraseRight = rectClip;
	rectEraseRight.top = rectTop.top;
	rectEraseRight.bottom = rectBottom.bottom;
	rectEraseRight.left = rectTop.right;
	pDC->FillSolidRect(rectEraseRight, m_crWindow);

	return TRUE;

	//return CListCtrl::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// FindDataItem
int CXListCtrl::FindDataItem(DWORD dwData)
{
	for (int nItem = 0; nItem < GetItemCount(); nItem++) 
	{
		if (GetItemData(nItem) == dwData)
			return nItem;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////
// OnKeyDown - check for disabled items
void CXListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnKeyDown\n"));

	int nOldItem = GetCurSel();

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	int nNewItem = GetCurSel();

	if (nNewItem >= 0)
	{
		int nTrial = nNewItem;

		BOOL bEnabled = GetEnabled(nTrial);

		if (!bEnabled)
		{
			// item is disabled, try another

			int nCount = GetItemCount();

			if (nChar == VK_DOWN || nChar == VK_NEXT)
			{
				int nDirection = 1;

				while (!bEnabled)
				{
					nTrial += 1 * nDirection;

					if (nTrial >= nCount)
					{
						// at the end, back up
						nTrial = nCount;
						nDirection = -1;
						continue;
					}
					else if (nTrial < 0)
					{
						// at beginning - must have been backing up
						nTrial = nOldItem;
						break;
					}

					bEnabled = GetEnabled(nTrial);
				}
			}
			else if (nChar == VK_UP || nChar == VK_PRIOR)
			{
				int nDirection = -1;

				while (!bEnabled)
				{
					nTrial += 1 * nDirection;

					if (nTrial < 0)
					{
						// at the beginning, go forward
						nTrial = 0;
						nDirection = 1;
						continue;
					}
					else if (nTrial >= nCount)
					{
						// at end - must have been going forward
						nTrial = nOldItem;
						break;
					}

					bEnabled = GetEnabled(nTrial);
				}
			}
			else
			{
				// don't know how user got here, just go back to previous
				nTrial = nOldItem;
			}
		}

		SetCurSel(nTrial, TRUE);	// set new selection, scroll into view
	}
}

///////////////////////////////////////////////////////////////////////////////
// SetButton - set button in subitem
BOOL  CXListCtrl::SetButton(int nItem, int nSubItem, LPCTSTR lpszButtonText, int nFixedWidth /*= 0*/)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return FALSE;
	}

	//pXLCD[nSubItem].bButton = TRUE;

	CRect rect;
	GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);

	CXButton* pCtrl = new CXButton(this, /*nItem, nSubItem,*/ lpszButtonText, nFixedWidth);
	ASSERT(pCtrl);
	if (pCtrl)
	{
		ASSERT(pXLCD[nSubItem].pButton == NULL);
		pXLCD[nSubItem].pButton = pCtrl;
		pXLCD[0].bItemHasButton = TRUE;
	}

	// create image list - this will give more height for button
	if (GetImageList(LVSIL_SMALL) == NULL)
	{
		if (m_nItemHeight == -2)
		{
			// auto adjust item height for buttons, depending on font
			CDC *pDC = GetDC();
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);
			ReleaseDC(pDC);
			XLISTCTRL_TRACE(_T("tm.tmHeight=%d\n"), tm.tmHeight);
			m_nItemHeight = tm.tmHeight + 5;	// allow for button borders
		}

		if (m_nItemHeight > 0)
		{
			m_defImageList.Create(1, m_nItemHeight, ILC_COLOR, 1, 1);
			SetImageList(&m_defImageList, LVSIL_SMALL);
		}
	}
   
   UpdateSubItem(nItem, nSubItem);

   return true;
}

///////////////////////////////////////////////////////////////////////////////
// OnHeaderCheckboxClicked
LRESULT CXListCtrl::OnHeaderCheckboxClicked(WPARAM wParam, LPARAM lParam)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnHeaderCheckboxClicked\n"));

	int nItem = (int)wParam;		// always -1
	int nSubItem = (int)lParam;		// column

	ASSERT(nItem == -1);
	if (nItem != -1)
		return 0;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return 0;

	SendRegisteredMessage(WM_XLISTCTRL_CHECKBOX_CLICKED, nItem, nSubItem);

	int nCheckedState = GetHeaderCheckedState(nSubItem);

	// 0 = no checkbox
	if (nCheckedState != XLISTCTRL_NO_CHECKBOX)
	{
		// flip check
		nCheckedState = (nCheckedState == XLISTCTRL_UNCHECKED_CHECKBOX) ? 
											XLISTCTRL_CHECKED_CHECKBOX : XLISTCTRL_UNCHECKED_CHECKBOX;
		SetHeaderCheckedState(nSubItem, nCheckedState);

		m_HeaderCtrl.UpdateWindow();

		LockWindowUpdate();

		for (int nItem = 0; nItem < GetItemCount(); nItem++)
		{
			XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
			if (!pXLCD || !pXLCD[nSubItem].pCheckbox)
			{
				continue;
			}

			if (pXLCD[nSubItem].pCheckbox->GetCheck() != XLISTCTRL_NO_CHECKBOX)
			{
				pXLCD[nSubItem].pCheckbox->SetCheck(nCheckedState);		// same value as header
				pXLCD[nSubItem].bModified = TRUE;
				m_bListModified = TRUE;
			}
		}

		UnlockWindowUpdate();

		if (m_bListModified)
		{
			CRect rectHeader;
			m_HeaderCtrl.GetItemRect(nSubItem, &rectHeader);
			CRect rectClient;
			GetClientRect(&rectClient);

			// invalidate only this column
			rectClient.left = rectHeader.left;
			rectClient.right = rectHeader.right;

			InvalidateRect(&rectClient, FALSE);
			UpdateWindow();
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnXButtonClicked
LRESULT CXListCtrl::OnXButtonClicked(WPARAM wParam, LPARAM lParam)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::OnXButtonClicked\n"));

	int nItem = (int)wParam;
	int nSubItem = (int)lParam;

	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return 0;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return 0;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);
	if (!pXLCD)
	{
		return 0;
	}

	if (!pXLCD[0].bEnabled)
		return 0;

	SendRegisteredMessage(WM_XLISTCTRL_BUTTON_CLICKED, nItem, nSubItem);

	SetCurSel(nItem);
	SetFocus();

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ResetCurrentButton
void CXListCtrl::ResetCurrentButton()
{
	if ((m_nCurrentButtonItem < 0) || (m_nCurrentButtonItem >= GetItemCount()))
		return;
	if ((m_nCurrentButtonSubItem < 0) || (m_nCurrentButtonSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA * pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(m_nCurrentButtonItem);
	if (pXLCD && pXLCD[m_nCurrentButtonSubItem].pButton)
	{
		pXLCD[m_nCurrentButtonSubItem].pButton->SetMouseOver(FALSE);
		pXLCD[m_nCurrentButtonSubItem].pButton->SetPressed(FALSE);
	}
	UpdateSubItem(m_nCurrentButtonItem, m_nCurrentButtonSubItem);

	m_nCurrentButtonItem = -1;
	m_nCurrentButtonSubItem = -1;
}

///////////////////////////////////////////////////////////////////////////////
// ResetCurrentCheckbox
void CXListCtrl::ResetCurrentCheckbox()
{
	if ((m_nCurrentCheckboxItem < 0) || (m_nCurrentCheckboxItem >= GetItemCount()))
		return;
	if ((m_nCurrentCheckboxSubItem < 0) || (m_nCurrentCheckboxSubItem >= GetColumns()))
		return;

	XLISTCTRLDATA * pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(m_nCurrentCheckboxItem);
	if (pXLCD && pXLCD[m_nCurrentCheckboxSubItem].pCheckbox)
	{
		pXLCD[m_nCurrentCheckboxSubItem].pCheckbox->SetMouseOver(FALSE);
		pXLCD[m_nCurrentCheckboxSubItem].pCheckbox->SetPressed(FALSE);
	}
	UpdateSubItem(m_nCurrentCheckboxItem, m_nCurrentCheckboxSubItem);

	m_nCurrentCheckboxItem = -1;
	m_nCurrentCheckboxSubItem = -1;
}

///////////////////////////////////////////////////////////////////////////////
// ProcessMouseOverButton
void CXListCtrl::ProcessMouseOverButton(int nItem, int nSubItem, CPoint point, XLISTCTRLDATA *pXLCD)
{
	//XLISTCTRL_TRACE(_T("in CXListCtrl::ProcessMouseOverButton\n"));

	if (!pXLCD[0].bEnabled || (pXLCD[nSubItem].pButton == NULL))
	{
		// cursor is over a disabled item, or there is no button
		if ((m_nCurrentButtonItem != nItem) || (m_nCurrentButtonSubItem != nSubItem))
		{
			ResetCurrentButton();
		}
		m_nCurrentButtonItem = -1;
		m_nCurrentButtonSubItem = -1;
	}
	else
	{
		// save current state flags
		BOOL bMouseOver = pXLCD[nSubItem].pButton->GetMouseOver();
		BOOL bIsPressed = pXLCD[nSubItem].pButton->GetPressed();

		if (PtInButtonRect(nItem, nSubItem, point))
		{
			//XLISTCTRL_TRACE(_T("mouse over button (%d,%d)~~~~~\n"), nItem, nSubItem);

			pXLCD[nSubItem].pButton->SetMouseOver(TRUE);
			SetCapture();

			if ((m_nCurrentButtonItem != nItem) || (m_nCurrentButtonSubItem != nSubItem))
			{
				ResetCurrentButton();
			}

			// set new active button
			m_nCurrentButtonItem = nItem;
			m_nCurrentButtonSubItem = nSubItem;
		}
		else
		{
			//XLISTCTRL_TRACE(_T("mouse NOT over button ~~~~~\n"));

			ResetCurrentButton();
		}

		if ((bMouseOver != pXLCD[nSubItem].pButton->GetMouseOver()) || 
			(bIsPressed != pXLCD[nSubItem].pButton->GetPressed()))
		{
			// state has changed, repaint button
			UpdateSubItem(nItem, nSubItem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// ProcessMouseOverCheckbox
void CXListCtrl::ProcessMouseOverCheckbox(int nItem, int nSubItem, CPoint point, XLISTCTRLDATA *pXLCD)
{
	//XLISTCTRL_TRACE(_T("in CXListCtrl::ProcessMouseOverCheckbox\n"));

	if (!pXLCD[0].bEnabled || (pXLCD[nSubItem].pCheckbox == NULL))
	{
		// cursor is over a disabled item, or there is no checkbox
		if ((m_nCurrentCheckboxItem != nItem) || (m_nCurrentCheckboxSubItem != nSubItem))
		{
			ResetCurrentCheckbox();
		}
		m_nCurrentCheckboxItem = -1;
		m_nCurrentCheckboxSubItem = -1;
	}
	else
	{
		// save current state flags
		BOOL bMouseOver = pXLCD[nSubItem].pCheckbox->GetMouseOver();
		BOOL bIsPressed = pXLCD[nSubItem].pCheckbox->GetPressed();

		if (PtInCheckboxRect(nItem, nSubItem, point))
		{
			//XLISTCTRL_TRACE(_T("mouse over checkbox (%d,%d)~~~~~\n"), nItem, nSubItem);

			pXLCD[nSubItem].pCheckbox->SetMouseOver(TRUE);
			SetCapture();

			if ((m_nCurrentCheckboxItem != nItem) || (m_nCurrentCheckboxSubItem != nSubItem))
			{
				ResetCurrentCheckbox();
			}

			// set new active checkbox
			m_nCurrentCheckboxItem = nItem;
			m_nCurrentCheckboxSubItem = nSubItem;
		}
		else
		{
			//XLISTCTRL_TRACE(_T("mouse NOT over button ~~~~~\n"));

			ResetCurrentCheckbox();
		}

		if ((bMouseOver != pXLCD[nSubItem].pCheckbox->GetMouseOver()) || 
			(bIsPressed != pXLCD[nSubItem].pCheckbox->GetPressed()))
		{
			// state has changed, repaint checkbox
			UpdateSubItem(nItem, nSubItem);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseMove
void CXListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	//XLISTCTRL_TRACE(_T("in CXListCtrl::OnMouseMove\n"));

	// check to see if mouse is over button

	int nItem = -1;
	int nSubItem = -1;
	XLISTCTRLDATA *pXLCD = NULL;

	LVHITTESTINFO lvhit;
	lvhit.pt = point;

	if ((SubItemHitTest(&lvhit) == -1) || 
		((lvhit.flags & LVHT_ONITEM) == 0) ||
		((lvhit.iItem < 0) || (lvhit.iItem >= GetItemCount())) ||
		((lvhit.iSubItem < 0) || (lvhit.iSubItem >= GetColumns())))
	{
		// mouse is not over an item --
		// reset pressed & mouse over states for previous button

		ResetCurrentButton();
		ResetCurrentCheckbox();
	}
	else
	{
		// mouse is over an item

		//XLISTCTRL_TRACE(_T("OnMouseMove:  lvhit.iItem=%d  lvhit.iSubItem=%d  ~~~~~\n"), lvhit.iItem, lvhit.iSubItem);

		nItem = lvhit.iItem;
		nSubItem = lvhit.iSubItem;

		pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);

		if (pXLCD)
		{
			ProcessMouseOverButton(nItem, nSubItem, point, pXLCD);

			ProcessMouseOverCheckbox(nItem, nSubItem, point, pXLCD);
		}
	}

	if ((m_nCurrentButtonItem == -1) && (m_nCurrentButtonSubItem == -1) &&
		(m_nCurrentCheckboxItem == -1) && (m_nCurrentCheckboxSubItem == -1))
	{
		ReleaseCapture();
	}

	CListCtrl::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
// PtInButtonRect
BOOL CXListCtrl::PtInButtonRect(int nItem, int nSubItem, POINT point)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);

	BOOL bRet = FALSE;

	if (pXLCD && pXLCD[nSubItem].pButton)
	{
		CRect rectButton(0,0,0,0);
		pXLCD[nSubItem].pButton->GetRect(rectButton);

		CRect rectItem;
		GetItemRect(nItem, &rectItem, LVIR_BOUNDS);

		// button height is same as item
		rectButton.top = rectItem.top;
		rectButton.bottom = rectItem.bottom;

		bRet = rectButton.PtInRect(point);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// PtInCheckboxRect
BOOL CXListCtrl::PtInCheckboxRect(int nItem, int nSubItem, POINT point)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || (nItem >= GetItemCount()))
		return FALSE;
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || (nSubItem >= GetColumns()))
		return FALSE;

	XLISTCTRLDATA *pXLCD = (XLISTCTRLDATA *) CListCtrl::GetItemData(nItem);

	BOOL bRet = FALSE;

	if (pXLCD && pXLCD[nSubItem].pCheckbox)
	{
		CRect rectCheckbox(0,0,0,0);
		pXLCD[nSubItem].pCheckbox->GetRect(rectCheckbox);

		CRect rectItem;
		GetItemRect(nItem, &rectItem, LVIR_BOUNDS);

		int h = XLISTCTRL_CHECKBOX_SIZE;			// height and width are the same
		rectCheckbox.top = rectItem.top + (rectItem.Height() - h) / 2;
		rectCheckbox.bottom = rectCheckbox.top + h;

		bRet = rectCheckbox.PtInRect(point);
	}

	return bRet;
}

///////////////////////////////////////////////////////////////////////////////
// OnMouseWheel - close combo and edit boxes
BOOL CXListCtrl::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	//XLISTCTRL_TRACE(_T("in CXListCtrl::OnMouseWheel\n"));

#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	if (m_pCombo)
		OnComboComplete(0, 0);
#endif

	OnXEditKillFocus(0, 0);

	return CListCtrl::OnMouseWheel(fFlags, zDelta, point);
}

///////////////////////////////////////////////////////////////////////////////
// SaveColumns
void CXListCtrl::SaveColumns(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	if (lpszSection && (lpszSection[0] != _T('\0')) &&
		lpszEntry && (lpszEntry[0] != _T('\0')))
	{
		CString strCol = _T("");

		for (int i = 0; i < GetColumns(); i++)
		{
			int cx = GetColumnWidth(i);
			CString str = _T("");
			str.Format(_T("%d"), cx);
			if (!strCol.IsEmpty())
				strCol += _T(',');
			strCol += str;
		}
		if (!strCol.IsEmpty())
		{
			XLISTCTRL_TRACE(_T("SaveColumns:  <%s>\n"), strCol);
			AfxGetApp()->WriteProfileString(lpszSection, lpszEntry, strCol);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// LoadColumns
BOOL CXListCtrl::LoadColumns(LPCTSTR lpszSection, LPCTSTR lpszEntry)
{
	XLISTCTRL_TRACE(_T("in CXListCtrl::LoadColumns\n"));

	BOOL bRet = FALSE;

	if (lpszSection && (lpszSection[0] != _T('\0')) &&
		lpszEntry && (lpszEntry[0] != _T('\0')))
	{
		CString strCol = AfxGetApp()->GetProfileString(lpszSection, lpszEntry, _T(""));
		XLISTCTRL_TRACE(_T("LoadColumns:  <%s>\n"), strCol);

		if (!strCol.IsEmpty())
		{
			TCHAR * seps = _T(",\n");
			int nSize = strCol.GetLength() + 2;
			TCHAR *s = new TCHAR [nSize];
			memset(s, 0, nSize * sizeof(TCHAR));
			_tcscpy(s, strCol);

			TCHAR * token = _tcstok(s, seps);

			for (int i = 0; i < GetColumns(); i++)
			{
				if (token == NULL)
					break;

				int cx = _ttoi(token);
				XLISTCTRL_TRACE(_T("col %d: cx=%d\n"), i, cx);

				SetColumnWidth(i, cx);

				token = _tcstok(NULL, seps);
			}
			delete [] s;
		}
		bRet = TRUE;
	}
	return bRet;
}
