// LyricsFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LyricsFileDlg.h"

/*****************************************************************************************/
//定义线程函数
DWORD WINAPI ThreadCopy(LPVOID pParam)//此线程复制部分数据
{
	CString* pString=(CString*)pParam;
	int size=pString->GetLength()*2+1;
	char* pStr=new char[size];
	CCodeCovert cc;
	char* c=cc.WcharToChar(pString->GetBuffer());
	strcpy(pStr,c);
	pStr[size-1]='\0';
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		HGLOBAL hMem= GlobalAlloc(GMEM_DDESHARE,size);
        char* p= (char*)GlobalLock(hMem);
		memcpy(p,pStr,size);
        GlobalUnlock(hMem);
        SetClipboardData(CF_TEXT,hMem);
        CloseClipboard();
		GlobalFree(hMem);
	}
	delete[] pStr;
	pStr=NULL;
	return 0;
}
DWORD WINAPI ThreadCopyAll(LPVOID pParam)// 复制所有数据
{
	CString* pString=(CString*)pParam;
	int size=pString->GetLength()*2+1;
	char* pStr=new char[size];
	CCodeCovert cc;
	char* c=cc.WcharToChar(pString->GetBuffer());
	strcpy(pStr,c);
	pStr[size-1]='\0';
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		HGLOBAL hMem= GlobalAlloc(GMEM_DDESHARE,size);
        char* p= (char*)GlobalLock(hMem);
		memcpy(p,pStr,size);
        GlobalUnlock(hMem);
        SetClipboardData(CF_TEXT,hMem);
        CloseClipboard();
		GlobalFree(hMem);
	}
	delete[] pStr;
	pStr=NULL;
	return 0;
}

// CLyricsFileDlg 对话框

IMPLEMENT_DYNAMIC(CLyricsFileDlg, CDialog)

CLyricsFileDlg::CLyricsFileDlg(const CString& strLyricPath,CWnd* pParent /*=NULL*/)
	: CDialog(CLyricsFileDlg::IDD, pParent)
	,CErrorLog(_T("CLyricsFileDlg"))
	, m_strRichEdit(_T(""))
	, m_strText(_T(""))
	,m_strLyricPath(strLyricPath)
{

}

CLyricsFileDlg::~CLyricsFileDlg()
{
}

void CLyricsFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_richEdit);
	DDX_Text(pDX, IDC_RICHEDIT21, m_strRichEdit);
}


BEGIN_MESSAGE_MAP(CLyricsFileDlg, CDialog)
	ON_COMMAND(ID_MENU_COPY, &CLyricsFileDlg::OnRbtCopy)
	ON_COMMAND(ID_MENU_COPYALL, &CLyricsFileDlg::OnRbtCopyAll)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLyricsFileDlg 消息处理程序
void CLyricsFileDlg::ReadLyricsFile()
{
	int pos=m_strLyricPath.ReverseFind('\\');
	int len=m_strLyricPath.GetLength();
	m_strText=m_strLyricPath.Right(len-pos-1);
	m_strRichEdit=_T("");
	CReadFile rf(m_strLyricPath);
	CODETYPE ct=rf.GetFileCodeType();
	std::string strLine;
	CCodeCovert cc;
	while(rf.ReadLine(strLine))
	{
		if(CODETYPE::utf8==ct)
			strLine=cc.Utf8ToAscii(strLine);
		CString strTemp(strLine.c_str());
		m_strRichEdit+=strTemp+char(10);
	}
	UpdateData(FALSE);
}
BOOL CLyricsFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ReadLyricsFile();
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask|=CFM_BOLD;
	cf.dwEffects|=CFE_BOLD;//设置粗体，取消用cf.dwEffects&=~CFE_BOLD;
	cf.dwMask|=CFM_COLOR;
	cf.crTextColor =  RGB(155,0,200);//设置颜色
	cf.dwMask|=CFM_SIZE;
	cf.yHeight =300;//设置高度
	cf.dwMask|=CFM_FACE;
	wcscpy_s(cf.szFaceName,_T("宋体"));
	m_richEdit.SetDefaultCharFormat(cf);
	m_richEdit.SetSel(0,0);
	SetWindowText(m_strText);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CLyricsFileDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_RBUTTONDOWN)
	{
		long start,end;
		m_richEdit.GetSel(start,end);
		CPoint point;
		::GetCursorPos(&point);
		CMenu menu,*pMenu=NULL;
		menu.LoadMenu(IDR_MENU1);
		pMenu=menu.GetSubMenu(0);
		if(start==end)
			pMenu->ModifyMenu(0,MF_BYPOSITION|MF_GRAYED,ID_MENU_COPY,_T("复制"));
		pMenu->TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this);
		pMenu->DestroyMenu();
		pMenu=NULL;
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLyricsFileDlg::OnRbtCopy()
{
	// TODO: 在此添加命令处理程序代码
	CString s=m_richEdit.GetSelText();
	HANDLE hThread=CreateThread(NULL,0,ThreadCopy,(LPVOID)&s,0,NULL);
	CloseHandle(hThread);
	Sleep(300);
}
void CLyricsFileDlg::OnRbtCopyAll()
{
	// TODO: 在此添加命令处理程序代码
	CString s=m_strRichEdit;
	HANDLE hThread=CreateThread(NULL,0,ThreadCopyAll,(LPVOID)&s,0,NULL);
	CloseHandle(hThread);
	Sleep(300);
}

void CLyricsFileDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(IsWindow(m_hWnd)&&IsWindow(m_richEdit.GetSafeHwnd()))
	{
		CRect rc;
		GetClientRect(&rc);
		m_richEdit.MoveWindow(&rc);
	}
	// TODO: 在此处添加消息处理程序代码
}

