#pragma once
#include<vector>
#include <string>

// CPublicChatDialog dialog

class CPublicChatDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPublicChatDialog)

public:
	CPublicChatDialog(CWnd* pParent,CString username);   // standard constructor
	virtual ~CPublicChatDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PUBLIC_CHAT };
#endif


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	BOOL CPublicChatDialog::OnInitDialog();
private :
	CString _username;


public:
	
	afx_msg void OnLbnSelchangePublicChat();
	CListBox mActiveUsersList;
	CEdit mEdtChat;
	afx_msg void OnBnClickedSend();
	void UpdateListActiveUsers(std::vector<std::string> listActiveUsers);
};
