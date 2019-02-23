#pragma once

EXPORT_CLASS CChangeBackBase
{
public:
	CChangeBackBase(void);
	~CChangeBackBase(void);
	//»»·ô½Ó¿Ú
	virtual void ChangeBack()=0;
protected:
	//±³¾°
	HDC m_hBackDC;
	HBITMAP m_hBackBitmap;
	BOOL m_bChangeBack;
};
