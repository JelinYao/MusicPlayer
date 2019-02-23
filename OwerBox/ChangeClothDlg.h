#pragma once
#include "Resource.h"
#include "ImageCtrl.h"
#include "afxwin.h"
#include "e:\mymusicplayer\owerdrawcontrl\linkbutton.h"
#include "e:\mymusicplayer\owerdrawcontrl\skinbutton.h"


// CChangeClothDlg 对话框

class CChangeClothDlg : public CDialog,public CErrorLog,public CMemBase
{
	DECLARE_DYNAMIC(CChangeClothDlg)

public:
	CChangeClothDlg(const BYTE bAlphaValue,const CString& strBackImagePath,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChangeClothDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CHANGECLOTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
public:
	virtual BOOL OnInitDialog();
private:
	// 调节窗口透明度
	CSliderButton m_btnTransparent;
	CImageCtrl* m_arrContrls[2][9];
	//保存当前选中的图片位置
	INDEX m_indSelect;
	//保存窗口大小
	CRect m_rcClient;
	// 保存窗口的ALPHA值
	BYTE m_bAlphaValue;
	CSkinButton m_btnChange;
	//保存选中的皮肤的路径
	CString m_strBackImagePath;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeImages();
};
