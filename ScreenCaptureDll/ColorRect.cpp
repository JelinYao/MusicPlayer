#include "stdafx.h"
#include "ColorRect.h"
#include "resource.h"
#include "CapDlg.h"
CColorRect::CColorRect()
{
	m_linePen.CreatePen(PS_SOLID,2,RGB(155,0,200));
}
CColorRect::~CColorRect()
{
	m_linePen.DeleteObject();
}
void CColorRect::Draw(CDC *pDC)
{
	CRect rect = m_rect;
	rect.NormalizeRect();

	CPen* pOldPen = NULL;
	CBrush* pOldBrush = NULL;
	CGdiObject* pTemp;
	int nOldROP;

	// draw lines
	if ((m_nStyle & (dottedLine|solidLine)) != 0)
	{
		pOldPen = pDC->SelectObject(&m_linePen);//(CPen::FromHandle(_afxBlackDottedPen));
		pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		nOldROP = pDC->SetROP2(R2_COPYPEN);
		rect.InflateRect(+1, +1);   // borders are one pixel outside
		pDC->Rectangle(rect.left, rect.top, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// if hatchBrush is going to be used, need to unrealize it
	if ((m_nStyle & (hatchInside|hatchedBorder)) != 0)
		UnrealizeObject(_afxHatchBrush);

	// hatch inside
	if ((m_nStyle & hatchInside) != 0)
	{
		pTemp = pDC->SelectStockObject(NULL_PEN);
		if (pOldPen == NULL)
			pOldPen = (CPen*)pTemp;
		pTemp = pDC->SelectObject(CBrush::FromHandle(_afxHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(TRANSPARENT);
		nOldROP = pDC->SetROP2(R2_MASKNOTPEN);
		pDC->Rectangle(rect.left+1, rect.top+1, rect.right, rect.bottom);
		pDC->SetROP2(nOldROP);
	}

	// draw hatched border
	if ((m_nStyle & hatchedBorder) != 0)
	{
		pTemp = pDC->SelectObject(CBrush::FromHandle(_afxHatchBrush));
		if (pOldBrush == NULL)
			pOldBrush = (CBrush*)pTemp;
		pDC->SetBkMode(OPAQUE);
		CRect rectTrue;
		GetTrueRect(&rectTrue);
		pDC->PatBlt(rectTrue.left, rectTrue.top, rectTrue.Width(),
			rect.top-rectTrue.top, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.bottom,
			rectTrue.Width(), rectTrue.bottom-rect.bottom, 0x000F0001 /* Pn */);
		pDC->PatBlt(rectTrue.left, rect.top, rect.left-rectTrue.left,
			rect.Height(), 0x000F0001 /* Pn */);
		pDC->PatBlt(rect.right, rect.top, rectTrue.right-rect.right,
			rect.Height(), 0x000F0001 /* Pn */);
	}

	// draw resize handles
	if ((m_nStyle & (resizeInside|resizeOutside)) != 0)
	{
		UINT mask = GetHandleMask();
		for (int i = 0; i < 8; ++i)
		{
			if (mask & (1<<i))
			{
				GetHandleRect((TrackerHit)i, &rect);
				pDC->FillSolidRect(rect, RGB(0, 200, 0));
			}
		}
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}
void CColorRect::DrawTrackerRect(LPCRECT lpRect,CWnd *pWndClipTo,CDC *pDC,CWnd *pWnd)
{
	TRACE(_T("LPRECT=%d,%d,%d,%d\n"),lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
	if(m_rect.left>m_rect.right)
		std::swap(m_rect.left,m_rect.right);
	if(m_rect.top>m_rect.bottom)
		std::swap(m_rect.top,m_rect.bottom);
	Draw(pDC);
	((CCapDlg*)pWnd)->Invalidate();
	((CCapDlg*)pWnd)->MoveToolDlg();
	//((CCapDlg*)pWnd)->MoveInfoDlg();
}