// SetAttributeDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "SetAttributeDlg.h"
#include <Shlwapi.h>

/////////////////////////////////////////////////////////////////////////////////
//设置程序自动启动线程
DWORD WINAPI BootStartThread(LPVOID lpParam)
{
	CString strRunPath=CString(GetAppPath().c_str())+_T("\\MusicPlayer.exe");//这里加上你要查找的执行文件名称   
	CFileFind   fFind;   
	BOOL  bSuccess;   
	bSuccess=fFind.FindFile(strRunPath);   
	fFind.Close();   
	if(bSuccess)   
	{   
		HKEY hKey=NULL;  
		LONG lResult=ERROR_SUCCESS;//HKEY_LOCAL_MACHINE
		lResult=::RegOpenKey(HKEY_CURRENT_USER,\
			_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),&hKey); 
		int nLength=strRunPath.GetLength()*2;
		BYTE* pData=(BYTE*)strRunPath.GetBuffer();
		lResult=::RegSetValueEx(hKey,_T("MusicPlayer"),0,REG_SZ,\
			pData,nLength);//这里加上你需要在注册表中注册的内容   
		::RegCloseKey(hKey); 
	}   
	else    
		::AfxMessageBox(_T("没找到执行程序，自动运行失败"));      
	return 0;
}
DWORD WINAPI RemoveBootStartThread(LPVOID lpParam)
{
	HKEY hKey=NULL;
	::RegOpenKey(HKEY_CURRENT_USER,\
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),&hKey);
	if(NULL==hKey)
	{
		::AfxMessageBox(_T("打开指定注册表位置失败!"));
		return 1;
	}
	CString strKeyName=_T("MusicPlayer");
	::RegDeleteValue(hKey,strKeyName);
	::RegCloseKey(hKey);
	return 0;
}
// CSetAttributeDlg 对话框

IMPLEMENT_DYNAMIC(CSetAttributeDlg, CDialog)

CSetAttributeDlg::CSetAttributeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAttributeDlg::IDD, pParent)
	,CErrorLog(_T("CSetAttributeDlg"))
	, m_strConfigPath(_T(""))
	,m_bRunBoot(FALSE)
{

}

CSetAttributeDlg::~CSetAttributeDlg()
{
}

void CSetAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LVOICE, m_leftVoice);
	DDX_Control(pDX, IDC_RVOICE, m_rightVoice);
	DDX_Control(pDX, IDC_RUNBOOT, m_checkButton);
	DDX_Control(pDX, IDC_RATIO, m_btnRate);
	DDX_Control(pDX, IDC_PITCH, m_btnPitch);
	DDX_Control(pDX, IDC_SPEED, m_btnSpeed);
}


BEGIN_MESSAGE_MAP(CSetAttributeDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RUNBOOT, &CSetAttributeDlg::OnBnClickedCheck1)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CSetAttributeDlg 消息处理程序

BOOL CSetAttributeDlg::OnInitDialog()
{
// 	try
// 	{
		CDialog::OnInitDialog();

		// TODO:  在此添加额外的初始化
		m_leftVoice.MoveWindow(94,66,130,6);
		m_leftVoice.InitCtrl(IDB_BITMAP_RED,_T("左声道"),100,WM_SETATTRIBUTE_MESSAGE,1);
		m_leftVoice.SetBkColor(RGB(155,0,200));
		m_rightVoice.MoveWindow(94,92,130,6);
		m_rightVoice.InitCtrl(IDB_BITMAP_RED,_T("右声道"),100,WM_SETATTRIBUTE_MESSAGE,2);
		m_rightVoice.SetBkColor(RGB(200,0,155));

		m_btnRate.MoveWindow(94,136,130,6);
		m_btnRate.InitCtrl(IDB_BITMAP_YELLOW,_T("比特率"),200,WM_SETATTRIBUTE_MESSAGE,3);
		m_btnRate.SetBkColor(RGB(100,0,200));
		m_btnRate.SetRatio(0.5);

		m_btnPitch.MoveWindow(94,167,130,6);
		m_btnPitch.InitCtrl(IDB_BITMAP_YELLOW,_T("音调"),200,WM_SETATTRIBUTE_MESSAGE,4);
		m_btnPitch.SetBkColor(RGB(0,200,155));
		m_btnPitch.SetRatio(0.5);

		m_btnSpeed.MoveWindow(94,199,130,6);
		m_btnSpeed.InitCtrl(IDB_BITMAP_YELLOW,_T("播放速度"),200,WM_SETATTRIBUTE_MESSAGE,5);
		m_btnSpeed.SetBkColor(RGB(0,200,0));
		m_btnSpeed.SetRatio(0.5);

		CString strTemp;
		strTemp.Append(GetAppPath().c_str());
		m_strConfigPath=strTemp+_T("\\AppData\\PlayerData.xml");
		ReadConfigData();
		HKEY hKey=NULL;
		LONG lResult=::RegGetValue(HKEY_CURRENT_USER,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),\
			_T("MusicPlayer"),RRF_RT_REG_SZ,NULL,NULL,NULL);
		if(lResult==ERROR_SUCCESS)
		{
			m_bRunBoot=TRUE;
			((CButton*)GetDlgItem(IDC_RUNBOOT))->SetCheck(TRUE);
		}
		else
			m_bRunBoot=FALSE;
