// LyricsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LyricsModule.h"
#include "LyricsDlg.h"

#include <Shlwapi.h>
// CLyricsDlg 对话框

IMPLEMENT_DYNAMIC(CLyricsDlg, CDialog)

CLyricsDlg::CLyricsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLyricsDlg::IDD, pParent)
	,CErrorLog(_T("CLyricsDlg"))
	, m_bResizing(FALSE)
	, m_bMouseOver(FALSE)
	, m_iDirect(0)
	, m_strShowText(_T("音乐，让生活更美好"))
	, m_nIndex(0)
	, m_bFileExist(TRUE)
	, m_strCurLyricsPath(_T(""))
	, m_nFontHeight(0)
	,m_hFont(NULL)
	,m_bSizeChange(FALSE)
	,m_bLock(FALSE)
	,m_fSpeed(0.0)
{
	m_cUnderColor=RGB(0,200,0);
	m_cOverColor=RGB(200,0,0);
	InitializeCriticalSection(&cs);
}

CLyricsDlg::~CLyricsDlg()
{
	ReleaseLyricDatas();
	ReleaseGDIHandle((HANDLE*)&m_hFont);
	DeleteCriticalSection(&cs);
}

void CLyricsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLyricsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
ON_WM_ERASEBKGND()
ON_COMMAND(ID_32773, &CLyricsDlg::OnRBSetColor)
ON_COMMAND(ID_32774, &CLyricsDlg::OnRBSetFont)
ON_WM_RBUTTONDOWN()
ON_COMMAND(ID_32775, &CLyricsDlg::OnSetOverColor)
ON_WM_SIZE()
ON_WM_MOVE()
END_MESSAGE_MAP()


// CLyricsDlg 消息处理程序

BOOL CLyricsDlg::OnInitDialog()
{
	BOOL bRet=TRUE;
	EXCEPTION_INFO ei;
	try
	{
		bRet=CDialog::OnInitDialog();
		SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)|0x80000);
		SetLayeredWindowAttributes(0xffffff,100,LWA_COLORKEY);
		m_vecLyricDatas.reserve(100);
		m_hFont=::CreateFont(50, 0, 0, 0, 900,
			FALSE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, 
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, 
			DEFAULT_PITCH|FF_SWISS, _T("宋体"));
		m_nFontHeight=50;
		CRect rcSave;
		if(!ReadLyricsDialogData(&rcSave))
		{
			int nxScreen=::GetSystemMetrics(SM_CXSCREEN);
			int nyScreen=::GetSystemMetrics(SM_CYSCREEN);
			rcSave.left=(nxScreen-700)/2;
			rcSave.right=rcSave.left+700;
			rcSave.top=0;
			rcSave.bottom=60;
		}
		if(!m_bLock)
			SetTimer(1,300,NULL);
		MoveWindow(&rcSave);
	}
	catch(...)
	{
		bRet=FALSE;
		ei.tstrExceptionFunc=_T("OnInitDialog");
		WriteErrorLog(&ei);
	}
	return bRet;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CLyricsDlg::OnPaint()
{
	EXCEPTION_INFO ei;
	try
	{
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 
		if(NULL==m_hMemDC)
		{
			m_hMemDC=CreateCompatibleDC(dc.m_hDC);
			::SetBkMode(m_hMemDC,TRANSPARENT);
		}
		HFONT hOldFont=(HFONT)::SelectObject(m_hMemDC,m_hFont);
		if(m_bSizeChange)
		{
			ReleaseGDIHandle((HANDLE*)&m_hMemBitmap);
			m_hMemBitmap=CreateCompatibleBitmap(dc.m_hDC,m_rcClient.Width(),m_rcClient.Height());
			::SelectObject(m_hMemDC,m_hMemBitmap);
			m_bSizeChange=FALSE;
		}
		if(m_bMouseOver)
			FillDCRect(m_hMemDC,&m_rcClient,0xff00ff);
		else
			FillDCRect(m_hMemDC,&m_rcClient,0xffffff);
		if(m_vecLyricDatas.empty())
		{
			::SetTextColor(m_hMemDC,0x0000ff);
			::SetBkMode(m_hMemDC,TRANSPARENT);
			::DrawText(m_hMemDC,m_strShowText,m_strShowText.GetLength(),&m_rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		else
			DrawLyricText(m_hMemDC);
		::BitBlt(dc.m_hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),\
			m_hMemDC,0,0,SRCCOPY);
		::SelectObject(m_hMemDC,hOldFont);
	}
	catch(const int nCode)
	{
		ei.tstrExceptionFunc=_T("OnPaint");
		WriteErrorLog(&ei);
	}
}

void CLyricsDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bLock)
	{
		if(m_bResizing) 
		{ 
			WPARAM wp=0;
			if((m_iDirect==left)||(m_iDirect==right))
			{
				SetCursor(::LoadCursor(NULL,IDC_SIZEWE));
				if(m_iDirect==left)
					wp=(WPARAM)HTLEFT;
				else
					wp=(WPARAM)HTRIGHT;
			}
			if((m_iDirect==top)||(m_iDirect==bottom))
			{
				SetCursor(::LoadCursor(NULL,IDC_SIZENS));
				if(m_iDirect==top)
					wp=(WPARAM)HTTOP;
				if(m_iDirect==bottom)
					wp=(WPARAM)HTBOTTOM;
			}
			SendMessage(WM_NCLBUTTONDOWN,wp, MAKELPARAM(point.x,   point.y)) ; 
		} 
		else
			SendMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
	}
	CDialog::OnLButtonDown(nFlags, point);
}
void CLyricsDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialog::OnMouseMove(nFlags, point);
	if(m_bMouseOver)
	{
		LPWSTR lpCursorID=IDC_SIZEALL;
		if   (!nFlags&MK_LBUTTON) 
		{ 
			if(point.x<10)
			{
				m_iDirect=left;
				lpCursorID=IDC_SIZEWE;
				m_bResizing=TRUE   ;
				goto END;
			}
			if(m_rcClient.Width()-point.x < 10)   
			{ 
				m_iDirect=right;
				lpCursorID=IDC_SIZEWE;
				m_bResizing=TRUE; 
				goto END;
			} 
			if(point.y<10)
			{
				m_iDirect=top;
				lpCursorID=IDC_SIZENS;
				m_bResizing=TRUE;
				goto END;
			}
			if(m_rcClient.Height()-point.y<10)
			{
				m_iDirect=bottom;
				lpCursorID=IDC_SIZENS;
				m_bResizing=TRUE;
				goto END;
			}
			m_bResizing=FALSE;
		}
END:
		SetCursor(::LoadCursor(NULL,lpCursorID)); 
	}
}

void CLyricsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(1==nIDEvent)
	{
		static CPoint pt;
		::GetCursorPos(&pt);
		ScreenToClient(&pt);
		if(m_rcClient.PtInRect(pt))
			m_bMouseOver=TRUE;
		else
			m_bMouseOver=FALSE;
		Invalidate();
	}
	CDialog::OnTimer(nIDEvent);
}

BOOL CLyricsDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	pDC->FillSolidRect(&m_rcClient,0xffffff);
	return TRUE;
}

void CLyricsDlg::OnRBSetColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog dlg;
	if(IDOK==dlg.DoModal())
	{
		m_cUnderColor=dlg.GetColor();
		Invalidate();
	}
}

void CLyricsDlg::OnRBSetFont()
{
	// TODO: 在此添加命令处理程序代码
	CFontDialog fdlg;
	if(IDOK==fdlg.DoModal())
	{
		LOGFONT lf;
		fdlg.GetCurrentFont(&lf);
		ReleaseGDIHandle((HANDLE*)&m_hFont);
		m_nFontHeight=fdlg.GetSize()/10;
		m_hFont=::CreateFontIndirect(&lf);
		Invalidate();
	}
}


void CLyricsDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_bMouseOver)
		return;
	CPoint pt;
	::GetCursorPos(&pt);
	CMenu menu,*pMenu=NULL;
	menu.LoadMenu(IDR_MENU1);
	pMenu=menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_RIGHTBUTTON,pt.x,pt.y,this);
	pMenu->DestroyMenu();
	CDialog::OnRButtonDown(nFlags, point);
}


