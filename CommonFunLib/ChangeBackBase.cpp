#include "StdAfx.h"
#include "ChangeBackBase.h"
#include "GlobalFunction.h"

CChangeBackBase::CChangeBackBase(void)
:m_hBackBitmap(NULL)
,m_hBackDC(NULL)
,m_bChangeBack(TRUE)
{
}

CChangeBackBase::~CChangeBackBase(void)
{
	ReleaseDCHandle(&m_hBackDC);
	ReleaseGDIHandle((HANDLE*)&m_hBackBitmap);
}
