// CPublicChatDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CPublicChatDialog.h"
#include "afxdialogex.h"
#include "TcpClient.h"
#include "ConvertString.h"


// CPublicChatDialog dialog

IMPLEMENT_DYNAMIC(CPublicChatDialog, CDialogEx)

CPublicChatDialog::CPublicChatDialog(CWnd* pParent)// ,CString username)
	: CDialogEx(IDD_PUBLIC_CHAT, pParent)
{
	//this->_username = username;

}

CPublicChatDialog::~CPublicChatDialog()
{
}

void CPublicChatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDD_LIST2, mActiveUsersList);
	DDX_Control(pDX, IDC_EDIT1, mEdtChat);
}




BOOL CPublicChatDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	return 0; 
}


BEGIN_MESSAGE_MAP(CPublicChatDialog, CDialogEx)
	ON_LBN_SELCHANGE(IDD_PUBLIC_CHAT, &CPublicChatDialog::OnLbnSelchangePublicChat)
	ON_BN_CLICKED(IDC_BUTTON1, &CPublicChatDialog::OnBnClickedSend)
END_MESSAGE_MAP()


// CPublicChatDialog message handlers


void CPublicChatDialog::OnLbnSelchangePublicChat()
{
	// TODO: Add your control notification handler code here
}


void CPublicChatDialog::OnBnClickedSend()
{
	CString message;
	mEdtChat.GetWindowTextW(message);
	TcpClient::GetInstance()->SendPacketRaw(ConvertString::ConvertCStringToString(message));
	//TODO: update local message


}

void CPublicChatDialog::UpdateListActiveUsers(std::vector<std::string> listActiveUsers)
{
	mActiveUsersList.ResetContent();

	for (int i = 0; i < listActiveUsers.size();i++) {
		mActiveUsersList.InsertString(i,ConvertString::ConvertStringToCString(listActiveUsers.at(i)));
	}
}
