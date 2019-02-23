#include "stdafx.h"
#include "CodeConvert.h"
/*********************宏定义快速替换************************/
//出错后，抛出异常
#define WCODE_ERROR(x) {if(0==x)throw "WideCharToMultiByte函数调用失败！";}
#define MCODE_ERROR(x) {if(0==x)throw "MultiByteToWideChar函数调用失败！";}
/////////////////////////////////////////////////////////////
/********************静态成员变量声明***********************/
std::string CCodeCovert::m_strError;
/////////////////////////////////////////////////////////////
/********************类成员函数的声明***********************/
CCodeCovert::CCodeCovert()
:m_pBuffer(NULL)
,m_wpBuffer(NULL)
{
	::setlocale(LC_ALL,"");
}
CCodeCovert::~CCodeCovert()
{
	Release();
}
char* CCodeCovert::WcharToChar(const wchar_t* wpRes)
{
	char* pRet=NULL;//函数调用失败将返回空指针
	try
	{
		Free(m_pBuffer);//清理释放内存资源
		int nLen= WideCharToMultiByte(CP_ACP,0,wpRes,wcslen(wpRes),NULL,0,NULL,NULL);
		WCODE_ERROR(nLen)
		m_pBuffer=new char[nLen+1];
		memset(m_pBuffer,0,nLen+1);
		nLen=WideCharToMultiByte(CP_ACP,0,wpRes,wcslen(wpRes),m_pBuffer,nLen,NULL,NULL);
		WCODE_ERROR(nLen)
		m_pBuffer[nLen]='\0';
		pRet=m_pBuffer;
	}
	catch(char* pError)
	{
		Free(m_pBuffer);
		SetCodeError(pError);
		PrintErrorString();
	}
	return pRet;
}
wchar_t* CCodeCovert::CharToWchar(const char* pRes)
{
	wchar_t* wpRet=NULL;
	try
	{
		Free(m_wpBuffer);
		int nLen = MultiByteToWideChar(CP_ACP,0,pRes,strlen(pRes),NULL,0);
		MCODE_ERROR(nLen)
		m_wpBuffer=new wchar_t[nLen+1];
		memset(m_wpBuffer,0,nLen+1);
		nLen=MultiByteToWideChar(CP_ACP,0,pRes,strlen(pRes),m_wpBuffer,nLen);
		MCODE_ERROR(nLen)
		m_wpBuffer[nLen]='\0';
		wpRet=m_wpBuffer;
	}
	catch(char* pError)
	{
		Free(m_pBuffer);
		SetCodeError(pError);
		PrintErrorString();
	}
	return wpRet;
}
// 释放动态申请的内存空间
bool CCodeCovert::Release()
{
	bool bRet=true;
	try
	{
		Free(m_pBuffer);
		Free(m_wpBuffer);
	}
	catch(...)
	{
		bRet=false;
		SetCodeError("Release出错：释放内存空间出错！");
		PrintErrorString();
	}
	return bRet;
}
char* CCodeCovert::WstringToChar(const std::wstring &wstrRes)
{
	char* pRet=NULL;
	try
	{
		if(wstrRes.empty())
			throw "WstringToChar出错：参数wstrRes为空字符串!";
		const wchar_t* wp=wstrRes.c_str();
		pRet=WcharToChar(wp);
	}
	catch(char* pError)
	{
		pRet=NULL;
		SetCodeError(pError);
		PrintErrorString();
	}
	return pRet;
}
wchar_t* CCodeCovert::StringToWchar(const std::string& strRes)
{
	wchar_t* wpRet=NULL;
	try
	{
		if(strRes.empty())
			throw "StringToWchar出错：参数strRes为空字符串！";
		const char* p=strRes.c_str();
		wpRet=CharToWchar(p);
	}
	catch(char* pError)
	{
		wpRet=NULL;
		SetCodeError(pError);
		PrintErrorString();
	}
	return wpRet;
}
std::string CCodeCovert::WstringToString(const std::wstring &wstrRes)
{
	//失败返回空串
	std::string strRet("");
	try
	{
		if(wstrRes.empty())
			throw "WstringToString出错：参数wstrRes为空字符串！";
		char* p=WcharToChar(wstrRes.c_str());
		strRet.append(p);
	}
	catch(char* pError)
	{
		strRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return strRet;
}
std::wstring CCodeCovert::StringToWstring(const std::string &strRes)
{
	std::wstring wstrRet(L"");
	try
	{
		if(strRes.empty())
			throw "StringToWstring出错：参数strRes为空字符串！";
		wchar_t* wp=CharToWchar(strRes.c_str());
		wstrRet.append(wp);
	}
	catch(char* pError)
	{
		wstrRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return wstrRet;
}
std::wstring CCodeCovert::Utf8ToUnicode(const std::string& strUtf8)  
{  
	std::wstring wstrRet(L"");
	try
	{
		if(strUtf8.empty())
			throw "Utf8ToUnicode出错：参数strUtf8为空字符串！";
		int nLen=MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);  
		if (nLen == ERROR_NO_UNICODE_TRANSLATION)  
			throw "Utf8ToUnicode出错：无效的UTF-8字符串。";  
		MCODE_ERROR(nLen)
		wstrRet.resize(nLen+1,'\0');
		nLen=MultiByteToWideChar(CP_UTF8,0,strUtf8.c_str(), -1,(LPWSTR)wstrRet.c_str(), nLen);
		WCODE_ERROR(nLen);
	}  
	catch(char* pError)
	{
		wstrRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return wstrRet;  
}
std::string CCodeCovert::Utf8ToAscii(const std::string& strUtf8)
{
	std::string strRet("");
	try
	{
		std::wstring wstr=Utf8ToUnicode(strUtf8);
		if(wstr.empty())
			throw "Utf8ToAscii出错：参数strUtf8为空字符串！";
		strRet=WstringToString(wstr);
	}
	catch(char* pError)
	{
		strRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return strRet;
}
std::string CCodeCovert::UnicodeToUtf8(const std::wstring& wstrUnicode)  
{  
	std::string strRet("");
	try
	{
		if(wstrUnicode.empty())
			throw "UnicodeToUtf8出错：参数wstrUnicode为空字符串！";
		int nLen=WideCharToMultiByte(CP_UTF8,0,wstrUnicode.c_str(),-1,NULL,0,NULL,NULL);  
		WCODE_ERROR(nLen)
		strRet.resize(nLen+1,'\0');
		nLen=WideCharToMultiByte(CP_UTF8,0,wstrUnicode.c_str(),-1,\
			(LPSTR)strRet.c_str(),nLen,NULL,NULL);  
		WCODE_ERROR(nLen)
	}  
	catch(char* pError)
	{
		strRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return strRet;  
}
std::string CCodeCovert::AsciiToUtf8(const std::string& strUnicode)
{
	std::string strRet("");
	try
	{
		if(strUnicode.empty())
			throw "AsciiToUtf8出错：参数strUnicode为空字符串！";
		std::wstring wstrTemp=StringToWstring(strUnicode);
		strRet=UnicodeToUtf8(wstrTemp);
	}
	catch(char* pError)
	{
		strRet.clear();
		SetCodeError(pError);
		PrintErrorString();
	}
	return strRet;
}
bool CCodeCovert::RemoveSpaces(std::string& strRes)
{
	bool bRet=true;
	try
	{
		if(strRes.empty())
			throw "RemoveSpaces出错：参数strRes为空字符串！";
		int nSpaceCount=0;
		//获取前面的连续空格数
		while(strRes[nSpaceCount]!='\0'&&(strRes[nSpaceCount]==' '))
			nSpaceCount++;
		strRes=strRes.substr(nSpaceCount);
	}
	catch(char* pError)
	{
		bRet=false;
		SetCodeError(pError);
		PrintErrorString();
	}
	return bRet;
}
bool CCodeCovert::RemoveSpaces(std::wstring& wstrRes)
{
	bool bRet=true;
	try
	{
		if(wstrRes.empty())
			throw "RemoveSpaces出错：参数wstrRes为空字符串！";
		int nSpaceCount=0;
		while(wstrRes[nSpaceCount]!='\0'&&(wstrRes[nSpaceCount]==' '))
			nSpaceCount++;
		wstrRes=wstrRes.substr(nSpaceCount);
	}
	catch(char* pError)
	{
		bRet=false;
		SetCodeError(pError);
		PrintErrorString();
	}
	return bRet;
}
void CCodeCovert::PrintErrorString()
{
	//获取失败信息
	LPVOID lpMsgBuf; 
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, 
		GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language 
		(LPTSTR) &lpMsgBuf, 
		0, 
		NULL 
		); 
	std::cout<<"CodeTrans错误信息:"<<m_strError<<std::endl;
	std::cout<<"系统出错信息："<<lpMsgBuf<<std::endl;
	::LocalFree(lpMsgBuf);
}
bool CCodeCovert::SetCodeError(const char* pError)
{
	bool bRet=true;
	try
	{
		if(NULL==pError)
			throw "SetCodeError出错：参数pError指针为空！";
		m_strError.clear();//清理错误信息
		m_strError.append(pError);
	}
	catch(char* pStr)
	{
		bRet=false;
		SetCodeError(pStr);
		PrintErrorString();
	}
	return bRet;
}
bool CCodeCovert::SetCodeError(const std::string& strError)
{
	bool bRet=true;
	try
	{
		if(strError.empty())
			throw "SetCodeError出错：参数strError为空字符串！";
		m_strError.clear();
		m_strError=strError;
	}
	catch(char* pError)
	{
		bRet=false;
		SetCodeError(pError);
		PrintErrorString();
	}
	return bRet;
}
std::string CCodeCovert::GetCodeError()const
{
	return m_strError;
}
char* CCodeCovert::GetCodeError()
{
	return const_cast<char*>(m_strError.c_str());
}