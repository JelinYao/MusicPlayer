#pragma once
#include <windows.h>
#include <iostream>
using std::cout;
using std::endl;
class CFileAttribute
{
public:
	CFileAttribute(const wchar_t* fileName);
	~CFileAttribute(void);
	//返回文件大小（KB为单位)
	float GetSize()const;
	SYSTEMTIME GetCreateTime()const;//获取文件创建时间
	SYSTEMTIME GetAccessTime()const;//获取文件最后访问时间
	SYSTEMTIME GetWriteTime()const;//获取文件最后写入时间
	SYSTEMTIME FiletimeToSystemtime(const FILETIME& ft)const;
	BOOL GetFileAttribute();//获取文件属性
	void ResetFile(const wchar_t* fileName);//改变文件
private:
	wchar_t* pFile;//指向文件的指针
	WIN32_FILE_ATTRIBUTE_DATA attributeData;
};
