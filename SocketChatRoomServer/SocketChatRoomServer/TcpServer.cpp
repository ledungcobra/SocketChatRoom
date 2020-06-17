#include "pch.h"
#include "TcpServer.h"
#include "ConvertString.h"

TcpServer* TcpServer::_instance = 0;
std::map<SOCKET, bool> TcpServer::_flagRunningThread = std::map<SOCKET, bool>(); 
TcpServer::TcpServer()
{
	this->_port = 54000;
	this->_isRunning = false;
	this->_ipAddress = "10.0.130.251"; //TODO: sửa lại sau

	// Tạo winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2); // version 2,2

	int winsock_check = WSAStartup(ver, &data);

	if (winsock_check != 0) // check xem có lỗi không
	{
		std::cerr << "Can't start winsock ! Error # " << WSAGetLastError << std::endl;
	}
	
	this->_listeningSocket = CreateSocket(); // Tạo socket listening

}

SOCKET TcpServer::CreateSocket()
{

	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);
			
	if (sock_server != INVALID_SOCKET)	//Check xem cấp có bị lỗi không 
	{
		// Hoàn thiện cấu trúc _hint ( báo là mình dùng kiểu protocol nào, hiện đang dùng IPv4 )
		this->_hint.sin_family = AF_INET;
		this->_hint.sin_port = htons(this->_port);
		inet_pton(AF_INET, this->_ipAddress.c_str(), &this->_hint.sin_addr);
		
		// Bind ipaddress và port vào socket
		int bind_check = bind(sock_server, (sockaddr*)&this->_hint, sizeof(this->_hint));
	}
	else
	{
		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
	}
	return sock_server;

}

void TcpServer::SendPacketRaw(SOCKET clientSocket, std::string packet)
{
	send(clientSocket, packet.c_str(), (int)(packet.size() + 1), 0);
}

