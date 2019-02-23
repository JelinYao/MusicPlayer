// TransListBox.cpp : implementation file
//

#include "stdafx.h"
#include "TransListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransListBox

CTransListBox::CTransListBox()
:  m_ItemHeight(20),
   m_HasBackGround(FALSE),
   m_Color(RGB(0,0,0)),
   m_SelColor(RGB(255,0,0)),
   m_ShadowColor(RGB(0,0,0)),
   m_Shadow(FALSE),
   m_ShadowOffset(3)
{
   m_SelColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
   m_Color = GetSysColor(COLOR_WINDOWTEXT);
}

CTransListBox::~CTransListBox()
{
}


BEGIN_MESSAGE_MAP(CTransListBox, CListBox)
	//{{AFX_MSG_MAP(CTransListBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
    ON_WM_RBUTTONDOWN()
   	ON_COMMAND(ID_LISTBOX_32773, &CTransListBox::OnListboxAttribute)
	ON_COMMAND(ID_LISTBOX_32774, &CTransListBox::OnListboxDeleteItem)
	ON_COMMAND(ID_LISTBOX_32796, &CTransListBox::OnListboxLookLyrics)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransListBox message handlers

void CTransListBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CListBox::PreSubclassWindow();
	ModifyStyle(0,SS_NOTIFY|LBS_NOSEL);

   CWnd *pParent = GetParent();
   if (pParent)
   {
      CRect Rect;
      GetClientRect(&Rect);
      ClientToScreen(&Rect);
      pParent->ScreenToClient(&Rect);
      CDC *pDC = pParent->GetDC();
      m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
   }
}
void CTransListBox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
}

void CTransListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
   CDC *DC = GetDC();
   CRect Rect;
   Rect.SetRectEmpty();

   CFont *oldFont = DC->SelectObject(GetFont());
   DC->DrawText(_T("XYZ"),&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
   m_ItemHeight =lpMeasureItemStruct->itemHeight = static_cast<long>(Rect.Height());
   DC->SelectObject(oldFont);
   ReleaseDC(DC);
}


void CTransListBox::DrawItem(CDC &Dc,int Index,CRect &Rect,BOOL Selected)
{
   if (Index == LB_ERR || Index >= GetCount())
      return;

   CRect TheRect = Rect;
   Dc.SetBkMode(TRANSPARENT);

   CDC memdc;
   memdc.CreateCompatibleDC(&Dc);

   CFont *pFont = GetFont();
   CFont *oldFont = Dc.SelectObject(pFont);
   CBitmap *oldbmp = memdc.SelectObject(&m_Bmp);
   Dc.BitBlt(TheRect.left,TheRect.top,TheRect.Width(),TheRect.Height(),&memdc,TheRect.left,TheRect.top,SRCCOPY);
   CString Text;
   GetText(Index,Text);
   if (m_Shadow)
   {
      if (IsWindowEnabled())
      {
         Dc.SetTextColor(m_ShadowColor);
      }
      else
      {
         Dc.SetTextColor(RGB(255,255,255));
      }
      TheRect.OffsetRect(m_ShadowOffset,m_ShadowOffset);
      Dc.DrawText(Text,TheRect,DT_LEFT|DT_EXPANDTABS|DT_NOPREFIX);
      TheRect.OffsetRect(-m_ShadowOffset,-m_ShadowOffset);
   }

   if (IsWindowEnabled())
   {
      if (Selected)
      {
            Dc.SetTextColor(m_SelColor);
      }
      else
      {
            Dc.SetTextColor(m_Color);
      }
   }
   else
   {
      Dc.SetTextColor(RGB(140,140,140));
   }
   Dc.DrawText(Text,TheRect,DT_LEFT|DT_EXPANDTABS|DT_NOPREFIX);
   Dc.SelectObject(oldFont);
   memdc.SelectObject(oldbmp);
}
void CTransListBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	   CRect Rect;
   GetClientRect(&Rect);

   int Width = Rect.Width();
   int Height = Rect.Height();

   CDC MemDC;
   MemDC.CreateCompatibleDC(&dc);
   CBitmap MemBmp;
   MemBmp.CreateCompatibleBitmap(&dc,Width,Height);

   CBitmap *pOldMemBmp = MemDC.SelectObject(&MemBmp);

   CDC MemDC2;
   MemDC2.CreateCompatibleDC(&dc);
   CBitmap *pOldbmp = MemDC2.SelectObject(&m_Bmp);
   MemDC.BitBlt(0,0,Width,Height,&MemDC2,0,0,SRCCOPY);
   MemDC2.SelectObject(pOldbmp);


   Rect.top = 0;
   Rect.left = 0;
   Rect.bottom = Rect.top + GetItemHeight(0);
   Rect.right = Width;
   
   int size = GetCount();
   for (int i = GetTopIndex(); i < size && Rect.top <= Height;++i)
   {
      DrawItem(MemDC,i,Rect,GetSel(i));
      Rect.OffsetRect(0,GetItemHeight(i));
   }

   dc.BitBlt(0,0,Width,Height,&MemDC,0,0,SRCCOPY);

   MemDC.SelectObject(pOldMemBmp);
	// Do not call CListBox::OnPaint() for painting messages
}

