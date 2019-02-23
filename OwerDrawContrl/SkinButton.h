#pragma once

// SkinButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSkinButton window

EXPORT_CLASS CSkinButton : public CButton,public CErrorLog,public CChangeBackBase,CMemBase
{
// Construction
public:
	CSkinButton();
	virtual ~CSkinButton();	
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	//初始化控件 
	void Init(UINT nNormalID,UINT nMouseOverID,const CString& strTipText);
	//重新初始化
	void ReInit(UINT nNormalID,UINT nMouseOverID,const CString& strTipText){SetBitmap(nNormalID,nMouseOverID);SetToolTipText(strTipText);}
	//改变控件的位图
	BOOL SetBitmap(UINT nNormalID,UINT nMouseOverID);
	//设置控件提示字符串
	void SetToolTipText(const CString& strText);
	BOOL SetButtonCursor(HCURSOR hCursor);
	//换肤
	void ChangeBack() { m_bChangeBack=TRUE;Invalidate(); }
	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinButton)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG//

	DECLARE_MESSAGE_MAP()
private:
	void AdjustPosition();
	UINT m_nMouseOverID;
	UINT m_nNormalID;
	CToolTipCtrl m_ToolTip;
	void SetDefaultCursor();
	HCURSOR m_hCursor;
	BOOL m_bMouseOver;
	CRect m_rcClient;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