bool TcpServer::AnalyzeAndProcess(SOCKET clientSocket, std::string packet)
{
	if (packet.empty() == true || packet[0] == '\r')
	{
		return true;
	}
	int pos = packet.find('\r');
	std::string flag_head_str = packet.substr(0,pos);
	int flag_num = stoi(flag_head_str);
	FlagClientToServer flag = static_cast<FlagClientToServer>(flag_num);

	// biến tạm thời 


	std::string message = "";

	switch (flag)
	{
	case FlagClientToServer::SignUp:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet,'\0');
		if (IsValid(info[1]))
		{
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Fail_Sign_Up)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);
			
		}
		else
		{
			WriteUserInfo(info[1], info[2]);
			
			message = info[1] + " has signed up and logged in";
			
			
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::SignUp_Success)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);
			this->_listUser[clientSocket] = info[1];
			this->UpdateUserList();
			this->_serverDlg->UpdateActiveUserListView();
			// Gửi cờ cập nhập log
			std::string Another_logIn = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogIn)) + '\0';
			Another_logIn += info[1];
			this->SendToAll(Another_logIn);
		}
	}
		break;
	case FlagClientToServer::Login:
	{
		//TODO:
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		if (IsExists(info[1], info[2]))
		{
			bool flag = 0; // đã có ai đăng nhập bằng tài khoản này chưa

			// kiểm tra xem tài khoản này đã có người đăng nhập trước đó
			for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
				if (it->second == info[1])
				{
					std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Already_Login)) + '\0';
					this->SendPacketRaw(clientSocket, backMess);
					flag = 1; // đã có đăng nhập
					break;
				}

			// tài khoản hợp lệ và không có ai dùng tài khoản này để đăng nhập trước đó

			if (!flag)
			{
				std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Login_Success)) + '\0';
				this->SendPacketRaw(clientSocket, backMess);
				this->_listUser[clientSocket] = info[1];
				
				message = info[1] + " has logged in";
				this->_serverDlg->UpdateActiveUserListView();
				
				this->UpdateUserList();

				// Gửi cờ để hiện log hoạt động bên client
				std::string Another_logIn = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogIn))+'\0';
				Another_logIn += info[1];
				this->SendToAll(Another_logIn);
			}
		}
		else // không tồn tại tài khoản
		{
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Fail_Login)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);
		}
	}

	break;
	case FlagClientToServer::LogOut:
	{
		message = _listUser[clientSocket] + " has logged out";

		
		std::string Another_logOut = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogOut)) + '\0';
		Another_logOut += this->_listUser[clientSocket] + '\0';
		this->_listUser[clientSocket]="";
		
		this->_serverDlg->UpdateActiveUserListView();
		this->UpdateUserList();

		//Gửi cờ cập nhật log
		
		this->SendToAll(Another_logOut);
	}
		break;

	case FlagClientToServer::Disconnect_To_Server:
	{
		if (_listUser[clientSocket] != "") {

		
			message = _listUser[clientSocket] + " has disconnected";
			//Gửi cờ cập nhật log
			std::string Another_logOut = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogOut)) + '\0';
			Another_logOut += this->_listUser[clientSocket];
			this->SendToAll(Another_logOut);
			//
		}
		this->_listUser.erase(clientSocket);
		this->_serverDlg->UpdateActiveUserListView();

		//Handle thread 
		if (_flagRunningThread.find(clientSocket) != _flagRunningThread.end()) {
			_flagRunningThread.erase(clientSocket);
		}

		closesocket(clientSocket);
		this->UpdateUserList();
		this->_serverDlg->UpdateLogBox(message);
		return false;

	}
		break;

	case FlagClientToServer::Download_Request:
	{
		
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		for (int i = 0; i < this->container.size(); i++)
		{
			std::vector<std::string> content = stringTokenizer((this->container[i]), '\0');
			if (content[0] == info[1] && content[2] == info[2]) // sender - filename
			{
				// lấy receiver ra khỏi content 
				int pos = this->container[i].find(content[1]);
				this->container[i].erase(pos, content[1].length() + 1);

				// tạo gói tin gửi đi

				std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Send_File_Content)) + '\0' + this->container[i];
				SendPacketRaw(clientSocket, backMess);
				if (content[1] == "ALL")
				{
					// do nothing
				}
				else
				{
					this->container.erase(this->container.begin() + i);
				}
				break;
			}
		}
	}
		break;
	case FlagClientToServer::PrivateChat:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		//info[2] là username người nhận, info[3] là nội dung tin nhắn
		message = _listUser[clientSocket] + " -> " + info[2] + ": " + info[3];

		std::string private_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Private_Message)) + '\0'; //Gửi cờ 
		private_msg += this->_listUser[clientSocket] + '\0' + info[3] +'\0';
		SOCKET receiver;
		for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
		{
			if (it->second == info[2])
			{
				receiver = it->first;
			}
		}
		this->SendPacketRaw(receiver, private_msg);
	}
		break;

	case FlagClientToServer::PublicChat:
	{
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		//info[2] là nội dung tin nhắn
		//flagNULL sender NULL content NULL
		message = _listUser[clientSocket] + " -> " + "PUBLIC: " + info[2];
		std::string public_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Public_Message)) + '\0'; //Gửi cờ 
		public_msg += this->_listUser[clientSocket] + '\0' + info[2] + '\0';
		this->SendToAll(public_msg);
	}
		break;

	case FlagClientToServer::Send_File_Descriptor:
	{
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		// lấy tên người gửi 

		std::string sender;

		for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
			if (it->first == clientSocket)
			{
				sender = it->second;
				break;
			}


		// tách thông tin file

		std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Send_File_Desc)) + '\0' + sender + '\0' + info[2] + '\0' + info[3] + '\0'; // sender + file name + filesize




		// gửi thông tin của tin đi

		if (info[1] == "ALL")
		{
			SendToAllExcept(backMess, sender);
		}
		else
		{	
			// lấy thông tin socket từ username

			SOCKET receiver = NULL;

			for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
				if (it->second == info[1])
				{
					receiver = it->first;
					break;
				}

			if (receiver)
			{
				SendPacketRaw(receiver, backMess);
			}

		}
	}
		break;

	case FlagClientToServer::Send_Content:
	{
		// tách thông tin file
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		// lấy content
		packet.pop_back(); // bỏ '\0' được thêm vào từ send packet raw
		packet.pop_back(); // bỏ '\0' tương đương với null trong mẫu tin
		int size = packet.length();

		std::string fileContent = packet.substr(packet.length() - stoi(info[3]), stoi(info[3]));

		// lấy tên người gửi 

		std::string sender;

		for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
			if (it->first == clientSocket)
			{
				sender = it->second;
				break;
			}

		// tách thông tin

		std::string content = sender + '\0' + info[1] + '\0' + info[2] + '\0' + info[3] + '\0' + fileContent + '\0'; // sender + receiver + filename + file size + content 

		this->container.push_back(content);

	}
		break;
    }
	
	this->_serverDlg->UpdateLogBox(message);

	return true;
}