BOOL CTransListBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	if (!m_HasBackGround)
   {
      CWnd *pParent = GetParent();
      if (pParent)
      {
         CRect Rect;
         GetClientRect(&Rect);
         ClientToScreen(&Rect);
         pParent->ScreenToClient(&Rect);
         CDC *pDC = pParent->GetDC();
         CDC memdc;
         memdc.CreateCompatibleDC(pDC);
         CBitmap *oldbmp = memdc.SelectObject(&m_Bmp);
         memdc.BitBlt(0,0,Rect.Width(),Rect.Height(),pDC,Rect.left,Rect.top,SRCCOPY);
         memdc.SelectObject(oldbmp);
         m_HasBackGround = TRUE;
         pParent->ReleaseDC(pDC);
      }
   }
   
   return TRUE;

}

void CTransListBox::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
   SetRedraw(FALSE);
   CListBox::OnVScroll(nSBCode,nPos,pScrollBar);
   SetRedraw(TRUE);

   RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
}

void CTransListBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	 RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
   //return FALSE;
}

void CTransListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
   SetRedraw(FALSE);
   CListBox::OnChar(nChar, nRepCnt, nFlags);
   SetRedraw(TRUE);
}

void CTransListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	 SetRedraw(FALSE);
	 CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
	 SetRedraw(TRUE);
}
void CTransListBox::SetFont(int nPointSize, CString FaceName,COLORREF Color,COLORREF SelColor,BOOL Shadow,int SOffset,COLORREF ShadowColor)
{
   m_Shadow = Shadow;
   m_ShadowColor = ShadowColor;
   m_ShadowOffset = SOffset;
   m_Color = Color;
   m_SelColor = SelColor;
   m_PointSize = nPointSize;
   CDC *DC = GetDC();
   m_Font.DeleteObject();
   m_Font.CreateFont(nPointSize,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,PROOF_QUALITY,DEFAULT_PITCH,FaceName);
   if (GetCount())
   {
      CRect Rect;
      Rect.SetRectEmpty();

      CFont *oldFont = DC->SelectObject(&m_Font);
      CString Text;
      GetText(0,Text);
      DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
      m_ItemHeight = static_cast<long>(Rect.Height());
      DC->SelectObject(oldFont);
   }
   ReleaseDC(DC);

}

void CTransListBox::SetColor(COLORREF Color,COLORREF SelColor,COLORREF ShadowColor)
{
   m_ShadowColor = ShadowColor;
   m_Color = Color;
   m_SelColor = SelColor;
}

void CTransListBox::ResetContent(BOOL bInvalidate)
{
    Default();
	if ( bInvalidate ) 
	{
		Invalidate();
		UpdateWindow();
	}
}


int CTransListBox::AddString(CString Text,DWORD ItemData,BOOL Enabled)
{
   if (!GetCount())
   {
      CDC *DC = GetDC();
      CRect Rect;
      Rect.SetRectEmpty();

      CFont *oldFont = DC->SelectObject(GetFont());
      DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
      m_ItemHeight = static_cast<long>(Rect.Height());
      DC->SelectObject(oldFont);
      ReleaseDC(DC);
   }
   
   int Index = CListBox::AddString(Text);
   CListBox::SetItemData(Index,ItemData);
   return Index;
}

