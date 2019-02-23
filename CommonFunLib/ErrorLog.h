#pragma once
/////////////////////////////////////////////////////////////////////////////////
//异常信息记录类

#include <string>
#ifdef UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif





//定义异常信息结构体
typedef struct _EXCEPTION_INFO
{
	tstring tstrExceptionFunc;//异常模块函数名
	tstring tstrExceptionInfo;//异常信息字符串
	int     nExceptionLine;	//出现异常的代码行
	char	szSourcePath[260];//出错源文件的完整路径
	DWORD   dwErrorCode;//异常码
}EXCEPTION_INFO,LPEXCEPTION_INFO;

//填充异常结构体
inline EXCEPTION_INFO FillExceptionInfo(TCHAR* pErrorInfo,int nLine,char* pSourcePath,DWORD dwErrorCode,TCHAR* pFuncName=NULL)
{
	EXCEPTION_INFO ei;
	if(NULL!=pFuncName)
		ei.tstrExceptionFunc.append(pFuncName);
	ei.tstrExceptionInfo.append(pErrorInfo);
	ei.nExceptionLine=nLine;
	ei.dwErrorCode=dwErrorCode;
	memset(ei.szSourcePath,0,260);
	strcpy(ei.szSourcePath,pSourcePath);
	strcat(ei.szSourcePath,"\r\n");
	return ei;
}
#define THROW_ERROR(x) {ei=FillExceptionInfo(_T(x),__LINE__-2,__FILE__,::GetLastError());throw 1;}

EXPORT_CLASS CErrorLog
{
public:
	CErrorLog(TCHAR* pModuleName);
	virtual~ CErrorLog();
	//记录异常,参数依次为函数名、错误信息、模块名
	bool WriteErrorLog(const EXCEPTION_INFO* pExcept,TCHAR* pModuleName=NULL);
protected:
	/********************受保护的成员函数**********************/
	//记录自身异常
	bool WriteOwerErrorLog(TCHAR* strFuncName,TCHAR* pErrorMsg,int nLine);
	bool WriteOwerErrorLog(const EXCEPTION_INFO* pException);
	//格式化当前时间
	inline std::string FormatTime();
	//获取系统错误信息
	inline std::string FormatLastError(const DWORD& dwErrorCode);
	//字节转换
	char* WcharToChar(const wchar_t* wp);
	wchar_t* CharToWchar(const char* p);
private:
	//保存当前模块的名称
	tstring m_strModuleName;
	static  TCHAR g_szModulePath[MAX_PATH];
};