std::string TcpServer::ReceivePacket(SOCKET clientSocket)
{
	//Tạo buffer
	char* buffer = new char[RAWSIZE];
	//char buffer[6144]; //Tĩnh

	ZeroMemory(buffer, RAWSIZE);
	
	//nhận tin nhắn
	int bytesin = recv(clientSocket , buffer, RAWSIZE, 0);
	std::string packet = std::string(buffer, bytesin);
	delete[] buffer;
	return packet;
}

bool TcpServer::Listen()
{
	int listen_check = listen(this->_listeningSocket, SOMAXCONN); // SOMAXCONN là số connect tối đa có thể nghe được, là hằng số
	if (listen_check == SOCKET_ERROR) // check xem nghe có được không
	{
		std::cerr << "Can't listen to socket ! Error #" << WSAGetLastError() << std::endl; // Xuất ra báo lỗi
		return false;
	}
	return true;

	// SOCKET client = accept(this->_listeningSocket, nullptr, nullptr); TODO: bỏ vào hàm chạy thread
	

}

struct Param {
	SOCKET clientSocket;
	TcpServer* tcpServer;
};

void TcpServer::CloseServer()
{
	std::string close_server = std::to_string(static_cast<int>(FlagServerToClient::Close_All_Connection)) + '\0';
	this->SendToAll(close_server);
	closesocket(this->_listeningSocket);
	WSACleanup();
}

bool TcpServer::IsExists(std::string username, std::string password)
{
	std::ifstream file;
	file.open("data.bin", std::ifstream::binary);
	std::vector<std::string> usersInfo;
	if (file.is_open())
	{
		while (!file.eof())	
		{
			std::string buffer;
			safeGetline(file, buffer);
			usersInfo = stringTokenizer(buffer, ';');
			if (!usersInfo.empty())
			{
				if (usersInfo[0] == username && usersInfo[1] == password)
				{
					file.close();
					return true;
				}
			}
			else
			{
				return false;
			}
			usersInfo.clear();
		}
	}
	else
	{
		std::ofstream file("data.bin", std::ofstream::binary);
		file.close();
	}
	file.close();
	return false;
}
bool TcpServer::IsValid(std::string username)
{
	std::ifstream file;
	file.open("data.bin", std::ifstream::binary);
	std::vector<std::string> usersInfo;
	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string buffer;
			safeGetline(file, buffer);
			usersInfo = stringTokenizer(buffer, ';');
			if (!usersInfo.empty())
			{
				if (usersInfo[0] == username)
				{
					file.close();
					return true;
				}
			}
			else
			{
				return false;
			}
			usersInfo.clear();
		}
	}
	else
	{
		std::ofstream file("data.bin", std::ofstream::binary);
		file.close();
	}
	file.close();
	return false;
}

