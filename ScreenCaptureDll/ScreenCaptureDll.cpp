// ScreenCaptureDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ScreenCaptureDll.h"
#include "CapDlg.h"
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

// CScreenCaptureDllApp

BEGIN_MESSAGE_MAP(CScreenCaptureDllApp, CWinApp)
END_MESSAGE_MAP()


// CScreenCaptureDllApp 构造

CScreenCaptureDllApp::CScreenCaptureDllApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CScreenCaptureDllApp 对象

CScreenCaptureDllApp theApp;
//CCapDlg* pDlg=NULL;

// CScreenCaptureDllApp 初始化

BOOL CScreenCaptureDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////
//定义模块接口
//初始化截屏窗口
void InitCaptureDlg()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	HINSTANCE hOldInst=AfxGetResourceHandle();
//	AfxSetResourceHandle(theApp.m_hInstance);
	CCapDlg dlg;
	dlg.DoModal();
//	AfxSetResourceHandle(hOldInst);
}
//捕获屏幕位图句柄,获取后一定要记得释放
HBITMAP GetScreenBitmapHandle()
{
	return CCapDlg::GetScreenBitmap();
}
HBITMAP GetRectBitmapHandle(CRect& rc)
{
	return CCapDlg::GetScreenBitmap(rc);
}
