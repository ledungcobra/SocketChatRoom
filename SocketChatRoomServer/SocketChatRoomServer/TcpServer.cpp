﻿#include "pch.h"
#include "TcpServer.h"
#include "ConvertString.h"
//
//SOCKET TcpServer::CreateSocket()
//{
//	//Khởi tạo socket
//	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);
//	
//	if (sock_server != INVALID_SOCKET)	//Check xem cấp có bị lỗi không 
//	{
//		// Hoàn thiện cấu trúc hint ( báo là mình dùng kiểu protocol nào, hiện đang dùng IPv4 )
//		this->hint.sin_family = AF_INET;
//		this->hint.sin_port = htons(this->port_number);
//		inet_pton(AF_INET, this->ipaddress.c_str(), &this->hint.sin_addr);
//
//		// Bind ipaddress và port vào socket
//		int bind_check = bind(sock_server, (sockaddr*)&this->hint, sizeof(this->hint));
//
//		if (bind_check != SOCKET_ERROR) //check xem bind thành công không
//		{
//			// Báo winsock là socket đang đợi nghe tín hiệu
//			int listen_check = listen(sock_server, SOMAXCONN); // SOMAXCONN là số connect tối đa có thể nghe được, là hằng số
//			if (listen_check == SOCKET_ERROR) // check xem nghe có được không
//			{
//				std::cerr << "Can't listen to socket ! Error #" << WSAGetLastError() << std::endl; // Xuất ra báo lỗi
//				return SOCKET_ERROR;
//			}
//		}
//		else
//		{
//			std::cerr << "Can't bind IP and port to socket! Error #" << WSAGetLastError() << std::endl; // Xuất lỗi bind không được
//			return SOCKET_ERROR;
//		}
//	}
//	else
//	{
//		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
//		WSACleanup(); // Gọi hàm để huỷ socket trong fd
//	}
//
//	return sock_server;
//
//}
//
//TcpServer::~TcpServer() // destructor
//{
//	// Đóng hết tất cả socket
//	// Báo cho client biết là " dm bố đi ngủ "
//	std::string shutdown_msg = "Server is shutting down. GOODBYE ! \n";
//	while (master.fd_count > 0) // nghĩa là đang còn socket mở
//	{
//		// Lấy thông tin socket 
//		SOCKET sock = master.fd_array[0]; //0 là vì mỗi lần đóng thì dồn lên, tương tự như queue
//
//		//Gửi tin nhắn 
//		send(sock, shutdown_msg.c_str(), shutdown_msg.size() + 1, 0);
//
//		// Xoá socket đó khỏi fd và đóng hẳn socket đó 
//		FD_CLR(sock, &master);
//		closesocket(sock);
//	}
//
//	WSACleanup();
//	getchar();
//
//
//}
//
////Khởi tạo winsock
//bool TcpServer::Init()
//{
//	WSADATA data;
//	WORD ver = MAKEWORD(2, 2); // version 2,2
//
//	int winsock_check = WSAStartup(ver, &data);
//
//	if (winsock_check != 0) // check xem có lỗi không
//	{
//		std::cerr << "Can't start winsock ! Error # " << WSAGetLastError << std::endl;
//		return false;
//	}
//
//	return true;
//}
//
//bool TcpServer::Send(SOCKET socketclient, std::string message)
//{
//	if (!message.empty())
//	{
//		return send(socketclient, message.c_str(), message.size() + 1, 0);
//	}
//	return false;
//}
//
//void TcpServer::SendAllClientExcept(SOCKET socketclient, std::string message)
//{
//	if (!message.empty())
//	{
//		// Gửi tin nhắn thông qua các socket trong fd ( nhớ TRÁNH SOCKET DÙNG ĐỂ LISTEN )
//		for (int i = 0; i < this->master.fd_count; i++)
//		{
//			SOCKET sock = this->master.fd_array[i];
//			if ((sock != this->listening) && (sock != socketclient)) // Điều kiện tránh
//			{
//				send(sock, message.c_str(), message.size() + 1, 0);
//			}
//		}
//	}
//}
//
//void TcpServer::Run()
//{
//	//Tạo socket dùng để nghe
//	this->listening = this->CreateSocket();
//
//	if (listening == INVALID_SOCKET) // check
//	{
//		std::cerr << "Error in Run(), counld't start the server !"<< std::endl;
//		return;
//	}
//
//	// Cho socket nghe vào fd, cần thiết vì có nó mới nghe tín hiệu từ các client được
//	FD_SET(this->listening, &this-> master);
//
//	this->thread_running = true;
//	std::cout << "Server is running !" << std::endl;
//	
//	AfxBeginThread(Thread, this);
//
//
//}
//
//
//// Bất kì tin nhắn nào của client vào server đều gọi hàm này
//
//UINT Thread(LPVOID server)
//{
//	auto serv = (TcpServer*)server;
//	while (serv->thread_running == true)
//	{
//		fd_set copy = serv->master; // Cần tạo copy master vì hàm khi gọi thì hàm select sẽ huỷ luôn fd đó nên bước này quan trọng vl
//		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
//
//		// Vòng lặp duyệt qua các kết nối đã có / các kết nối đang đợi
//		for (int i = 0; i < socketCount; i++)
//		{
//			SOCKET sock = copy.fd_array[i]; // Dùng copy
//
//			if (sock == serv->listening)
//			{
//				// Cho client mới vào
//				SOCKET client = accept(serv->listening, nullptr, nullptr);
//
//				// Cho client mới vào danh sách các client đã kết nối
//				FD_SET(client, &serv->master);
//
//				// Gửi tin nhắn chào mừng client mới
//				std::string welcommessage = "WELCOME TO THE AWSOME CHAT ROOM ! \r\n";
//				send(client, welcommessage.c_str(), welcommessage.size() + 1, 0);
//
//			}
//
//			else // Xử lí tin nhắn yêu cầu của client 
//			{
//				// buffer
//				char buffer[4096];
//				ZeroMemory(buffer, 4096);
//
//				//Nhận tin nhắn
//				int bytesIn = recv(sock, buffer, 4096, 0);
//				std::string cmd = std::string(buffer, bytesIn);
//				// Cho client thoát
//				if (cmd == "quit -all\r\n")
//				{
//					closesocket(sock);
//					FD_CLR(sock, &serv->master);
//				}
//
//				//Thêm cái yêu càu khác phía dưới
//				//...
//				else
//				{
//					if (buffer[0] == '\\')
//					{
//						// command thoat
//						if (cmd == "\\\\quit\r\n")
//						{
//							serv->thread_running = false;
//							break;
//						}
//						continue;
//					}
//
//					if (serv->MessageRecieved != NULL)
//					{
//						serv->MessageRecieved(serv, sock, std::string(buffer, 0, bytesIn));
//
//					}
//				}
//			}
//		}
//	}
//
//
//	std::string msg = "Server is shutting down. Goodbye\r\n";
//
//	while (serv->master.fd_count > 0)
//	{
//		// Lấy số socket
//		SOCKET sock = serv->master.fd_array[0];
//
//		// Gửi tin nhắn tạm biệt
//		send(sock, msg.c_str(), (int)(msg.size() + 1), 0);
//
//		// Xoá gửi master và đóng
//		FD_CLR(sock, &serv->master);
//		closesocket(sock);
//	}
//
//	FD_CLR(serv->listening, &serv->master);
//	closesocket(serv->listening);
//	std::cout << "Server stopped !" << std::endl;
//	// Message to let users know what's happening.
//	return 0;
//
//}
//
//void MessageReceived(TcpServer* listen_server, SOCKET client, std::string msg)
//{
//	// Phần login thêm vào đây
//
//// Gửi tin nhắn đến tất cả client khác
//	std::string strOut;
//	strOut = strOut + "SOCKET #" + std::to_string(client) + ": " + msg + "\r";
//	listen_server->SendAllClientExcept(client, strOut);
//
//	//Đưa tin nhắn đó lên server
//	std::cout << "SOCKET " << client << ": " << msg << "\r";
//}
//
//

