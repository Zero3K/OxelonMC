// XListCtrl.h  Version 1.5
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
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

#ifndef XLISTCTRL_H
#define XLISTCTRL_H

#include "XHeaderCtrl.h"
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
#include "XCombo.h"
#include "AdvComboBox.h"
#endif
#include "XEdit.h"
#include "XButton.h"
#include "XCheckbox.h"
#include "XListCtrlLibDefs.h"


#if !defined XLISTCTRLLIB_EXPORTS && !defined XLISTCTRLLIB_STATIC
	XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_COMBO_SELECTION;
	XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_EDIT_END;
	XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_CHECKBOX_CLICKED;
	XLISTCTRLLIBDLLEXPORT UINT WM_XLISTCTRL_BUTTON_CLICKED;
#elif !defined XLISTCTRLLIB_EXPORTS && defined XLISTCTRLLIB_STATIC
	extern UINT WM_XLISTCTRL_COMBO_SELECTION;
	extern UINT WM_XLISTCTRL_EDIT_END;
	extern UINT WM_XLISTCTRL_CHECKBOX_CLICKED;
	extern UINT WM_XLISTCTRL_BUTTON_CLICKED;
#endif


///////////////////////////////////////////////////////////////////////////////
// CXListCtrl data

struct XLISTCTRLDATA
{
	// ctor
	XLISTCTRLDATA()
	{
		bEnabled             = TRUE;
		bAllowUserInput      = TRUE;
		bBold                = FALSE;
		bModified            = FALSE;
		nImage               = -1;
#ifndef NO_XLISTCTRL_TOOL_TIPS
		strToolTip           = _T("");
#endif
		bCombo               = FALSE;
		bSort                = FALSE;
		psa                  = NULL;
		nComboListHeight     = 10;
		nInitialComboSel     = -1;	// use default
		bEdit                = FALSE;
		crText               = ::GetSysColor(COLOR_WINDOWTEXT);
		crBackground         = ::GetSysColor(COLOR_WINDOW);

		bShowProgress        = FALSE;
		nProgressPercent     = 0;
		strProgressMessage   = _T("");
		bShowProgressMessage = TRUE;
		dwItemData           = 0;
		pButton              = NULL;
		bItemHasButton       = FALSE;
		pCheckbox            = NULL;
		bItemHasCheckbox     = FALSE;
		nFormat              = DT_DEFAULT;
	}

	// dtor
	virtual ~XLISTCTRLDATA()
	{
		if (pButton)
			delete pButton;
		pButton = NULL;
		if (pCheckbox)
			delete pCheckbox;
		pCheckbox = NULL;
	}

	BOOL			bEnabled;				// TRUE = enabled, FALSE = disabled (gray text)
	BOOL			bAllowUserInput;		// TRUE = normal state;  FALSE = user input
											// actions will be ignored for buttons,
											// checkboxes, edit boxes, and combo boxes
	BOOL			bBold;					// TRUE = display bold text
	BOOL			bModified;				// TRUE = subitem has been modified
	int				nImage;					// index in image list, else -1
#ifndef NO_XLISTCTRL_TOOL_TIPS
	CString			strToolTip;				// tool tip text for cell
#endif

	// for combo
	BOOL			bCombo;					// TRUE = display combobox
	BOOL			bSort;					// TRUE = add CBS_SORT style;  this means
											// that the list will be re-sorted on
											// each AddString()
	CStringArray *	psa;					// pointer to string array for combo listbox
	int				nComboListHeight;		// combo listbox height (in rows)
	int				nInitialComboSel;		// initial combo listbox selection (0 = first)

	//for edit
	BOOL			bEdit;					// TRUE = enable editing

	// for button
	CXButton *		pButton;				// pointer to CXButton object - note that
											// buttons are not like subitem combo or edit 
											// boxes - there can be multiple button objects
											// in existence at the same time, so the dtor
											// has to destroy them
	BOOL			bItemHasButton;			// set in subitem 0 - optimization to check 
											// if there are any buttons in item
	UINT			nFormat;				// subitem text alignment: DT_LEFT, DT_RIGHT,
											// or DT_CENTER;  defaults to DT_DEFAULT,
											// which means "use same alignment as header"

