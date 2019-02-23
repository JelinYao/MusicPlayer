/*********************************************************************************************************/
/*由于在实际开发过程中，涉及到编码问题而引起文件读取错误的频发，以及C++处理文件的某些不足                */
/*看了下c++文件操作的实现，貌似其底层也是用c语言实现的                                                   */
/*于是决定自己用c++把标准C的文件流封装起来,以后用起来也比较方便                                          */
/*能力有限，欢迎不吝赐教                                                                                 */
/*author:Jelin                                                                                           */
/*time:2012-5-6  17:19                                                                                   */
/*********************************************************************************************************/
#ifndef READFILE_H_
#define READFILE_H_
#include "..\\CommonDefine\\StandarC++.h"
#include "ErrorLog.h"
#include <assert.h>

/*********************************************************/
//定义几种常见的编码类型
typedef enum{
	ansi,utf8,unicode,big_unicode,unknow
}CODETYPE;

EXPORT_CLASS CReadFile:public CErrorLog
{
public:
	//定义文件的位置标识
	CReadFile(const char* pFile);
	CReadFile(const wchar_t* pFile);
	virtual~CReadFile();

	//读取count个大小为size 字节到缓冲区
	bool Read(void *buffer,const int zise,const int count);
	//读取文件的一行，count小于一行字符串长度时将截断
	bool ReadLine(char* buffer,const int nCount=MAX_PATH);
	bool ReadLine(std::string& s,const int nCount=MAX_PATH);
	bool ReadLine(std::wstring& ws,const int nCount=MAX_PATH);
	//读取文件所有内容到缓冲区
	void GetBuffer(char* buffer);
	//此函数针对宽字符编码的文件，ANSI下建议使用char*参数的函数
	void GetBuffer(wchar_t* buffer);
	void GetBuffer(std::string& s);
	//定位文件指针到指定位置,第一个参数是移动的字符数，第二个参数默认情况下是从当前位置开始(begin,current,end)
	bool Seek(const long lOffset,const int nFlag=SEEK_CUR);
	/*********************************************************************/
	//内联功能函数
	/*********************************************************************/
	//判断文件是否已经打开
	inline bool IsOpen()const;
	//判断文件指针是否到达末尾
	inline bool IsEof()const;
	//获取文件指针位置
	inline long GetPos()const;
	//释放文件资源
	inline void Release();
	//获取文件编码方式,返回值:0->ANSI编码.1->UTF-8编码,2->UNICODE编码,3->BIG_UNICODE编码
	inline CODETYPE GetFileCodeType()const ;
	//文件指针回到文件开始处
	inline void Rewind();
	//获取文件的长度（字节数)
	inline long GetLength();
	//读取一个字节
	inline int GetChar();

protected:
	//获取文件的编码方式并且跳过编码字节
	bool SkipCodeType();
private:
	//文件指针
	FILE* m_ptrFile;
	//保存文件的编码类型
	CODETYPE m_ctType;
};
#endif;