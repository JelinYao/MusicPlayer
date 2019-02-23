#include "stdafx.h"
#include "ReadFile.h"
#include "CodeConvert.h"

typedef unsigned char byte;
CReadFile::CReadFile(const char* pFile)
:CErrorLog(_T("CReadFile"))
,m_ptrFile(NULL)
,m_ctType(CODETYPE::unknow)
{
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==pFile)
			THROW_ERROR(NULL_POINT_ERROR)
		m_ptrFile=fopen(pFile,"rb");
		if(NULL==m_ptrFile)
			THROW_ERROR(OPEN_FILE_ERROR)
		SkipCodeType();
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("CReadFile");
		WriteErrorLog(&ei);
	}
}
CReadFile::CReadFile(const wchar_t *pFile)
:CErrorLog(_T("CReadFile"))
,m_ptrFile(NULL)
,m_ctType(CODETYPE::unknow)
{
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==pFile)
			THROW_ERROR(NULL_POINT_ERROR)
			m_ptrFile=_tfopen(pFile,_T("rb"));
		if(NULL==m_ptrFile)
			THROW_ERROR(OPEN_FILE_ERROR)
		SkipCodeType();
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("CReadFile");
		WriteErrorLog(&ei);
	}
}
CReadFile::~CReadFile(void)
{
	Release();
}
inline void CReadFile::Release()
{
	EXCEPTION_INFO ei;
	try
	{
		if(m_ptrFile)
		{
			fclose(m_ptrFile);
			m_ptrFile=NULL;
		}
	}
	catch(...)
	{
		THROW_ERROR(UNKNOW_ERROR)
		WriteErrorLog(&ei);
	}
}
inline bool CReadFile::IsOpen()const
{
	return m_ptrFile?true:false;
}
inline CODETYPE CReadFile::GetFileCodeType()const
{
	return m_ctType;
}
bool CReadFile::Read(void *pBuffer,const int nSize,const int nCount)
{
	bool bResult=false;
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==pBuffer)
			THROW_ERROR(NULL_POINT_ERROR)
		if(nSize<=0||nCount<=0)
			THROW_ERROR(PARAMETER_ERROR)
		if(nCount!=fread(pBuffer,nSize,nCount,m_ptrFile))
			THROW_ERROR(READ_FILE_ERROR)
		bResult=true;
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("Read");
		WriteErrorLog(&ei);
	}
	return bResult;
}
bool CReadFile::ReadLine(char* buffer, const int nCount)
{
	bool bResult=false;
	EXCEPTION_INFO ei;
	try
	{
		if(NULL==m_ptrFile)
			THROW_ERROR(NULL_POINT_ERROR)
		if(NULL==buffer)
			THROW_ERROR(NULL_POINT_ERROR)
		if(nCount<=0)
			THROW_ERROR(PARAMETER_ERROR)
		std::string strLine;
		switch(m_ctType)
		{
		case ansi:
		case utf8:
			{
				char ch;
				while(!feof(m_ptrFile))
				{
					ch=fgetc(m_ptrFile);
					if(ch==13)
					{
						fgetc(m_ptrFile);//跳过一个字节char(10)
						bResult=true;
						break;
					}
					strLine.append(1,ch);
				}
			}
			break;
		case unicode:
		case big_unicode:
			{
				wchar_t wc;
				std::wstring ws;
				while(!feof(m_ptrFile))
				{
					fread(&wc,1,2,m_ptrFile);
					std::locale loc("chs");
					std::wcout.imbue(loc);
					if(wc==0x000A)
					{
						bResult=true;
						break;
					}
					ws.append(1,wc);
				}
				if(!ws.empty())
				{
					CCodeCovert cc;
					strLine=cc.WstringToString(ws);
				}
			}
			break;
		default:
			break;
		}
		if(bResult)
		{
			int nLen=strLine.size();
			if(nLen>nCount)//截断处理
				nLen=nCount;
			memcpy(buffer,strLine.c_str(),nLen);
			buffer[nLen]='\0';
		}
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("ReadLine");
		WriteErrorLog(&ei);
	}
	return bResult;
}
bool CReadFile::ReadLine(std::string &s,const int nCount)
{
	bool bResult=false;
	try{
		s.clear();
		char* pLine=new char[nCount+1];
		if(ReadLine(pLine,nCount))
		{
			s.append(pLine);
			bResult=true;
		}
		delete[] pLine;//释放内存
		pLine=NULL;
	}
	catch(...)
	{

	}
	return bResult;
}
bool CReadFile::ReadLine(std::wstring& ws,const int nCount)
{
	bool bResult=false;
	try
	{
		ws=L"";
		std::string s;
		CCodeCovert cc;
		if(ReadLine(s,nCount))
		{
			ws=cc.StringToWstring(s);
			bResult=true;
		}
	}
	catch(...)
	{

	}
	return bResult;
}
inline bool CReadFile::IsEof()const
{
	assert(m_ptrFile);
	return feof(m_ptrFile);
}
inline long CReadFile::GetPos()const
{
	assert(m_ptrFile);
	return ftell(m_ptrFile);
}

