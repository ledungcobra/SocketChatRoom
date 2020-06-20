
#if !defined(AFX_SCROLLEDIT_H__0E3C2DAA_0C6F_11D7_9EB2_00E029363140__INCLUDED_)
#define AFX_SCROLLEDIT_H__0E3C2DAA_0C6F_11D7_9EB2_00E029363140__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScrollEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScrollEdit window

class CScrollEdit : public CEdit
{
// Construction
public:
	CScrollEdit();

// Attributes
public:

// Operations
public:
	//Overloaded Function
	void SetWindowText(LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollEdit)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScrollEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScrollEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnCheckText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
	//
	void ShowHorizScrollBar(BOOL bShow=TRUE);
	void ShowVertScrollBar(BOOL bShow=TRUE);
	void CheckScrolling(LPCTSTR lpszString);

private:
	//Check Text Message
	static const UINT UWM_CHECKTEXT;
	//Keep the current values
	BOOL m_bShowHoriz;
	BOOL m_bShowVert;
};

/////////////////////////////////////////////////////////////////////////////

inline void CScrollEdit::ShowHorizScrollBar(BOOL bShow)
{
	if(m_bShowHoriz != bShow)
	{
		ShowScrollBar(SB_HORZ, bShow);
		m_bShowHoriz = bShow;
	}
}

inline void CScrollEdit::ShowVertScrollBar(BOOL bShow)
{
	if(m_bShowVert != bShow)
	{
		ShowScrollBar(SB_VERT, bShow);
		m_bShowVert = bShow;
	}
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCROLLEDIT_H__0E3C2DAA_0C6F_11D7_9EB2_00E029363140__INCLUDED_)

