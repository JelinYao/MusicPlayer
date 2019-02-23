#pragma once

// CNoLyricsDlg 对话框

class CNoLyricsDlg : public CDialog
{
	DECLARE_DYNAMIC(CNoLyricsDlg)

public:
	CNoLyricsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNoLyricsDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	// 超链接按钮
	CLinkButton m_linkBtn1;
public:
	afx_msg void OnLoadLyrics();
	virtual BOOL OnInitDialog();
};
