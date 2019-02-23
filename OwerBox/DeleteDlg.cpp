// DeleteDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OwerBox.h"
#include "DeleteDlg.h"

//自定义消息

// CDeleteDlg 对话框

IMPLEMENT_DYNAMIC(CDeleteDlg, CDialog)

CDeleteDlg::CDeleteDlg(const CString& strPath,CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteDlg::IDD, pParent)
	,m_strFilePath(strPath)
{

}

CDeleteDlg::~CDeleteDlg()
{
}

void CDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, m_icon);
	DDX_Control(pDX, IDC_CHECK3, m_checkBox);
}


BEGIN_MESSAGE_MAP(CDeleteDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDeleteDlg::OnDeleteOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CDeleteDlg::OnDeleteCancel)
END_MESSAGE_MAP()


// CDeleteDlg 消息处理程序

BOOL CDeleteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	hIcon=AfxGetApp()->LoadIcon(IDI_ICON1);
	//GetDlgItem(IDC_IMAGE)->SetIcon(hIcon,TRUE);
	m_icon.SetIcon(hIcon);
	int pos=m_strFilePath.ReverseFind('\\');
	CString strName=m_strFilePath.Mid(pos+1);
	pos=strName.ReverseFind('.');
	strName=_T("是否删除歌曲：")+strName.Left(pos);
	SetWindowText(strName);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDeleteDlg::OnDeleteOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_checkBox.GetCheck())//选中了，删除文件
	{
		if(!DeleteFile(m_strFilePath))
			MessageBox(_T("可能是文件正在被使用"),_T("删除文件失败!"),MB_OK|MB_ICONQUESTION);
	}
	::PostMessage(::GetParent(m_hWnd),WM_DELETE_DIALOG,1,0);
}

void CDeleteDlg::OnDeleteCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostMessage(::GetParent(m_hWnd),WM_DELETE_DIALOG,2,0);
}

// 保存路径
void CDeleteDlg::SetFilePath(const CString& strPath)
{
	m_strFilePath=strPath;
}
