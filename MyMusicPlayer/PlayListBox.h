#pragma once

// CPlayListBox


class CPlayerList : public CListBox ,public CErrorLog,public CChangeBackBase,CMemBase
{
	DECLARE_DYNAMIC(CPlayerList)

public:
	CPlayerList();
	virtual ~CPlayerList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListboxAttribute();
	afx_msg void OnListboxDeleteItem();
	//重载自绘控件
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	//重载AddString函数
	int AddString(LPCTSTR lpszItem);
	//换肤
	void ChangeBack() { m_bChangeBack=TRUE;Invalidate(); }
private:
	// 获取父窗口指针
	CWnd* m_pParent;
	//客户度矩形
	CRect m_rcClient;
	//每个节点的高度
	int m_nHeight;
	//水平滚动条出现的临界值
	int m_nMaxWidth;
	SCROLLINFO m_hScrollInfo;
	//字体句柄
	HFONT m_hFont;
	HPEN m_hPen;
	//当前鼠标所在位置索引
	int m_nCursorPos;
public:
	afx_msg void OnListboxLookLyrics();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void PreSubclassWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//用间隔相间的颜色填充矩形区域
	void DrawColorItem(HDC hdc,const CRect& rect);
	//绘制间隔线条
	void DrawLines(HDC hdc);
	//获取滚动条的大小等信息,内联函数
	void GetSInfo(SCROLLINFO& sInfo,const int nFlag =SB_HORZ);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


