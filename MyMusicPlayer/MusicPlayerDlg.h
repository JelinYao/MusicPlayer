// MyMusicPlayerDlg.h : 头文件
//

#pragma once
#include <vector>
#include "Resource.h"
#include "libzplay.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "NoLyricsDlg.h"
#include "direct.h"
#include "PlayListBox.h"
#include "atltypes.h"
#include "VoiceSliderButton.h"
#include "ProgressSliderButton.h"
#include "DrawStatic.h"

#pragma comment(lib,"libzplay.lib")
#include <wtypes.h>
using namespace libZPlay;
/*********************************************************************************/
// 各模块的接口声明

//歌词属性窗口
void ShowAttributeDlg();
void ReleaseAttributeDlg();
////////////////////////////////////////////
//歌词面板窗口
BOOL InitLyricsDlg();
void ShowLyricsDlg(BOOL bShow);
void ReleaseLyricsDlg();
void SendLyricsMsg(std::wstring& ws);
void SetLyricsTime(int nCurTime);
void StopInvalide();
void ReInvalide();
void SetLyricsColor();
void SetLyricsFont();
void NoLyricsFile();
void LockLyricsDlg(BOOL lock);
////////////////////////////////////////////
//删除歌曲窗口
void ShowDeleteDlg(std::wstring& ws);
void ReleaseDeleteDlg();
void ShowInfoDlg(std::wstring& ws);
void ReleaseInfoDlg();
////////////////////////////////////////////
//换肤窗口
void InitChangeClothDlg(const BYTE bValue,const CString& strBackImagePath);
void ReleaseChangeClothDlg();
BOOL ShowChangeClothDlg();
BOOL HideChangeClothDlg();
/*********************************************************************************/
//using namespace std;

//跳动音符结构
typedef struct _RECT_DATA
{
	CRect rcBig;
	CRect rcSmall;
}RECT_DATA,*PRECT_DATA;
//枚举播放模式
typedef enum{
	single_play,order_play,rand_play
}PLAYMODE;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
#include "StarWarsCtrl.h"
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
private:
	CStarWarsCtrl m_transparent;
public:
	//afx_msg void OnStnClickedAbout();
private:
	// 当前窗口是否已经最大化
	BOOL m_bMax;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	CRect client;
public:
	afx_msg void OnStnDblclickAbout();
};
// CMyMusicPlayerDlg 对话框

