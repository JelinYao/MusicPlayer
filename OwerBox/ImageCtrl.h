#pragma once
typedef struct _INDEX
{
	int nGroupNumber;//组数
	int nIndex;//索引值
}INDEX;//所有索引均从1开始计算



// CImageCtrl

class CImageCtrl : public CWnd,public CErrorLog,public CMemBase
{
	DECLARE_DYNAMIC(CImageCtrl)

public:
	CImageCtrl();
	virtual ~CImageCtrl();
	/***********************************************/
	//定义外部接口                                 */
	/***********************************************/
	void Init( CString& strImagePath,const CString& strTipText);
	//选中
	void SetCheck() { m_bCheck=TRUE; Invalidate(); }
	//取消选中
	void UnCheck() { m_bCheck=FALSE; Invalidate(); }
	//位置信息的设置雨获取
	void SetIndex(const int nGroupNumber,const int nIndex){m_owerIndex.nGroupNumber=nGroupNumber;m_owerIndex.nIndex=nIndex;}
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
private:
	//双缓冲绘图资源
	Gdiplus::Graphics* m_pGraphics;
	Gdiplus::Bitmap* m_pBackBitmap;
	//客户区大小
	CRect m_rcClient;
	//保存当前图片的完整路径
	CString m_strImagePath;
	BOOL m_bMouseOver;
	//高亮显示的画刷
	HBRUSH m_hOverBrush;
	HBRUSH m_hCheckBrush;
	//是否处于选中状态
	BOOL m_bCheck;
	//保存自己的位置信息
	INDEX m_owerIndex;
	//保存提示控件窗口句柄
	HWND m_hTipWnd;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


