#pragma once
#include "stdafx.h"
#define ID_TOOLTIP 110
// CSliderButton

class __declspec(dllexport) CSliderButton : public CButton,  public CErrorLog,public CMemBase//,CChangeBackBase
{
	DECLARE_DYNAMIC(CSliderButton)

public:
	CSliderButton();
	virtual ~CSliderButton();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
protected:
	virtual void PreSubclassWindow();
	CToolTipCtrl m_toolTip;
	//客户区矩形大小
	CRect m_rcClient;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private://成员变量
	// 填充进度的颜色
	COLORREF m_bkColor;
	//完成进度的颜色
	COLORREF m_overColor;
	//当前进度对应在控件客户区的位置
	int m_nCurrentPos;
	//当前坐标x位置
	int m_nxPos;
	//设置的进度最大值
	int m_nMaxPos;
	//滑块位图兼容DC句柄
	HDC m_hSliderDC;
	//滑块位图
	HBITMAP m_hSliderBitmap;
	//背景画刷
	HBRUSH m_hBkBrush;
	int m_nSliderWidth;
	//定义存储消息的数据
	UINT m_uMessage;
	WPARAM	m_wParam;
public://外部接口
	// 设置/获取背景颜色
	void SetBkColor(COLORREF color){m_bkColor=color;ReleaseGDIHandle((HANDLE*)&m_hBkBrush);m_hBkBrush=CreateSolidBrush(m_bkColor);}
	COLORREF GetBkColor()const {return m_bkColor;}
	//设置/获取完成进度的颜色
	void SetFinishColor(COLORREF color){m_overColor=color;}
	COLORREF GetFinishColor()const{return m_overColor;}
	//设置/获取当前进度值
	int GetCurPos()const {return m_nCurrentPos;}
	void SetCurPos(const int nPos)
	{
		m_nCurrentPos=nPos>m_nMaxPos?m_nMaxPos:nPos;
		m_nxPos=nPos/(float)m_nMaxPos*m_rcClient.Width();
		SendOwerMessage();
		Invalidate();
	}
	BOOL MaxTest()const {return m_nCurrentPos==m_nMaxPos;}
	BOOL MinTest()const {return m_nCurrentPos==0;}
	void SetToolTipText(const CString& strText){m_toolTip.UpdateTipText(strText,this,ID_TOOLTIP);}
	//获取控件宽度
	int GetWidth()const {return m_rcClient.Width();}
	//初始化控件的相关属性
	void InitCtrl(UINT uBitmapID,const CString& strTipText,int nMaxPos=100,UINT uMessage=0,WPARAM wParam=0);
	//根据比例来设置进度
	void SetRatio(float fRatio){m_nCurrentPos=int(fRatio*m_nMaxPos);m_nxPos=int(fRatio*GetWidth());}
	float GetRatio()const {return m_nCurrentPos/(float)m_nMaxPos;}
	//向父窗口的父窗口发送消息
	inline BOOL SendOwerMessage();
};