// 读取歌词文件
BOOL CLyricsDlg::ReadLyricsFile()
{
	m_bFileExist=TRUE;
	ReleaseLyricDatas();
	CReadFile cf(m_strCurLyricsPath);
	CODETYPE ct=cf.GetFileCodeType();
	std::string strLine;
	CCodeCovert cc;
	while(cf.ReadLine(strLine))
	{
		if(CODETYPE::utf8==ct)
			strLine=cc.Utf8ToAscii(strLine);
		AnaliseString(strLine);
	}
	SortLyricDatas();
	return TRUE;
}

// 分解出字符串中的时间和歌词语句
void CLyricsDlg::AnaliseString(std::string& strLineText)
{
	try
	{
		int nLen=strLineText.size();
		if(nLen<3)
			return;
		int nLastPos=strLineText.find_last_of(']');
		if(std::string::npos==nLastPos)//没有找到
			return;
		std::string strText;
		if(nLastPos==nLen-1)
			strText="……  Music  ……";
		else
		{
			strText=strLineText.substr(nLastPos+1,nLen-nLastPos-1);
			if(strText.find("QQ")!=std::string::npos)
				strText="……  Music  ……";
			strLineText.erase(nLastPos+1);
		}
		int nHeader=0,nTail=0;
		std::string strValue;//夹在[]之间的字符串
		LYRIC_DATA* pData=NULL;
		while(true)
		{
			nHeader=strLineText.find_first_of('[');
			if(nHeader==std::string::npos)
				break;
			nTail=strLineText.find_first_of(']');
			if(nTail==std::string::npos)
				break;
			strValue=strLineText.substr(nHeader+1,nTail-nHeader-1);
			UINT uTime=0;
			int nPos1=strValue.find(':');
			if(strValue[0]>0&&isdigit(strValue[0]))//是数字
			{
				if(nPos1==std::string::npos)
					throw _T("歌词格式有误");
				std::string strMin,strSec,strMs;
				int nPos2=strValue.find('.');
				if(nPos2==std::string::npos)
					strMs="0";
				else
					strMs=strValue.substr(nPos2+1);
				strMin=strValue.substr(0,nPos1);
				strSec=strValue.substr(nPos1+1,nPos2-nPos1-1);
				uTime=(atoi(strMin.c_str())*60+atoi(strSec.c_str()))*1000+atoi(strMs.c_str());
			}
			else
			{
				if(nPos1==std::string::npos)
					strText=strValue;
				else
					strText=strValue.erase(0,nPos1+1);
				if(m_vecLyricDatas.empty())
					uTime=300;
				else
					uTime=m_vecLyricDatas[m_vecLyricDatas.size()-1]->nStartTime+300;
			}
			pData=new LYRIC_DATA;
			pData->nStartTime=uTime;
			pData->strLyricText=strText;
			m_vecLyricDatas.push_back(pData);
			strLineText.erase(0,nTail+1);
		}
	}
	catch(wchar_t* pError)
	{
		MessageBox(pError,_T("异常信息："),MB_OK|MB_ICONERROR);
	}
}

