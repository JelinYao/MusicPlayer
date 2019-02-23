#pragma once

#include "resource.h"
// CTransDlg 对话框

class CInfoDlg : public CDialog,CMemBase
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInfoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
private:
	//保存鼠标当前位置的RGB颜色值
	int m_rValue;
	int m_gValue;
	int m_bValue;
	//保存当前截屏区域的大小
	int m_nWidth;
	int m_nHeight;
	CRect m_rcClient;
	HDC m_hBkMemDC;
	//保存位图句柄
	HBITMAP m_hBkMemBitmap;
	//画笔
	HPEN m_hRedPen;
	HPEN m_hBlackPen;
	//字体
	HFONT m_hFont;
	//画刷
	HBRUSH m_hBrush;
private:
	void DrawText(HDC hDC);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	void DrawBitmap(HDC hDC);
	// 画出边界线
	void DrawLine(HDC hDC);
public:
	///////////////////////////////////////////////////定义接口部分
	// 设置RGB颜色值接口
	void SetRGBValue(const int r, const int g,const int b);
	// 设置截取区域大小接口
	void SetRectValue(const int nWidth, const int nHeight);
	// 设置当前鼠标所在区域位图句柄
	void SetBitmap(HBITMAP hBmp);
};
