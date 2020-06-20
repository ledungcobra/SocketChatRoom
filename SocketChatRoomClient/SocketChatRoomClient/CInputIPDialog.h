#pragma once


// CInputIPDialog dialog

class CInputIPDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CInputIPDialog)

public:
	CInputIPDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CInputIPDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_IP_DIALOG};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit mEdtIPAddress;

};
