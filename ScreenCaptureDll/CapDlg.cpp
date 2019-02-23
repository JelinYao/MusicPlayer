// CapDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenCaptureDll.h"
#include "CapDlg.h"
#define MAKECURSOR(x) LoadCursor(NULL,x)
#define BRIGHT_VALUE 155>>8//亮度变化率
//亮度处理
#define GET_BRIGHT_RVALUE(x) GetRValue(x)*BRIGHT_VALUE
#define GET_BRIGHT_GVALUE(x) GetGValue(x)*BRIGHT_VALUE
#define GET_BRIGHT_BVALUE(x) GetBValue(x)*BRIGHT_VALUE
//亮度还原处理
#define RESET_RVALUE(x) GetRValue(x)<<8/155
#define RESET_GVALUE(x) GetGValue(x)<<8/155
#define RESET_BVALUE(x) GetBValue(x)<<8/155
////////////////////////////////////////////////////////////////////////////////////////////////////
//静态变量初始化
int CCapDlg::m_nxScreen=0;
int CCapDlg::m_nyScreen=0;
////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct _DATA
{
	BYTE* pData;//数据域
	int nLen;//数据长度
}DATA;
//亮度处理线程
DWORD WINAPI ChangeBrightThread(LPVOID lpParam)
{
	ASSERT(lpParam);
	DATA* pData=(DATA*)lpParam;
	BYTE* pHead=pData->pData;
	BYTE* pTail=pHead+pData->nLen;
	DWORD dw1=0,dw2=0;//像素点的RGB颜色值
	BYTE r1=0,g1=0,b1=0,r2=0,g2=0,b2=0;
	int nFlag=sizeof(DWORD);
	while(pTail>=pHead)
	{
		memcpy(&dw1,pHead,nFlag);
		memcpy(&dw2,pTail,nFlag);
		r1=GET_BRIGHT_RVALUE(dw1);
		g1=GET_BRIGHT_GVALUE(dw1);
		b1=GET_BRIGHT_BVALUE(dw1);
		r2=GET_BRIGHT_RVALUE(dw2);
		g2=GET_BRIGHT_GVALUE(dw2);
		b2=GET_BRIGHT_BVALUE(dw2);
		dw1=RGB(r1,g1,b1);
		dw2=RGB(r2,g2,b2);
		memcpy(pHead,&dw1,nFlag);
		memcpy(pTail,&dw2,nFlag);
		pHead+=nFlag;
		pTail-=nFlag;
	}
	delete pData;
	pData=NULL;
	return 0;
}
// CCapDlg 对话框
HBITMAP CCapDlg::m_hScreenBmp=NULL;
HBITMAP g_hScreenBmp=NULL;
IMPLEMENT_DYNAMIC(CCapDlg, CDialog)

CCapDlg::CCapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCapDlg::IDD, pParent)
	,m_curState(undo)
	,m_hMemDC1(NULL)
	,m_hMemDC2(NULL)
	,m_hCursor(NULL)
	,m_hBrightBmp(NULL)
	,m_pToolDlg(NULL)
{
	m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);
}

CCapDlg::~CCapDlg()
{//释放资源
	ReleaseDCHandle(&m_hMemDC1);
	ReleaseDCHandle(&m_hMemDC2);
	ReleaseGDIHandle((HANDLE*)&m_hScreenBmp);
	ReleaseGDIHandle((HANDLE*)&m_hBrightBmp);
	ReleaseGDIHandle((HANDLE*)&g_hScreenBmp);
	DestroyCursor(m_hCursor);
	if(m_pInfoDlg)
	{
		m_pInfoDlg->DestroyWindow();
		delete m_pInfoDlg;
		m_pInfoDlg=NULL;
	}
	if(m_pToolDlg)
	{
		m_pToolDlg->DestroyWindow();
		delete m_pToolDlg;
		m_pToolDlg=NULL;
	}
}

void CCapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCapDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDC_BUTTON1,OnSave)
	ON_COMMAND(IDC_BUTTON2,OnExit)
END_MESSAGE_MAP()
// 截取区域图片保存到句柄中

HBITMAP CCapDlg::GetScreenBitmap(LPRECT lpRect/*=NULL*/)
{
	RECT rect;
	if(NULL==lpRect)
	{
		rect.left=0;
		rect.top=0;
		rect.right=m_nxScreen;
		rect.bottom=m_nyScreen;
	}
	else
		rect=*lpRect;
	if(IsRectEmpty(&rect))
		return NULL;
	HDC hScreenDC,hMemDC;
	hScreenDC=::CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	hMemDC=::CreateCompatibleDC(hScreenDC);
	HBITMAP hBmp=NULL,hOldBmp=NULL;
	int nWidth=rect.right-rect.left;
	int nHeight=rect.bottom-rect.top;
	hBmp=CreateCompatibleBitmap(hScreenDC,nWidth,nHeight);
	hOldBmp=(HBITMAP)SelectObject(hMemDC,hBmp);
	BitBlt(hMemDC,0,0,nWidth,nHeight,\
		hScreenDC,rect.left,rect.top,SRCCOPY);
	hBmp=(HBITMAP)SelectObject(hMemDC,hOldBmp);
	ReleaseDCHandle(&hScreenDC);
	ReleaseDCHandle(&hMemDC);
	return hBmp;
}

BOOL CCapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化各句柄
	m_ptStart.x=0;
	m_ptStart.y=0;
	m_rcTrack.m_nStyle=CRectTracker::solidLine|CRectTracker::resizeOutside;
	m_rcTrack.m_rect.SetRect(-10,-10,-10,-10);
	HDC hSrc=::GetDC(NULL);
	m_nxScreen=GetDeviceCaps(hSrc,HORZRES);
	m_nyScreen=GetDeviceCaps(hSrc,VERTRES);
	::ReleaseDC(NULL,hSrc);
	//创建透明信息对话框
	m_pInfoDlg=new CInfoDlg(this);
	m_pInfoDlg->Create(IDD_INFO_DIALOG,this);
	//创建工具栏
	m_pToolDlg=new CToolDlg(this);
	m_pToolDlg->Create(IDD_TOOL_DIALOG,this);
	m_hScreenBmp=GetScreenBitmap();
	g_hScreenBmp=CopyBitmap(m_hScreenBmp);
	m_hBrightBmp=CopyBitmap(m_hScreenBmp);
	ChangeBitmapBright(m_hBrightBmp);
	MoveWindow(-1,-1,m_nxScreen+1,m_nyScreen+1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BOOL CCapDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	HDC hPaintDC=pDC->Detach();
	ASSERT(hPaintDC);
	if(m_hMemDC1==NULL)
		m_hMemDC1=CreateCompatibleDC(hPaintDC);
	if(m_hMemDC2==NULL)
	{
		m_hMemDC2=CreateCompatibleDC(m_hMemDC1);
		SelectObject(m_hMemDC2,m_hScreenBmp);
	}
	HBITMAP hOldBmp1=NULL,hCopyBmp=NULL;
	if(m_curState!=undo)
	{
		hCopyBmp=CopyBitmap(m_hBrightBmp);
		hOldBmp1=(HBITMAP)::SelectObject(m_hMemDC1,hCopyBmp);
		::BitBlt(m_hMemDC1,m_rcTrack.m_rect.left,m_rcTrack.m_rect.top,\
			m_rcTrack.m_rect.right-m_rcTrack.m_rect.left,\
			m_rcTrack.m_rect.bottom-m_rcTrack.m_rect.top,\
			m_hMemDC2,m_rcTrack.m_rect.left,m_rcTrack.m_rect.top,SRCCOPY);
	}
	else
		hOldBmp1=(HBITMAP)::SelectObject(m_hMemDC1,m_hBrightBmp);
	CDC dc;
	dc.Attach(m_hMemDC1);
	m_rcTrack.Draw(&dc);
	m_hMemDC1=dc.Detach();
	dc.DeleteDC();
	::BitBlt(hPaintDC,0,0,m_nxScreen,m_nyScreen,m_hMemDC1,0,0,SRCCOPY);
	::SelectObject(m_hMemDC1,hOldBmp1);
	ReleaseGDIHandle((HANDLE*)&hCopyBmp);
	pDC->Attach(hPaintDC);
	::DeleteDC(hPaintDC);
	TRACE(_T("RECT=%d,%d,%d,%d\n"),m_rcTrack.m_rect.left,m_rcTrack.m_rect.top\
		,m_rcTrack.m_rect.right,m_rcTrack.m_rect.bottom);
	return TRUE;
}
inline void CCapDlg::SendExitMsg()
{
	EndDialog(1);
}
LRESULT CCapDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	switch(message)
	{
	case WM_RBUTTONDOWN:
		{
			if(m_curState==undo)//没有截屏，直接退出
				SendExitMsg();
			else//截屏完成，取消截屏
			{
				CRect rc=m_rcTrack.m_rect;
				rc.InflateRect(4,4);
				m_rcTrack.m_rect.SetRect(-10,-10,-10,-10);
				m_curState=undo;
				m_pToolDlg->ShowWindow(SW_HIDE);
				InvalidateRect(&rc);
			}
			return 1;
		}
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_ESCAPE:
				{
					SendExitMsg();
					return 1;
				}
			case VK_RETURN:
				return 1;
			default:
				break;
			}
		}
	default:
		return CDialog::WindowProc(message, wParam, lParam);
	}
}

void CCapDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_curState==undo)
	{//开始截屏
		m_curState=doing;
		m_ptStart=point;
		m_pInfoDlg->ShowWindow(SW_HIDE);
	}
	else//截屏完成后
		m_rcTrack.Track(this,point,TRUE);
	CDialog::OnLButtonDown(nFlags, point);
}

void CCapDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(m_curState)
	{
	case undo:
		MoveInfoDlg();
		break;
	case doing:
		{
			m_rcTrack.m_rect.SetRect(m_ptStart,point);
			MoveToolDlg();
			Invalidate();
			break;
		}
	case finish:
		{
			static CRect rc;
			rc=m_rcTrack.m_rect;
			int nWidth=rc.Width();
			int nHeight=rc.Height();
			if(rc.left<0)
			{
				rc.left=0;
				rc.right=nWidth;
			}
			if(rc.top<0)
			{
				rc.top=0;
				rc.bottom=nHeight;
			}
			if(rc.right>m_nxScreen)
			{
				rc.left=m_nxScreen-nWidth;
				rc.right=m_nxScreen;
			}
			if(rc.bottom>m_nyScreen)
			{
				rc.top=m_nyScreen-nHeight;
				rc.bottom=m_nyScreen;
			}
			m_rcTrack.m_rect=rc;
			MoveToolDlg();
			InvalidateRect(&rc);
			break;
		}
	default:
		break;
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CCapDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_curState==doing)
	{
		TRACE(_T("finished\n"));
		m_curState=finish;
	}
	TRACE(_T("OnLButtonUp finished\n"));
	CDialog::OnLButtonUp(nFlags, point);
}

