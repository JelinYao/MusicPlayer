/*****************************************************************************************/
/*                             ★★★★★★★★★★★★★                                */
/*描述:CCodeTrans是用标准C++（当然包含少数Windows API的调用）写的一个用于字符编码格式转换*/
/*的类，最近加入了一点点异常处理，希望对于代码的稳定性能够有所增强。调用这个类的方法,可以*/
/*很方便地对一些常用C++类型及类进行转换。                                                */
/*备注：由于个人能力及时间有限，出现bug在所难免，希望不吝指教                            */
/*作者：Jelin                                                                            */
/*时间：2012年12月27日22:19:05                                                           */
/*邮箱：jelinyao@163.com                                                                 */
/*****************************************************************************************/
#ifndef CODE_CONVER_H_
#define CODE_CONVER_H_
#pragma once
#include <iostream>
#include <string>
/*#include <tchar.h>*/
//可以在这里将此类导出到DLL中 
EXPORT_CLASS CCodeCovert
{
public:
	//默认构造函数，初始化成员变量
	CCodeCovert();
	//析构函数，释放资源
	virtual~ CCodeCovert();
	//UNICODE指针转ANSCII指针
	char* WcharToChar(const wchar_t* wpRes);
	//wstring转ANSCII指针
	char* WstringToChar(const std::wstring& wstrRes);
	//ANSCII指针转UNICODE指针
	wchar_t* CharToWchar(const char* pRes);
	//string对象转UNICODE指针
	wchar_t* StringToWchar(const std::string& strRes);
	//string对象转wstring对象
	std::wstring StringToWstring(const std::string& strRes);
	//wstring对象转string对象
	std::string WstringToString(const std::wstring& wstrRes);
	//UTF8编码字符串转UNICODE编码字符串
	static std::wstring Utf8ToUnicode(const std::string& strUtf8); 
	//UTF8编码字符串转ASCII编码字符串
	std::string Utf8ToAscii(const std::string& strUtf8);
	//UNICODE编码字符串转UTF8编码字符串
	static std::string UnicodeToUtf8(const std::wstring& wstrUnicode); 
	//ASCII编码字符串转UTF8编码字符串
	std::string AsciiToUtf8(const std::string& strUnicode);
	//删除字符串前面的连续空格
	bool RemoveSpaces(std::string& strRes);
	bool RemoveSpaces(std::wstring& wstrRes);
	//获取字符转换错误信息
	inline std::string GetCodeError()const;
	inline char* GetCodeError();
private:
	//转码用的内部缓冲区
	char* m_pBuffer;
	wchar_t* m_wpBuffer;
	//出错信息
	static std::string m_strError;
protected: 
	//打印出错信息
	static void PrintErrorString();
	//释放动态申请的内存资源
	bool Release();
	//设置字符转换错误信息
	static bool SetCodeError(const char* pError);
	static bool SetCodeError(const std::string& strError);
	//释放指针指向的内存资源
	inline void Free(wchar_t* pBuffer){if(pBuffer){delete[] pBuffer;pBuffer=NULL;}}
	inline void Free(char* pBuffer){if(pBuffer){delete[] pBuffer;pBuffer=NULL;}}
};
#endif
/*****************************************************************************************************/
/*最近在开发一个小程序时，经常遇到各种字符串类型的相互转换，处理起来感觉很多代码都是复制粘贴         */
/*既浪费时间也不易于测试代码，于是就用C++封装在了一起，调用起来也就轻松多了                          */
/*能力有限。欢迎不吝赐教                                                                             */
/*作者:Jelin                                                                                       */
/*时间:2012-4-20  10:14                                                                              */
/*****************************************************************************************************/
/*===================================================================================================*/
/*****************************************************************************************************/
/*由于静态成员变量为所有类对象所共有，会不会存在一个对象在处理字符串时另一个对象也在进行处理呢？     */
/*这样就会导致所谓的“读脏数据”，那么就试试用临界区这个解决冲突的办法吧！                           */
/*由于可能存在多个线程同时转换字符串的问题，还是去掉static声明                                       */
/*因此，对于转换后的指针必须自己调用Release()释放                                                    */
/*作者:Jelin                                                                                       */
/*时间:2012-12-27  16:38                                                                             */
/*****************************************************************************************************/