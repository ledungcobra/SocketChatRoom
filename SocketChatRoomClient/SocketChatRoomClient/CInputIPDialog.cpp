// CInputIPDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CInputIPDialog.h"
#include "afxdialogex.h"
#include "TcpClient.h"
#include <regex>

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
	mEdtIPAddress.SetWindowTextW(L"127.0.0.1");
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

	std::regex pattern("\\d{1,4}\\.\\d{1,4}.\\d{1,4}.\\d{1,4}");
	if (!std::regex_match(ConvertString::ConvertCStringToString(buff), pattern)) {
		AfxMessageBox(L"Your ip is wrong");
		exit(0);
	}
	
	TcpClient::GetInstance()->SetIPAddress(ConvertString::ConvertCStringToString(buff));
	TcpClient::GetInstance()->Init();
	CSignUpLogInDlg* dialog = new CSignUpLogInDlg();
	ShowWindow(SW_HIDE);
	dialog->DoModal();

	//OnDestroy();
}