	// for color
	COLORREF		crText;
	COLORREF		crBackground;

	// for progress
	BOOL			bShowProgress;			// TRUE = show progress control
	int				nProgressPercent;		// 0 - 100
	CString			strProgressMessage;		// custom message for progress indicator -
											// MUST INCLUDE %d
	BOOL			bShowProgressMessage;	// TRUE = display % message, or custom message
											// if one is supplied
	// for checkbox
	CXCheckbox *	pCheckbox;				// pointer to CXCheckbox object - note that
											// checkboxes are not like subitem combo or edit 
											// boxes - there can be multiple checkbox objects
											// in existence at the same time, so the dtor
											// has to destroy them
	BOOL			bItemHasCheckbox;		// set in subitem 0 - optimization to check 
											// if there are any checkboxes in item

	DWORD			dwItemData;				// pointer to app's data
};


///////////////////////////////////////////////////////////////////////////////
// CXListCtrl class

class XLISTCTRLLIBDLLEXPORT CXListCtrl : public CListCtrl
{
// Construction
public:
	CXListCtrl();
	virtual ~CXListCtrl();

// Attributes
public:

// Operations
public:
	int		CountCheckedItems(int nSubItem);
	BOOL	DeleteAllItems();
	BOOL	DeleteColumn(int nCol);
	BOOL	DeleteItem(int nItem);
	void	DeleteProgress(int nItem, int nSubItem);
	int		FindDataItem(DWORD dwData);
	BOOL	GetAllowUserInput(int nItem, int nSubItem);
	BOOL	GetAllowUserInputHeader(int nCol) { return m_HeaderCtrl.GetAllowUserInput(nCol); }
	BOOL	GetBold(int nItem, int nSubItem);

	CString	GetButtonText(int nItem, int nSubItem);
	int		GetButtonWidth(int nItem, int nSubItem);

	int		GetCellPadding() { return m_nPadding; }
	int		GetCheckedState(int nItem, int nSubItem);
	int		GetColumns();
	CString	GetComboText(int nItem, int nSubItem);
	int		GetCurSel();
	BOOL	GetEllipsis() { return m_bUseEllipsis; }
	BOOL	GetEnabled(int nItem);
	DWORD	GetExtendedStyleX() { return m_dwExtendedStyleX; }

	UINT			GetHeaderAlignment() { return m_HeaderCtrl.GetAlignment(); }
	int				GetHeaderCheckedState(int nSubItem);
	CXHeaderCtrl *	GetXHeaderCtrl() { return &m_HeaderCtrl; }
	BOOL			GetHeaderItem(int nPos, HDITEM* pHeaderItem) { return m_HeaderCtrl.GetItem(nPos, pHeaderItem); }
	COLORREF		GetHeaderItemTextColor(int nSubItem) { return m_HeaderCtrl.GetItemColor(nSubItem); }
	int				GetHeaderSpacing() { return m_HeaderCtrl.GetSpacing(); }
	COLORREF		GetHeaderTextColor() { return m_HeaderCtrl.GetTextColor(); }

	BOOL	GetItemColors(int nItem,
						  int nSubItem, 
						  COLORREF& crText, 
						  COLORREF& crBackground);
	DWORD	GetItemData(int nItem);
	int		GetItemHeight() { return m_nItemHeight; }
	BOOL	GetListModified() { return m_bListModified; }
	BOOL	GetModified(int nItem, int nSubItem);
	UINT	GetSubItemAlignment(int nItem, int nSubItem);
	BOOL	GetSubItemRect(int nItem, int nSubItem, int nArea, CRect& rect);

	int		InsertColumn(int nCol, const LVCOLUMN* pColumn);
	int		InsertColumn(int nCol, 
						 LPCTSTR lpszColumnHeading, 
						 int nFormat = LVCFMT_LEFT, 
						 int nWidth = -1, 
						 int nSubItem = -1,
						 int nCheckboxState = XLISTCTRL_NO_CHECKBOX);

