#pragma once
AFX_STATIC_DATA HBRUSH _afxHatchBrush = 0;
class CColorRect:public CRectTracker
{
private:
	CPen *pOldPen;
public:
	CColorRect(void);
	~CColorRect(void);
	virtual void DrawTrackerRect(LPCRECT lpRect,CWnd *pWndClipTo,CDC *pDC,CWnd *pWnd);
	virtual void Draw(CDC *pDC);
private:
	//绘制矩形线条的画笔
	CPen m_linePen;
};