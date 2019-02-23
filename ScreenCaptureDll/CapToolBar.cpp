// CapToolBar.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenCaptureDll.h"
#include "CapToolBar.h"


// CCapToolBar

//IMPLEMENT_DYNAMIC(CCapToolBar, CToolBarCtrl)

CCapToolBar::CCapToolBar(CWnd *pParent)
{
	Create(0,CRect(0,0,50,25),pParent,CAP_TOOLBAR);
	m_nWidth=50;
	m_nHeight=25;
}

CCapToolBar::~CCapToolBar()
{
}

BEGIN_MESSAGE_MAP(CCapToolBar, CToolBarCtrl)
	//{{AFX_MSG_MAP(CCapToolBar)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX(TTN_NEEDTEXT,0,OnTooltip)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCapToolBar message handlers

BOOL CCapToolBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style=WS_CHILD|WS_VISIBLE|WS_BORDER|
		CCS_NODIVIDER|TBSTYLE_FLAT|CCS_NOPARENTALIGN|
		TBSTYLE_TRANSPARENT|CCS_NORESIZE|TBSTYLE_TOOLTIPS;
	return CToolBarCtrl::PreCreateWindow(cs);
}

int CCapToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBarCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ImgList.Create(16,16,ILC_MASK|ILC_COLOR32,0,1);
	CBitmap bmp;
	bmp.LoadBitmap(IDB_EXIT);
	m_ImgList.Add(&bmp,RGB(214,211,206));
	bmp.DeleteObject();
	bmp.LoadBitmap(IDB_SAVE);
	m_ImgList.Add(&bmp,RGB(214,211,206));
	SetImageList(&m_ImgList);

	_TBBUTTON tbBtn[2];
	tbBtn[0].iBitmap=0;
	tbBtn[0].idCommand=CAPTOOLBAR_EXIT;
	tbBtn[0].fsState=TBSTATE_ENABLED;
	tbBtn[0].fsStyle=TBSTYLE_BUTTON;
	tbBtn[0].dwData=NULL;
	tbBtn[0].iString=NULL;

	tbBtn[1].iBitmap=1;
	tbBtn[1].idCommand=CAPTOOLBAR_SAVE;
	tbBtn[1].fsState=TBSTATE_ENABLED;
	tbBtn[1].fsStyle=TBSTYLE_BUTTON;
	tbBtn[1].dwData=NULL;
	tbBtn[1].iString=NULL;

	AddButtons(2,tbBtn);
	return 0;
}
BOOL CCapToolBar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
	//return CToolBarCtrl::OnEraseBkgnd(pDC);
}

BOOL CCapToolBar::OnTooltip(UINT id,NMHDR *pTTTStruct,
							LRESULT *pResult)
{	
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID =pTTTStruct->idFrom;
	if(nID==CAPTOOLBAR_EXIT)
	{
		pTTT->lpszText=_T("退出截屏");
		return(TRUE);
	}
	if (nID==CAPTOOLBAR_SAVE)
	{
		pTTT->lpszText=_T("保存图片");
		return(TRUE);
	}
	return(FALSE);
}
