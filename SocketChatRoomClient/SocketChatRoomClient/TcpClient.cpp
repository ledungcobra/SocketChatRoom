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
			_signUpLogInDlg->LoginSuccess();
		else
			AfxMessageBox(L"Couldn't find sign up login dialog");

		//TODO:
		return false;
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
		
		break;

	case FlagServerToClient::Send_File_Content:

		break;

	}

	return true;
}

std::string TcpClient::ReceivePacket()
{
	//Tạo buffer
	char* buffer = new char[6144];
	//char buffer[6144]; //Tĩnh
	ZeroMemory(buffer, 6144);

	//nhận tin nhắn
	int bytesin = recv(this->_serverSocket, buffer, 6144, 0);
	std::string packet = std::string(buffer, bytesin);
	delete[] buffer;
	return packet;

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

	while (pTcpClient->_isRunning) {
		std::string packet = pTcpClient->ReceivePacket();
		pTcpClient->AnalyzeAndProcess(packet);
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
	else if (strcmp(name, "class CPrivateChatDialog") == 0) {

		_privateChatDialog = dynamic_cast<CPrivateChatDialog*> (dialog);

	}
	
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



