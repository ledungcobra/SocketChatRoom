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
	this->username = username;

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
	if (messageFont.CreatePointFont(150, L"Arial")) {
		mMessageBox.SetFont(&messageFont);
	}
	SetWindowText(L"Public Chat ");
	mMessageBox.ShowScrollBar(WS_VSCROLL);
	 mEdtActiveLog.ShowScrollBar(WS_VSCROLL);
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
			TcpClient::GetInstance()->isRunning = false;
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
	if (message == L"") {
		return;
	}
	message = ConvertString::EmojiConverter(message);
	std::string packet = std::to_string(static_cast<int>
	(FlagClientToServer::PublicChat)) + '\0' + ConvertString::EncodeCStringToString(username) + '\0' +
		ConvertString::EncodeCStringToString(message) + '\0';
	
	//UpdateUI
	CString buff;
	mMessageBox.GetWindowTextW(buff);
	buff += username + L": " +message+L"\r\n";
	mMessageBox.SetWindowTextW(buff);
	mEdtChat.SetWindowTextW(L"");
	

	TcpClient::GetInstance()->SendPacketRaw(packet);
	


}

void CPublicChatDialog::UpdateListActiveUsers(std::vector<std::string> listActiveUsers)
{
	mActiveUsersList.ResetContent();
	for (int i = 0; i < listActiveUsers.size();i++) {
		//if (ConvertString::EncodeCStringToString(username) != listActiveUsers.at(i)) {
			mActiveUsersList.InsertString(i, ConvertString::DecodeStringToCString(listActiveUsers.at(i)));
		///}
	}

}


void CPublicChatDialog::OnBnClickedLogout()
{

	
	std::string packet = "";
	packet += std::to_string(static_cast<int>(FlagClientToServer::LogOut)) + '\0';
	TcpClient::GetInstance()->SendPacketRaw(packet);
	for (auto it = TcpClient::GetInstance()->_mapPrivateChatDialog.begin(); it != TcpClient::GetInstance()->_mapPrivateChatDialog.end(); it++) {
		 it->second->ShowWindow(SW_HIDE);
	}
	EndDialog(IDOK);
	TcpClient::GetInstance()->ShowSignUpLoginDialog();


}

void CPublicChatDialog::ReturnSignUpLoginDlg()
{
	EndDialog(IDOK);
	TcpClient::GetInstance()->ShowSignUpLoginDialog();
}


void CPublicChatDialog::OnBnClickedUploadFile()
{
	CFileDialog fileDlg(TRUE);
	int check = fileDlg.DoModal();
	if (check == IDOK) {

		CString filePath = fileDlg.GetPathName();
		_cwprintf(fileDlg.GetPathName());


		// đọc file lên
		std::ifstream file(filePath, std::ios::binary);
		if (file.is_open())
		{

			std::ostringstream ostrm;
			long long size = fileSize(filePath); // kich thuoc theo byte

			if (size > 5242880 || size < 1048576)
			{
				AfxMessageBox(L"File must be between 2 and 5 MB");
				return;
			}
			else
			{

				// ghi vào packet

				ostrm << file.rdbuf();

				std::string content = std::string(ostrm.str());

				// lấy tên file

				int curPos = 0;
				CString resToken = filePath.Tokenize(_T("\\"), curPos);
				while (filePath.Find(L"\\", curPos) != -1)
				{
					OutputDebugString(resToken);

					resToken = filePath.Tokenize(_T("\\"), curPos);
				}
				OutputDebugString(resToken);
				resToken = filePath.Tokenize(_T("\\"), curPos);

				std::string filename = ConvertString::EncodeCStringToString(resToken);

				// gửi đi desc
				std::string file_desc = std::to_string(static_cast<int>(FlagClientToServer::Send_File_Descriptor)) + '\0' + "ALL" + '\0' + filename + '\0' + std::to_string(size) + '\0'; // all + filename + filesize
				TcpClient::GetInstance()->SendPacketRaw(file_desc);
				// gửi đi content
				std::string file_content = std::to_string(static_cast<int>(FlagClientToServer::Send_Content)) + '\0' + "ALL" + '\0' + filename + '\0' + std::to_string(size) + '\0' + content + '\0'; // all + filename + filesize +content
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

	if (selectedUser == username) {
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
	dlg->UpdateChatView(ConvertString::EncodeCStringToString(message));
	return 0;
}

void CPublicChatDialog::UpdateMessage(std::string partnerUsername, std::string content)
{
	CString messages;
	mMessageBox.GetWindowTextW(messages);
	messages += ConvertString::DecodeStringToCString(partnerUsername) + L": " +
		ConvertString::DecodeStringToCString(content) + L"\r\n";
	mMessageBox.SetWindowTextW(messages);
	mEdtChat.SetWindowTextW(L"");



}

void CPublicChatDialog::UpdateLogMessage(std::string message,int flag)
{
	CString buffer;

	if (flag == 0) {
		mEdtActiveLog.GetWindowTextW(buffer);
		buffer += ConvertString::DecodeStringToCString(message) + L" has logged in\r\n";
		

	}
	else if (flag == 1) {
		mEdtActiveLog.GetWindowTextW(buffer);
		buffer += ConvertString::DecodeStringToCString(message) + L" has logged out\r\n";
	}

	
}
