#include "pch.h"
#include "TcpClient.h"
#include "resource.h"
TcpClient* TcpClient::_instance = NULL;

TcpClient::TcpClient()
{
	this->isRunning = false;
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
	this->serverSocket = this->CreateSocket();
}

std::string TcpClient::ReceivePacket()
{
	//Tạo buffer
	char* buffer = new char[RAWSIZE];
	//char buffer[6144]; //Tĩnh
	ZeroMemory(buffer, RAWSIZE);

	//nhận tin nhắn
	int bytesin = recv(this->serverSocket, buffer, RAWSIZE, 0);
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
		this->_sockAddr.sin_family = AF_INET;
		this->_sockAddr.sin_port = htons(this->_serverPort);
		inet_pton(AF_INET, this->_serverIpaddress.c_str(), &this->_sockAddr.sin_addr);

		// Bind ipaddress và port vào socket
		int error = bind(sock_server, (sockaddr*)&this->_sockAddr, sizeof(this->_sockAddr));
		//TODO:FIX
		_cwprintf(L"%d", error);
		if (error != 0) {
			this->isRunning = true;
		}
		else {
			this->isRunning = false;
			AfxMessageBox(L"Cannot connect to server");
			exit(0);
		}
	}
	else
	{
		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
	}
	return sock_server;

}

void TcpClient::SendPacketRaw(std::string packet)
{
	if (isRunning) {
		send(this->serverSocket, packet.c_str(), (int)(packet.size() + 1), 0);
	}
	
}

bool TcpClient::AnalyzeAndProcess(std::string packet)
{
	if (packet.empty() == true || packet[0] == '\r')
	{
		return true;
	}
	// lấy flag 
	int pos = packet.find('\0');
	std::string flag_head_str = packet.substr(0, pos);
	int flag_num = stoi(flag_head_str);
	FlagServerToClient flag = static_cast<FlagServerToClient>(flag_num);

	
	switch (flag)
	{
	case FlagServerToClient::Send_Active_User:	
	{
		std::vector<std::string> listActiveUsers = stringTokenizer(packet, '\0');
		listActiveUsers = stringTokenizer(listActiveUsers[1], '|');
		if (_publicChatDialog) 
		{

			_publicChatDialog->UpdateListActiveUsers(listActiveUsers);
			
		}
		else 
		{
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
		if (_signUpLogInDlg) 
		{
			SendMessage(_signUpLogInDlg->GetSafeHwnd(), SIGNUP_SUCCESS_MSG,0,0);
			//CWnd
		}
		else
			AfxMessageBox(L"Couldn't find Sign Up login dialog");
		break;

	case FlagServerToClient::Send_File_Desc:
	{

		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		std::string backMess = std::to_string(static_cast<int>(FlagClientToServer::Download_Request)) + '\0' + info[1] + '\0' + info[2] + '\0' + info[3] + '\0';

		// xử kích thước file từ byte ra MB
		double num = std::stod(info[3]);

		num /= 1000000;

		std::ostringstream ostrm;
		ostrm << std::fixed;
		ostrm << std::setprecision(2);
		ostrm << num;

		std::string size = ostrm.str();

		// hiện thông báo

		CString notificationMessage = _T("Do you want to keep \"") + ConvertString::DecodeStringToCString(info[2]) + _T("\" from: ") + ConvertString::DecodeStringToCString(info[1]) + _T(" size: ") + ConvertString::ConvertStringToCString(size) + _T(" MB");

		auto i = AfxMessageBox(notificationMessage, 1, 1);
		if (i == IDOK)
		{
			CFileDialog fileDlg(FALSE, 0, ConvertString::DecodeStringToCString(info[2]));
			fileDlg.DoModal();
			CString filePath = fileDlg.GetPathName();

			this->_filePath = filePath;
			this->SendPacketRaw(backMess);
		}
	}
		break;
	case FlagServerToClient::Send_File_Content:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0', 5);
		std::string fileContent;

		// lấy content
		packet.pop_back(); // bỏ '\0' được thêm vào từ send packet raw
		packet.pop_back(); // bỏ '\0' tương đương với null trong mẫu tin

		fileContent = packet.substr(packet.length() - stoi(info[3]), stoi(info[3]));

		std::ofstream file(this->_filePath, std::ios::binary);
		if (file.is_open())
		{
			file << fileContent;
			file.close();
		}

	}
		break;
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


			SendMessage(_publicChatDialog->GetSafeHwnd(), OPEN_PRIVATE_CHAT_DIALOG,(WPARAM)(LPCTSTR)ConvertString::DecodeStringToCString(info[2]), (LPARAM)(LPCTSTR)ConvertString::DecodeStringToCString(info[1]));


		}
		else {
			//Tìm được cửa số người gửi 
			_mapPrivateChatDialog[info[1]]->UpdateChatView(info[2]);
			_mapPrivateChatDialog[info[1]]->ShowWindow(SW_SHOW);
			
		}
	
	}
		break;
	case FlagServerToClient::Send_Public_Message:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		_publicChatDialog->UpdateMessage(info[1],info[2]);
	}
		break;
	case FlagServerToClient:: Close_All_Connection:
	{
		for (auto it = _mapPrivateChatDialog.begin(); it != _mapPrivateChatDialog.end(); it++) {
			it->second->~CPrivateChatDialog();
		}
		auto i = MessageBox(_publicChatDialog->GetSafeHwnd(), L"The server has shut down !!", L"Press OK  to return Sign Up Log In dialog",0);
		_publicChatDialog->ReturnSignUpLoginDlg();
		if (i == IDOK) {
			_signUpLogInDlg->ShowWindow(SW_SHOW);
		}
		

		this->CloseConnection();
		return false;
	}
	break;
	case FlagServerToClient::Another_Client_LogIn:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		_publicChatDialog->UpdateLogMessage(info[1],0);
	}
	break;
	case FlagServerToClient::Another_Client_LogOut:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		_publicChatDialog->UpdateLogMessage(info[1],1);
	}
	}

	return true;
}

