// InfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OwerBox.h"
#include "InfoDlg.h"


// CInfoDlg 对话框

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(const CString& strPath,CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
	, m_strFilePath(strPath)
{

}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
END_MESSAGE_MAP()


// CInfoDlg 消息处理程序

BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	libZPlay::ZPlay* zp=NULL;
	zp=libZPlay::CreateZPlay();
	libZPlay::TID3InfoW info;
	memset(&info,0,sizeof(libZPlay::TID3InfoW));
	CString file=m_strFilePath;
	if(!zp->LoadFileID3W(m_strFilePath.GetBuffer(),libZPlay::sfAutodetect, libZPlay::id3Version2,&info))
	{
		MessageBox(_T("获取文件属性失败!"),_T("错误提示:"),MB_OK|MB_ICONERROR);
		return FALSE;
	}
	m_strFilePath.ReleaseBuffer();
	CFileAttribute attr(m_strFilePath.GetBuffer());
	float fSize=attr.GetSize()/1024.0f;
	int startPos=m_strFilePath.ReverseFind(' ');
	int	index=m_strFilePath.ReverseFind('\\');
	if(startPos<index)
		startPos=index;
	int endPos=m_strFilePath.ReverseFind('.');
	m_strFilePath=m_strFilePath.Mid(startPos+1,endPos-startPos-1);
	GetDlgItem(IDC_EDIT1)->SetWindowText(m_strFilePath);
	CString strInfo;
	strInfo.Format(_T("%0.2fM"),fSize);
	GetDlgItem(IDC_EDIT2)->SetWindowText(strInfo);
	strInfo.Empty();
	if(*info.Artist)
		strInfo.Append(info.Artist);
	else
		strInfo=_T("未知音乐家");
	GetDlgItem(IDC_EDIT3)->SetWindowText(strInfo);
	strInfo.Empty();
	if(*info.Album)
		strInfo.Append(info.Album);
	else
		strInfo=_T("未知专辑");
	GetDlgItem(IDC_EDIT4)->SetWindowText(strInfo);
	strInfo.Empty();
	if(*info.Year)
		strInfo.Append(info.Year);
	else
		strInfo=_T("未知年代");
	GetDlgItem(IDC_EDIT5)->SetWindowText(strInfo);
	strInfo.Empty();
	if(*info.TrackNum)
		strInfo.Append(info.TrackNum);
	else
		strInfo=_T("未知流派");
	GetDlgItem(IDC_EDIT6)->SetWindowText(strInfo);
	strInfo.Empty();
	if(*info.Genre)
		strInfo.Append(info.Genre);
	else
		strInfo=_T("未知曲目号");
	GetDlgItem(IDC_EDIT7)->SetWindowText(strInfo);
	//int iBitra=zp->GetBitrate(0);
	zp->Release();
	zp=NULL;
	zp=libZPlay::CreateZPlay();
	zp->OpenFileW(file.GetBuffer(),libZPlay::sfAutodetect);
	libZPlay::TStreamInfoW tinfo;
	memset(&tinfo,0,sizeof(libZPlay::TStreamInfoW));
	zp->GetStreamInfoW(&tinfo);
	strInfo.Empty();
	strInfo.Format(_T("%d 比特"),tinfo.Bitrate);
	GetDlgItem(IDC_EDIT8)->SetWindowText(strInfo);
	zp->Release();
	zp=NULL;
	//GetDlgItem(IDC_EDIT1)->SetWindowText();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