int CTransListBox::InsertString(int Index,CString Text, DWORD ItemData,BOOL Enabled)
{
   if (!GetCount())
   {
      CDC *DC = GetDC();
      CRect Rect;
      Rect.SetRectEmpty();

      CFont *oldFont = DC->SelectObject(GetFont());
      DC->DrawText(Text,&Rect,DT_CALCRECT|DT_EXPANDTABS|DT_NOPREFIX);
      m_ItemHeight = static_cast<long>(Rect.Height());
      DC->SelectObject(oldFont);
      ReleaseDC(DC);
   }

   Index = CListBox::InsertString(Index,Text);
   CListBox::SetItemData(Index,ItemData);
   return Index;
}
CFont *CTransListBox::GetFont()
{
   if (m_Font.m_hObject == NULL)
   {
      return CListBox::GetFont();
   }

   return &m_Font;
}
void CTransListBox::OnMove(int x, int y)
{
   CListBox::OnMove(x, y);

   ShowWindow(SW_HIDE);
   m_HasBackGround = FALSE;
   Invalidate();
   UpdateWindow();
   ShowWindow(SW_SHOW);
   
}

void CTransListBox::OnSize(UINT nType, int cx, int cy)
{
   CListBox::OnSize(nType, cx, cy);

   CWnd *pParent = GetParent();
   if (pParent)
   {
      m_Bmp.DeleteObject();
      CRect Rect;
      GetClientRect(&Rect);
      ClientToScreen(&Rect);
      pParent->ScreenToClient(&Rect);
      CDC *pDC = pParent->GetDC();
      m_Bmp.CreateCompatibleBitmap(pDC,Rect.Width(),Rect.Height());
   }
   ShowWindow(SW_HIDE);
   m_HasBackGround = FALSE;
   Invalidate();
   UpdateWindow();
   ShowWindow(SW_SHOW);
}
int CTransListBox::ScrollUp(int Lines)
{
   int Index = GetTopIndex();
   if (Index-Lines < 0)
   {
      Index = Lines;
   }
   return SetTopIndex(Index-Lines);
}

int CTransListBox::ScrollDown(int Lines)
{
   int Index = GetTopIndex();
   if (Index+Lines > GetCount()-1)
   {
      Index = GetCount()-Lines;
   }
   return SetTopIndex(Index+Lines);
}
int CTransListBox::SetTopIndex(int Index)
{
   SetRedraw(FALSE);
   int Ret = CListBox::SetTopIndex(Index);
   SetRedraw(TRUE);
   RedrawWindow(0,0,RDW_FRAME|RDW_INVALIDATE|RDW_UPDATENOW);
   return Ret;
}
BOOL CTransListBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//SetRedraw(TRUE);
	Invalidate();
	return CListBox::OnMouseWheel(nFlags, zDelta, pt);
}

void CTransListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(this->GetCurSel()==LB_ERR)
		return;
	pAttributeDlg=new AttributeDlg();
	pAttributeDlg->Create(IDD_DIALOG1);
	CMenu menu,*pMenu;
	menu.LoadMenuW(IDR_MENU2);
	pMenu=menu.GetSubMenu(0);
	//ScreenToClient(&point);
	CPoint pt;
	GetCursorPos(&pt);
	pMenu->TrackPopupMenu(TPM_RIGHTBUTTON,pt.x,pt.y,this);
	pMenu=NULL;
	CListBox::OnRButtonDown(nFlags, point);
}
void CTransListBox::OnListboxAttribute()
{
	// TODO: 在此添加命令处理程序代码
	::SendMessage(pParent->GetSafeHwnd(),WM_ATTRIBUTE,0,0);
	pAttributeDlg->ShowWindow(SW_SHOW);

}

void CTransListBox::OnListboxDeleteItem()
{
	// TODO: 在此添加命令处理程序代码
	if(this->GetCurSel()==-1)//没有选中项
		return;
	if(pParent==NULL)
	{
		MessageBox(_T("句柄为空!"));
		return;
	}
	::SendMessage(pParent->GetSafeHwnd(),WM_LISTBOX_DELETEITEM,0,0);
}

// 获取父窗口指针

BOOL CTransListBox::GetParentPointer(void)
{
	pParent=this->GetParent();
	if(pParent==NULL)
		return FALSE;
	return TRUE;
}

void CTransListBox::OnListboxLookLyrics()
{
	// TODO: 在此添加命令处理程序代码
	::SendMessage(pParent->GetSafeHwnd(),WM_LOOK_LYRICS,0,0);
}