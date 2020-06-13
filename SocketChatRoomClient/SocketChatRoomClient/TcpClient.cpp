#include "pch.h"
#include "TcpClient.h"

TcpClient::TcpClient()
{
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
	this->_clientSocket = this->CreateSocket();
}

SOCKET TcpClient::CreateSocket()
{
	SOCKET sock_client = socket(AF_INET, SOCK_STREAM, 0);

	if (sock_client != INVALID_SOCKET)	//Check xem cấp có bị lỗi không 
	{
		// Hoàn thiện cấu trúc _hint ( báo là mình dùng kiểu protocol nào, hiện đang dùng IPv4 )
		this->_hint.sin_family = AF_INET;
		this->_hint.sin_port = htons(this->_serverPort);
		inet_pton(AF_INET, this->_serverIpaddress.c_str(), &this->_hint.sin_addr);

		// Bind ipaddress và port vào socket
		int bind_check = bind(sock_client, (sockaddr*)&this->_hint, sizeof(this->_hint));
	}
	else
	{
		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
	}
	return sock_client;

}

void TcpClient::SendPacketRaw(SOCKET serverSocket, std::string packet)
{
	send(serverSocket, packet.c_str(), (int)(packet.size() + 1), 0);
}

bool TcpClient::AnalyzeAndProcess(SOCKET serverSocket, std::string packet)
{
	if (packet.empty() == true || packet[0] == '\r')
	{
		return true;
	}
	int pos = packet.find('\r');
	std::string flag_head_str = packet.substr(0, pos);
	int flag_num = stoi(flag_head_str);
	FlagServerToClient flag = static_cast<FlagServerToClient>(flag_num);

	std::string packet_send = "Hello there"; // TODO:Dùng chung
	switch (flag)
	{
	case FlagServerToClient::Send_Active_User:
		_cwprintf(L"da nhan sign up");
		SendPacketRaw(serverSocket, packet_send);
		break;

	case FlagServerToClient::Fail_Sign_Up:
		_cwprintf(L"da nhan login ");
		packet_send = "Hello there 2";
		SendPacketRaw(serverSocket, packet_send);
		break;

	case FlagServerToClient::Fail_Login:

		break;

	case FlagServerToClient::Login_Success:

		return false;
		break;

	case FlagServerToClient::SignUp_Success:

		break;

	case FlagServerToClient::Send_File_Desc:
		break;

	case FlagServerToClient::Send_File_Content:
		break;

	}

	return true;
}

std::string TcpClient::ReceivePacket(SOCKET serverSocket)
{
	//Tạo buffer
	char* buffer = new char[6144];
	//char buffer[6144]; //Tĩnh
	ZeroMemory(buffer, 6144);

	//nhận tin nhắn
	int bytesin = recv(serverSocket, buffer, 6144, 0);
	std::string packet = std::string(buffer, bytesin);
	delete[] buffer;
	return packet;

}

bool TcpClient::Connect()
{
	int connResult = connect(this->_clientSocket , (sockaddr*)&this->_hint, sizeof(this->_hint));
	if (connResult == SOCKET_ERROR)
	{
		closesocket(this->_clientSocket); // Do la client nen truoc khi dong can don
		WSACleanup();
		return false;
	}
	return true;
}

void TcpClient::CloseConnection() // Close dùng trong analyze
{
	closesocket(this->_clientSocket);
	WSACleanup();
}
