#pragma once
//双缓冲绘图基类，主要对GDI对象的初始化以及析构
/*class __declspec(dllexport)*/
EXPORT_CLASS CMemBase
{
public:
	CMemBase(void);
	virtual ~CMemBase(void);
protected:
	HDC m_hMemDC;
	HBITMAP m_hMemBitmap;
};
