// ImageCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageCtrl.h"
#include <Shlwapi.h>
#include "Resource.h"
#define TOOLTIP_ID 1000

// CImageCtrl

IMPLEMENT_DYNAMIC(CImageCtrl, CWnd)

CImageCtrl::CImageCtrl()
:CErrorLog(_T("CImageCtrl"))
,m_pBackBitmap(NULL)
,m_pGraphics(NULL)
,m_strImagePath(_T(""))
,m_rcClient(0,0,0,0)
,m_bMouseOver(FALSE)
,m_hOverBrush(NULL)
,m_bCheck(FALSE)
,m_hCheckBrush(NULL)
,m_hTipWnd(NULL)
{

}

CImageCtrl::~CImageCtrl()
{
	::DestroyWindow(m_hTipWnd);
	if(m_pGraphics)
	{
		delete m_pGraphics;
		m_pGraphics=NULL;
	}
	if(m_pBackBitmap)
	{
		delete m_pBackBitmap;
		m_pBackBitmap=NULL;
	}
	ReleaseDCHandle(&m_hMemDC);
	ReleaseGDIHandle((HANDLE*)&m_hOverBrush);
	ReleaseGDIHandle((HANDLE*)&m_hCheckBrush);
}


BEGIN_MESSAGE_MAP(CImageCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CImageCtrl 消息处理程序



void CImageCtrl::OnPaint()
{
	EXCEPTION_INFO ei;
	try
	{
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CWnd::OnPaint()
		if(NULL==m_hMemDC)
		{
			m_hMemDC=::CreateCompatibleDC(dc.m_hDC);
			if(NULL==m_hMemDC)
				THROW_ERROR(CREATE_COMDC_ERROR)
			m_hMemBitmap=::CreateCompatibleBitmap(dc.m_hDC,m_rcClient.Width(),m_rcClient.Height());
			::SelectObject(m_hMemDC,m_hMemBitmap);
		}
		if(NULL==m_pGraphics)
		{
			m_pGraphics=Gdiplus::Graphics::FromHDC(m_hMemDC);
			if(NULL==m_pGraphics)
				THROW_ERROR("Gdiplus::Graphics::FromHDC调用失败！");
			// 内存DC上面绘图
			if(Gdiplus::Status::Ok!=m_pGraphics->DrawImage(m_pBackBitmap,Gdiplus::RectF(0,0,m_rcClient.Width(),m_rcClient.Height())\
				,0,0,m_pBackBitmap->GetWidth(),m_pBackBitmap->GetHeight(),Gdiplus::UnitPixel))
				THROW_ERROR(GDIPLUS_DRAWIMAGE_ERROR)
		}
		TRACE(_T("Client Rect: %d,%d,%d,%d\n"),m_rcClient.left,m_rcClient.top,m_rcClient.right,m_rcClient.bottom);
		//贴图到目标DC上
		if(FALSE==::BitBlt(dc.m_hDC,0,0,m_rcClient.Width(),m_rcClient.Height()
			,m_hMemDC,0,0,SRCCOPY))
			THROW_ERROR(BITBLT_ERROR)
		if(m_bCheck)
			::FrameRect(dc.m_hDC,&m_rcClient,m_hCheckBrush);
		if(m_bMouseOver)
			::FrameRect(dc.m_hDC,&m_rcClient,m_hOverBrush);
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("OnPaint");
		WriteErrorLog(&ei);
	}
}


void CImageCtrl::Init( CString& strImagePath,const CString& strTipText )
{
	EXCEPTION_INFO ei;
	try
	{
		if(!PathFileExists(strImagePath))
		{
			m_strImagePath=_T("");
			m_pBackBitmap=Gdiplus::Bitmap::FromResource(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BITMAP_ERROR));
		}
		else
		{
			m_strImagePath=strImagePath;
			m_pBackBitmap=Gdiplus::Bitmap::FromFile(strImagePath.GetBuffer());
		}
		if(NULL==m_pBackBitmap)
			THROW_ERROR("加载位图资源失败，可能是文件不存在！ ");
 		GetClientRect(&m_rcClient);
		m_hTipWnd = CreateWindow(//WS_EX_TOPMOST
			TOOLTIPS_CLASS,
			NULL,
			WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			GetSafeHwnd(),
			NULL,
			AfxGetInstanceHandle(),
			NULL
			);
		TOOLINFO ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS;
		ti.hwnd = GetSafeHwnd();
		ti.hinst = AfxGetInstanceHandle();
		ti.uId = TOOLTIP_ID;
		ti.lpszText =const_cast<CString*>(&strTipText)->GetBuffer();
		ti.rect=m_rcClient;
		::SendMessage(m_hTipWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
		m_hOverBrush=::CreateSolidBrush(0x00ff00);
		m_hCheckBrush=::CreateSolidBrush(0x0000ff);
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("Init");
		WriteErrorLog(&ei);
	}
}

void CImageCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tm;
	tm.cbSize=sizeof(tm);
	tm.dwFlags=TME_HOVER|TME_LEAVE;
	tm.dwHoverTime=0;
	tm.hwndTrack=m_hWnd;
	_TrackMouseEvent(&tm);
	CWnd::OnMouseMove(nFlags, point);
}

void CImageCtrl::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_strImagePath!=_T(""))
	{
		if(!m_bMouseOver)
		{
			CWnd* pWnd=GetParent()->GetParent();
			if(pWnd)
				::SendMessage(pWnd->GetSafeHwnd(),WM_IMAGECTRL_MESSAGE,1,(LPARAM)m_strImagePath.GetBuffer());
		}
		m_bMouseOver=TRUE;
		Invalidate();
	}
	CWnd::OnMouseHover(nFlags, point);
}

void CImageCtrl::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_strImagePath!=_T(""))
	{
		m_bMouseOver=FALSE;
		Invalidate();
		CWnd* pWnd=GetParent()->GetParent();
		if(pWnd)
			::SendMessage(pWnd->GetSafeHwnd(),WM_IMAGECTRL_MESSAGE,2,0);
	}
	CWnd::OnMouseLeave();
}

void CImageCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_strImagePath!=_T(""))
	{
		CWnd* pWnd=GetParent();
		if(pWnd)//通知父窗口选中项已经改变
			::SendMessage(pWnd->GetSafeHwnd(),WM_IMAGECTRL_MESSAGE,(WPARAM)m_owerIndex.nGroupNumber,(LPARAM)m_owerIndex.nIndex);
		pWnd=pWnd->GetParent();
		if(pWnd)//告诉主窗口背景已经更换
			::SendMessage(pWnd->GetSafeHwnd(),WM_IMAGECTRL_MESSAGE,3,(LPARAM)m_strImagePath.GetBuffer());
	}
	CWnd::OnLButtonDown(nFlags, point);
}
