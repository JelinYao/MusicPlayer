// ChangeClothDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ChangeClothDlg.h"

// CChangeClothDlg 对话框

IMPLEMENT_DYNAMIC(CChangeClothDlg, CDialog)

CChangeClothDlg::CChangeClothDlg(const BYTE bAlphaValue,const CString& strBackImagePath,CWnd* pParent /*=NULL*/)
	: CDialog(CChangeClothDlg::IDD, pParent)
	,CErrorLog(_T("CChangeClothDlg"))
	,m_bAlphaValue(bAlphaValue)
	,m_strBackImagePath(strBackImagePath)
{
	memset(&m_indSelect,0,sizeof(m_indSelect));
}

CChangeClothDlg::~CChangeClothDlg()
{
	CImageCtrl* pCtrl=NULL;
	for(int i=0;i<2;++i)
		for (int j=0;j<9;++j)
		{
			pCtrl=m_arrContrls[i][j];
			if(pCtrl)
			{
				delete pCtrl;
				pCtrl=NULL;
			}
		}
}

void CChangeClothDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_btnTransparent);
	DDX_Control(pDX, IDC_BUTTON2, m_btnChange);
}


BEGIN_MESSAGE_MAP(CChangeClothDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON2, &CChangeClothDlg::OnChangeImages)
END_MESSAGE_MAP()


// CChangeClothDlg 消息处理程序

void CChangeClothDlg::OnPaint()
{
	EXCEPTION_INFO ei;
	try
	{
		CPaintDC dc(this); // device context for painting
		// TODO: 在此处添加消息处理程序代码
		// 不为绘图消息调用 CDialog::OnPaint()
		if(NULL==m_hMemDC)
		{
			m_hMemDC=::CreateCompatibleDC(dc.m_hDC);
			if(NULL==m_hMemDC)
				THROW_ERROR(CREATE_COMDC_ERROR);
			m_hMemBitmap=::LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_BACK));
			if(NULL==m_hMemBitmap)
				THROW_ERROR("加载位图失败！");
			::SelectObject(m_hMemDC,m_hMemBitmap);
		}
		//HBITMAP hOldBitmap=(HBITMAP)::SelectObject(m_hMemDC,m_hMemBitmap);
		if(!::BitBlt(dc.m_hDC,0,0,m_rcClient.Width(),m_rcClient.Height(),\
			m_hMemDC,0,0,SRCCOPY))
			THROW_ERROR("BitBlt双缓冲贴图失败！");
		//::SelectObject(m_hMemDC,hOldBitmap);
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("OnPaint");
		WriteErrorLog(&ei);
	}
}


BOOL CChangeClothDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd,GWL_EXSTYLE)|0x80000);
	SetLayeredWindowAttributes(0xffffff,255	,LWA_ALPHA);
	RECT rect={0,0,0,0};
	CImageCtrl* pCtrl=NULL;
	CString strPath,strTipText;
	WCHAR szText[][2]={_T("一"),_T("二"),_T("三"),_T("四"),_T("五"),_T("六"),_T("七"),_T("八"),_T("九")};
	std::wstring wstrModulePath=GetAppPath();
	for(int nGroup=0;nGroup<2;++nGroup)
	{
		for(int i=0;i<3;++i)
			for(int j=0;j<3;++j)
			{
				rect.left=70*j+10;
				rect.top=70*i+30;
				rect.right=rect.left+60;
				rect.bottom=rect.top+60;
				pCtrl=new CImageCtrl;
				pCtrl->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rect,this,1);
				if(nGroup)//第二组，全是美女
				{
					strPath.Format(_T("%s\\BackImage\\m%d.jpg"),wstrModulePath.c_str(),i*3+j+1);
					strTipText.Format(_T("美女%s"),szText[i*3+j]);
					
				}
				else//第一组
				{
					strPath.Format(_T("%s\\BackImage\\%d.jpg"),wstrModulePath.c_str(),i*3+j+1);
					strTipText.Format(_T("风景%s"),szText[i*3+j]);
					pCtrl->ShowWindow(SW_HIDE);
				}
				pCtrl->Init(strPath,strTipText);
				pCtrl->SetIndex(nGroup,i*3+j);
				if(m_strBackImagePath==strPath)
				{
					m_indSelect.nGroupNumber=nGroup;
					m_indSelect.nIndex=i*3+j;
					pCtrl->SetCheck();
				}
				m_arrContrls[nGroup][i*3+j]=pCtrl;
			}
	}
	m_btnTransparent.MoveWindow(50,250,160,5);
	m_btnTransparent.InitCtrl(IDB_SLIDER,_T("设置透明度"),255,WM_IMAGECTRL_MESSAGE,4);
	m_btnTransparent.SetCurPos(m_bAlphaValue);
	m_btnChange.MoveWindow(0,0,51,25);
	m_btnChange.Init(IDB_BITMAP_CHANGE1,IDB_BITMAP_CHANGE2,_T("换一批试试"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LRESULT CChangeClothDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(WM_IMAGECTRL_MESSAGE==message)
	{
		if((m_indSelect.nGroupNumber!=(int)wParam)||(m_indSelect.nIndex!=(int)lParam))
		{
			m_arrContrls[m_indSelect.nGroupNumber][m_indSelect.nIndex]->UnCheck();
			m_indSelect.nGroupNumber=(int)wParam;
			m_indSelect.nIndex=(int)lParam;
			m_arrContrls[m_indSelect.nGroupNumber][m_indSelect.nIndex]->SetCheck();
		}
		return TRUE;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

void CChangeClothDlg::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(IsWindow(m_hWnd))
	{
		GetClientRect(&m_rcClient);
		if(IsWindow(m_btnChange.GetSafeHwnd()))
			m_btnChange.ChangeBack();
	}
}

BOOL CChangeClothDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CChangeClothDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nCurPos=m_btnTransparent.GetCurPos();
	if(zDelta==120)
		nCurPos+=10;
	else
		nCurPos-=10;
	if(nCurPos>255)
		nCurPos=255;
	else if(nCurPos<0)
		nCurPos=0;
	m_btnTransparent.SetCurPos(nCurPos);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

HBRUSH CChangeClothDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if(CTLCOLOR_STATIC==nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(0XF000F0);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CChangeClothDlg::OnChangeImages()
{
	// TODO: 在此添加控件通知处理程序代码
	static BOOL bFlag=TRUE;//标识用
	for(int nGroup=0;nGroup<2;++nGroup)
		for (int i=0;i<9;++i)
		{
				m_arrContrls[bFlag][i]->ShowWindow(SW_HIDE);
				m_arrContrls[!bFlag][i]->ShowWindow(SW_SHOW);
		}
	bFlag=!bFlag;
}