inline void CReadFile::Rewind()
{
	assert(m_ptrFile);
	rewind(m_ptrFile);
}
bool CReadFile::Seek(const long lOffset,const int nFlag/*=SEEK_CUR*/)
{
	bool bResult=false;
	EXCEPTION_INFO ei;
	try
	{
		assert(m_ptrFile);
		if(nFlag<0||nFlag>=3)
			THROW_ERROR(PARAMETER_ERROR)
		long lLength=GetLength();
		long lCurPos=ftell(m_ptrFile);//获取当前位置
		switch(nFlag)
		{
		case SEEK_SET:
			{
				if(lOffset<0||lOffset>lLength-1)
					THROW_ERROR(PARAMETER_ERROR)
				fseek(m_ptrFile,lOffset,SEEK_SET);
				break;
			}
		case SEEK_CUR:
			{
				if(lOffset<0)
				{
					if(-lOffset>lCurPos)
						THROW_ERROR(PARAMETER_ERROR)
				}
				else
				{
					if(lOffset>(lLength-1-lCurPos))
						THROW_ERROR(PARAMETER_ERROR)
				}
				fseek(m_ptrFile,lOffset,SEEK_CUR);
				break;
			}
		case SEEK_END:
			{
				if(lOffset>0||(-lOffset)>lLength-1)
					THROW_ERROR(PARAMETER_ERROR)
				fseek(m_ptrFile,lOffset,SEEK_END);
				break;
			}
		default:
			break;
		}
		bResult=true;
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("Seek");
		WriteErrorLog(&ei);
	}
	return bResult;
}
inline long CReadFile::GetLength()
{
	long lResult=0;
	try
	{
		assert(m_ptrFile);
		long nPos=ftell(m_ptrFile);//保存文件指针的位置
		rewind(m_ptrFile);
		fseek(m_ptrFile,0,SEEK_END);
		lResult=ftell(m_ptrFile);
		fseek(m_ptrFile,nPos,SEEK_SET);//文件归位
	}
	catch(...)
	{

	}
	return lResult;
}
inline int CReadFile::GetChar()
{
	assert(m_ptrFile);
	return fgetc(m_ptrFile);
}

void CReadFile::GetBuffer(char *buffer)
{
	assert(buffer);
	std::string s;
	GetBuffer(s);
	memcpy(buffer,s.c_str(),s.size());
}
void CReadFile::GetBuffer(wchar_t *buffer)
{
	assert(m_ptrFile&&buffer);
	long pos=ftell(m_ptrFile);
	std::wstring ws;
	CCodeCovert cc;
	if(GetFileCodeType()==ansi)
	{
		while(!feof(m_ptrFile))
		{
			char c=fgetc(m_ptrFile);
			wchar_t* wc=new wchar_t;
			wc=cc.CharToWchar(&c);
			ws.append(1,*wc);
			wc=NULL;
		}
	}
	else
	{
		fseek(m_ptrFile,2,SEEK_SET);
		while(!feof(m_ptrFile))
		{
			wchar_t wc;
			fread(&wc,1,2,m_ptrFile);
			ws.append(1,wc);
		}
	}
	memcpy(buffer,ws.c_str(),ws.size());	
	fseek(m_ptrFile,pos,SEEK_SET);
}
void CReadFile::GetBuffer(std::string& s)
{
	assert(m_ptrFile);
	long pos=ftell(m_ptrFile);// 记录文件指针位置
	rewind(m_ptrFile);
	if(GetFileCodeType()==ansi)//单字节文件
	{
		while(!feof(m_ptrFile))
		{
			char c=fgetc(m_ptrFile);
			s.append(1,c);
		}
	}
	else//多字节
	{
		if(ftell(m_ptrFile)==0)
			fseek(m_ptrFile,2,0);
		std::wstring ws;
		while(!feof(m_ptrFile))
		{
			wchar_t wc;
			fread(&wc,1,2,m_ptrFile);
			std::locale loc("chs");
		    std::wcout.imbue(loc);
			ws.append(1,wc);
		}
		//CCodeCovert::WstringToString(ws);
	}
	fseek(m_ptrFile,pos,SEEK_SET);//文件指针归位
}

bool CReadFile::SkipCodeType()
{
	EXCEPTION_INFO ei;
	bool bResult=false;
	try
	{
		if(NULL==m_ptrFile)
			THROW_ERROR(NULL_POINT_ERROR)
		rewind(m_ptrFile);//回到文件开始位置
		byte bCode[3]={0};
		if(Read(bCode,sizeof(byte),3))
		{
			if(bCode[0]==0xFF&&bCode[1]==0xFE)//unicode类型
			{
				m_ctType=unicode;
				fseek(m_ptrFile,-1,SEEK_CUR);//后退一个字节
			}
			else if(bCode[0]==0xEF&&bCode[1]==0xBB&&bCode[2]==0xBF)//utf_8
				m_ctType=utf8;
			else if(bCode[0]==0xFE&&bCode[1]==0xFF)//big unicode类型
			{
				m_ctType=big_unicode;
				fseek(m_ptrFile,-1,SEEK_CUR);//后退一个字节
			}
			else
			{
				m_ctType=ansi;//默认情况下是ANSI编码
				fseek(m_ptrFile,-3,SEEK_CUR);//后退三个字节
			}
		}
		else
			THROW_ERROR(UNKNOW_ERROR)
		bResult=true;
	}
	catch(...)
	{
		ei.tstrExceptionFunc=_T("SkipCodeType");
		WriteErrorLog(&ei);
		m_ctType=CODETYPE::unknow;
	}
	return bResult;
}