UINT ReceiveAndSend(LPVOID params) {
	Param* p = (Param*)params;
	bool isConnect = true;
	MSG msg;
	
	if (p != NULL) {
		while (isConnect && TcpServer::GetInstance()->_flagRunningThread[p->clientSocket]) {
			
			std::string packet = p->tcpServer->ReceivePacket(p->clientSocket);
			if (packet == "-1") {
				p->tcpServer->RemoveUserFromActiveList(p->clientSocket);
				isConnect = false;
				if (TcpServer::GetInstance()->_flagRunningThread.find(p->clientSocket) != TcpServer::GetInstance()->_flagRunningThread.end()) {
					TcpServer::GetInstance()->_flagRunningThread.erase(p->clientSocket);
				}
				break;

			}
			isConnect = p->tcpServer->AnalyzeAndProcess(p->clientSocket, packet);

		}
		if (p != nullptr) {
			//TODO:
			delete p;
		}

	}
	return 0;


}
UINT ListeningThreadFunc(LPVOID serv) {
	TcpServer* server = (TcpServer*)serv;

	while (server->_isRunning && TcpServer::GetInstance()->_flagRunningThread[server->_listeningSocket]) {
		bool listen = server->Listen();

		if (listen ) {
			SOCKET clientSock = accept(server->_listeningSocket, nullptr, nullptr);
			Param* param = new Param;
			param->tcpServer = server;
			param->clientSocket = clientSock;
			TcpServer::GetInstance()->_flagRunningThread[clientSock] = true;
			 AfxBeginThread(ReceiveAndSend, param);//TODO: Tat ket noi server
			
		}
	}

	return 0;
}

void TcpServer::Run()
{
	_isRunning = true;
	
	_flagRunningThread[this->_listeningSocket] = true;
	AfxBeginThread(ListeningThreadFunc,this);
}

void TcpServer::WriteUserInfo(std::string username, std::string password)
{
	std::ofstream file("data.bin", std::ios::app | std::ios::binary);
	if (file.is_open())
	{
		file << username << ";" << password << '\n';
		file.close();
	}
}

void TcpServer::SendToAll(std::string packet)
{
	for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
	{
		SendPacketRaw(it->first, packet);
	}
}

void TcpServer::SendToAllExcept(std::string packet, std::string exception)
{
	for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
	{
		if (it->second != exception)
		{
			SendPacketRaw(it->first, packet);
		}
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


std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

	// The characters in the stream are read one-by-one using a std::streambuf.
	// That is faster than reading them one-by-one using the std::istream.
	// Code that uses streambuf this way must be guarded by a sentry object.
	// The sentry object performs various tasks,
	// such as thread synchronization and updating the stream state.

	std::istream::sentry se(is, true);
	std::streambuf* sb = is.rdbuf();

	for (;;) {
		int c = sb->sbumpc();
		switch (c) {
		case '\n':
			return is;
		case '\r':
			if (sb->sgetc() == '\n')
				sb->sbumpc();
			return is;
		case std::streambuf::traits_type::eof():
			// Also handle the case when the last line has no line ending
			if (t.empty())
				is.setstate(std::ios::eofbit);
			return is;
		default:
			t += (char)c;
		}
	}
}

void TcpServer::UpdateUserList()
{
	std::string send_active_user = std::to_string(static_cast<int>(FlagServerToClient::Send_Active_User)) + '\0';
	for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
	{
		if (it->second != "")
		{
			send_active_user += it->second + '|';
		}
	}
	send_active_user.pop_back();
	send_active_user += '\0';
	this->SendToAll(send_active_user);
}

void TcpServer::RemoveUserFromActiveList(SOCKET clientSocket)
{

	this->_listUser.erase(clientSocket);

}

TcpServer* TcpServer::GetInstance()
{
	if (_instance == NULL) {
		_lock.acquire();
		if (_instance == NULL) {
			_instance = new TcpServer();
		}
		_lock.release();

	}
	return _instance;
}


TcpServer::~TcpServer()
{
	delete _instance;
	_instance = nullptr;
}

void TcpServer::SetDialog(SocketChatRoomServerDlg* dlg)
{
	this->_serverDlg = dlg;

}
