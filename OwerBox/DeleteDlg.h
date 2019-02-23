#pragma once
#include "Resource.h"
#include "afxwin.h"

// CDeleteDlg 对话框

class CDeleteDlg : public CDialog
{
	DECLARE_DYNAMIC(CDeleteDlg)

public:
	CDeleteDlg(const CString& strPath,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDeleteDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteOk();
	afx_msg void OnDeleteCancel();
	HICON hIcon;
	CStatic m_icon;
	CButton m_checkBox;
	//保存可能会被删除的文件的路径
	CString m_strFilePath;
	// 保存路径
	void SetFilePath(const CString& strPath);
};
