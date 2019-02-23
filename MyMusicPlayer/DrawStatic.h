#pragma once


// CDrawStatic

class CDrawStatic : public CStatic
{
	DECLARE_DYNAMIC(CDrawStatic)

public:
	CDrawStatic();
	virtual ~CDrawStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
private:
	RECT m_rcClient;//客户区大小
	COLORREF m_colorText;//文字颜色
	CString m_strText;//显示的文字
	HFONT m_hFont;//绘制文字的字体
	HDC m_hMemDC;//内存兼容DC
	HBITMAP m_hMemBitmap;//内存兼容位图句柄
	HDC m_hBackDC;
	HBITMAP m_hBackBitmap;
public:
	//改变文字的颜色
	void ChangeTextColor(COLORREF color) { m_colorText=color; }
	CString GetText() const { return m_strText; }
	void SetText(const CString& val) { m_strText = val; Invalidate();}
protected:
	virtual void PreSubclassWindow();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