// 改变歌词的时间
void CLyricsDlg::ChangeCurTime(const int nTime)
{
	EnterCriticalSection(&cs);
	int nSize=m_vecLyricDatas.size();
	if(nSize)
	{
		m_nTotalTime=nTime;
		BOOL bInvalidateAll=FALSE;
		m_nIndex=0;
		if(nTime>m_vecLyricDatas[nSize-1]->nStartTime)//最后一句了
		{
			m_nIndex=nSize-1;
			m_bRoll=FALSE;
			return;
		}
		else
		{
			while(m_nIndex<nSize&&nTime>m_vecLyricDatas[m_nIndex]->nStartTime)
				++m_nIndex;
			if(m_nIndex>0)
				--m_nIndex;
		}
		static int nNumber=-1;
		if(nNumber!=m_nIndex)//播放的索引改变了
		{
			nNumber=m_nIndex;
			CString strTemp(m_vecLyricDatas[m_nIndex]->strLyricText.c_str());
			m_strShowText=strTemp;
			bInvalidateAll=TRUE;
			CSize sz;
			HFONT hOldFont=(HFONT)::SelectObject(m_hMemDC,m_hFont);
			::GetTextExtentPoint32(m_hMemDC,m_strShowText,m_strShowText.GetLength(),&sz);
			::SelectObject(m_hMemDC,hOldFont);
			m_nTextExtent=sz.cx;
			m_fSpeed=m_nTextExtent/(float)(m_vecLyricDatas[m_nIndex+1]->nStartTime-m_vecLyricDatas[m_nIndex]->nStartTime);
			if(sz.cx>m_rcClient.Width())
				m_bRoll=TRUE;
			else
				m_bRoll=FALSE;
			TRACE("speed=%f\n",m_fSpeed);
		}
		if(m_bRoll)
		{
			int nRemaTime=(m_nTextExtent-m_rcClient.Width())/m_fSpeed;
			int nRemaind=m_nTotalTime-m_vecLyricDatas[m_nIndex]->nStartTime;
			if(nRemaind<nRemaTime)
			{
				m_bFinishRoll=FALSE;
				if(nRemaind<200)
					m_rcDraw.left=0;
				else
					m_rcDraw.left=-(m_nTextExtent-m_rcClient.Width())*nRemaind/(float)nRemaTime;
			}
			else
				m_bFinishRoll=TRUE;
			m_rcOver.left=m_rcDraw.left;
		}
		else
			m_rcOver.left=(m_rcClient.Width()-m_nTextExtent)/2.0;
		////////////////////////////////////////////////////////////////////////
		int nValue=0;
		if((m_nIndex<nSize-1)&&m_nTotalTime+100>=m_vecLyricDatas[m_nIndex+1]->nStartTime)
		{
			if(!m_bRoll)
				nValue=(m_rcClient.Width()-m_nTextExtent)/2.0+m_nTextExtent;
			else
				nValue=m_rcClient.Width();
			bInvalidateAll=TRUE;
		}
		else
			nValue=(m_nTotalTime-m_vecLyricDatas[m_nIndex]->nStartTime)*m_fSpeed;
		m_rcOver.right=nValue;
		//////////////////////////////////
		if(bInvalidateAll||(m_bRoll&&(!m_bFinishRoll)))
			Invalidate();
		else
		{
			RECT rect;
			rect.left=nValue-m_fSpeed*150;
			rect.right=nValue+m_fSpeed*150;
			rect.top=0;
			rect.bottom=m_rcClient.bottom;
			InvalidateRect(&rect);
		}
	}
	LeaveCriticalSection(&cs);
}

void CLyricsDlg::FileNotExist()
{
	m_bFileExist=FALSE;
	m_strShowText=_T("音乐，让生活更美好");
	ReleaseLyricDatas();
	Invalidate();
}
void CLyricsDlg::LockLyrics(BOOL bRet)
{
	try
	{
		if(bRet)//锁定
		{
			KillTimer(1);
			m_bMouseOver=FALSE;
			m_bLock=TRUE;
			Invalidate();
		}
		else//解锁
		{
			m_bLock=FALSE;
			SetTimer(1,300,NULL);
		}
	}
	catch(...)
	{
		EXCEPTION_INFO ei=FillExceptionInfo(_T("未知异常"),__LINE__,__FILE__,::GetLastError(),_T("LockLyrics"));
		WriteErrorLog(&ei);
	}
}


