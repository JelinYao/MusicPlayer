// MyMusicPlayer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MusicPlayer.h"
#include "MusicPlayerDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyMusicPlayerApp

BEGIN_MESSAGE_MAP(CMusicPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyMusicPlayerApp 构造

CMusicPlayerApp::CMusicPlayerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMyMusicPlayerApp 对象

CMusicPlayerApp theApp;


// CMyMusicPlayerApp 初始化

BOOL CMusicPlayerApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
#ifdef DEBUG
	//显示控制台
	AllocConsole();
	_tfreopen(_T("CONOUT$"), _T("w"),stdout);
	setlocale(LC_CTYPE, "chs");
#endif
	AfxEnableControlContainer();
	//AfxMessageBox(_T("1"));
	Gdiplus::GdiplusStartup(&m_GdiplusPtr,&m_GdiplusInput,NULL);
	hMutex=CreateMutex(NULL,FALSE,_T("MyMusicPlayer"));
	//AfxMessageBox(_T("2"));
// 	if(GetLastError()==ERROR_ALREADY_EXISTS)
// 	{
// 		HWND hWnd=FindWindow(NULL,_T("MusicPlayer"));
// 		if(hWnd)
// 		{
// 			if(::IsWindowVisible(hWnd))
// 				return FALSE;
// 			 SendMessage(hWnd,MYWM_NOTIFYICON,NULL,(LPARAM)WM_LBUTTONDOWN);
// 		}
// 		return FALSE;
// 	}
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	CMusicPlayerDlg dlg;
	//AfxMessageBox(_T("3"));
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CMusicPlayerApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
#ifdef DEBUG
	::FreeConsole();//释放控制台资源
#endif
	Gdiplus::GdiplusShutdown(m_GdiplusPtr);
	return CWinApp::ExitInstance();
}
