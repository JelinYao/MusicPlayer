// NoLyricsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MusicPlayer.h"
#include "NoLyricsDlg.h"


// CNoLyricsDlg 对话框

IMPLEMENT_DYNAMIC(CNoLyricsDlg, CDialog)

CNoLyricsDlg::CNoLyricsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNoLyricsDlg::IDD, pParent)
{

}

CNoLyricsDlg::~CNoLyricsDlg()
{
}

void CNoLyricsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_linkBtn1);
}


BEGIN_MESSAGE_MAP(CNoLyricsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CNoLyricsDlg::OnLoadLyrics)
END_MESSAGE_MAP()


// CNoLyricsDlg 消息处理程序

void CNoLyricsDlg::OnLoadLyrics()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(m_hWnd,_T("open"), _T("http://www.5ilrc.com"),_T(""),_T(""),SW_SHOW );
}

BOOL CNoLyricsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_linkBtn1.Init(_T("点击下载歌词"),_T("链接歌词网站"));
	m_linkBtn1.SetBkColor(RGB(240,240,240));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
