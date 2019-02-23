//////////////////////////////////////////////////////
//自定义消息、宏等结构

#pragma once 

/*****************************************************/
/*自定义消息                                         */
/*                                                   */
/*****************************************************/
#define MYWM_NOTIFYICON			WM_USER+101			//托盘消息

//CDeleteDlg消息
#define WM_DELETE_OK			WM_USER+106			//确定删除歌曲文件
#define WM_DELETE_CANCEL		WM_USER+107			//取消删除歌曲文件

//自定义系统快捷键
#define WM_LOCK_LYRICS			WM_USER+109			//锁定\解锁歌词面板
#define WM_PLAY_NEXT			WM_USER+110			//下一曲
#define WM_PLAY_PREV			WM_USER+111			//上一曲
#define WM_PLAY					WM_USER+112					//播放\暂停控制
#define WM_HIDE_LYRICS			WM_USER+113			//显示\隐藏歌词面板
#define WM_SCREEN_CAPTURE		WM_USER+114			//截屏
#define WM_VOICE_RAISE			WM_USER+115			//提高音量
#define WM_VOICE_REDUCE			WM_USER+116			//降低音量

//ListBox右键菜单发送到界面的消息
#define WM_LISTBOX_MESSAGE		WM_USER+117
/****************************************************/
/*为了区分不同的消息，用WPARAM来区分
/*1			//查看歌词，弹出歌词显示对话框
/*2			//查看歌曲信息
/*3			//删除节点消息 
/*4			//双击节点消息，LPARAM传入双击的节点索引
/****************************************************/

//DeleteDialog发送到界面的消息
#define WM_DELETE_DIALOG		WM_USER+118
/****************************************************/
/*区分删除成功过失败以及取消
/*wParam=1					//从播放列表中删除
/*wParam=2					//取消删除
/****************************************************/
//SliderButton类的消息
#define WM_SLIDER_SETPROGRESS	WM_USER+119			//播放器进度条控件设置进度
#define WM_SLIDER_SETVOICE		WM_USER+120			//播放器设置音量

//CImageCtrl类消息
#define WM_IMAGECTRL_MESSAGE	WM_USER+121
/***************************************************/
/*WPARAM用来区分消息类别
/*wParam=1					//鼠标位于图片控件之上
/*wParam=2					//鼠标离开了图片控件
/*wParam=3					//选择了该图片作为背景
/***************************************************/

//SetAttributeDlg类消息
#define WM_SETATTRIBUTE_MESSAGE	WM_USER+122
/***************************************************/
/*WPARAM用来区分消息类别
/*wParam=1					//调节左声道大小
/*wParam=2					//调节右声道大小
/*wParam=3					//调节播放比特率
/*wParam=4                  //调节播放音调
/*wParam=5                  //调节播放速度
/***************************************************/



/*****************************************************/
/*自定义DLL导出类的形式                              */
/*                                                   */
/*****************************************************/
#define EXPORT_CLASS class __declspec(dllexport)
#define IMPORT_CLASS class __declspec(dllexport)


/*****************************************************/
/*自定义宏替换                                       */
/*                                                   */
/*****************************************************/

//菜单文字
#define MENU_PLAY				_T("播放     Ctrl+Alt+P")
#define MENU_PAUSE				_T("暂停     Ctrl+Alt+P")
#define MENU_PREV				_T("上一曲  Ctrl+Alt+←")
#define MENU_NEXT				_T("下一曲  Ctrl+Alt+→")

#define MENU_UNLOCK_LYRIC		_T("解锁歌词  Ctrl+Alt+L")
#define MENU_LOCK_LYRIC			_T("锁定歌词  Ctrl+Alt+L")
#define MENU_HIDE_LYRIC			_T("隐藏歌词  Ctrl+Alt+H")
#define MENU_SHOW_LYRIC			_T("显示歌词  Ctrl+Alt+H")

#define MENU_SINGLE_PLAY		_T("单曲播放(&O)")
#define MENU_ORDER_PLAY			_T("顺序播放(&C)")
#define MENU_RAND_PLAY			_T("随机播放(&R)")





/*****************************************************/
/*定义几种常见的异常问题                             */
/*                                                   */
/*****************************************************/
#define CREATE_COMDC_ERROR			"创建内存兼容DC失败！"
#define CREATE_COMBITMAP_ERROR		"创建内存兼容位图失败！"
#define BITBLT_ERROR				"BitBlt贴图失败！"
#define GDIPLUS_DRAWIMAGE_ERROR		"GDI+绘图失败！"
#define GETBACK_HBITMAP_ERROR		"获取背景位图句柄失败！"
#define LOAD_RESOURCE_ERROR			"加载资源文件失败！"
#define NULL_POINT_ERROR			"指针为空！"
#define EMPTY_BUFFER_ERROR			"指针指向的缓冲区为空！"
#define OPEN_FILE_ERROR				"打开文件失败，可能是文件不存在！"
#define READ_FILE_ERROR				"读取文件失败！"
#define PARAMETER_ERROR				"传入参数有误！"
#define UNKNOW_ERROR				"未知异常！"
#define GETOBJECT_ERROR				"获取GDI对象信息失败！"