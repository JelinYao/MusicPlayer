#if !defined(AFX_TRANSPARENTSTATIC_H__953E2DEB_14FD_4EC8_95EB_0B0996E6E9FA__INCLUDED_)
#define AFX_TRANSPARENTSTATIC_H__953E2DEB_14FD_4EC8_95EB_0B0996E6E9FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransparentStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransparentStatic window

class CTransparentStatic : public CStatic
{
// Construction
public:
	CTransparentStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransparentStatic)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetToolTipText(CString strText);
	void SetCursor(HCURSOR hCursor);
	virtual ~CTransparentStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransparentStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CToolTipCtrl m_ToolTip;
	void SetDefaultCursor();
	HCURSOR m_hCursor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPARENTSTATIC_H__953E2DEB_14FD_4EC8_95EB_0B0996E6E9FA__INCLUDED_)
