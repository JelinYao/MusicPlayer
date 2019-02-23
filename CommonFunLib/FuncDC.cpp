#include "StdAfx.h"
#include "GlobalFunction.h"
#include "FuncDC.h"
#include "CodeConvert.h"

/**********************************************************************/
/*                             线程函数                               */
/**********************************************************************/
//创建位图文件线程
DWORD WINAPI CreateBimtapThread(LPVOID lpParam)
{
	SAVEDATA* pSave=(SAVEDATA*)lpParam;
	BITMAP bmMetric;
	memset(&bmMetric,0,sizeof(bmMetric));
	::GetObject(pSave->hBitmap,sizeof(bmMetric),&bmMetric);
	//获取位图像素信息
	LONG bmSize=(bmMetric.bmWidth*bmMetric.bmBitsPixel+31)/8*bmMetric.bmHeight;
	BITMAPFILEHEADER fileHeader;//位图文件头
	memset(&fileHeader,0,sizeof(fileHeader));
	BITMAPINFOHEADER infoHeader;//位图信息头
	memset(&infoHeader,0,sizeof(infoHeader));
	//首先初始化位图信息头 
	infoHeader.biBitCount=32;
	infoHeader.biClrImportant=0;
	infoHeader.biClrUsed=0;
	infoHeader.biCompression=BI_RGB;
	infoHeader.biHeight=bmMetric.bmHeight;
	infoHeader.biWidth=bmMetric.bmWidth;
	infoHeader.biPlanes=1;//必须为1
	infoHeader.biSize=sizeof(BITMAPINFOHEADER);
	infoHeader.biSizeImage=0;//如果是BI_RGB，必须为0
	infoHeader.biXPelsPerMeter=0;
	infoHeader.biYPelsPerMeter=0;
	HGLOBAL hMem=::GlobalAlloc(GHND,bmSize);
	LPBITMAPINFOHEADER pData=(LPBITMAPINFOHEADER)::GlobalLock(hMem);
	*pData=infoHeader;
	//::GetBitmapBits(hBitmap,bmSize,pData);
	HDC hDC=::CreateCompatibleDC(NULL);
	::GetDIBits(hDC,pSave->hBitmap,0,bmMetric.bmHeight,pData,\
		(LPBITMAPINFO)pData,DIB_RGB_COLORS);
	::DeleteObject(hDC);
	//初始化文件信息头
	fileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	fileHeader.bfSize=bmSize;
	fileHeader.bfType=0x4D42;//必须是BM
	//写入新文件
	FILE* fp=NULL;
	fp=fopen(pSave->pSavepath,"wb+");
	fwrite(&fileHeader,sizeof(fileHeader),1,fp);
	fwrite(&infoHeader,sizeof(infoHeader),1,fp);
	fwrite(pData,bmSize,1,fp);
	fclose(fp);
	::GlobalUnlock(hMem);
	::GlobalFree(hMem);
	return 0;
}
HDC CFuncDC::m_hMemDC=NULL;
HBITMAP CFuncDC::m_hMemBitmap=NULL;
char* CFuncDC::m_pError=NULL;
CFuncDC::CFuncDC(void)
:CErrorLog(_T("CFuncDC"))
{
}

CFuncDC::~CFuncDC(void)
{
	Release();
	if(NULL!=m_pError)
	{
		delete[] m_pError;
		m_pError=NULL;
	}
}

void CFuncDC::Release()
{
	ReleaseDCHandle(&m_hMemDC);
	ReleaseGDIHandle((HANDLE*)&m_hMemBitmap);
}

HBITMAP CFuncDC::GetBackBitmap(IN HDC hDC )
{
	int nWidth=::GetDeviceCaps(hDC,HORZRES);
	int nHeight=::GetDeviceCaps(hDC,VERTRES); 
	return GetBackBitmap(hDC,nWidth,nHeight);
}

HBITMAP CFuncDC::GetBackBitmap( IN HDC hDC,IN const int nWidth,IN const int nHeight )
{
	HBITMAP hRetBitmap=NULL;
	try
	{
		if(NULL!=hDC)
		{
			Release();
			m_hMemDC=::CreateCompatibleDC(hDC);
			m_hMemBitmap=::CreateCompatibleBitmap(hDC,nWidth,nHeight);
			HBITMAP hOldBitmap=(HBITMAP)::SelectObject(m_hMemDC,m_hMemBitmap);
			//复制目标DC位图到内存DC
			::BitBlt(m_hMemDC,0,0,nWidth,nHeight,\
				hDC,0,0,SRCCOPY);
			hRetBitmap=(HBITMAP)::SelectObject(m_hMemDC,hOldBitmap);
		}
		else
			throw "传入参数：设备句柄为空！";
	}
	catch(char* pError)
	{
		std::cout<<pError<<std::endl;
	}
	return hRetBitmap;
}

