// MyMusicPlayer.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CMyMusicPlayerApp:
// 有关此类的实现，请参阅 MyMusicPlayer.cpp
//

class CMusicPlayerApp : public CWinApp
{
public:
	CMusicPlayerApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
private:
	// 互斥量，每次只一允许运行一个实例
	HANDLE hMutex;
	Gdiplus::GdiplusStartupInput m_GdiplusInput;
	ULONG_PTR m_GdiplusPtr;
public:
	virtual int ExitInstance();
};

extern CMusicPlayerApp theApp;