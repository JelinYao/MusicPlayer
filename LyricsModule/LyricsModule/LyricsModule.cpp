// LyricsModule.cpp : 定义 DLL 的初始化例程。
//
/******************************************/
/*修改时间：2013年2月21日18:24:30         */
/*姚佳宁								  */
/******************************************/
#include "stdafx.h"
#include "LyricsModule.h"
#include <iostream>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CLyricsModuleApp




BEGIN_MESSAGE_MAP(CLyricsModuleApp, CWinApp)
END_MESSAGE_MAP()


// CLyricsModuleApp 构造

CLyricsModuleApp::CLyricsModuleApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CLyricsModuleApp 对象

CLyricsModuleApp theApp;


// CLyricsModuleApp 初始化

BOOL CLyricsModuleApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

int CLyricsModuleApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	return CWinApp::ExitInstance();
}


/**********************************************************/
//***************定义全局变量
//歌词面板对话框指针
CLyricsDlg* g_pLyricsDlg=NULL;
//消息循环线程句柄
HANDLE      g_hMsgLoopThread=NULL;
//线程运行开关
BOOL		g_bSwitch=TRUE;
//异常日志记录对象
CErrorLog   g_logError=CErrorLog(LYRICS_MOUDLE_GLOBALFUN);


//线程函数进行对话框消息循环
DWORD WINAPI DialogMessageLoopThread(LPVOID lpParam)
{
	DWORD dwRet=0;
	try
	{
		if(NULL!=g_pLyricsDlg)
		{
			HWND hLyricsDlg=g_pLyricsDlg->GetSafeHwnd();
			if(NULL==hLyricsDlg)
				throw _T("获取对话框句柄失败！");
			//进入消息循环
			BOOL bRet=0;
			MSG msg;
			while(g_bSwitch&&(bRet=::GetMessage(&msg,hLyricsDlg,0,0)))
			{
				if(-1==bRet)
					throw ;
				if(hLyricsDlg&&(!IsDialogMessage(hLyricsDlg,&msg)))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
			g_pLyricsDlg->DestroyWindow();
			delete g_pLyricsDlg;
			g_pLyricsDlg=NULL;
			hLyricsDlg=NULL;
			g_hMsgLoopThread=NULL;
		}
	}
	catch(TCHAR* pError)
	{
		_tprintf(_T("DialogMessageLoopThread异常信息:%s\n"),pError);
		EXCEPTION_INFO ei=FillExceptionInfo(pError,__LINE__,__FILE__,::GetLastError(),_T("DialogMessageLoopThread"));
		g_logError.WriteErrorLog(&ei);
		dwRet=1;
	}
	return dwRet;
}
inline BOOL EnsureDlg()
{
	return g_pLyricsDlg!=NULL&&(::IsWindow(g_pLyricsDlg->m_hWnd));
}
//////////////////////////////////////////////////////////////////////
//模块接口
BOOL InitLyricsDlg()//初始化歌词面板
{
	try
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		HINSTANCE hOldInst=AfxGetResourceHandle();
		AfxSetResourceHandle(theApp.m_hInstance);
		g_pLyricsDlg=new CLyricsDlg;
		if(NULL==g_pLyricsDlg)
			throw _T("申请内存失败！");
		g_pLyricsDlg->Create(IDD_DIALOG_LYRICS);
		g_pLyricsDlg->ShowWindow(SW_SHOW);
		g_pLyricsDlg->UpdateWindow();
// 		if(NULL==g_hMsgLoopThread)
// 		{
// 			//创建歌词面板消息循环线程
// 			g_hMsgLoopThread=::CreateThread(NULL,0,DialogMessageLoopThread,\
// 				NULL,0,NULL);
// 			if(NULL==g_hMsgLoopThread)//创建线程失败
// 				throw _T("创建消息循环线程失败！");
// 			::CloseHandle(g_hMsgLoopThread);
// 			g_bSwitch=TRUE;
// 		}
		AfxSetResourceHandle(hOldInst);
	}
	catch(TCHAR* pError)
	{
		_tprintf(_T("InitLyricsDlg异常信息:%s\n"),pError);
		EXCEPTION_INFO ei=FillExceptionInfo(pError,__LINE__,__FILE__,::GetLastError(),_T("InitLyricsDlg"));
		g_bSwitch=FALSE;
		g_logError.WriteErrorLog(&ei);
	}
	return g_pLyricsDlg->IsLock();
}
void ShowLyricsDlg(BOOL bShow)//显示\隐藏歌词面板
{
	try
	{
		if(!EnsureDlg())
			throw _T("歌词面板对话框指针为空!");
		if(bShow)
			g_pLyricsDlg->ShowWindow(SW_SHOW);
		else
			g_pLyricsDlg->ShowWindow(SW_HIDE);
	}
	catch(TCHAR* pError)
	{
		_tprintf(_T("InitLyricsDlg异常信息:%s\n"),pError);
		EXCEPTION_INFO ei=FillExceptionInfo(pError,__LINE__,__FILE__,::GetLastError(),_T("ShowLyricsDlg"));
		g_pLyricsDlg->WriteErrorLog(&ei);
	}
}
void SendLyricsMsg(std::wstring& wstrPath)//发送歌词文件信息到模块内
{
	try
	{
		if(!EnsureDlg())
			throw _T("歌词面板对话框指针为空!");
		g_pLyricsDlg->StrCurLyricsPath(CString(wstrPath.c_str()));
		g_pLyricsDlg->ReadLyricsFile();
	}
	catch(TCHAR* pError)
	{
		_tprintf(_T("SendLyricsMsg异常信息:%s\n"),pError);
		EXCEPTION_INFO ei=FillExceptionInfo(pError,__LINE__,__FILE__,::GetLastError(),_T("SendLyricsMsg"));
		g_logError.WriteErrorLog(&ei);
	}
}
void SetLyricsTime(int nCurTime)//快进了或者退后了之后进行相应处理
{
	if(EnsureDlg())
		g_pLyricsDlg->ChangeCurTime(nCurTime);
}
void SetLyricsColor()//改变歌词颜色
{
	if(EnsureDlg())
		g_pLyricsDlg->OnRBSetColor();
}
void SetLyricsFont()//改变歌词字体
{
	if(EnsureDlg())
		g_pLyricsDlg->OnRBSetFont();
}
void NoLyricsFile()//没有找到歌词文件
{
	if(EnsureDlg())
		g_pLyricsDlg->FileNotExist();
}
void LockLyricsDlg(BOOL lock)//锁定、解锁歌词对话框
{
	if(EnsureDlg())
		g_pLyricsDlg->LockLyrics(lock);
}
void ReleaseLyricsDlg()//释放对话框资源
{
	if(EnsureDlg())
	{
		//线程退出前销毁窗口
		g_pLyricsDlg->SaveLyricsDialogData();
		g_pLyricsDlg->OnCancel();
	}
}