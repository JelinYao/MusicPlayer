#pragma once


// CCapToolBar

#define  CAP_TOOLBAR WM_USER+2
#define  CAPTOOLBAR_EXIT WM_USER+3
#define CAPTOOLBAR_SAVE WM_USER+4
class CCapToolBar : public CToolBarCtrl
{
	// Construction
public:
	CCapToolBar(CWnd *pParent);

	// Attributes
public:
	CImageList m_ImgList;
	int m_nWidth;
	int m_nHeight;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCapToolBar)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CCapToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCapToolBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	BOOL OnTooltip(UINT id,NMHDR * pTTTStruct,LRESULT * pResult);
	DECLARE_MESSAGE_MAP()
};