	int		InsertItem(int nItem, LPCTSTR lpszItem);
	int		InsertItem(int nItem, 
					   LPCTSTR lpszItem, 
					   COLORREF crText, 
					   COLORREF crBackground);
	int		InsertItem(const LVITEM* pItem);
	virtual BOOL LoadColumns(LPCTSTR lpszSection, LPCTSTR lpszEntry);
	virtual void SaveColumns(LPCTSTR lpszSection, LPCTSTR lpszEntry);
	BOOL	SetAllowUserInput(int nItem, int nSubItem, BOOL bAllowUserInput);
	BOOL	SetAllowUserInputHeader(int nCol, BOOL bAllowUserInput) 
			{ 
				return m_HeaderCtrl.SetAllowUserInput(nCol, bAllowUserInput); 
			}
	BOOL	SetBold(int nItem, int nSubItem, BOOL bBold);

	BOOL	SetButton(int nItem, int nSubItem, LPCTSTR lpszButtonText, int nFixedWidth = 0);
	BOOL	SetButtonText(int nItem, int nSubItem, LPCTSTR lpszText);
	BOOL	SetButtonWidth(int nItem, int nSubItem, int nFixedWidth);

	void	SetCellPadding(int nPadding) { m_nPadding = nPadding; }
	BOOL	SetComboBox(int nItem, 
						int nSubItem, 
						BOOL bEnableCombo, 
						CStringArray *psa,
						int nComboListHeight,
						int nInitialComboSel,
						BOOL bSort = FALSE);
	BOOL	SetCheckbox(int nItem, int nSubItem, int nCheckedState);
	void	SetCheckedState(int nItem, int nSubItem, int nCheckedState);

#if 0  // -----------------------------------------------------------
	virtual int SetColumnWidth(int nCol, int cx)
	{
		CListCtrl::SetColumnWidth(nCol, cx);
		int w = CListCtrl::GetColumnWidth(nCol);
		return CListCtrl::SetColumnWidth(nCol, w + (m_HeaderCtrl.GetSpacing()*2));
	}
#endif // -----------------------------------------------------------
	BOOL	SetCurSel(int nItem, BOOL bEnsureVisible = FALSE);
	BOOL	SetEdit(int nItem, int nSubItem);
	BOOL	SetEllipsis(BOOL bEllipsis)
	{
		BOOL bOldEllipsis = m_bUseEllipsis;
		m_bUseEllipsis = bEllipsis;
		return bOldEllipsis;
	}
	void	SetEmptyMessage(LPCTSTR lpszMessage, COLORREF crText = RGB(0,0,0))
	{
		ASSERT(lpszMessage != NULL);
		if (lpszMessage)
		{
			// message can be pointer to string, or resource id (use MAKEINTRESOURCE)
			if (HIWORD(lpszMessage) == NULL)
				VERIFY(m_strEmptyMessage.LoadString(LOWORD((DWORD)(DWORD_PTR)lpszMessage)));
			else
				m_strEmptyMessage = lpszMessage;
			m_crEmptyTextColor = crText;
		}
	}
	BOOL	SetEnabled(int nItem, BOOL bEnable);
	DWORD	SetExtendedStyleX(DWORD dwNewStyle) 
	{
		DWORD dwOldStyle = m_dwExtendedStyleX;
		m_dwExtendedStyleX = dwNewStyle;
		return dwOldStyle;
	}

