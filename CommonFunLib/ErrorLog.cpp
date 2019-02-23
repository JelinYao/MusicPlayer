#include "stdafx.h"
/*#include "CodeConvert.h"*/
#include "ErrorLog.h"
#include <time.h>
#include <assert.h>
TCHAR CErrorLog::g_szModulePath[MAX_PATH]={0};
CErrorLog::CErrorLog(TCHAR* pModuleName)
{
	assert(pModuleName);
	m_strModuleName.append(pModuleName);
	m_strModuleName.append(_T(".log"));//文件名
	::GetModuleFileName(NULL,g_szModulePath,MAX_PATH);
	int nIndex=0;
	for(nIndex=_tcslen(g_szModulePath)-1;nIndex>=0;--nIndex)
	{
		if('\\'==g_szModulePath[nIndex])
			break;
	}
	g_szModulePath[nIndex+1]='\0';
	_tcscat(g_szModulePath,_T("ErrorLogs\\"));
	::CreateDirectory(g_szModulePath,NULL);
}
CErrorLog::~CErrorLog()
{

}
bool CErrorLog::WriteOwerErrorLog(TCHAR* pFuncName,TCHAR* pErrorMsg,int nLine)
{
	EXCEPTION_INFO ei=FillExceptionInfo(pErrorMsg,nLine,__FILE__,::GetLastError(),pFuncName);
	return WriteErrorLog(&ei,_T("CErrorLog"));
}

bool CErrorLog::WriteOwerErrorLog(const EXCEPTION_INFO* pException )
{
	return WriteErrorLog(pException,_T("CErrorLog"));
}

bool CErrorLog::WriteErrorLog( const EXCEPTION_INFO* pExcept,TCHAR* pModuleName/*=NULL*/ )
{
	bool bRet=true;
	try
	{
		if(NULL==pExcept)
			throw _T("异常信息结构指针为空！");
		TCHAR szFileName[MAX_PATH]={0};//日志文件的全路径 
		_tcscpy(szFileName,g_szModulePath);
		if(NULL==pModuleName)//模块名存在
			_tcscat(szFileName,m_strModuleName.c_str());
		else
		{
			_tcscat(szFileName,pModuleName);
			_tcscat(szFileName,_T(".log"));
		}
		std::string strFuncName,strExceptInfo;
#ifdef UNICODE
		char* pTemp=WcharToChar(pExcept->tstrExceptionFunc.c_str());
		if(pTemp)
		{
			strFuncName.append(pTemp);
			delete[] pTemp;
		}
		pTemp=WcharToChar(pExcept->tstrExceptionInfo.c_str());
		if(pTemp)
		{
			strExceptInfo.append(pTemp);
			delete[] pTemp;
		}
		pTemp=NULL;
#else
		strFuncName=pExcept->tstrExceptionFunc;
		strExceptInfo=pExcept->tstrExceptionInfo;
#endif
		//开始写文件
		FILE* fp=_tfopen(szFileName,_T("a+"));
		char* pBuffer="*************************************************************************\n";
		printf("%s",pBuffer);
		int nRet=fwrite(pBuffer,strlen(pBuffer),1,fp);
		pBuffer="系统时间：";
		fwrite(pBuffer,strlen(pBuffer),1,fp);
		std::string strTime=FormatTime();
		fwrite(strTime.c_str(),strTime.size(),1,fp);
		printf("%s%s",pBuffer,strTime.c_str());
		pBuffer="文件路径：";
		fwrite(pBuffer,strlen(pBuffer),1,fp);
		fwrite(pExcept->szSourcePath,strlen(pExcept->szSourcePath),1,fp);
		printf("%s%s",pBuffer,pExcept->szSourcePath);
		pBuffer="异常行数：";
		fwrite(pBuffer,strlen(pBuffer),1,fp);
		char szLine[7]={0};
		sprintf(szLine,"%d\r\n",pExcept->nExceptionLine);
		fwrite(szLine,strlen(szLine),1,fp);
		printf("%s%s",pBuffer,szLine);
		pBuffer="异常函数：";
		fwrite(pBuffer,strlen(pBuffer),1,fp);
		strFuncName.append("\n");
		fwrite(strFuncName.c_str(),strFuncName.size(),1,fp);
		printf("%s%s",pBuffer,strFuncName.c_str());
		pBuffer="异常信息：";
		fwrite(pBuffer,strlen(pBuffer),1,fp);
		strExceptInfo.append("\n");
		fwrite(strExceptInfo.c_str(),strExceptInfo.size(),1,fp);
		std::string strSyserror=FormatLastError(pExcept->dwErrorCode);
		fwrite(strSyserror.c_str(),strSyserror.size(),1,fp);
		printf("%s%s%s",pBuffer,strExceptInfo.c_str(),strSyserror.c_str());
		fclose(fp);
	}
	catch(TCHAR* pError)
	{
		bRet=false;
		WriteOwerErrorLog(_T("WriteErrorLog"),pError,__LINE__);
	}
	return bRet;
}