TcpServer::TcpServer()
{
	this->_port = 54000;
	this->_isRunning = false;
	this->_ipAddress = "127.0.0.1"; //TODO: sửa lại sau

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


	std::string packet_send = "Hello there"; // TODO:Dùng chung
	switch (flag)
	{
	case FlagClientToServer::SignUp:
	{
		_cwprintf(ConvertString::ConvertStringToCString(packet));
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
			std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::SignUp_Success)) + '\0';
			this->SendPacketRaw(clientSocket, backMess);
			this->_listUser[clientSocket] = info[1];
			this->UpdateUserList();
		}
	}
		break;
	case FlagClientToServer::Login:
	{
		//TODO:
		_cwprintf(ConvertString::ConvertStringToCString(packet));
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

				/*std::string send_active_user = std::to_string(static_cast<int>(FlagServerToClient::Send_Active_User)) + '\0';
				for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
				{
					send_active_user += it->second + '|';
				}
				send_active_user.pop_back();
				send_active_user += '\0';
				this->SendToAll(send_active_user);*/
				this->UpdateUserList();
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
		this->_listUser.erase(clientSocket);
		this->UpdateUserList();
	}
		break;

	case FlagClientToServer::Disconnect_To_Server:
		this->_listUser.erase(clientSocket);
		closesocket(clientSocket);
		this->UpdateUserList();
		return false;
		break;

	case FlagClientToServer::Download_Request:
	{
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');

		for (int i = 0; i < this->container.size(); i++)
		{
			std::vector<std::string> content = stringTokenizer((this->container[i]), '\0');
			if (content[0] == info[1] && content[2] == info[2])
			{
				std::string backMess = std::to_string(static_cast<int>(FlagServerToClient::Send_File_Desc)) + '\0' + info[1] + '\0' + info[2] + '\0' + content[3] + '\0'; // sender + file name + content
				SendPacketRaw(clientSocket, backMess);
				if (content[1] == "ALL")
				{

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
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		//info[1] là username người nhận, info[2] là nội dung tin nhắn
		std::string private_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Private_Message)) + '\0'; //Gửi cờ 
		private_msg += this->_listUser[clientSocket] + '\0' + info[2] +'\0';
		SOCKET receiver;
		for (auto it = this->_listUser.begin(); it != this->_listUser.end(); it++)
		{
			if (it->second == info[1])
			{
				receiver = it->first;
			}
		}
		this->SendPacketRaw(receiver, private_msg);
	}
		break;

	case FlagClientToServer::PublicChat:
	{
		_cwprintf(ConvertString::ConvertStringToCString(packet));
		std::vector<std::string> info;
		info = stringTokenizer(packet, '\0');
		//info[1] là nội dung tin nhắn
		std::string public_msg = std::to_string(static_cast<int>(FlagServerToClient::Send_Private_Message)) + '\0'; //Gửi cờ 
		public_msg += this->_listUser[clientSocket] + '\0' + info[1] + '\0';
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
			SendToAll(backMess);
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

		// lấy tên người gửi 

		std::string sender;

		for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
			if (it->first == clientSocket)
			{
				sender = it->second;
				break;
			}

		// tách thông tin

		std::string content = sender + '\0' + info[1] +'\0' + info[2] + '\0' + info[3] + '\0'; // sender + receiver + filename + content 

		this->container.push_back(content);

		//// gửi thông tin của tin đi

		//if (info[1] == "ALL")
		//{
		//	SendToAll(backMess);
		//}
		//else
		//{
		//	// lấy thông tin socket từ username

		//	SOCKET receiver = NULL;

		//	for (auto it = this->_listUser.begin(); it != this->_listUser.end(); ++it)
		//		if (it->second == info[1])
		//		{
		//			receiver = it->first;
		//			break;
		//		}

		//	if (receiver)
		//	{
		//		SendPacketRaw(receiver, backMess);
		//	}
		//}

	}
		break;
	}

	return true;
}

