#include "StdAfx.h"
#include "VoiceSliderButton.h"

CVoiceSliderButton::CVoiceSliderButton(void)
{
}

CVoiceSliderButton::~CVoiceSliderButton(void)
{
}
BEGIN_MESSAGE_MAP(CVoiceSliderButton, CSliderButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CVoiceSliderButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString strTipText=_T("");
	int nVoice=(int)(point.x/(float)m_rcClient.Width()*100)+10;
	strTipText.Format(_T("音量：%d%%"),nVoice);
	SetToolTipText(strTipText);
	//CSliderButton::OnMouseMove(nFlags, point);
}

void CVoiceSliderButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int nCurVoice=point.x/(float)m_rcClient.Width()*100;
	::SendMessage(::GetParent(m_hWnd),WM_SLIDER_SETVOICE,nCurVoice,0);
	CSliderButton::OnLButtonDown(nFlags, point);
}