bool CFuncDC::SaveHBitmap( HBITMAP hBitmap,const char* pSavePath )
{
	bool bRet=false;
	try
	{
		if((NULL!=hBitmap)&&(NULL!=pSavePath))
		{
			BITMAP bmMetric;
			memset(&bmMetric,0,sizeof(bmMetric));
			::GetObject(hBitmap,sizeof(bmMetric),&bmMetric);
			//获取位图像素信息
			LONG bmSize=(bmMetric.bmWidth*bmMetric.bmBitsPixel+31)/8*bmMetric.bmHeight;
			BITMAPFILEHEADER fileHeader;//位图文件头
			memset(&fileHeader,0,sizeof(fileHeader));
			BITMAPINFOHEADER infoHeader;//位图信息头
			memset(&infoHeader,0,sizeof(infoHeader));
			//首先初始化位图信息头 
			infoHeader.biBitCount=32;
			infoHeader.biClrImportant=0;
			infoHeader.biClrUsed=0;
			infoHeader.biCompression=BI_RGB;
			infoHeader.biHeight=bmMetric.bmHeight;
			infoHeader.biWidth=bmMetric.bmWidth;
			infoHeader.biPlanes=1;//必须为1
			infoHeader.biSize=sizeof(BITMAPINFOHEADER);
			infoHeader.biSizeImage=0;//如果是BI_RGB，必须为0
			infoHeader.biXPelsPerMeter=0;
			infoHeader.biYPelsPerMeter=0;
			HGLOBAL hMem=::GlobalAlloc(GPTR,bmSize);
			LPBITMAPINFOHEADER pData=(LPBITMAPINFOHEADER)::GlobalLock(hMem);
			*pData=infoHeader;
			//::GetBitmapBits(hBitmap,bmSize,pData);
			HDC hDC=::CreateCompatibleDC(NULL);
			::GetDIBits(hDC,hBitmap,0,bmMetric.bmHeight,pData,\
				(LPBITMAPINFO)pData,DIB_RGB_COLORS);
			::DeleteObject(hDC);
			//初始化文件信息头
			fileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
			fileHeader.bfSize=bmSize;
			fileHeader.bfType=0x4D42;//必须是BM
			//写入新文件
			FILE* fp=NULL;
			fp=fopen(pSavePath,"wb+");
			if(fp)
			{
				fwrite(&fileHeader,sizeof(fileHeader),1,fp);
				fwrite(&infoHeader,sizeof(infoHeader),1,fp);
				fwrite(pData,bmSize,1,fp);
				fclose(fp);
			}
			::GlobalUnlock(hMem);
			::GlobalFree(hMem);
		}
		else
			SetError("传入参数：保存路径指针为空！");
	}
	catch(...)
	{
		bRet=false;
		DWORD code=GetLastError();
	}
	//::DeleteObject(hBitmap);
	//hBitmap=NULL;
	return bRet;
}

bool CFuncDC::SaveHBitmap( HBITMAP hBitmap,const wchar_t* pSavePath )
{
	CCodeCovert cc;
	char* pPath=cc.WcharToChar(pSavePath);
	bool bRet=CFuncDC::SaveHBitmap(hBitmap,pPath);
	return bRet;
}

HBITMAP CFuncDC::CopyHBitmap( IN HBITMAP hBitmap )
 {
	HDC hSrcDC=NULL,hDesDC=NULL;
	HDC hDC=::CreateDC(_T("DISPLAY"),NULL,NULL,NULL);
	hSrcDC=::CreateCompatibleDC(hDC);
	hDesDC=::CreateCompatibleDC(hDC);
	BITMAP bm = {0};
	::GetObject(hBitmap, sizeof(bm), &bm);
	HBITMAP hRetBmp = ::CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);
	HBITMAP hSrcOld = (HBITMAP)::SelectObject(hSrcDC, hBitmap);
	HBITMAP hDesOld = (HBITMAP)::SelectObject(hDesDC, hRetBmp);
	::BitBlt(hDesDC,0, 0, bm.bmWidth, bm.bmHeight,hSrcDC, 0, 0, SRCCOPY );
	::SelectObject(hSrcDC, hSrcOld);
	hRetBmp=(HBITMAP)::SelectObject(hDesDC, hDesOld);
	ReleaseDCHandle(&hSrcDC);
	ReleaseDCHandle(&hDesDC);
	ReleaseDCHandle(&hDC);
	return hRetBmp;
}
void CFuncDC::SetError(const char* pError)
{
	if(NULL!=pError)
	{
		if(NULL!=m_pError)
		{
			delete[] m_pError;
			m_pError=NULL;
		}
		int nLen=strlen(pError);
		m_pError=new char[nLen+1];
		strcpy(m_pError,pError);
		m_pError[nLen]='\0';
	}
}
char* CFuncDC::GetError()
{
	return m_pError;
}

bool CFuncDC::SaveBackBitmap( IN HDC hDC,IN const char* pSavePath )
{
	bool bRet=false;
	HBITMAP hBitmap=GetBackBitmap(hDC);
	if(hBitmap)
	{
		bRet=SaveHBitmap(hBitmap,pSavePath);
		ReleaseGDIHandle((HANDLE*)&hBitmap);
	}
	return bRet;
}

bool CFuncDC::SaveBackBitmap( IN HDC hDC,IN const int nWidth,IN const int nHeight,IN const char* pSavePath )
{
	bool bRet=false;
	HBITMAP hBitmap=GetBackBitmap(hDC,nWidth,nHeight);
	if(hBitmap)
	{
		bRet=SaveHBitmap(hBitmap,pSavePath);
		ReleaseGDIHandle((HANDLE*)&hBitmap);
	}
	return bRet;
}
HBITMAP CFuncDC::CutHBitmap(IN HBITMAP hBitmap)
{
	HDC hDC=GetDC(NULL);
	HBITMAP hResultBitmap=NULL;
	return hResultBitmap;
}