	void	EnableHeaderDividerLines(BOOL bEnable) { m_HeaderCtrl.EnableDividerLines(bEnable); }
	void	SetHeaderAlignment(UINT nFormat) { m_HeaderCtrl.SetAlignment(nFormat); }
	BOOL	SetHeaderItem(int nPos, HDITEM* pHeaderItem) { return m_HeaderCtrl.SetItem(nPos, pHeaderItem); }
	void	SetHeaderItemTextColor(int nSubItem, COLORREF rgbText) { m_HeaderCtrl.SetItemColor(nSubItem, rgbText); }
	BOOL	SetHeaderCheckedState(int nSubItem, int nCheckedState);
	void	SetHeaderSpacing(int nSpacing) { m_HeaderCtrl.SetSpacing(nSpacing); }
	// following is obsolete - use SetHeaderItemTextColor(-1, rgbText) instead
	//void	SetHeaderTextColor(COLORREF rgbText) { m_HeaderCtrl.SetTextColor(rgbText); }

	int		SetItem(const LVITEM* pItem);
	void	SetItemColors(int nItem,
						  int nSubItem, 
						  COLORREF crText, 
						  COLORREF crBackground);
	BOOL	SetItemData(int nItem, DWORD dwData);
	void	SetItemHeight(int nHeight) { m_nItemHeight = nHeight; }
	BOOL	SetItemImage(int nItem, int nSubItem, int nImage);
	BOOL	SetItemText(int nItem, int nSubItem, LPCTSTR lpszText); 
	BOOL	SetItemText(int nItem, 
						int nSubItem, 
						LPCTSTR lpszText,
						COLORREF crText, 
						COLORREF crBackground);
	void	SetListModified(BOOL bListModified) { m_bListModified = bListModified; }
	void	SetModified(int nItem, int nSubItem, BOOL bModified);
	BOOL	SetProgress(int nItem, 
						int nSubItem, 
						BOOL bShowProgressText = TRUE, 
						LPCTSTR lpszProgressText = NULL);
	void	SetSubItemAlignment(int nItem, int nSubItem, UINT nFormat);
	void	UpdateProgress(int nItem, int nSubItem, int nPercent);
	void	UpdateSubItem(int nItem, int nSubItem);

#ifndef NO_XLISTCTRL_TOOL_TIPS
	void DeleteAllToolTips();
	BOOL SetItemToolTipText(int nItem, int nSubItem, LPCTSTR lpszToolTipText);
	CString GetItemToolTipText(int nItem, int nSubItem);
	virtual int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
public:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	CImageList		m_cImageList;	// Image list for the header control
	CImageList		m_defImageList;	// default image list to resize rows if buttons are present

protected:
	int		DrawButton(int nItem,
					int nSubItem,
					CDC *pDC, 
					COLORREF crText,
					COLORREF crBkgnd,
					CRect& rect,
					XLISTCTRLDATA *pCLD);
	void	DrawCheckbox(int nItem, 
						int nSubItem, 
						CDC *pDC, 
						COLORREF crText,
						COLORREF crBkgnd,
						CRect& rect, 
						XLISTCTRLDATA *pCLD);
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	void	DrawComboBox(int nItem, 
					  int nSubItem);
#endif
	void	DrawEdit(int nItem, int nSubItem);
	int		DrawImage(int nItem, 
				  int nSubItem, 
				  CDC* pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect rect,
				  XLISTCTRLDATA *pXLCD);
	void	DrawProgress(int nItem, 
					  int nSubItem, 
					  CDC *pDC, 
					  COLORREF crText,
					  COLORREF crBkgnd,
					  CRect& rect, 
					  XLISTCTRLDATA *pCLD);
	int		DrawSubItemText(int nItem, 
				  int nSubItem, 
				  CDC *pDC, 
				  COLORREF crText,
				  COLORREF crBkgnd,
				  CRect& rect, 
				  XLISTCTRLDATA *pCLD);
	void	GetColors();
	void	GetDrawColors(int nItem,
					   int nSubItem,
					   COLORREF& colorText,
					   COLORREF& colorBkgnd);
	void	ProcessMouseOverButton(int nItem, int nSubItem, CPoint point, XLISTCTRLDATA *pXLCD);
	void	ProcessMouseOverCheckbox(int nItem, int nSubItem, CPoint point, XLISTCTRLDATA *pXLCD);
	BOOL	PtInButtonRect(int nItem, int nSubItem, POINT point);
	BOOL	PtInCheckboxRect(int nItem, int nSubItem, POINT point);
	void	ResetCurrentButton();
	void	ResetCurrentCheckbox();
	BOOL	SendLabelEditMessage(UINT nCode, 
								 int nItem, 
								 int nSubItem, 
								 LPCTSTR lpszText, 
								 BOOL bCancel);
	void	SendRegisteredMessage(UINT nMessage, int nItem, int nSubItem);
	void	SubclassHeaderControl();

