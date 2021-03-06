#include "pch.h"
#include "TcpServer.h"
#include "ConvertString.h"
#include "HelperFunction.h"
TcpServer* TcpServer::_instance = 0;
std::map<SOCKET, bool> TcpServer::flagRunningThread = std::map<SOCKET, bool>(); 

TcpServer::TcpServer()
{
	this->_port = 54000;
	this->isRunning = false;
	this->_ipAddress = GetIPAddress();
	_cwprintf(ConvertString::ConvertStringToCString(_ipAddress));
	// Tạo winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2); // version 2,2

	int winsock_check = WSAStartup(ver, &data);

	if (winsock_check != 0) // check xem có lỗi không
	{
		std::cerr << "Can't start winsock ! Error # " << WSAGetLastError << std::endl;
	}
	
	this->listeningSocket = CreateSocket(); // Tạo socket listening

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
	// kiểm tra packet xem có trống hay không
	if (packet.empty() == true || packet[0] == '\r')
	{
		return true;
	}

	// lấy flag từ packet

	int pos = packet.find('\r');
	std::string flag_head_str = packet.substr(0,pos);
	int flag_num = stoi(flag_head_str);
	FlagClientToServer flag = static_cast<FlagClientToServer>(flag_num);

	// tách packet
	std::vector<std::string> info;
	info = stringTokenizer(packet, '\0');
	
	// ĐỪNG CÓ XÓA 
	std::string message = "";
	//

	switch (flag)
	{
	case FlagClientToServer::SignUp:
	{
		if (IsValid(info[1])) // kiểm tra xem tên có trong data hay không
		{
			// có thì báo đăng ký fail
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Fail_Sign_Up)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);

		}
		else // không thì cho đăng ký
		{
			// ghi thông tin file 
			WriteUserInfo(info[1], info[2]);

			// thông báo người dùng đã xuất hiện trong server
			message = info[1] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(" has signed up and logged in"));

			// báo về cho người dùng
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::SignUp_Success)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);

			// cập nhật UI server 

			this->listUser[clientSocket] = info[1];
			this->UpdateUserList();
			this->serverDlg->UpdateActiveUserListView();

			// Gửi cờ cập nhập log
			std::string Another_logIn = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogIn)) + '\0';
			Another_logIn += info[1];
			this->SendToAll(Another_logIn);
		}
	}
	break;
	case FlagClientToServer::Login:
	{
		if (IsExists(info[1], info[2])) // nếu đúng mật khẩu và tên
		{
			bool flag = 0; // đã có ai đăng nhập bằng tài khoản này chưa

			// kiểm tra xem tài khoản này đã có người đăng nhập trước đó
			for (auto it = this->listUser.begin(); it != this->listUser.end(); ++it)
				if (it->second == info[1])
				{
					std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Already_Login)) + '\0';
					this->SendPacketRaw(clientSocket, backMess);
					flag = 1; // đã có đăng nhập
					break;
				}

			// tài khoản hợp lệ và không có ai dùng tài khoản này để đăng nhập trước đó

			if (!flag) // nếu không có ai đã đăng nhập trước đó mà chưa log out thì có thể đăng nhập
			{
				// thông báo đã đăng nhập thành công 
				std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Login_Success)) + '\0';
				this->SendPacketRaw(clientSocket, backMess);

				// cho user đã đăng nhập vào list online
				this->listUser[clientSocket] = info[1];

				// thông báo đã đăng nhập
				message = info[1] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(" has logged in"));
				this->serverDlg->UpdateActiveUserListView();

				// cập nhật danh sách user
				this->UpdateUserList();

				// Gửi cờ để hiện log hoạt động bên client
				std::string Another_logIn = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogIn)) + '\0';
				Another_logIn += info[1];
				this->SendToAll(Another_logIn);
			}
		}
		else // không tồn tại tài khoản
		{
			// báo là đăng nhập thất bại
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Fail_Login)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);
		}
	}

	break;
	case FlagClientToServer::LogOut:
	{
		// thông báo user đã log out
		message = listUser[clientSocket] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(" has logged out"));

		// Gửi cờ để hiện log hoạt động bên client
		std::string Another_logOut = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogOut)) + '\0';
		Another_logOut += this->listUser[clientSocket] + '\0';
		this->listUser[clientSocket] = "";

		// cập nhật danh sách user

		this->serverDlg->UpdateActiveUserListView();
		this->UpdateUserList();

		//Gửi cờ cập nhật log

		this->SendToAll(Another_logOut);
	}
	break;

	case FlagClientToServer::Disconnect_To_Server:
	{
		if (listUser[clientSocket] != "")
		{
			message = listUser[clientSocket] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(" has disconnected"));
			//Gửi cờ cập nhật log
			std::string Another_logOut = std::to_string(static_cast<int>(FlagServerToClient::Another_Client_LogOut)) + '\0';
			Another_logOut += this->listUser[clientSocket];
			this->SendToAll(Another_logOut);
		}
		this->listUser.erase(clientSocket);
		this->serverDlg->UpdateActiveUserListView();

		//Handle thread 
		if (flagRunningThread.find(clientSocket) != flagRunningThread.end())
		{
			flagRunningThread.erase(clientSocket);
		}

		closesocket(clientSocket);
		this->UpdateUserList();
		this->serverDlg->UpdateLogBox(message);
		return false;

	}
	break;
	case FlagClientToServer::Download_Request:
	{
		CString fileName = ConvertString::DecodeStringToCString(info[1]) + "-" + ConvertString::DecodeStringToCString(info[2]) + "-" + ConvertString::ConvertStringToCString(info[3]) + DOWNLOADFORMAT;

		std::ifstream inFile(DOWNLOADDIR + fileName, std::ios::in | std::ios::binary);

		if (inFile.is_open())
		{
			// ghi vào content;
			std::ostringstream ostrm;
			ostrm << inFile.rdbuf();

			std::string fromFile(ostrm.str());

			// lấy dữ liệu ra từ file
			std::vector<std::string> content = stringTokenizer(fromFile, '\0');


			// lấy receiver ra khỏi content 

			std::string temp = fromFile;
			int pos = temp.find(content[1]);
			temp.erase(pos, content[1].length() + 1);

			// tạo gói tin gửi đi

			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Send_File_Content)) + '\0' + temp;
			SendPacketRaw(clientSocket, backMess);
			inFile.close();
		}
	}
	break;
	case FlagClientToServer::PrivateChat:
	{

		//info[2] là username người nhận, info[3] là nội dung tin nhắn
		message = listUser[clientSocket] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(" -> ")) + info[2]
			+ ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(": ")) + info[3];

		std::string private_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Private_Message)) + '\0'; //Gửi cờ 
		private_msg += this->listUser[clientSocket] + '\0' + info[3] + '\0';
		SOCKET receiver = NULL;

		for (auto it = this->listUser.begin(); it != this->listUser.end(); it++)
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

		//info[2] là nội dung tin nhắn
		//flagNULL sender NULL content NULL
		std::string extraMessage = " -> PUBLIC: ";
		message = listUser[clientSocket] + ConvertString::EncodeCStringToString(ConvertString::ConvertStringToCString(extraMessage)) + info[2];
		std::string public_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Public_Message)) + '\0'; //Gửi cờ 
		public_msg += this->listUser[clientSocket] + '\0' + info[2] + '\0';
		this->SendToAllExcept(public_msg,this->listUser[clientSocket]);
	}
	break;

	case FlagClientToServer::Send_File_Descriptor:
	{

		// lấy tên người gửi 

		std::string sender;

		for (auto it = this->listUser.begin(); it != this->listUser.end(); ++it)
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

			for (auto it = this->listUser.begin(); it != this->listUser.end(); ++it)
				if (it->second == info[1])
				{
					receiver = it->first;
					break;
				}
			SendPacketRaw(receiver, backMess);
		}
	}
	break;

	case FlagClientToServer::Send_Content:
	{
		// tách thông tin file

				// lấy content
		packet.pop_back(); // bỏ '\0' được thêm vào từ send packet raw
		packet.pop_back(); // bỏ '\0' tương đương với null trong mẫu tin
		int size = packet.length();

		std::string fileContent = packet.substr(packet.length() - stoi(info[3]), stoi(info[3]));

		// lấy tên người gửi 

		std::string sender;

		for (auto it = this->listUser.begin(); it != this->listUser.end(); ++it)
			if (it->first == clientSocket)
			{
				sender = it->second;
				break;
			}

		// tách thông tin

		std::string content = sender + '\0' + info[1] + '\0' + info[2] + '\0' + info[3] + '\0' + fileContent + '\0'; // sender + receiver + filename + file size + content 

		//// nhét content vào container
		//this->_container.push_back(content);
		//
		//// chạy thread đếm thời gian để refresh container
		//AfxBeginThread(Timer, this);

		CString fileName = ConvertString::DecodeStringToCString(sender) + "-" + ConvertString::DecodeStringToCString(info[2]) + "-" + ConvertString::ConvertStringToCString(info[3]) + DOWNLOADFORMAT;

		CreateDirectory(DOWNLOADDIR, NULL);

		std::ofstream outFile( DOWNLOADDIR + fileName, std::ios::binary | std::ios::out);

		if (outFile.is_open())
		{
			outFile << content;
			outFile.close();
		}
		break;
	}

	
	}
	this->serverDlg->UpdateLogBox(message);
	return true;
}

