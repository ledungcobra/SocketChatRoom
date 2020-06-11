#pragma once


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
	virtual void OnDestroy();

private :
	CString _username;

};
