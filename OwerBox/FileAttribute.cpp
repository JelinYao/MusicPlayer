#include "StdAfx.h"
#include "FileAttribute.h"

CFileAttribute::CFileAttribute(const wchar_t* fileName):pFile(NULL)
{
	pFile=const_cast<wchar_t*>(fileName);
	GetFileAttribute();
}

CFileAttribute::~CFileAttribute(void)
{
	//Release();
}
float CFileAttribute::GetSize() const
{
	ULONG highSize=attributeData.nFileSizeHigh;
	highSize<<=sizeof(DWORD)*8;
	return (highSize+attributeData.nFileSizeLow)/1024.0f;
}
SYSTEMTIME CFileAttribute::GetAccessTime() const
{
	return FiletimeToSystemtime(attributeData.ftLastAccessTime);
}
SYSTEMTIME CFileAttribute::GetCreateTime() const
{
	return FiletimeToSystemtime(attributeData.ftCreationTime);
}
SYSTEMTIME CFileAttribute::GetWriteTime()const
{
	return FiletimeToSystemtime(attributeData.ftLastWriteTime);
}
SYSTEMTIME CFileAttribute::FiletimeToSystemtime(const FILETIME& ft)const
{
	SYSTEMTIME sm;
	FILETIME lft;//本地时间
	FileTimeToLocalFileTime(&ft,&lft);
	FileTimeToSystemTime(&lft,&sm);
	return sm;
}
BOOL CFileAttribute::GetFileAttribute()
{
	if(!::GetFileAttributesEx(pFile,GetFileExInfoStandard,&attributeData))
	{
		::MessageBox(NULL,_T("可能是文件不存在"),_T("获取文件信息失败:"),MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}
void CFileAttribute::ResetFile(const wchar_t *fileName)
{
	//Release();
	pFile=const_cast<wchar_t*>(fileName);//去掉const限定
	GetFileAttribute();
}