void CLyricsDlg::OnSetOverColor()
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		CColorDialog dlg;
		if(IDOK==dlg.DoModal())
			m_cOverColor=dlg.GetColor();
	}
	catch(...)
	{
		EXCEPTION_INFO ei=FillExceptionInfo(_T("未知异常"),__LINE__,__FILE__,::GetLastError(),_T("OnSetOverColor"));
		WriteErrorLog(&ei);
	}
}
inline int compare(const PLYRIC_DATA pElem1,const PLYRIC_DATA pElem2)
{
	int nResult=0;
	if(pElem1->nStartTime>pElem2->nStartTime)
		nResult=1;
	else if(pElem1->nStartTime<pElem2->nStartTime)
		nResult=-1;
	return nResult;
}
void CLyricsDlg::SortLyricDatas()//给打乱的时间排序
{
	int nSize=m_vecLyricDatas.size();
	for(int i=0;i<nSize-1;++i)
		for(int j=0;j<nSize-i-1;++j)
		{
			if(1==compare(m_vecLyricDatas[j],m_vecLyricDatas[j+1]))
			{
				PLYRIC_DATA temp=m_vecLyricDatas[j];
				m_vecLyricDatas[j]=m_vecLyricDatas[j+1];
				m_vecLyricDatas[j+1]=temp;
			}
		}
}
void CLyricsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if(::IsWindow(m_hWnd))
	{
		GetClientRect(&m_rcClient);
		m_rcDraw=m_rcClient;
		m_rcOver=m_rcClient;
		m_bSizeChange=TRUE;
	}
	CRect rcTemp=m_rcClient;
	ClientToScreen(&rcTemp);
	int nTopPos=0;
	if(m_rcClient.Height()<m_nFontHeight+15)
	{
		if(top==m_iDirect)
			nTopPos=rcTemp.bottom-m_nFontHeight-15;
		else if (bottom==m_iDirect)
			nTopPos=rcTemp.top;
		MoveWindow(rcTemp.left,nTopPos,rcTemp.Width(),m_nFontHeight+15);
	}
	else if(m_rcClient.Height()>m_nFontHeight+30)
	{
		if(top==m_iDirect)
			nTopPos=rcTemp.bottom-m_nFontHeight-30;
		else if(bottom==m_iDirect)
			nTopPos=rcTemp.top;
		MoveWindow(rcTemp.left,nTopPos,rcTemp.Width(),m_nFontHeight+30);
	}
	Invalidate();
}

void CLyricsDlg::OnMove(int x, int y)
{
	try
	{
		CDialog::OnMove(x, y);
		// TODO: 在此处添加消息处理程序代码
		static int nyScreen=::GetSystemMetrics(SM_CYSCREEN);
		if(y+m_rcClient.Height()>=nyScreen)
			MoveWindow(x,nyScreen-m_rcClient.Height(),m_rcClient.Width(),m_rcClient.Height());
	}
	catch(...)
	{
	}
}

// 保存窗口坐标到文件
BOOL CLyricsDlg::SaveLyricsDialogData(void)
{
	BOOL bRet=TRUE;
	try
	{
		CString strPath=CString(GetAppPath().c_str())+_T("\\AppData\\LyricsDlgInfo.xml");
		CMarkup xml;
		if(!PathFileExists(strPath))
		{	//创建文件
			xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>"));
			xml.AddElem(_T("config"));
			xml.IntoElem();
				xml.AddElem(_T("DialogRect"));
				xml.AddAttrib(_T("xPos"),0);
				xml.AddAttrib(_T("yPos"),0);
				xml.AddAttrib(_T("Width"),0);
				xml.AddAttrib(_T("Height"),0);
				xml.AddElem(_T("TextColor"));
					xml.IntoElem();
						xml.AddElem(_T("OverColor"));
						xml.AddAttrib(_T("R"),0);
						xml.AddAttrib(_T("G"),0);
						xml.AddAttrib(_T("B"),0);
						xml.AddElem(_T("UnderColor"));
						xml.AddAttrib(_T("R"),0);
						xml.AddAttrib(_T("G"),0);
						xml.AddAttrib(_T("B"),0);
					xml.OutOfElem();
				xml.AddElem(_T("LockDialog"),0);
			xml.OutOfElem();
			xml.Save(strPath);
		}
		xml.Load(strPath);
		CRect rcClient=m_rcClient;
		ClientToScreen(&rcClient);
		while(xml.FindElem(_T("config")))
		{
			xml.IntoElem();
				if(xml.FindElem(_T("DialogRect")))
				{
					xml.SetAttrib(_T("xPos"),rcClient.left);
					xml.SetAttrib(_T("yPos"),rcClient.top);
					xml.SetAttrib(_T("Width"),rcClient.Width());
					xml.SetAttrib(_T("Height"),rcClient.Height());
				}
				if(xml.FindElem(_T("TextColor")))
				{
					xml.IntoElem();
						if(xml.FindElem(_T("OverColor")))
						{
							xml.SetAttrib(_T("R"),GetRValue(m_cOverColor));
							xml.SetAttrib(_T("G"),GetGValue(m_cOverColor));
							xml.SetAttrib(_T("B"),GetBValue(m_cOverColor));
						}
						if(xml.FindElem(_T("UnderColor")))
						{
							xml.SetAttrib(_T("R"),GetRValue(m_cUnderColor));
							xml.SetAttrib(_T("G"),GetGValue(m_cUnderColor));
							xml.SetAttrib(_T("B"),GetBValue(m_cUnderColor));
						}						
					xml.OutOfElem();
				}
				if(xml.FindElem(_T("LockDialog")))
					xml.SetData(m_bLock);
				else
					xml.AddElem(_T("LockDialog"),m_bLock);
		}
		xml.Save(strPath);
		//SetFileAttributes(strPath,FILE_ATTRIBUTE_HIDDEN);//设置文件属性为隐藏
	}
	catch(...)
	{
		bRet=FALSE;
		EXCEPTION_INFO ei=FillExceptionInfo(_T("未知异常"),__LINE__,__FILE__,::GetLastError(),_T("SaveLyricsDialogData"));
		WriteErrorLog(&ei);
	}
	return bRet;
}

