
// SocketChatRoomClientDlg.h : header file
//
#pragma once
#include <iostream>
#include "CPublicChatDialog.h"
#include <memory>
#include "ScrollEdit.h"
#define SIGNUP_SUCCESS_MSG WM_APP+1
#define LOGIN_SUCCESS_MSG WM_APP+2


// CSocketChatRoomClientDlg dialog
class CSignUpLogInDlg : public CDialogEx
{
// Construction
public:
	CSignUpLogInDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_SIGNUP_DIALOG	};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOK();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSignUp();
	afx_msg void OnBnClickedLogIn();
	CScrollEdit mEdtUsername;
	CEdit mEdtPassword;
	void FailSignUp();
	void FailLogin();
	afx_msg LRESULT LoginSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT SignUpSuccess(WPARAM wParam, LPARAM lParam);
	void AccountAlreadyUsed();
	CMFCButton  mBtnLogIn;
};
