#pragma once

class CVoiceSliderButton :
	public CSliderButton
{
public:
	CVoiceSliderButton(void);
	~CVoiceSliderButton(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
