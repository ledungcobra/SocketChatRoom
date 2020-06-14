#pragma once


// CPrivateChatDialog dialog

class CPrivateChatDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPrivateChatDialog)

public:
	CPrivateChatDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPrivateChatDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_PRIVATE_CHAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSendUploadFile();
};
