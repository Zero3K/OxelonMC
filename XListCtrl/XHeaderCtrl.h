// XHeaderCtrl.h  Version 1.5
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

#ifndef XHEADERCTRL_H
#define XHEADERCTRL_H

#include "XCheckbox.h"
#include "XListCtrlLibDefs.h"

#define DT_DEFAULT	((UINT)-1)		//+++

#define FLATHEADER_TEXT_MAX	80

#define FH_PROPERTY_SPACING			1
#define FH_PROPERTY_ARROW			2
#define FH_PROPERTY_STATICBORDER	3
#define FH_PROPERTY_DONTDROPCURSOR	4
#define FH_PROPERTY_DROPTARGET		5

///////////////////////////////////////////////////////////////////////////////
// CXHeaderCtrl data
struct XHEADERCTRLDATA
{
	// ctor
	XHEADERCTRLDATA()
	{
		pCheckbox       = NULL;
		crText          = (DWORD) -1;
		bAllowUserInput = TRUE;
	}

	// dtor
	virtual ~XHEADERCTRLDATA()
	{
		if (pCheckbox)
			delete pCheckbox;
		pCheckbox = NULL;
	}

	CXCheckbox *	pCheckbox;				// pointer to CXCheckbox object
	COLORREF		crText;
	BOOL			bAllowUserInput;		// TRUE = normal state;  FALSE = user input
											// actions will be ignored
};

///////////////////////////////////////////////////////////////////////////////
// CXHeaderCtrl class
class XLISTCTRLLIBDLLEXPORT CXHeaderCtrl : public CHeaderCtrl
{
    DECLARE_DYNCREATE(CXHeaderCtrl)

// Construction
public:
	CXHeaderCtrl();
	virtual ~CXHeaderCtrl();

// Attributes
public:
	int			ColumnHitTest(CPoint point);
	BOOL		DeleteColumn(int nCol);
	BOOL		DeleteItem(int nCol);
	void		EnableDividerLines(BOOL bEnable) { m_bDividerLines = bEnable; }
	UINT		GetAlignment() { return m_nFormat; }
	BOOL		GetAllowUserInput(int nCol);
	int			GetCheck(int nCol);
	BOOL		GetDividerLines() { return m_bDividerLines; }
	COLORREF	GetItemColor(int nCol);
	int			GetSpacing() { return m_nSpacing; }
	COLORREF	GetTextColor() { return m_crText; }
	void		InsertColumn(int nCol, int nCheckboxState);
	int			InsertItem(int nPos, HDITEM* phdi);
	BOOL		ModifyProperty(WPARAM wParam, LPARAM lParam);
	void		SetAlignment(UINT nFormat) { m_nFormat = nFormat; }
	BOOL		SetAllowUserInput(int nCol, BOOL bAllowUserInput);
	void		SetCheck(int nCol, int nCheckedState);
	BOOL		SetItemColor(int nCol, COLORREF crText);
	void		SetListCtrl(CListCtrl *pListCtrl) { m_pListCtrl = pListCtrl; }
	void		SetSpacing(int nSpacing) { m_nSpacing = nSpacing; }

// Overrides
public:
	virtual void DrawItem(int nCol,
						  CDC *pDC,
						  COLORREF crText,
						  COLORREF crBkgnd,
						  CRect& rect,
						  HDITEM& hditem);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
protected:
	static HTHEME	m_hThemeHeader;
	int				m_nCurrentHot;
	CListCtrl *		m_pListCtrl;
	UINT			m_nFormat;
	COLORREF		m_crText;				// this is global text color;  you
											// can set each column individually
	BOOL			m_bMouseInWindow;
	BOOL			m_bDividerLines;
	BOOL			m_bStaticBorder;
	int				m_nSpacing;
	SIZE			m_sizeImage;
	SIZE			m_sizeArrow;
	UINT			m_nDontDropCursor;
	UINT			m_nClickFlags;
	CPoint			m_ptClickPoint;
	CPtrArray		m_ColumnData;
	BOOL			m_bAllowUserInput;		// TRUE = normal;  FALSE = all user
											// input for checkboxes will be ignored;
											// this is global flag; you can set
											// each column individually
	int				m_nCurrentCheckboxItem;

	COLORREF		m_cr3DHighLight;
	COLORREF		m_cr3DShadow;
	COLORREF		m_cr3DFace;
	COLORREF		m_crBtnText;

	int DrawBitmap(CDC* pDC, CRect rect, HDITEM hditem, CBitmap* pBitmap, 
		BITMAP* pBitmapInfo, BOOL bRight);
	int DrawHeaderText(int nCol, CDC* pDC, COLORREF crText, COLORREF crBkgnd, CRect rect, HDITEM hditem);
	void ProcessMouseOverCheckbox(int nItem, CPoint point);
	BOOL PtInCheckboxRect(int nItem, CPoint point);
	void ResetCurrentCheckbox();
	void UpdateItem(int nItem);

// Generated message map functions
protected:
	//{{AFX_MSG(CXHeaderCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnLayout(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnSetImageList(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XHEADERCTRL_H
