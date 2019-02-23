// LinkButton.cpp : 实现文件
//

#include "stdafx.h"
#include "LinkButton.h"


// CLinkButton

IMPLEMENT_DYNAMIC(CLinkButton, CButton)

CLinkButton::CLinkButton()
:CErrorLog(_T("CLinkButton"))
,m_strDrawText(_T(""))
,m_fontHeight(12)
,m_hFont(NULL)
{
}

CLinkButton::~CLinkButton()
{
	ReleaseGDIHandle((HANDLE*)&m_hFont);
	DestroyCursor(m_hCursor);
}


BEGIN_MESSAGE_MAP(CLinkButton, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEHOVER()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CLinkButton 消息处理程序


void CLinkButton::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_hFont=::CreateFont(m_fontHeight,0,0,0,700,0,0,0,0,0,0,0,0,_T("宋体"));
	m_normalColor=RGB(0,155,0);
	m_overColor=RGB(0,0,155);
	m_bkColor=RGB(255,255,255);
	m_textColor=RGB(0,200,0);
	m_hCursor=LoadCursor(NULL,MAKEINTRESOURCE(32649));
	CButton::PreSubclassWindow();
}

void CLinkButton::Init(const CString &strDrawText, const CString &strTipText)
{
	m_strDrawText=strDrawText;
	m_toolTip.Create(this);
	m_toolTip.SetDelayTime(100);
	m_toolTip.AddTool(this,_T(""),&m_rcClient,TOOLTIP);
	m_toolTip.SetTipTextColor(RGB(155,0,200));
	m_toolTip.UpdateTipText(strTipText,this,TOOLTIP);
	Invalidate();
}
// 设置显示文字的颜色
void CLinkButton::SetTextColor(COLORREF& normal, COLORREF& over)
{
	m_normalColor=normal;
	m_overColor=over;
	Invalidate();
}
void CLinkButton::OnPaint()
{
	EXCEPTION_INFO ei;
	try
	{
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		if(NULL==m_hMemDC)
		{
			GetClientRect(&m_rcClient);
			if(NULL==(m_hMemDC=::CreateCompatibleDC(dc.m_hDC)))
			{
				ei=FillExceptionInfo(_T("创建内存兼容DC失败！"),__LINE__-2,__FILE__,::GetLastError());
				throw 1;
			}
			if(NULL==(m_hMemBitmap=::CreateCompatibleBitmap(dc.m_hDC,m_rcClient.Width(),m_rcClient.Height())))
			{
				ei=FillExceptionInfo(_T("创建内存兼容位图句柄失败！"),__LINE__-2,__FILE__,::GetLastError());
				throw 1;
			}
			::SelectObject(m_hMemDC,m_hMemBitmap);
			::SetBkMode(m_hMemDC,TRANSPARENT);
		}
		if(NULL==m_hBackDC)
		{
			if(NULL==(m_hBackDC=::CreateCompatibleDC(dc.m_hDC)))
			{
				ei=FillExceptionInfo(_T("创建内存兼容DC失败！"),__LINE__-2,__FILE__,::GetLastError());
				throw 1;
			}
		}
		if(m_bChangeBack)
		{
			m_bChangeBack=FALSE;
			ReleaseGDIHandle((HANDLE*)&m_hBackBitmap);
// 			CString strMsg;
// 			strMsg.Format(_T("left=%d,top=%d,right=%d,bottom=%d"),m_rcClient.left,m_rcClient.top,m_rcClient.right,m_rcClient.bottom);
// 			MessageBox(strMsg);
			m_hBackBitmap=GetDCBackBitmap(dc.m_hDC,m_rcClient.Width(),m_rcClient.Height());
			if(NULL==m_hBackBitmap)
			{
				ei=FillExceptionInfo(_T("获取背景位图句柄失败！"),__LINE__-3,__FILE__,GetLastError());
				throw 1;
			}
			::SelectObject(m_hBackDC,m_hBackBitmap);
		}
		::BitBlt(dc.m_hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),m_hBackDC,0,0,SRCCOPY);
		HFONT hOldFont=(HFONT)::SelectObject(m_hMemDC,m_hFont);
		FillDCRect(m_hMemDC,&m_rcClient,0xffffff);
		::SetTextColor(m_hMemDC,m_textColor);
		::DrawText(m_hMemDC,m_strDrawText,m_strDrawText.GetLength(),&m_rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		::TransparentBlt(dc.m_hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),\
			m_hMemDC,0,0,m_rcClient.Width(),m_rcClient.Height(),0xffffff);
		::SelectObject(m_hMemDC,hOldFont);
	}
	catch(...)
	{	ei.tstrExceptionFunc=_T("OnPaint");
		WriteErrorLog(&ei);
	}
	// 不为绘图消息调用 CButton::OnPaint()
}

BOOL CLinkButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		m_toolTip.RelayEvent(pMsg);
		break;
	}
	return CButton::PreTranslateMessage(pMsg);
}

// 设置背景颜色
void CLinkButton::SetBkColor(COLORREF color)
{
	m_bkColor=color;
}

BOOL CLinkButton::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}
BOOL CLinkButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_hCursor)  //如果设置了光标，就使用新设置的 光标
	{	
		::SetCursor(m_hCursor);		
		return TRUE;
	}
	return FALSE;
}

void CLinkButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tm;
	tm.cbSize=sizeof(tm);
	tm.dwFlags=TME_HOVER|TME_LEAVE;
	tm.dwHoverTime=0;
	tm.hwndTrack=m_hWnd;
	_TrackMouseEvent(&tm);
	CButton::OnMouseMove(nFlags, point);
}

void CLinkButton::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseGDIHandle((HANDLE*)&m_hFont);
	m_hFont=::CreateFont(m_fontHeight,0,0,0,700,0,0,0,0,0,0,0,0,_T("宋体"));
	m_textColor=m_normalColor;
	Invalidate();
	CButton::OnMouseLeave();
}

void CLinkButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ReleaseGDIHandle((HANDLE*)&m_hFont);
	m_hFont=::CreateFont(m_fontHeight+2,0,0,0,700,0,1,0,0,0,0,0,0,_T("宋体"));
	m_textColor=m_overColor;
	Invalidate();
	CButton::OnMouseHover(nFlags, point);
}
BOOL CLinkButton::EnableWindow(BOOL bEnable /* = TRUE */)
{
	if(!bEnable)
		m_normalColor=RGB(186,148,128);
	else
		m_normalColor=RGB(0,155,0);
	//Invalidate();
	return 	CButton::EnableWindow(bEnable);
}
void CLinkButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(IsWindow(m_hWnd))
		GetClientRect(&m_rcClient);
}