// 	}
// 	catch(...)
// 	{
// 		CString strError;
// 		strError.Format(_T("出现异常，异常码：%d"),GetLastError());
// 		MessageBox(strError);
// 	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CSetAttributeDlg::ReadConfigData(void)
{
	if(!PathFileExists(m_strConfigPath))
		CreateConfigFile();
	//文件存在，直接读取
	CMarkup xml;
	xml.Load(m_strConfigPath);
	while(xml.FindElem(_T("config")))
	{
		xml.IntoElem();
		CString strValue=_T("");
		int nData=100;
		if(xml.FindElem(_T("LeftVoice")))
		{
			strValue=xml.GetData();
			nData=_ttoi(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			if(nData<0||nData>100)
				nData=100;
		}
		else
			xml.AddElem(_T("LeftVoice"),100);
		m_leftVoice.SetCurPos(nData);
		if(xml.FindElem(_T("RightVoice")))
		{
			strValue=xml.GetData();
			nData=_ttoi(strValue.GetBuffer());
			strValue.ReleaseBuffer();
			if(nData<0||nData>100)
				nData=100;
		}
		else
			xml.AddElem(_T("RightVoice"),100);
		m_rightVoice.SetCurPos(nData);
		xml.OutOfElem();
	}
}

void CSetAttributeDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SaveConfigData();
	CDialog::OnClose();
}

void CSetAttributeDlg::SaveConfigData(void)
{
	if(!PathFileExists(m_strConfigPath))
		CreateConfigFile();
	else
	{
		CMarkup xml;
		xml.Load(m_strConfigPath);
		xml.FindElem(_T("config"));
		xml.IntoElem();
		int nValue=0;
		nValue=m_leftVoice.GetCurPos();
		if(xml.FindElem(_T("LeftVoice")))
			xml.SetData(nValue);
		else
			xml.AddElem(_T("LeftVoice"),nValue);
		nValue=m_rightVoice.GetCurPos();
		if(xml.FindElem(_T("RightVoice")))
			xml.SetData(nValue);
		else
			xml.AddElem(_T("RightVoice"),nValue);
		nValue=m_btnRate.GetCurPos();
		xml.OutOfElem();
		xml.Save(m_strConfigPath);
	}
}

void CSetAttributeDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_bRunBoot)
	{
		SetBootStart();
		m_bRunBoot=TRUE;
	}
	else
	{
		m_bRunBoot=FALSE;
		RemoveBootStart();
	}
}

void CSetAttributeDlg::SetBootStart(void)
{
	HANDLE hThread=::CreateThread(NULL,0,BootStartThread,NULL,0,NULL);
	::CloseHandle(hThread);
}
void CSetAttributeDlg::RemoveBootStart()
{
	HANDLE hThread=::CreateThread(NULL,0,RemoveBootStartThread,NULL,0,NULL);
	::CloseHandle(hThread);
}
void CSetAttributeDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
// 	CBrush* pBr=NULL;
// 	CBrush br(RGB(255,0,20));
// 	pBr=dc.SelectObject(&br);
// 	//dc.Ellipse(&CRect(162,146,172,164));
// 	dc.FillSolidRect(CRect(164,144,168,204),RGB(200,0,0));
// 	dc.SelectObject(pBr);
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
}

void CSetAttributeDlg::CreateConfigFile(void)
{
	CMarkup xml;
	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"GB2312\"?>\r\n"));
	xml.AddElem(_T("config"));
	xml.IntoElem();
	m_leftVoice.SetRatio(1);
	xml.AddElem(_T("LeftVoice"),100);
	m_rightVoice.SetRatio(1);
	xml.AddElem(_T("RightVoice"),100);
	m_btnRate.SetRatio(0.5);
	xml.AddElem(_T("PlayRate"),100);
	m_btnPitch.SetRatio(0.5);
	xml.AddElem(_T("PlayPitch"),100);
	m_btnSpeed.SetRatio(0.5);
	xml.AddElem(_T("PlaySpeed"),100);
	xml.OutOfElem();
	xml.Save(m_strConfigPath);
}

void CSetAttributeDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
}

void CSetAttributeDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::PostNcDestroy();
	delete this;
	g_pSetAttributeDlg=NULL;
}
