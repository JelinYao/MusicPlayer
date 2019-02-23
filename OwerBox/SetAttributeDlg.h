#pragma once
#include "Resource.h"
#include "afxwin.h"

// CSetAttributeDlg 对话框

class CSetAttributeDlg : public CDialog,CErrorLog
{
	DECLARE_DYNAMIC(CSetAttributeDlg)

public:
	CSetAttributeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetAttributeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SETATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	void ReadConfigData(void);
	void SaveConfigData(void);
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnPaint();
	afx_msg void OnClose();
	//添加开机启动
	void SetBootStart(void);
	//删除开机启动
	void RemoveBootStart();
private:
	// 设置播放速度
	CSliderButton m_btnSpeed;
	// 设置比特率
	CSliderButton m_btnRate;
	// 设置音调
	CSliderButton m_btnPitch;
	CButton m_checkButton;
	BOOL m_bRunBoot;
	CString m_strConfigPath;
	// 左声道控制
	CSliderButton m_leftVoice;
	// 右声道控制
	CSliderButton m_rightVoice;
public:
	void CreateConfigFile(void);
	virtual void OnCancel();
protected:
	virtual void PostNcDestroy();
};
extern CSetAttributeDlg*	g_pSetAttributeDlg;