// 改变图像的亮度
void CCapDlg::ChangeBitmapBright(HBITMAP hBmp)
{
	ASSERT(hBmp);
	BITMAP bmMetric;
	GetObject(hBmp,sizeof(bmMetric),&bmMetric);
	DWORD dwCount=bmMetric.bmWidth*bmMetric.bmBitsPixel*bmMetric.bmHeight;//图像所占的内存比特数
	HGLOBAL hMem=GlobalAlloc(GHND,dwCount);
	BYTE* pBuffer=(BYTE*)GlobalLock(hMem);
	GetBitmapBits(hBmp,dwCount,pBuffer);
	int nFlag=sizeof(COLORREF);
	DWORD dwLen=dwCount/nFlag;//所有像素点的个数
	//每处理50,0000个像素点开一个线程
	int nThreadNum=(dwLen+49999)/500000;
	HANDLE* pThread=new HANDLE[nThreadNum];
	for(int i=0;i<nThreadNum;++i)
	{
		DATA* pData=new DATA;
		pData->pData=pBuffer+i*500000;
		pData->nLen=500000;
		if(i==nThreadNum-1)//最后一次循环
		{
			int nRet=dwLen%500000;
			if(nRet)
				pData->nLen=nRet;
		}
		pThread[i]=CreateThread(NULL,0,ChangeBrightThread,pData,0,NULL);
	}
	::WaitForMultipleObjects(nThreadNum,pThread,TRUE,INFINITE);
	delete[] pThread;
	pThread=NULL;
	SetBitmapBits(hBmp,dwCount,pBuffer);
	GlobalUnlock(hMem);
	GlobalFree(hMem);
}

BOOL CCapDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CPoint pt;
	::GetCursorPos(&pt);
	int nHitFlag=m_rcTrack.HitTest(pt);
	if(nHitFlag<0)//外部
		::SetCursor(m_hCursor);
	else if(nHitFlag==0||nHitFlag==2)//左上和右下
		::SetCursor(MAKECURSOR(IDC_SIZENWSE));
	else if(nHitFlag==1||nHitFlag==3)//左下和右上
		::SetCursor(MAKECURSOR(IDC_SIZENESW));
	else if(nHitFlag==4||nHitFlag==6)//顶部和底部
		::SetCursor(MAKECURSOR(IDC_SIZENS));
	else if(nHitFlag==5||nHitFlag==7)//左部和右部
		::SetCursor(MAKECURSOR(IDC_SIZEWE));
	else//内部
		::SetCursor(MAKECURSOR(IDC_SIZEALL));
	return TRUE;
	//return CDialog::OnSetCursor(pWnd, nHitTest, message);//进入后会调用默认的鼠标指针
}
void CCapDlg::MoveInfoDlg()
{
	static CPoint point;
	::GetCursorPos(&point);
	static RECT rect;
	if(point.x<9)
		rect.left=0;
	else if(point.x>m_nxScreen-9)
		rect.left=m_nxScreen-18;
	else
		rect.left=point.x-9;
	if(point.y<9)
		rect.top=0;
	else if(point.y>m_nyScreen-9)
		rect.top=m_nyScreen-18;
	else
		rect.top=point.y-9;
	rect.right=rect.left+18;
	rect.bottom=rect.top+18;
	m_pInfoDlg->SetBitmap(GetBitmapRect(g_hScreenBmp,&rect));//GetScreenBitmap(&rect)
	COLORREF color=::GetPixel(m_hMemDC2,point.x,point.y);
	m_pInfoDlg->SetRGBValue(RESET_RVALUE(color),RESET_GVALUE(color),RESET_BVALUE(color));
	m_pInfoDlg->SetRectValue(m_rcTrack.m_rect.Width(),m_rcTrack.m_rect.Height());
	m_pInfoDlg->Invalidate();
	static int xPos,yPos;
	if(point.x>(m_nxScreen-183)&&point.y>(m_nyScreen-168))
	{
		xPos=point.x-183;
		yPos=point.y-168;
	}
	else if(point.x>(m_nxScreen-183))
	{
		xPos=point.x-193;
		yPos=point.y+5;
	}
	else if(point.y>(m_nyScreen-193))
	{
		xPos=point.x-5;
		yPos=point.y-178;
	}
	else
	{
		xPos=point.x+5;
		yPos=point.y+30;
	}
	m_pInfoDlg->MoveWindow(xPos,yPos,200,160,TRUE);
	m_pInfoDlg->ShowWindow(SW_SHOW);
}

