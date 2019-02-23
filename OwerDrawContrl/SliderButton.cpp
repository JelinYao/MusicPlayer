// SliderButton.cpp : 实现文件
//

#include "stdafx.h"
#include "SliderButton.h"


// CSliderButton
IMPLEMENT_DYNAMIC(CSliderButton, CButton)

CSliderButton::CSliderButton()
:CErrorLog(_T("CSliderButton"))
, m_nCurrentPos(0)
, m_nMaxPos(0)
,m_nxPos(0)
,m_hSliderBitmap(NULL)
,m_hBkBrush(NULL)
,m_hSliderDC(NULL)
,m_nSliderWidth(0)
,m_uMessage(0)
,m_wParam(0)
{
	m_bkColor=0xff00ff;
	m_overColor=0x008000;
}

CSliderButton::~CSliderButton()
{
	ReleaseDCHandle(&m_hSliderDC);
	ReleaseGDIHandle((HANDLE*)&m_hSliderBitmap);
	ReleaseGDIHandle((HANDLE*)&m_hBkBrush);
}


BEGIN_MESSAGE_MAP(CSliderButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()
void CSliderButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==m_hMemDC)
		{
			m_hMemDC=::CreateCompatibleDC(lpDrawItemStruct->hDC);
			if(NULL==m_hMemDC)
				THROW_ERROR(CREATE_COMDC_ERROR)
			SetBkMode(m_hMemDC,TRANSPARENT);
		}
		if(NULL==m_hMemBitmap)
		{
			m_hMemBitmap=::CreateCompatibleBitmap(lpDrawItemStruct->hDC,m_rcClient.Width(),m_rcClient.Height());
			if(NULL==m_hMemBitmap)
				THROW_ERROR(CREATE_COMBITMAP_ERROR)
			::SelectObject(m_hMemDC,m_hMemBitmap);
			::SelectObject(m_hMemDC,m_hBkBrush);
		}
		FillDCRect(m_hMemDC,&m_rcClient,0x000000);
 		::RoundRect(m_hMemDC,m_rcClient.left,m_rcClient.top,\
 			m_rcClient.right,m_rcClient.bottom,4,4);
		static CRect rcFinish;
		rcFinish=m_rcClient;//已经完成的进度矩形
		rcFinish.right=m_nxPos;
		rcFinish.DeflateRect(0,1);
		FillDCRect(m_hMemDC,&rcFinish,m_overColor);
		if(NULL==m_hSliderDC)
		{
			m_hSliderDC=::CreateCompatibleDC(m_hMemDC);
			if(NULL==m_hSliderDC)
			{
				ei=FillExceptionInfo(_T("CreateCompatibleDC调用失败！"),__LINE__-2,__FILE__,::GetLastError());
				throw 1;
			}
			::SelectObject(m_hSliderDC,m_hSliderBitmap);
		}
		if(m_nxPos>m_rcClient.Width()-m_nSliderWidth)
			m_nxPos=m_rcClient.Width()-m_nSliderWidth+2;
		if(!::BitBlt(m_hMemDC,m_nxPos-2,0,m_nSliderWidth,m_rcClient.Height(),m_hSliderDC,0,0,SRCCOPY))
			THROW_ERROR(BITBLT_ERROR)
 		if(!::BitBlt(lpDrawItemStruct->hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),m_hMemDC,0,0,SRCCOPY))
 			THROW_ERROR(BITBLT_ERROR);
// 		::TransparentBlt(lpDrawItemStruct->hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),
// 		m_hMemDC,0,0,m_rcClient.Width(),m_rcClient.Height(),0xffffff);

	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("DrawItem");
		WriteErrorLog(&ei);
	}
}


// CSliderButton 消息处理程序



void CSliderButton::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	EXCEPTION_INFO ei;
	try
	{
		m_hBkBrush=::CreateSolidBrush(m_bkColor);
		if(NULL==m_hBkBrush)
		{
			ei=FillExceptionInfo(_T("CreateSolidBrush调用失败!"),__LINE__-2,__FILE__,::GetLastError());
			throw 1;
		}
		CWnd::PreSubclassWindow();
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("PreSubclassWindow");
		WriteErrorLog(&ei);
	}
}

void CSliderButton::InitCtrl( UINT uBitmapID,const CString& strTipText,int nMaxPos/*=100*/,
							 UINT uMessage/*=0*/,WPARAM wParam/*=0*/ )
{
	EXCEPTION_INFO	ei;
	try
	{
		if(uMessage)
		{
			m_uMessage=uMessage;
			m_wParam=wParam;
		}
		m_hSliderBitmap=::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(uBitmapID));
		if(NULL==m_hSliderBitmap)
			THROW_ERROR(LOAD_RESOURCE_ERROR);
		BITMAP bmMetric;
		memset(&bmMetric,0,sizeof(bmMetric));
		::GetObject(m_hSliderBitmap,sizeof(bmMetric),&bmMetric);
		m_nSliderWidth=bmMetric.bmWidth;
		m_nMaxPos=nMaxPos;
		m_nCurrentPos=0;
		m_nxPos=0;
		m_toolTip.Create(this);
		m_toolTip.SetDelayTime(100);
		m_toolTip.SetMaxTipWidth(200);
		m_toolTip.AddTool(this,_T(""),&m_rcClient,ID_TOOLTIP);
		m_toolTip.UpdateTipText(strTipText,this,ID_TOOLTIP);
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("InitCtrl");
		WriteErrorLog(&ei);
	}
}

BOOL CSliderButton::PreTranslateMessage(MSG* pMsg)
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

void CSliderButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nxPos=point.x;
	m_nCurrentPos=m_nxPos/(float)m_rcClient.Width()*m_nMaxPos;
	RedrawWindow();
	SendOwerMessage();
}

BOOL CSliderButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HCURSOR m_hCursor=LoadCursor(NULL,IDC_HAND);
	::SetCursor(m_hCursor);
	return TRUE;
}
void CSliderButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	if(::IsWindow(m_hWnd))
		GetClientRect(&m_rcClient);
	// TODO: 在此处添加消息处理程序代码
}

BOOL CSliderButton::SendOwerMessage()
{
	BOOL bResult=FALSE;
	if(m_uMessage)//不为0，说明用户选择了自定义的消息
	{
		//父窗口的父窗口才是接收消息的对象
		CWnd* pWnd=GetParent();
		if(pWnd)
		{
			pWnd=pWnd->GetParent();
			::SendMessage(pWnd->GetSafeHwnd(),m_uMessage,m_wParam,(LPARAM)m_nCurrentPos);
			bResult=TRUE;
		}
	}
	return bResult;
}
