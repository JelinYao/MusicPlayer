#pragma once

class CProgressSliderButton :
	public CSliderButton
{
public:
	CProgressSliderButton(void);
	~CProgressSliderButton(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
private:
	//当前歌曲的总时间（秒）
	int m_nTotalSecond;
public:
	int GetTotalSecond() const { return m_nTotalSecond; }
	void SetTotalSecond(const int val) { m_nTotalSecond = val; }
};
