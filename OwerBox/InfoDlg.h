#pragma once

#include "libzplay.h"
#pragma comment(lib,"libzplay")
#include "FileAttribute.h"
// CInfoDlg 对话框

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(const CString& strPath,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInfoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CString m_strFilePath;
public:
	virtual BOOL OnInitDialog();
};
