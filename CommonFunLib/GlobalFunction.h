# pragma once
#include "..\CommonDefine\StandarC++.h"
#include <assert.h>

//释放DC句柄
void ReleaseDCHandle(HDC*);

//释放位图句柄
void ReleaseGDIHandle(HANDLE*);

//获取系统时间的字符串 ****-**-** **_**_**格式
std::wstring FormatSystemTimeW();

//获取应用程序路径
std::wstring GetAppPathW();
std::string  GetAppPathA();

//指定颜色填充DC矩形局域
bool FillDCRect(IN HDC hDC,IN LPRECT lpRect,IN COLORREF color);

//获取位图的指定区域位图句柄
HBITMAP GetBitmapRect(IN HBITMAP hBitmap,IN LPRECT lpRect);

//获取DC指定大小的背景位图句柄
HBITMAP GetDCBackBitmap(IN HDC hDC,IN int nWidth,IN int nHeight);

//保存位图句柄到指定路径
bool SaveBitmapA(IN HBITMAP hBitmap,IN const char* pSavePath);
bool SaveBitmapW(IN HBITMAP hBitmap,IN const wchar_t* pSavePath);

//复制位图句柄
HBITMAP CopyBitmap(IN HBITMAP hBitmap);












#ifdef UNICODE 
	#define GetAppPath GetAppPathW
	#define SaveBitmap SaveBitmapW
#else
	#define GetAppPath GetAppPathA
	#define SaveBitmap SaveBitmapA
#endif
