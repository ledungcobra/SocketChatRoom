#pragma once

#include <string>
// CPrivateChatDialog dialog

class CPrivateChatDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPrivateChatDialog)

public:
	CPrivateChatDialog(CWnd* pParent,CString partnerUsername);   // standard constructor
	virtual ~CPrivateChatDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_PRIVATE_CHAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOK();
public:
	afx_msg void OnBnClickedSendUploadFile();
	
private:
	CString _partnerUsername;
public:
	void UpdateChatView(std::string incommingMessage);
	CEdit mMessageBox;
	
	afx_msg void OnBnClickedSend();
	CEdit mEdtMessage;
};
