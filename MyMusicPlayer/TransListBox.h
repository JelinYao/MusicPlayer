#if !defined(AFX_TRANSLISTBOX_H__91608ADE_1F33_48AC_9FA8_0E178BA141E6__INCLUDED_)
#define AFX_TRANSLISTBOX_H__91608ADE_1F33_48AC_9FA8_0E178BA141E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransListBox window
#include <afxtempl.h>
#include "AttributeDlg.h"
#include "Resource.h"
#define WM_LISTBOX_DELETEITEM WM_USER+105
#define WM_ATTRIBUTE WM_USER+106
#define WM_LOOK_LYRICS WM_USER+112
#define WM_SCROLL WM_USER+114
class CTransListBox : public CListBox
{
// Construction
public:
	CTransListBox();

// Attributes
public:
	afx_msg void OnListboxAttribute();
	afx_msg void OnListboxDeleteItem();
private:
	// 获取父窗口指针
	CWnd* pParent;
public:
	// 获取父窗口指针
	AttributeDlg* pAttributeDlg;
	BOOL GetParentPointer(void);
	afx_msg void OnListboxLookLyrics();
public:
   void SetFont(int nPointSize, CString FaceName,COLORREF Color,COLORREF SelColor,BOOL Shadow=TRUE,int SOffset = 2,COLORREF ShadowColor = RGB(0,0,0));
   void SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor = RGB(0,0,0));

   BOOL SetTopIndex(int Index);
   int ScrollUp(int Lines=1);
   int ScrollDown(int Lines=1);
   int AddString(CString Text,DWORD ItemData = 0,BOOL Enabled = TRUE);
   int InsertString(int Index,CString Text, DWORD ItemData = 0,BOOL Enabled = TRUE);
   virtual void ResetContent(BOOL bInvalidate = TRUE);
protected:
   int            m_ItemHeight;
   COLORREF       m_Color;
   COLORREF       m_SelColor;
   COLORREF       m_ShadowColor;
   CFont          m_Font;
   CBitmap        m_Bmp;
   BOOL           m_HasBackGround;
   int            m_ShadowOffset;
   BOOL           m_Shadow;
   long           m_PointSize; 
   

   virtual CFont *GetFont();
   virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
   virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
   virtual void DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransListBox)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransListBox)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSelchange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnMove(int x, int y);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSLISTBOX_H__91608ADE_1F33_48AC_9FA8_0E178BA141E6__INCLUDED_)