void CCapDlg::SaveBitmap(HBITMAP hBmp)
{
	SYSTEMTIME sm;
	::GetLocalTime(&sm);
	CString str;
	str.Format(_T("%d%02d%02d%02d%02d%02d"),sm.wYear,sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wMinute);
	CFileDialog fd(FALSE,_T("bmp"),_T("ScreenCapture")+str,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("Bmp  File   (*.bmp)|*.bmp|Jpeg File    (*.jpg)|*.jpg|Png  File    (*.png)|*.png|Gif  File      (*.gif)|*.gif||"));
	if(IDOK==fd.DoModal())
	{
		CImage image;
		image.Attach(hBmp);
		CString fullName=fd.m_ofn.lpstrFile;
		HRESULT hRet=image.Save(fullName);
		hBmp=image.Detach();
		if(FAILED(hRet))
			::MessageBox(NULL,_T("保存失败!"),_T("出错了:"),MB_OK|MB_ICONERROR);
	}
	::DeleteObject(hBmp);
}
// 移动工具栏
void CCapDlg::MoveToolDlg()
{
	if(IsRectEmpty(m_rcTrack.m_rect))
	{
		m_pToolDlg->ShowWindow(SW_HIDE);
		return;
	}
	static RECT rcToolDlg;
	if(m_rcTrack.m_rect.bottom<m_nyScreen-36)
		rcToolDlg.top=m_rcTrack.m_rect.bottom+6;
	else if(m_rcTrack.m_rect.top>36)
		rcToolDlg.top=m_rcTrack.m_rect.top-36;
	else
		rcToolDlg.top=m_rcTrack.m_rect.top+6;
	if(m_nxScreen-m_rcTrack.m_rect.left<190)
		rcToolDlg.left=m_rcTrack.m_rect.right-190;
	else
		rcToolDlg.left=m_rcTrack.m_rect.left;
	rcToolDlg.right=rcToolDlg.left+190;
	rcToolDlg.bottom=rcToolDlg.top+30;
	m_pToolDlg->SetSize(m_rcTrack.m_rect.Width(),m_rcTrack.m_rect.Height());
	m_pToolDlg->ShowWindow(SW_SHOW);
	m_pToolDlg->MoveWindow(&rcToolDlg);
}

void CCapDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_rcTrack.m_rect.PtInRect(point))
	{
		//保存截屏区域位图到剪切板
		if(::OpenClipboard(this->GetSafeHwnd()))
		{
			if(::EmptyClipboard())
			{
				HBITMAP hBmp=GetBitmapRect(g_hScreenBmp,&m_rcTrack.m_rect);
				::SetClipboardData(CF_BITMAP,hBmp);
				ReleaseGDIHandle((HANDLE*)&hBmp);
				::CloseClipboard();
			}
		}
		SendExitMsg();
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}
void CCapDlg::OnSave()
{
	HBITMAP hSaveBmp=GetBitmapRect(g_hScreenBmp,&m_rcTrack.m_rect);
	SaveBitmap(hSaveBmp);
	SendExitMsg();
}
void CCapDlg::OnExit()
{
	SendExitMsg();
}
BOOL CCapDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		CRect* pRect=&m_rcTrack.m_rect;
		switch(pMsg->wParam)
		{
		case VK_UP:
			{
				if(pRect->top>0)
				{
					--pRect->top;
					--pRect->bottom;
					Invalidate();
					MoveToolDlg();
				}
				return 1;
			}
		case VK_DOWN:
			{
				if(pRect->bottom<m_nyScreen)
				{
					++pRect->top;
					++pRect->bottom;
					Invalidate();
					MoveToolDlg();
				}
				return 1;
			}
		case VK_LEFT:
			{
				if (pRect->left>0)
				{
					--pRect->left;
					--pRect->right;
					Invalidate();
					MoveToolDlg();
				}
				return 1;
			}
		case VK_RIGHT:
			{
				if(pRect->right<m_nxScreen)
				{
					++pRect->left;
					++pRect->right;
					Invalidate();
					MoveToolDlg();
				}
				return 1;
			}
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

