#pragma once
#include "ErrorLog.h"
typedef struct _SAVEDATA
{
	HBITMAP hBitmap;
	const char* pSavepath;
}SAVEDATA;
EXPORT_CLASS CFuncDC:public CErrorLog
{
public:
	CFuncDC(void);
	~CFuncDC(void);
private:
	static HDC m_hMemDC;
	static HBITMAP m_hMemBitmap;
	//出错信息
	static char* m_pError;
protected:
	//************************************
	// Qualifier:释放GDI对象
	//************************************
	static void Release();
	//************************************
	// Qualifier:设置出错信息
	//************************************
	static void SetError(const char* pError);
public:
	/******************************************************************/
	/*                          外部接口部分                          */
	/******************************************************************/
	//************************************
	// Qualifier: 获取目标DC的背景位图
	//************************************
	static HBITMAP GetBackBitmap(IN HDC hDC);
	static HBITMAP GetBackBitmap(IN HDC hDC,IN const int nWidth,IN const int nHeight);
	//************************************
	//保存背景位图到指定文件
	static bool SaveBackBitmap(IN HDC hDC,IN const char* pSavePath);
	static bool SaveBackBitmap(IN HDC hDC,IN const int nWidth,IN const int nHeight,IN const char* pSavePath);
	//************************************
	// Qualifier: 保存位图句柄到指定路径文件中
	//************************************
	static bool SaveHBitmap(IN HBITMAP hBitmap,IN const char* pSavePath);
	static bool SaveHBitmap(IN HBITMAP hBitmap,IN const wchar_t* pSavePath);
	//************************************
	// Qualifier:复制位图句柄
	//************************************
	static HBITMAP CopyHBitmap(IN HBITMAP hBitmap);
	//获取位图的一个区域的位图句柄
	static HBITMAP CutHBitmap(IN HBITMAP hBitmap);
	//************************************
	// Qualifier:获取出错信息
	//************************************
	static char* GetError();
};
