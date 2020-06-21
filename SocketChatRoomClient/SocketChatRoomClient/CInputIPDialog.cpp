// CInputIPDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CInputIPDialog.h"
#include "afxdialogex.h"
#include "TcpClient.h"
#include <regex>
#include "HelperFunction.h"

// CInputIPDialog dialog

IMPLEMENT_DYNAMIC(CInputIPDialog, CDialogEx)

CInputIPDialog::CInputIPDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INPUT_IP_DIALOG, pParent)
{

}

CInputIPDialog::~CInputIPDialog()
{
}

void CInputIPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mEdtIPAddress);
}

BOOL CInputIPDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	std::string ip = GetIPAddress();
	mEdtIPAddress.SetWindowTextW(ConvertString::ConvertStringToCString(ip));
	return 0;
}


BEGIN_MESSAGE_MAP(CInputIPDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CInputIPDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CInputIPDialog message handlers


void CInputIPDialog::OnBnClickedOk()
{
	CString buff;
	mEdtIPAddress.GetWindowTextW(buff);
	//TODO:CHECK

	std::regex pattern("\\b\\d{1,4}\\.\\d{1,4}.\\d{1,4}.\\d{1,4}\\b");
	if (!std::regex_match(ConvertString::ConvertCStringToString(buff), pattern)) {
		AfxMessageBox(L"Your ip is wrong");
		exit(0);
	}
	
	TcpClient::GetInstance()->SetIPAddress(ConvertString::ConvertCStringToString(buff));
	TcpClient::GetInstance()->Init();
	std::shared_ptr<CSignUpLogInDlg> dialog = std::make_shared<CSignUpLogInDlg>();
	ShowWindow(SW_HIDE);
	dialog->DoModal();
}