std::string TcpServer::ReceivePacket(SOCKET clientSocket)
{
	//Tạo buffer
	char* buffer = new char[6144];
	//char buffer[6144]; //Tĩnh
	ZeroMemory(buffer, 6144);
	
	//nhận tin nhắn
	int bytesin = recv(clientSocket , buffer, 6144, 0);
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
	if (p != NULL) {
		while (isConnect) {
			
			std::string packet = p->tcpServer->ReceivePacket(p->clientSocket);
			isConnect = p->tcpServer->AnalyzeAndProcess(p->clientSocket,packet);

		}

	}
	return 0;


}
UINT ListeningThreadFunc(LPVOID param) {
	TcpServer* server = (TcpServer*)param;
	while (server->_isRunning) {
		bool listen = server->Listen();

		if (listen) {
			SOCKET clientSock = accept(server->_listeningSocket, nullptr, nullptr);
			Param* param = new Param;
			param->tcpServer = server;
			param->clientSocket = clientSock;

			AfxBeginThread(ReceiveAndSend, param);//TODO:
		}
	}
	return 0;
}

void TcpServer::Run()
{
	_isRunning = true;
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
		send_active_user += it->second + '|';
	}
	send_active_user.pop_back();
	send_active_user += '\0';
	this->SendToAll(send_active_user);
}