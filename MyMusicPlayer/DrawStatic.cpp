// DrawStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer.h"
#include "DrawStatic.h"


// CDrawStatic

IMPLEMENT_DYNAMIC(CDrawStatic, CStatic)

CDrawStatic::CDrawStatic()
{
	m_colorText=RGB(0,255,0);
	m_strText=_T("");
	m_hFont=NULL;
	m_hMemDC=NULL;
	m_hMemBitmap=NULL;
	m_hBackDC=NULL;
	m_hBackBitmap=NULL;
}

CDrawStatic::~CDrawStatic()
{
	ReleaseDCHandle(m_hMemDC);
	ReleaseGDIHandle(m_hMemBitmap);
	ReleaseGDIHandle(m_hFont);
	ReleaseGDIHandle(m_hBackDC);
	ReleaseGDIHandle(m_hBackBitmap);
}


BEGIN_MESSAGE_MAP(CDrawStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CDrawStatic 消息处理程序
void CDrawStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	static HDC hdc=::CreateCompatibleDC(dc.m_hDC);
	::SelectObject(hdc,m_hBackBitmap);
	::BitBlt(dc.m_hDC,0,0,m_rcClient.right-m_rcClient.left,\
		m_rcClient.bottom-m_rcClient.top,hdc,0,0,SRCCOPY);
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	if(NULL==m_hBackBitmap)
		m_hBackBitmap=CFuncDC::GetBackBitmap(dc.m_hDC,m_rcClient.right-m_rcClient.left,\
		m_rcClient.bottom-m_rcClient.top);
	if(NULL==m_hMemDC)
	{
		m_hMemDC=::CreateCompatibleDC(dc.m_hDC);
		m_hMemBitmap=::CreateCompatibleBitmap(dc.m_hDC,m_rcClient.right-m_rcClient.left,\
			m_rcClient.bottom-m_rcClient.top);
		::SelectObject(m_hMemDC,m_hMemBitmap);
	}
// 	ReleaseGDIHandle(m_hMemBitmap);
// 	m_hMemBitmap=CopyHBitmap(m_hBackBitmap);
	//CFuncDC::SaveHBitmap(m_hMemBitmap,"c:\\1.bmp");
	::SelectObject(m_hMemDC,m_hMemBitmap);
	::SelectObject(m_hMemDC,m_hFont);
	::SetBkMode(m_hMemDC,TRANSPARENT);
	::SetTextColor(m_hMemDC,m_colorText);
	::SetBkColor(m_hMemDC,0XFFFFFF);
	::ExtTextOut(m_hMemDC, 0, 0, ETO_OPAQUE, &m_rcClient, NULL, 0, NULL);
	::DrawText(m_hMemDC,m_strText,m_strText.GetLength(),&m_rcClient,DT_VCENTER);
	//CFuncDC::SaveBackBitmap(m_hMemDC,"c:\\back.bmp");
	//::SelectObject(dc.m_hDC,CopyHBitmap(m_hBackBitmap));
 	::TransparentBlt(dc.m_hDC,0,0,m_rcClient.right-m_rcClient.left,m_rcClient.bottom-m_rcClient.top,\
 		m_hMemDC,0,0,m_rcClient.right-m_rcClient.left,m_rcClient.bottom-m_rcClient.top,0XFFFFFF);
// 	::BitBlt(dc.m_hDC,0,0,m_rcClient.right-m_rcClient.left,m_rcClient.bottom-m_rcClient.top,\
// 		m_hMemDC,0,0,SRCCOPY);
}


void CDrawStatic::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetClientRect(&m_rcClient);
// 	LOGFONT lf;
// 	GetFont()->GetLogFont(&lf);
// 	_tcscpy(lf.lfFaceName,_T("宋体"));
// 	m_hFont=::CreateFontIndirect(&lf);
	m_hFont=::CreateFont(12, 0, 0, 0, 900,
		FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, 
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 
		DEFAULT_PITCH|FF_SWISS, _T("宋体"));
//  	SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)|0x80000);
//  	SetLayeredWindowAttributes(0xffffff,128,LWA_COLORKEY);
	CStatic::PreSubclassWindow();
}

BOOL CDrawStatic::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
// 	if(NULL==m_hBackDC)
// 	{
// 		m_hBackDC=::CreateCompatibleDC(pDC->m_hDC);
// 		::SelectObject(m_hBackDC,m_hBackBitmap);
// 	}
// 	::SetBkMode(m_hBackDC,TRANSPARENT);
// 	::BitBlt(pDC->m_hDC,0,0,m_rcClient.right-m_rcClient.left,m_rcClient.bottom-m_rcClient.top,\
// 		m_hBackDC,0,0,SRCCOPY);
	return TRUE;
	return CStatic::OnEraseBkgnd(pDC);
}
