#include "StdAfx.h"
#include "MemBase.h"
#include "GlobalFunction.h"

CMemBase::CMemBase(void)
:m_hMemDC(NULL)
,m_hMemBitmap(NULL)
{
}

CMemBase::~CMemBase(void)
{
	ReleaseDCHandle(&m_hMemDC);
	ReleaseGDIHandle((HANDLE*)&m_hMemBitmap);
}
