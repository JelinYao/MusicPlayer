#pragma once
#include "ColorRect.h"
#include "TransDlg.h"
/*#include "CapToolBar.h"*/
#include "ToolDlg.h"
#include <ATLIMAGE.H>
// CCapDlg 对话框
#define WM_EXITCAP WM_USER+101
//工具栏ID
#define IDC_TOOLBAR 1000
class CCapDlg : public CDialog
{
	DECLARE_DYNAMIC(CCapDlg)

public:
	CCapDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCapDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };
	//截屏状态
	typedef enum
	{
		undo,
		doing,
		finish
	}STATE;
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//点击保存按钮
	afx_msg void OnSave();
	afx_msg void OnExit();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CColorRect m_rcTrack;
	//截屏区域的两个坐标点
	CPoint m_ptStart;
	//向父窗口发送退出消息
	void SendExitMsg();
	void DrawBrightBack(CDC* pDC);
	void DrawRectBack(CDC* pDC);
private://私有成员变量
	//亮度处理后的句柄
	HBITMAP m_hBrightBmp;
	//内存兼容DC
	HDC m_hMemDC1;
	HDC m_hMemDC2;
	STATE m_curState;
	//截屏时鼠标指针
	HCURSOR m_hCursor;
	//显示鼠标位置信息的对话框
	CInfoDlg* m_pInfoDlg;
	//工具栏
	//CCapToolBar* m_pToolBar;
	CToolDlg* m_pToolDlg;
public://静态成员,接口部分
	// 保存整个屏幕的句柄
	static HBITMAP m_hScreenBmp;
	//显示器大小
	static int m_nxScreen;
	static int m_nyScreen;
	// 截取区域图片保存到句柄中
	static HBITMAP GetScreenBitmap(LPRECT lpRect=NULL);
	//保存图片
	static void SaveBitmap(HBITMAP);
	// 改变图像的亮度
	static void ChangeBitmapBright(HBITMAP hBmp);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// 移动工具栏
	void MoveToolDlg();
	//移动信息框
	void MoveInfoDlg();
};
