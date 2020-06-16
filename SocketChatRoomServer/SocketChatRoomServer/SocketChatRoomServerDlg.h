
// SocketChatRoomServerDlg.h : header file
//

#pragma once
#include "User.h"
#include <list>
#include <vector>
#include <map>
#include <string>
//#define PORT 54000
//#define WM_SOCKET WM_USER+1
class SocketChatRoomServerDlg : public CDialogEx
{
// Construction
public:
	SocketChatRoomServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOCKETCHATROOMSERVER_DIALOG };
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
	DECLARE_MESSAGE_MAP()
public:
	CButton mBTNSWITCH;
	afx_msg void OnBnClickedSwitch();
	CListBox mListBox;
	void UpdateLogBox(std::string message);
	void UpdateActiveUserListView();
public:
	CEdit mEdtLog;
};
