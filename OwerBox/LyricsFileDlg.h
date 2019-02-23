#pragma once
#include "Resource.h"
#include "afxcmn.h"

// CLyricsFileDlg 对话框

class CLyricsFileDlg : public CDialog,public CErrorLog
{
	DECLARE_DYNAMIC(CLyricsFileDlg)

public:
	CLyricsFileDlg(const CString& strLyricPath,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLyricsFileDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SHOWLYRIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_richEdit;
	CString m_strRichEdit;
public:
	// 读取歌词文件内容，文件名为全路径
	void ReadLyricsFile();
	void Init(CString& strFullName);
	virtual BOOL OnInitDialog();
private:
	// 窗口标题
	CString m_strText;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRbtCopy();
	afx_msg void OnRbtCopyAll();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CString m_strLyricPath;
};
