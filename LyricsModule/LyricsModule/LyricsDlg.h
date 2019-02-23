#pragma once


typedef struct _LYRIC_DATA
{
	int nStartTime;
	std::string strLyricText;
}LYRIC_DATA,*PLYRIC_DATA;
// CLyricsDlg 对话框

class CLyricsDlg : public CDialog,   public CErrorLog,public CMemBase
{
	DECLARE_DYNAMIC(CLyricsDlg)

public:
	CLyricsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLyricsDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_LYRICS };
	enum {left,right,top,bottom};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetColor();
	afx_msg void OnSetFont();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRBSetColor();
	afx_msg void OnRBSetFont();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetOverColor();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CRect m_rcClient;
	HFONT m_hFont;
	CString m_strShowText;
	std::vector<LYRIC_DATA*> m_vecLyricDatas;
	//累计时间
	int m_nTotalTime;
	//当前播放的歌词索引
	int m_nIndex;
	//歌词长度大于窗口长度的时候滚动标识
	BOOL m_bRoll;
	//歌词滚动是否完成
	BOOL m_bFinishRoll;
	//保存当前歌词在DC上的实际长度
	int m_nTextExtent;
	RECT m_rcDraw;
	RECT m_rcOver;
	/***********************************************************/
	// 判断是否可以改变窗口大小
	BOOL m_bResizing;
	// 设置字体颜色
	COLORREF m_cUnderColor;
	// 判断鼠标是否在窗口上
	BOOL m_bMouseOver;
	int m_iDirect;
	// 判断歌词文件是否存在
	BOOL m_bFileExist;
	//保存当前播放歌曲的歌词文件路径
	CString m_strCurLyricsPath;
	// 文字覆盖色
	COLORREF m_cOverColor;
	// 记录当前字体的高度
	int m_nFontHeight;
	//窗口大小是否改变了
	BOOL m_bSizeChange;
	//是否锁定窗口
	BOOL m_bLock;
	float m_fSpeed;
	CRITICAL_SECTION	cs;
protected:
	// 分解出字符串中的时间和歌词语句
	void AnaliseString(std::string& str);
	//释放容器内的指针
	void ReleaseLyricDatas();
	//绘制歌词文字
	void DrawLyricText(HDC hDC);
public:
	/******************************外部接口****************************/
	// 改变当前显示的歌词的时间
	void ChangeCurTime(const int nTime);
	void FileNotExist();
	void SortLyricDatas();//给打乱的时间排序
	void LockLyrics(BOOL ret);
	void SInvalide();
	void RInvalide();
	CString StrCurLyricsPath() const { return m_strCurLyricsPath; }
	void StrCurLyricsPath(const CString& strPath) { m_strCurLyricsPath = strPath; }
	// 读取文件中存放的歌词面板信息
	BOOL ReadLyricsDialogData(LPRECT pRect);
	// 读取歌词文件
	BOOL ReadLyricsFile();
	// 保存歌词面板信息到XML文件
	BOOL SaveLyricsDialogData(void);
	//获取窗口是否锁定的信息
	BOOL IsLock()const { return m_bLock; }
	virtual void OnCancel();
protected:
	virtual void PostNcDestroy();
};