std::string CErrorLog::FormatLastError(const DWORD& dwErrorCode)
{
	std::string strRet("系统错误：");
	try
	{
		LPVOID lpErrorMsg=NULL; 
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			dwErrorCode, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
			(LPTSTR)&lpErrorMsg, 
			0, 
			NULL 
			);
		tstring tstrError((LPTSTR)lpErrorMsg);
#ifdef UNICODE
		char* pTemp=WcharToChar(tstrError.c_str());
		if(pTemp)
		{
			std::string strError(pTemp);
			delete[] pTemp;
			pTemp=NULL;
			strRet+=strError;
		}
#else
		strRet+=tstrError;
#endif
		::LocalFree(lpErrorMsg);
	}
	catch(TCHAR* pError)
	{
		WriteOwerErrorLog(_T("FormatLastError"),pError,__LINE__);
	}
	return strRet;
}
std::string CErrorLog::FormatTime()
{
	std::string strRet;
	try
	{
		tm* st;
		time_t time64=time(NULL);
		st=localtime(&time64);
		strRet.append(asctime(st));
	}
	catch(TCHAR* pError)
	{

		WriteOwerErrorLog(_T("FormatTime"),pError,__LINE__);
		strRet.clear();
	}
	return strRet;
}

char* CErrorLog::WcharToChar( const wchar_t* wp )
{
	char* pResult=NULL;
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==wp)
			THROW_ERROR(NULL_POINT_ERROR)
		int nLen= WideCharToMultiByte(CP_ACP,0,wp,wcslen(wp),NULL,0,NULL,NULL);
		if(0==nLen)
			THROW_ERROR(EMPTY_BUFFER_ERROR)
		pResult=new char[nLen+1];
		memset(pResult,0,nLen+1);
		nLen=WideCharToMultiByte(CP_ACP,0,wp,wcslen(wp),pResult,nLen,NULL,NULL);
		pResult[nLen]='\0';
	}
	catch(...)
	{
		WriteOwerErrorLog(&ei);
	}
	return pResult;
}

wchar_t* CErrorLog::CharToWchar( const char* p )
{
	wchar_t* wpResult=NULL;
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==p)
			THROW_ERROR(NULL_POINT_ERROR)
		int nLen = MultiByteToWideChar(CP_ACP,0,p,strlen(p),NULL,0);
		if(0==nLen)
			THROW_ERROR(EMPTY_BUFFER_ERROR)
		wpResult=new wchar_t[nLen+1];
		memset(wpResult,0,nLen+1);
		nLen=MultiByteToWideChar(CP_ACP,0,p,strlen(p),wpResult,nLen);
		wpResult[nLen]='\0';
	}
	catch(...)
	{
		WriteOwerErrorLog(&ei);
	}
	return wpResult;
}