std::string TcpServer::ReceivePacket(SOCKET clientSocket)
{
	//Tạo buffer
	char* buffer = new char[RAWSIZE];
	std::string packet;
	try {
		ZeroMemory(buffer, RAWSIZE);

		//nhận tin nhắn
		int bytesin = recv(clientSocket, buffer, RAWSIZE, 0);
		packet = std::string(buffer, bytesin);
		delete[] buffer;
	}
	catch (...) {
		packet = "-1";
	}
	return packet;
}

bool TcpServer::Listen()
{
	int listen_check = listen(this->listeningSocket, SOMAXCONN); // SOMAXCONN là số connect tối đa có thể nghe được, là hằng số
	if (listen_check == SOCKET_ERROR) // check xem nghe có được không
	{
		std::cerr << "Can't listen to socket ! Error #" << WSAGetLastError() << std::endl; // Xuất ra báo lỗi
		return false;
	}
	return true;
	

}

void TcpServer::CloseServer()
{
	std::string close_server = std::to_string(static_cast<int>(FlagServerToClient::Close_All_Connection)) + '\0';
	this->SendToAll(close_server);
	closesocket(this->listeningSocket);
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

UINT ReceiveAndSend(LPVOID params)
{
	Param* p = (Param*)params;
	bool isConnect = true;
	MSG msg;
	
	if (p != NULL) {
		try {
			while (isConnect && TcpServer::GetInstance()->flagRunningThread[p->clientSocket]) {

				std::string packet = p->tcpServer->ReceivePacket(p->clientSocket);
				if (packet == "-1") {
					p->tcpServer->RemoveUserFromActiveList(p->clientSocket);
					isConnect = false;
					if (TcpServer::GetInstance()->flagRunningThread.find(p->clientSocket) != TcpServer::GetInstance()->flagRunningThread.end()) {
						TcpServer::GetInstance()->flagRunningThread.erase(p->clientSocket);
					}
					break;
				}
				isConnect = p->tcpServer->AnalyzeAndProcess(p->clientSocket, packet);

			}
			if (p != nullptr)
			{
				//TODO:
				delete p;
			}
		}
		catch (...) {
			AfxMessageBox(L"Error while receiving, the program will turn off in 2s");
			Sleep(2000);
			exit(0);
		}
	}
	return 0;


}


UINT ListeningThreadFunc(LPVOID serv) 
{
	TcpServer* server = (TcpServer*)serv;

	while (server->isRunning && TcpServer::GetInstance()->flagRunningThread[server->listeningSocket]) 
	{
		bool listen = server->Listen();

		if (listen ) {
			SOCKET clientSock = accept(server->listeningSocket, nullptr, nullptr);
			Param* param = new Param;
			param->tcpServer = server;
			param->clientSocket = clientSock;
			TcpServer::GetInstance()->flagRunningThread[clientSock] = true;
			AfxBeginThread(ReceiveAndSend, param); 
		}
	}

	return 0;
}


void TcpServer::Run()
{
	isRunning = true;
	flagRunningThread[this->listeningSocket] = true;
	AfxBeginThread(ListeningThreadFunc,this);
	
}

void TcpServer::WriteUserInfo(std::string username, std::string password)
{
	std::ofstream file(USERINFO, std::ios::app | std::ios::binary);
	if (file.is_open())
	{
		file << username << ";" << password << '\n';
		file.close();
	}
}

void TcpServer::SendToAll(std::string packet)
{
	for (auto it = this->listUser.begin(); it != this->listUser.end(); it++)
	{
		SendPacketRaw(it->first, packet);
	}
}

void TcpServer::SendToAllExcept(std::string packet, std::string exception)
{
	for (auto it = this->listUser.begin(); it != this->listUser.end(); it++)
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

	int count = 0;

	while (getline(check, intermediate, delim) && count < TOKENIZERLIMIT )
	{
		tokens.push_back(intermediate);
		++count;
	}
	return tokens;
}

std::vector<std::string> stringTokenizer(std::string input, char delim,bool flag )
{
	std::vector <std::string> tokens;
	std::stringstream check(input);
	std::string intermediate;

	int count = 0;

	while (getline(check, intermediate, delim))
	{
		tokens.push_back(intermediate);
		++count;
	}
	return tokens;
}

std::istream& safeGetline(std::istream& is, std::string& t)
{
	t.clear();

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
	for (auto it = this->listUser.begin(); it != this->listUser.end(); it++)
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

	this->listUser.erase(clientSocket);

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
	this->serverDlg= dlg;

}

