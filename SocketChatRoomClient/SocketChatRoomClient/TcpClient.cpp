#include "pch.h"
#include "TcpClient.h"
#include "resource.h"
TcpClient* TcpClient::_instance = NULL;
TcpClient::TcpClient()
{
	this->_isRunning = false;
	this->_isActive = false;
	this->_serverPort = 54000;
	this->_serverIpaddress = "127.0.0.1";

	// Tạo winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2); // version 2,2

	int winsock_check = WSAStartup(ver, &data);

	if (winsock_check != 0) // check xem có lỗi không
	{
		std::cerr << "Can't start winsock ! Error # " << WSAGetLastError << std::endl;
	}
	this->_serverSocket = this->CreateSocket();
}
std::string TcpClient::ReceivePacket()
{
	//Tạo buffer
	char* buffer = new char[RAWSIZE];
	//char buffer[6144]; //Tĩnh
	ZeroMemory(buffer, RAWSIZE);

	//nhận tin nhắn
	int bytesin = recv(this->_serverSocket, buffer, RAWSIZE, 0);
	std::string packet = std::string(buffer, bytesin);
	delete[] buffer;
	return packet;

}
SOCKET TcpClient::CreateSocket()
{
	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_server != INVALID_SOCKET)	//Check xem cấp có bị lỗi không 
	{
		// Hoàn thiện cấu trúc _hint ( báo là mình dùng kiểu protocol nào, hiện đang dùng IPv4 )
		this->_hint.sin_family = AF_INET;
		this->_hint.sin_port = htons(this->_serverPort);
		inet_pton(AF_INET, this->_serverIpaddress.c_str(), &this->_hint.sin_addr);

		// Bind ipaddress và port vào socket
		int bind_check = bind(sock_server, (sockaddr*)&this->_hint, sizeof(this->_hint));
	}
	else
	{
		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
	}
	return sock_server;

}

void TcpClient::SendPacketRaw(std::string packet)
{
	if (_isRunning) {
		send(this->_serverSocket, packet.c_str(), (int)(packet.size() + 1), 0);
	}
	
}

bool TcpClient::AnalyzeAndProcess(std::string packet)
{

	if (packet.empty() == true || packet[0] == '\r')
	{
		return true;
	}
	//TODO:
	int pos = packet.find('\0');
	std::string flag_head_str = packet.substr(0, pos);
	int flag_num = stoi(flag_head_str);
	FlagServerToClient flag = static_cast<FlagServerToClient>(flag_num);

	
	switch (flag)
	{
	case FlagServerToClient::Send_Active_User:	{
		std::vector<std::string> listActiveUsers = stringTokenizer(packet, '\0');
		listActiveUsers = stringTokenizer(listActiveUsers[1], '|');
		if (_publicChatDialog) {

			_publicChatDialog->UpdateListActiveUsers(listActiveUsers);
			
		}
		else {
			AfxMessageBox(L"Couldn't find Public Dialog");
		}
	}
		break;

	case FlagServerToClient::Fail_Sign_Up:
		if(_signUpLogInDlg)
			_signUpLogInDlg->FailSignUp();
		else
		AfxMessageBox(L"Couldn't find Sign Up dialog");
		break;

	case FlagServerToClient::Fail_Login:
		if (_signUpLogInDlg)
			_signUpLogInDlg->FailLogin();
		else
			AfxMessageBox(L"Couldnt find Login Dialog");
		break;

	case FlagServerToClient::Login_Success:
		if (_signUpLogInDlg)
			//_signUpLogInDlg->LoginSuccess();
			SendMessage(_signUpLogInDlg->GetSafeHwnd(), LOGIN_SUCCESS_MSG, 0, 0);
		else
			AfxMessageBox(L"Couldn't find sign up login dialog");
		break;

	case FlagServerToClient::SignUp_Success:
		if (_signUpLogInDlg) {
			SendMessage(_signUpLogInDlg->GetSafeHwnd(), SIGNUP_SUCCESS_MSG,0,0);
			//CWnd
		}
		else
			AfxMessageBox(L"Couldn't find Sign Up login dialog");
		break;

	case FlagServerToClient::Send_File_Desc:
	{
		//TODO:testing
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		std::string backMess = std::to_string(static_cast<int>(FlagClientToServer::Download_Request)) + '\0' + info[1] + '\0' + info[2] + '\0';
		int num = std::stoi(info[3]);
		std::string size = std::to_string((num >> 20));
		
		CString notificationMessage = _T("Do you want to keep the filename: ") + ConvertString::ConvertStringToCString(info[2]) + _T(" from ") + ConvertString::ConvertStringToCString(info[1]) + _T(" size:") + ConvertString::ConvertStringToCString(size) + _T("MB");

		auto i = AfxMessageBox(notificationMessage, 1, 1);
		if (i == IDOK) {	
			this->SendPacketRaw(backMess);
		}
	}
		break;
	case FlagServerToClient::Send_File_Content:
	{
		//TODO:testing
		// tách thông tin file 
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		// lấy content
		packet.pop_back(); // bỏ '\0' được thêm vào từ send packet raw
		packet.pop_back(); // bỏ '\0' tương đương với null trong mẫu tin

		std::string fileContent = packet.substr(packet.length() - stoi(info[3]), stoi(info[3]));
		CFileDialog fileDlg(FALSE);
		fileDlg.DoModal();
		CString filePath = fileDlg.GetPathName();
		_cwprintf(filePath);
		std::ofstream file(filePath, std::ios::binary);
		if (file.is_open())
		{
			file << fileContent;
			file.close();
		}

	}
		break;
		//TODO:
	case FlagServerToClient::Already_Login:
		if (_signUpLogInDlg)
			_signUpLogInDlg->AccountAlreadyUsed();
			//_signUpLogInDlg->FailLogin();
		else
			AfxMessageBox(L"Couldnt find Login Dialog");
		break;

	case FlagServerToClient::Send_Private_Message:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		
		if (_mapPrivateChatDialog.find(info[1]) == _mapPrivateChatDialog.end()) {


			SendMessage(_publicChatDialog->GetSafeHwnd(), OPEN_PRIVATE_CHAT_DIALOG,(WPARAM)(LPCTSTR)ConvertString::ConvertStringToCString(info[2]), (LPARAM)(LPCTSTR)ConvertString::ConvertStringToCString(info[1]));
			
			//pPrivateChatDlg->UpdateChatView(info[2]);


		}
		else {
			//Tìm được cửa số người gửi 
			_mapPrivateChatDialog[info[1]]->UpdateChatView(info[2]);
			
		}
	//TODO: Hiện khung chat riêng và đẩy tin nhắn lên, info[1] là người gửi, info[2] là tin nhắn
	}
		break;
	case FlagServerToClient::Send_Public_Message:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		//TODO: Đẩy lên khung chat chung ( info[1] là tên người nhắn lên chat public, info[2] là nội dung)
		_publicChatDialog->UpdateMessage(info[1],info[2]);
	}
		break;
	case FlagServerToClient:: Close_All_Connection:
	{
		for (auto it = _mapPrivateChatDialog.begin(); it != _mapPrivateChatDialog.end(); it++) {
			delete it->second;
		}
		auto i = MessageBox(_publicChatDialog->GetSafeHwnd(), L"The server has shut down !!", L"Press OK  to return Sign Up Log In dialog",0);
		_publicChatDialog->OnBnClickedLogout();
		if (i == IDOK) {
			_signUpLogInDlg->ShowWindow(SW_SHOW);
		}
		

		this->CloseConnection();
		return false;
	}
	break;
	
	}

	return true;
}

