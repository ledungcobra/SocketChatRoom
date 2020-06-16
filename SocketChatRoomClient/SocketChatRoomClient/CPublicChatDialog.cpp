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
	DDX_Control(pDX, IDC_EDIT2, mMessageBox);
	DDX_Control(pDX, IDC_EDIT3, mEdtActiveLog);
}




BOOL CPublicChatDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(L"Public Chat");
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
	OnBnClickedSend();
}


BEGIN_MESSAGE_MAP(CPublicChatDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_BUTTON1, &CPublicChatDialog::OnBnClickedSend)
	ON_BN_CLICKED(IDC_BUTTON2, &CPublicChatDialog::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_BUTTON3, &CPublicChatDialog::OnBnClickedUploadFile)
	ON_LBN_SELCHANGE(IDD_LIST2, &CPublicChatDialog::OnLbnSelchangeListActiveUsers)
	ON_MESSAGE(OPEN_PRIVATE_CHAT_DIALOG,&CPublicChatDialog::OpenDialog)
END_MESSAGE_MAP()


// CPublicChatDialog message handlers



void CPublicChatDialog::OnBnClickedSend()
{
	CString message;
	mEdtChat.GetWindowTextW(message);

	std::string packet = std::to_string(static_cast<int>
	(FlagClientToServer::PublicChat)) + '\0' + ConvertString::ConvertCStringToString(_username) + '\0' +
		ConvertString::ConvertCStringToString(message) + '\0';
	_cwprintf(ConvertString::ConvertStringToCString(packet));

	TcpClient::GetInstance()->SendPacketRaw(packet);
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

		// đọc file lên
		std::ifstream file(filePath, std::ios::binary);
		if (file.is_open())
		{

			std::ostringstream ostrm;
			long long size = fileSize(filePath); // kich thuoc theo byte

			if (size > 5242880)
			{
				AfxMessageBox(L"File is too big");
				return;
			}
			else
			{
				// ghi vào packet

				ostrm << file.rdbuf();

				std::string content = std::string(ostrm.str());



				// lấy tên file
				std::vector<std::string> info = stringTokenizer(filePath, '\\');
				// gửi đi desc
				std::string file_desc = std::to_string(static_cast<int>(FlagClientToServer::Send_File_Descriptor)) + '\0' + "ALL" + '\0' + info[info.size() - 1] + '\0' + std::to_string(size) + '\0'; // all + filename + filesize
				TcpClient::GetInstance()->SendPacketRaw(file_desc);
				// gửi đi content
				std::string file_content = std::to_string(static_cast<int>(FlagClientToServer::Send_Content)) + '\0' + "ALL" + '\0' + info[info.size() - 1] + '\0' + std::to_string(size) + '\0' + content + '\0'; // all + filename + filesize +content
				TcpClient::GetInstance()->SendPacketRaw(file_content);
				file.close();
			}
		}
	}
}


void CPublicChatDialog::OnLbnSelchangeListActiveUsers()
{
	
	auto index = mActiveUsersList.GetCurSel();
	if (index == -1) {
		return;
	}
	CString selectedUser;
	mActiveUsersList.GetText(index, selectedUser);

	if (selectedUser == _username) {
		return;
	}
	CPrivateChatDialog* dlg = TcpClient::GetInstance()->CreatePrivateChatDlg(selectedUser);

	dlg->ShowWindow(SW_SHOW);
	mActiveUsersList.SetCurSel(-1);



}
LRESULT CPublicChatDialog::OpenDialog(WPARAM wParam, LPARAM lParam)
{

	LPCTSTR  username = (LPCTSTR)lParam;
	LPCTSTR message = (LPCTSTR)wParam;

	_cwprintf(username);
	auto dlg = TcpClient::GetInstance()->CreatePrivateChatDlg(username);
	dlg->ShowWindow(SW_SHOW);
	dlg->UpdateChatView(ConvertString::ConvertCStringToString(message));
	return 0;
}

void CPublicChatDialog::UpdateMessage(std::string partnerUsername, std::string content)
{
	CString messages;
	mMessageBox.GetWindowTextW(messages);
	messages += ConvertString::ConvertStringToCString(partnerUsername) + L": " +
		ConvertString::ConvertStringToCString(content) + L"\r\n";
	mMessageBox.SetWindowTextW(messages);
	mEdtChat.SetWindowTextW(L"");



}