// 读取文件中存放的歌词面板信息
BOOL CLyricsDlg::ReadLyricsDialogData(LPRECT pRect)
{
	BOOL bRet=TRUE;
	try
	{
		CString strPath=CString(GetAppPath().c_str())+_T("\\AppData\\LyricsDlgInfo.xml");
		if(!PathFileExists(strPath))//文件不存在
			return FALSE;
		CMarkup xml;
		xml.Load(strPath);
		while(xml.FindElem(_T("config")))
		{
			xml.IntoElem();
			if(xml.FindElem(_T("DialogRect")))
			{
				CString strValue;
				strValue=xml.GetAttrib(_T("xPos"));
				pRect->left=_ttoi(strValue);
				strValue=xml.GetAttrib(_T("yPos"));
				pRect->top=_ttoi(strValue);
				strValue=xml.GetAttrib(_T("Width"));
				pRect->right=pRect->left+_ttoi(strValue);
				strValue=xml.GetAttrib(_T("Height"));
				pRect->bottom=pRect->top+_ttoi(strValue);
			}
			if(xml.FindElem(_T("TextColor")))
			{
				xml.IntoElem();
				CString strRed,strGreen,strBlue;
				if(xml.FindElem(_T("OverColor")))
				{
					strRed=xml.GetAttrib(_T("R"));
					strGreen=xml.GetAttrib(_T("G"));
					strBlue=xml.GetAttrib(_T("B"));
					m_cOverColor=RGB(_ttoi(strRed),_ttoi(strGreen),_ttoi(strBlue));
				}
				if(xml.FindElem(_T("UnderColor")))
				{
					strRed=xml.GetAttrib(_T("R"));
					strGreen=xml.GetAttrib(_T("G"));
					strBlue=xml.GetAttrib(_T("B"));
					m_cUnderColor=RGB(_ttoi(strRed),_ttoi(strGreen),_ttoi(strBlue));
				}
				xml.OutOfElem();
			}
			if(xml.FindElem(_T("LockDialog")))
				m_bLock=_ttoi(xml.GetData());
		}
	}
	catch(...)
	{
		EXCEPTION_INFO ei=FillExceptionInfo(_T("未知异常"),__LINE__,__FILE__,::GetLastError(),_T("ReadLyricsDialogData"));
		WriteErrorLog(&ei);
		bRet=FALSE;
	}
	return bRet;
}

void CLyricsDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialog::OnCancel();
}

void CLyricsDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PostNcDestroy();
	delete this;
}

void CLyricsDlg::ReleaseLyricDatas()
{
	LYRIC_DATA* pData=NULL;
	for(int i=0;i<(int)m_vecLyricDatas.size();++i)
	{
		pData=m_vecLyricDatas[i];
		if(pData)
		{
			delete pData;
			pData=NULL;
		}
	}
	m_vecLyricDatas.clear();
	m_nIndex=0;
	m_nTotalTime=0;
}

void CLyricsDlg::DrawLyricText( HDC hDC )
{
	::SetBkMode(hDC,TRANSPARENT);
	::SetTextColor(hDC,m_cUnderColor);
	if(m_bRoll)
		::DrawText(hDC,m_strShowText,m_strShowText.GetLength(),&m_rcDraw,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	else
		::DrawText(hDC,m_strShowText,m_strShowText.GetLength(),&m_rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	::SetTextColor(hDC,m_cOverColor);
	::DrawText(hDC,m_strShowText,m_strShowText.GetLength(),&m_rcOver,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
}
