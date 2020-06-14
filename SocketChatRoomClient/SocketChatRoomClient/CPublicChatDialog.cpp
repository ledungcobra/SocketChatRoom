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

CPublicChatDialog::CPublicChatDialog(CWnd* pParent,CString username)
	: CDialogEx(IDD_PUBLIC_CHAT, pParent)
{
	this->_username = username;

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

void CPublicChatDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		auto i = AfxMessageBox(_T("Do you wanna disconnect to this server"), 1, 1);
		if (i == IDOK) {
			
			std::string packet = "";
			packet += std::to_string(static_cast<int>(FlagClientToServer::Disconnect_To_Server)) + '\0';
			TcpClient::GetInstance()->SendPacketRaw(packet);
			OnDestroy();

		}
		

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CPublicChatDialog::OnOK()
{

	//TODO: ENTER 

	AfxMessageBox(L"ENTER");

}


BEGIN_MESSAGE_MAP(CPublicChatDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON1, &CPublicChatDialog::OnBnClickedSend)
	ON_BN_CLICKED(IDC_BUTTON2, &CPublicChatDialog::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_BUTTON3, &CPublicChatDialog::OnBnClickedUploadFile)
	ON_LBN_SELCHANGE(IDD_LIST2, &CPublicChatDialog::OnLbnSelchangeListActiveUsers)
END_MESSAGE_MAP()


// CPublicChatDialog message handlers



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
		//if (ConvertString::ConvertCStringToString(_username) != listActiveUsers.at(i)) {
			mActiveUsersList.InsertString(i, ConvertString::ConvertStringToCString(listActiveUsers.at(i)));
		///}
	}
}


void CPublicChatDialog::OnBnClickedLogout()
{

	std::string packet = "";
	packet += std::to_string(static_cast<int>(FlagClientToServer::LogOut)) + '\0';
	TcpClient::GetInstance()->SendPacketRaw(packet);
	EndDialog(IDOK);
	TcpClient::GetInstance()->ShowSignUpLoginDialog();


}


void CPublicChatDialog::OnBnClickedUploadFile()
{
	CFileDialog fileDlg(TRUE);
	int check = fileDlg.DoModal();
	if (check == IDOK) {

		std::string filePath = ConvertString::ConvertCStringToString(fileDlg.GetPathName());
		_cwprintf(fileDlg.GetPathName());


	}
}


void CPublicChatDialog::OnLbnSelchangeListActiveUsers()
{
	auto index = mActiveUsersList.GetCurSel();
	if (index == -1) {
		return;
	}
	CString buff;
	mActiveUsersList.GetText(index, buff);
	CPrivateChatDialog* dlg = TcpClient::GetInstance()->CreatePrivateChatDlg(buff);
	dlg->ShowWindow(SW_SHOW);


}