class CMusicPlayerDlg : public CDialog,	public CErrorLog,CMemBase
{
// 构造
public:
	CMusicPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYMUSICPLAYER_DIALOG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	/*****************************************************************************/
	/*                               播放控制功能函数                            */
	/*****************************************************************************/
	//查看歌词文件
	void ShowLyricFile();
	/*****************************************************************************/
	/*                              初始化子控件及其位置                         */
	/*****************************************************************************/
	void InitButtons(void);
	// 要在MINI窗口上写字时隐藏按钮
	void HideButtons(void);
	// MINI窗口正常化后显示按钮
	void ShowButtons(void);
	//注册系统热键
	void RegisterHotKeys(void);
	//初始化文件列表
	void InitFile();
	/*****************************************************************************/
	/*                                播放列表的维护                             */
	/*****************************************************************************/
	// 添加文件夹内所有可播放文件到播放列表
	void AddFolderFiles(const CString& strDir);
	// 添加文件名到播放列表并排序
	void AddPlayListToPlayer(const int nStartPos=0);
	// 删除播放列表中当前选中的节点
	void DeleteSelectItem(void);
	//显示默认播放列表
	void ShowDefaultPlayList(void);
	//显示我的收藏列表
	void ShowEnjoyPlayList(void);
private:
	/*****************************************************************************/
	/*                              绘制背景以及文字函数                         */
	/*****************************************************************************/
	void DrawMyText(HDC hDC);
	void DrawMyTextA(HDC hDC);
	// 绘制动态音符线程，没有图片时开启线程，有图片关闭线程
	void DrawPictrue(BOOL run);
	// MINI窗口上绘制文字 
/*	void DrawTextOnMiniWindow(HDC hDC);*/
	void ReBeginDrawText(void);
	//程序关闭后释放资源
	void Release();
	//显示主菜单
	void ShowMainMenu();
	/*****************************************************************************/
	/*                            播放状态监测线程控制函数                       */
	/*****************************************************************************/	
	//开启播放状态监测线程
	inline void StartPlayModeThread();
	//结束播放状态监测线程
	inline void StopPlayModeThread();
private:
	/*****************************************************************************/
	/*                                  控件变量                                 */
	/*****************************************************************************/
	CSkinButton m_btnExit;
	CSkinButton m_btnHide;
	CSkinButton m_btnMini;
	CSkinButton m_btnPrev;
	CSkinButton m_btnPlay;
	CSkinButton m_btnNext;
	// 更换皮肤按钮
	CSkinButton m_btnCloth;
	//默认播放列表按钮
	CSkinButton m_btnDefaultList;
	//我的收藏按钮
	//CSkinButton m_btnEnjoyList;
	// 进度条
	CProgressSliderButton m_btnProcess;
	CSkinButton m_btnAdd;
	// 我的收藏列表,未完成
	//CListBox m_enjoyPlaylist;
	// 静音设置按钮
	CSkinButton m_btnVoiceQuiet;
	// 调节音量大小
	CVoiceSliderButton m_btnVoiceSetting;
	// 控制播放模式
	CSkinButton m_btnPlayMode;
	// 发送反馈信息按钮
	CLinkButton m_btnFeedBack;
	//显示播放时间进度的控件
//	CDrawStatic m_ctrlTime;
	/*****************************************************************************/
	/*                                 私有成员变量                              */
	/*****************************************************************************/
	//是否处于MINI播放状态
	BOOL m_bMiniStyle;
	// 正在播放的文件完整路径名
	//CString m_strPlayFilePath;
	std::vector<CString> m_playList;
	// 当前曲目总时间
	UINT m_uMinuteCount;
	UINT m_uSecondCount;
	UINT m_uCurrentMinute;
	UINT m_uCurrentSecond;
	UINT m_uPlayVolume;//左声道
	// 是否静音
	BOOL m_bQuietVoice;
	// 判断延时是否结束
	BOOL m_bTimeOver;
	// 是否能够在MINI窗口上绘制文字
//	BOOL m_bDrawMiniText;
	// 歌曲信息对话框指针
	BOOL m_bMouseOverVoice;//MouseOnProgress;
//	BOOL m_bPlayListFlag;
	// 保存图片的路径
	CString m_strPictruePath;
	int m_rollFlag;
	CString m_strText;
	CString m_strTitle;
	// 判断当前文字是否到达坐标x=38处
	BOOL m_IsGet;
	// 绘制CString指定部分
	int m_index;
	// 指示歌词面板是否显示
	BOOL m_bShowLyrics;
	// 判断歌词文件是否存在
	BOOL m_bLyricExist;
	// 读取歌词语句的索引
	int m_iIndex;
	// 判断歌词面板是否锁定
	BOOL m_bLockLyrics;
	//CHotKeyCtrl m_hotKey;
	NOTIFYICONDATA nd;
	// 保存位图名称
	CString m_strBmpName;
	TID3InfoExW id3_infoEx;
	//背景内存兼容DC句柄
// 	HDC m_hBkMemDC;
// 	//背景位图句柄
// 	HBITMAP m_hBkMemBitmap;
	//字体句柄
	HFONT m_hFont;
	//GDI+绘图指针
	Gdiplus::Graphics* m_pGraphic;
	//背景图片指针
	Gdiplus::Bitmap* m_pBackBitmap;
	//歌手图片指针
	Gdiplus::Image* m_pAuthorImage;
	//保存当前窗口的大小
	//CRect m_rcClient;
	//保存窗口的宽度
	int m_nWidth;
	//保存窗口的高度
	int m_nHeight;
	//保存模块路径
	CString m_strModulePath;
	//是否正在截屏的标识
	BOOL m_bCaptureFlag;
	//显示播放歌曲的时间进度
	CString m_strProgressTime;
	//保存临时背景图片的路径
	CString m_strTempBackImage;
	//保存用户选择的背景图片的路径
	CString m_strSelectBackImage;
	// 指示是否显示换肤对话框
	BOOL m_bShowChangeClothDlg;
	//ALPHA透明度
	BYTE m_bAlphaValue;
public:
		/*****************************************************************************/
		/*                                 消息响应函数                              */
		/*****************************************************************************/
		afx_msg void OnIconExit();
		afx_msg void OnBnClickedVoice();
		afx_msg void OnBnClickedPrev();
		afx_msg void OnBnClickedPlay();
		afx_msg void OnBnClickedNext();
		afx_msg void OnBnClickedPlaymode();
		afx_msg void OnChangeCloth();
		afx_msg void OnModeOnly();
		afx_msg void OnModeOrder();
		afx_msg void OnModeRand();
		afx_msg void OnAddDeleteAll();
		afx_msg void On32785Play();
		afx_msg void On32783Prev();
		afx_msg void On32784Next();
		afx_msg void OnIconSetAttributes();
		afx_msg void On32789();
		afx_msg void On32788();
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnShowLyricsDlg();
		afx_msg void OnSetLyricsColor();
		afx_msg void OnSetLyricsFont();
		afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
		afx_msg void OnLyricsLock();
		afx_msg void OnSendEmail();
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
/*		afx_msg void OnBnClickedMyLike();*/
		afx_msg void OnBnClickedExit();
		afx_msg void OnBnClickedHide();
		afx_msg void OnBnClickedMini();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnAddFile();
		afx_msg void OnAddFolder();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnBnClickedAdd();
		afx_msg void OnIconAbout();
//		afx_msg void OnBnClickedList1();
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		//拖曳文件处理
		void GetDropFiles(HDROP hDrop);
		/*****************************************************************************/
		/*                              系统托盘图标处理函数                         */
		/*****************************************************************************/
		//创建托盘图标
		void CreateNotifyIcon(CString strTip=_T("Love Music,Love Life"));
		//删除托盘图标
		void DeleteNotifyIcon(void);
		//托盘图标消息响应
		LRESULT OnNotifyIcon(WPARAM wParam,LPARAM lParam);
		/*****************************************************************************/
		/*                                静态功能函数                               */
		/*****************************************************************************/
		// 将图片保存
		static void SaveBitmap(HBITMAP hBmp,CString& strFullName);
		// 判断是否是支持的文件类型
		inline static BOOL IsSupportFile(const CString& strFileName);
		inline static CString GetShortName(const CString& strFullName);
		/*****************************************************************************/
		/*                              外部接口函数                                 */
		/*****************************************************************************/
		//获取音量设置控件的指针
		CSliderButton* GetVoiceSettingCtrl() { return &m_btnVoiceSetting; }
		// 判断当前鼠标是否在区域内
		BOOL IsCursorInRect(CRect& rc);
		// 调用截屏模块的DLL，开始截屏
		void OnScreenCapture(void);
		// 读取文件信息，设置播放属性
		void CreateListFile(const CString& strPath);
		void CreateConfigFile(const CString& strPath);
		void ReadConfig();
		// 将播放信息属性写入文件
		void WriteListFile();
		// 开始播放文件
		void StartPlay(void);
		//歌曲图片目录属性
		CString GetPictruePath() const { return m_strPictruePath; }
		void SetPictruePath(const CString& val) { m_strPictruePath = val; }
		void ChangePlayerVoice(const int nValue)
		{
			VERIFY(0!=nValue);//传参数0无意义
			if((nValue>0&&!m_btnVoiceSetting.MaxTest())||(nValue<0&&!m_btnVoiceSetting.MinTest()))//音量增大
			{
				int nPos=m_btnVoiceSetting.GetCurPos();
				nPos+=nValue;
				m_btnVoiceSetting.SetCurPos(nPos);
				player->SetPlayerVolume(nPos,nPos);
			}
		}
		BOOL LyricFileExist()const {return m_bLyricExist;}
		//更换背景图片
		void ChangeBack();
		static ZPlay* player;
		// 当前播放曲目的索引
		static int m_nowPlayIndex;
		// 是否正在播放
		static BOOL m_bPlaying;
		CPlayerList m_defPlaylist;
};
/////////////////////////////////////////////////////////////////////////////////////////////
//★★★★★★★★★★★★★★★★★★★处女作后记★★★★★★★★★★★★★★★★★★★★//
/*******************************************************************************************/
/*今天运气特别不好，由于机器故障，花了大约一个月写的一千多行代码，
/*全部变成了乱码，我真的欲哭无泪啊……
/*时间：2011-11-9
/*******************************************************************************************/
/*今天上班不忙，把以前写的乱七八糟的代码改了下，包括命名、注释、接口……
/*发现程序很多地方可以优化，有时间的话我会继续改的
/*时间：2012-11-16 11:18:55
/*******************************************************************************************/