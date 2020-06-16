#pragma once
#include<vector>
#include <string>
#define OPEN_PRIVATE_CHAT_DIALOG WM_APP+3
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
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnOK();
private :
	CString _username;


public:
	afx_msg LRESULT OpenDialog(WPARAM wParam, LPARAM lParam);
	CListBox mActiveUsersList;
	CEdit mEdtChat;
	afx_msg void OnBnClickedSend();
	void UpdateListActiveUsers(std::vector<std::string> listActiveUsers);
	afx_msg void OnBnClickedLogout();
	
	afx_msg void OnBnClickedUploadFile();
	void ReturnSignUpLoginDlg();

	afx_msg void OnLbnSelchangeListActiveUsers();
	void UpdateMessage(std::string partnerUsername, std::string content);
	CEdit mMessageBox;
};