bool TcpClient::Connect()
{

	if (connect(this->_serverSocket, (sockaddr*)&this->_hint, sizeof(this->_hint)) )
	{
		closesocket(this->_serverSocket); // Do la client nen truoc khi dong can don
		WSACleanup();
		return false;
	}
	return true;
}

void TcpClient::CloseConnection() // Close dùng trong analyze
{
	closesocket(this->_serverSocket);
	WSACleanup();
}

TcpClient* TcpClient::GetInstance()
{
	if (_instance == NULL) {
		_lock.acquire();
		if (_instance == NULL) {
			_instance = new TcpClient();
		}
		_lock.release();

	}
	return _instance;
}

struct Param {
	SOCKET serverSocket;
	TcpClient* tcpClient;
};
UINT ReceiveThreadFunc(LPVOID param) {

	TcpClient* pTcpClient = (TcpClient*)param;
	bool connect=true;
	while (pTcpClient->_isRunning && connect) {
		std::string packet = pTcpClient->ReceivePacket();
		connect = pTcpClient->AnalyzeAndProcess(packet);
	}
	return 0;


}
void TcpClient::Run()
{
	
	this->_isRunning = true;
	bool success = this->Connect();
	if (success) {

		AfxBeginThread(ReceiveThreadFunc, this);
	}
	else {
		AfxMessageBox(L"Connect to server fail");
		_isRunning = false;
	}
	
}

//CPublicChatDialog* TcpClient::GetPublicChatDialog()
//{
//	return this->_publicChatDialog;
//}
//
//CPrivateChatDialog* TcpClient::GetPrivateChatDialog()
//{
//	return this->_privateChatDialog;
//}
//
//CSignUpLogInDlg* TcpClient::GetSignUpLogInDlg()
//{
//	return this->_signUpLogInDlg;
//}

void TcpClient::SetDialog(CDialog* dialog)
 {
	const char* name = typeid(*dialog).name();
	if (strcmp(name, "class CPublicChatDialog") == 0) {
		
		_publicChatDialog = dynamic_cast<CPublicChatDialog*> (dialog);
	}
	else if (strcmp(name, "class CSignUpLogInDlg") == 0) {
		
		_signUpLogInDlg = dynamic_cast<CSignUpLogInDlg*> (dialog);

	}
	
}

void TcpClient::ShowSignUpLoginDialog()
{

	_signUpLogInDlg->ShowWindow(SW_SHOWNORMAL);

}

CPrivateChatDialog* TcpClient::CreatePrivateChatDlg(CString _partnerUsername)
{
	CPrivateChatDialog* dlg = nullptr;

	auto partner = ConvertString::ConvertCStringToString(_partnerUsername);
	if (_mapPrivateChatDialog.find(partner) == _mapPrivateChatDialog.end()) {
		dlg = new CPrivateChatDialog(nullptr, _partnerUsername);
		dlg->Create(ID_PRIVATE_CHAT);
		_mapPrivateChatDialog[partner] = dlg;
		

	}
	else {
		dlg = _mapPrivateChatDialog[partner];
	}
	return dlg;

}


std::vector<std::string> stringTokenizer(std::string input, char delim)
{
	std::vector <std::string> tokens;
	std::stringstream check(input);
	std::string intermediate;

	while (getline(check, intermediate, delim))
	{
		tokens.push_back(intermediate);
	}
	return tokens;
}

long long fileSize(std::string add)
{
	std::ifstream mySource;
	mySource.open(add, std::ios_base::binary);
	mySource.seekg(0, std::ios_base::end);
	long long size = mySource.tellg();
	mySource.close();
	return size;
}