bool TcpClient::Connect()
{

	if (connect(this->serverSocket, (sockaddr*)&this->_sockAddr, sizeof(this->_sockAddr)) )
	{
		closesocket(this->serverSocket); // Do la client nen truoc khi dong can don
		WSACleanup();
		return false;
	}
	return true;
}

void TcpClient::CloseConnection() // Close dùng trong analyze
{
	closesocket(this->serverSocket);
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
	while (pTcpClient->isRunning && connect) {
		std::string packet = pTcpClient->ReceivePacket();
		connect = pTcpClient->AnalyzeAndProcess(packet);
	}
	return 0;


}

void TcpClient::Run()
{
	
	this->isRunning = true;
	bool success = this->Connect();	
	AfxBeginThread(ReceiveThreadFunc, this);

	
}


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
	std::shared_ptr<CPrivateChatDialog> dlg;

	auto partner = ConvertString::EncodeCStringToString(_partnerUsername);
	if (_mapPrivateChatDialog.find(partner) == _mapPrivateChatDialog.end()) {
		dlg = std::shared_ptr<CPrivateChatDialog>(new CPrivateChatDialog(nullptr, _partnerUsername));
		dlg->Create(ID_PRIVATE_CHAT);
		_mapPrivateChatDialog[partner] = dlg;
		

	}
	else {
		dlg = _mapPrivateChatDialog[partner];
	}
	return dlg.get();

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

std::vector<std::string> stringTokenizer(std::string input, char delim, int lim)
{

	std::vector <std::string> tokens;
	std::stringstream check(input);
	std::string intermediate;

	int count = 0;

	while (getline(check, intermediate, delim) && count < lim)
	{
		tokens.push_back(intermediate);
		++count;
	}
	return tokens;
}

long long fileSize(CString add)
{
	std::ifstream mySource;
	mySource.open(add, std::ios_base::binary);
	mySource.seekg(0, std::ios_base::end);
	long long size = mySource.tellg();
	mySource.close();
	return size;
}




