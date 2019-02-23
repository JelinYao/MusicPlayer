// SkinButton.cpp : implementation file
//

#include "stdafx.h"
#include "SkinButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIP_ID  10000
/////////////////////////////////////////////////////////////////////////////
// CSkinButton

CSkinButton::CSkinButton()
:CErrorLog(_T("CSkinButton"))
{	
}

CSkinButton::~CSkinButton()
{
	DestroyCursor(m_hCursor);
}


BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	//{{AFX_MSG_MAP(CSkinButton)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinButton message handlers
void CSkinButton::Init(UINT nNormalID, UINT nMouseOverID,const CString& strTipText)
{
	GetClientRect(&m_rcClient);
	m_ToolTip.Create(this);
	m_ToolTip.SetDelayTime(100);
	m_ToolTip.SetMaxTipWidth(200);
	m_ToolTip.AddTool(this,_T(""),m_rcClient,TIP_ID);
    m_nNormalID=nNormalID;
    m_nMouseOverID=nMouseOverID;
    m_ToolTip.UpdateTipText(strTipText,this,TIP_ID);
	m_bMouseOver=FALSE;
  //  Invalidate();
}
void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==m_hBackDC)
			m_hBackDC=::CreateCompatibleDC(lpDrawItemStruct->hDC);
		if(m_bChangeBack)
		{
			m_bChangeBack=FALSE;
			ReleaseGDIHandle((HANDLE*)&m_hBackBitmap);
			m_hBackBitmap=GetDCBackBitmap(lpDrawItemStruct->hDC,m_rcClient.Width(),m_rcClient.Height());
			::SelectObject(m_hBackDC,m_hBackBitmap);
		}
		::BitBlt(lpDrawItemStruct->hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),\
			m_hBackDC,0,0,SRCCOPY);
		//::SelectObject(m_hBackDC,hOldBitmap);
		static UINT nOldID=0;//记忆上次位图的ID
		UINT nNewID;
		UINT nState=lpDrawItemStruct->itemState;
		if(m_bMouseOver&&(!(nState & ODS_SELECTED)))
			nNewID=m_nMouseOverID;
		else
			nNewID=m_nNormalID;
		if(NULL==m_hMemDC)
		{
			if(NULL==(m_hMemDC=::CreateCompatibleDC(lpDrawItemStruct->hDC)))
			{
				ei=FillExceptionInfo(_T("CreateCompatibleDC创建内存兼容DC失败！"),__LINE__-3,__FILE__,::GetLastError());
				throw 1;
			}
		}
		if(nOldID!=nNewID)//两次ID不同，位图句柄重新加载
		{
			ReleaseGDIHandle((HANDLE*)&m_hMemBitmap);
			m_hMemBitmap=::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(nNewID));
			if(NULL==m_hMemBitmap)
			{
				ei=FillExceptionInfo(_T("LoadBitmap加载位图失败!"),__LINE__-3,__FILE__,::GetLastError());
				throw 1;
			}
			nOldID=nNewID;
			::SelectObject(m_hMemDC,m_hMemBitmap);
		}
		BITMAP bmpMetric;
		if(0==::GetObject(m_hMemBitmap,sizeof(bmpMetric),&bmpMetric))
		{
			ei=FillExceptionInfo(_T("GetObject获取位图尺寸失败！"),__LINE__-3,__FILE__,::GetLastError());
			throw 1;
		}
		::TransparentBlt(lpDrawItemStruct->hDC,0,0,bmpMetric.bmWidth,bmpMetric.bmHeight,\
			m_hMemDC,0,0,bmpMetric.bmWidth,bmpMetric.bmHeight,0xffffff);
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("DrawItem");
		WriteErrorLog(&ei);
	}
}

BOOL CSkinButton::SetButtonCursor(HCURSOR hCursor)
{
	m_hCursor=hCursor;
	if(m_hCursor==NULL)
	{
		SetDefaultCursor();
		return FALSE;
	}
	return TRUE;
}

void CSkinButton::SetDefaultCursor()
{
	m_hCursor=LoadCursor(NULL,IDC_HAND);
}

void CSkinButton::SetToolTipText(const CString& strText)
{
	m_ToolTip.UpdateTipText(strText,this,TIP_ID);

}

BOOL CSkinButton::SetBitmap(UINT nNormalID, UINT nMouseOverID)
{
	m_nNormalID=nNormalID;
	m_nMouseOverID=nMouseOverID;
	AdjustPosition();
	Invalidate();
	return TRUE;
}

void CSkinButton::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetDefaultCursor();
	CButton::PreSubclassWindow();
}

BOOL CSkinButton::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		m_ToolTip.RelayEvent(pMsg);
		break;
	}
	return CButton::PreTranslateMessage(pMsg);
}



BOOL CSkinButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_hCursor)  //如果设置了光标，就使用新设置的 光标
    {	
		::SetCursor(m_hCursor);		
        return TRUE;
    }
	return FALSE;
}

void CSkinButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT tm;
	tm.cbSize=sizeof(tm);
	tm.dwFlags=TME_HOVER|TME_LEAVE;
	tm.dwHoverTime=0;
	tm.hwndTrack=m_hWnd;
	_TrackMouseEvent(&tm);
	CButton::OnMouseMove(nFlags, point);
}

void CSkinButton::AdjustPosition()
{
	try
	{
		if(NULL==m_hMemBitmap)
		{
			m_hMemBitmap=::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(m_nNormalID));
			if(NULL==m_hMemBitmap)
				throw _T("加载位图失败！");
		}
		BITMAP bmMetric;
		if(0==::GetObject(m_hMemBitmap,sizeof(bmMetric),&bmMetric))
			throw _T("获取位图尺寸失败！");
		CRect rect;
		GetWindowRect(&rect);//根据图象的大小调整按钮大小
		CWnd *pWnd=GetParent();
		if(NULL==pWnd)
			throw _T("获取父窗口指针失败！");
		if(pWnd)
		{
			pWnd->ScreenToClient(&rect);
			rect.right=rect.left+bmMetric.bmWidth;
			rect.bottom=rect.top+bmMetric.bmHeight;
			MoveWindow(&rect,TRUE);
		}
	}
	catch(TCHAR* pError)
	{
		EXCEPTION_INFO ei=FillExceptionInfo(pError,__LINE__,__FILE__,::GetLastError(),_T("AdjustPosition"));
		WriteErrorLog(&ei);
	}
}

BOOL CSkinButton::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}

void CSkinButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMouseOver=TRUE;
	Invalidate();
	CButton::OnMouseHover(nFlags, point);
}

void CSkinButton::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMouseOver=FALSE;
	Invalidate();
	CButton::OnMouseLeave();
}

void CSkinButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(IsWindow(m_hWnd))
		GetClientRect(&m_rcClient);
}