	CXHeaderCtrl	m_HeaderCtrl;
	int				m_nItemHeight;			// item height in pixels;  this can 
											// be set to larger than normal size
											// if buttons are used - for example,
											// a height of 16 is very snug, a
											// height of 18 is loose for letters
											// with no descenders, and a height
											// of 21 accommodates all letters
											// very loosely.
											// special values:
											//    -1 = no height adjustment
											//    -2 = auto height adjustment 
	BOOL			m_bHeaderIsSubclassed;
	BOOL			m_bUseEllipsis;
	BOOL			m_bListModified;
	DWORD			m_dwExtendedStyleX;
	int				m_nPadding;
	CString			m_strInitialString;		// used to save the initial string
											// for edit and combo fields
	BOOL			m_bInitialCheck;		// initial check state of a checkbox field
	int				m_nCurrentButtonItem;	// button that cursor is over
	int				m_nCurrentButtonSubItem;
	int				m_nCurrentCheckboxItem;	// checkbox that cursor is over
	int				m_nCurrentCheckboxSubItem;
	int				m_nHeaderHeight;
	CString			m_strEmptyMessage;		// message to display when no items
	COLORREF		m_crEmptyTextColor;
	BOOL			m_bAllowUserInput;		// TRUE = normal;  FALSE = all user
											// input for buttons, checkboxes, edit 
											// and combo boxes will be ignored;
											// this is global flag; you can set
											// each subitem individually

	// for edit box 
	CXEdit *		m_pEdit;
	int				m_nEditItem;
	int				m_nEditSubItem;
	BOOL			m_bCancelEditLabel;


#ifndef DO_NOT_INCLUDE_XCOMBOLIST	//-----------------------------------------
	BOOL			m_bComboIsClicked;
	int				m_nComboItem;
	int				m_nComboSubItem;
	CRect			m_rectComboButton;
	CRect			m_rectComboList;
	CXCombo *		m_pCombo;
	CFont			m_ListboxFont;
	BOOL			m_bFontIsCreated;
#endif	//---------------------------------------------------------------------

	COLORREF		m_cr3DFace;
	COLORREF		m_cr3DHighLight;
	COLORREF		m_cr3DShadow;
	COLORREF		m_crActiveCaption;
	COLORREF		m_crBtnFace;
	COLORREF		m_crBtnShadow;
	COLORREF		m_crBtnText;
	COLORREF		m_crGrayText;
	COLORREF		m_crHighLight;
	COLORREF		m_crHighLightText;
	COLORREF		m_crInactiveCaption;
	COLORREF		m_crInactiveCaptionText;
	COLORREF		m_crWindow;
	COLORREF		m_crWindowText;

	// Generated message map functions
protected:
	//{{AFX_MSG(CXListCtrl)
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	afx_msg LRESULT OnCancelEditLabel(WPARAM, LPARAM);
	afx_msg LRESULT OnGetEditControl(WPARAM, LPARAM);
	//}}AFX_MSG
#ifndef DO_NOT_INCLUDE_XCOMBOLIST
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnComboEscape(WPARAM, LPARAM);
	afx_msg LRESULT OnComboComplete(WPARAM, LPARAM);
#endif

#ifndef NO_XLISTCTRL_TOOL_TIPS
	virtual afx_msg BOOL OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
#endif

	afx_msg LRESULT OnXEditKillFocus(WPARAM, LPARAM);
	afx_msg LRESULT OnXEditEscape(WPARAM, LPARAM);
	afx_msg LRESULT OnXButtonClicked(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHeaderCheckboxClicked(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XLISTCTRL_H
