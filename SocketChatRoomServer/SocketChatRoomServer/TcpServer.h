#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <iostream>
#include <fstream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <map>
#include "FlagClientToServer.h"
#include "FlagServerToClient.h"
#pragma comment (lib,"ws2_32.lib")


//class TcpServer;
//
//typedef void (*MessageRecievedHandler)(TcpServer* listener, SOCKET socketID, std::string msg); // con trỏ hàm 
//
//class TcpServer
//{
//public:
//	std::string ipaddress;
//	int port_number;
//	sockaddr_in hint;
//	bool thread_running; // điều khiển thread
//	SOCKET listening; //socket để nghe tín hiệu
//	
//	fd_set master; //fd chứa các socket
//
//	MessageRecievedHandler MessageRecieved; 
//
//	//Tạo socket 
//	SOCKET CreateSocket();	
//
//public:
//	TcpServer(std::string IPaddress, int port_num, MessageRecievedHandler handler)
//	{
//		this->ipaddress = IPaddress;
//		this->port_number = port_num;
//		this->MessageRecieved = handler;
//		FD_ZERO(&master); // clean cho set trống
//		thread_running = false;
//	}
//
//	~TcpServer();
//
//	bool Init();
//
//	//Hàm để gửi message đến 1 client nhất định
//	bool Send(SOCKET socketclient, std::string message); 
//	//Hàm gửi tin nhắn đến tất cả client trừ 1 người gửi ra ( thường là trừ người gửi tin nhắn đó )
//	//Nếu dùng để gửi tin nhắn từ server đến tất cả client thì cho socketclient = NULL
//	void SendAllClientExcept(SOCKET socketclient, std::string message);
//	void Run();
//	
//};
//
//UINT Thread(LPVOID server);
//
//void MessageReceived(TcpServer* listen_server, SOCKET client, std::string msg);
//
class TcpServer
{
private:
	std::map<SOCKET, std::string> _listUser; // Lưu đang on
	int _port ;
	std::string _ipAddress;
	sockaddr_in _hint;

public:
	//TODO:
	SOCKET _listeningSocket;
	bool _isRunning;
	// Khởi tạo -> Run -> Listen -> receive -> analyzeAndProcess -> SendPacketRaw
	TcpServer();
	SOCKET CreateSocket(); //Tạo socket nghe
	void SendPacketRaw(SOCKET clientSocket, std::string packet );
	//TODO: Analyze gọi SendPacketRaw
	bool AnalyzeAndProcess(SOCKET clientSocket,std::string packet);
	std::string ReceivePacket(SOCKET clientSocket);
	bool Listen(); // Gọi receivePacket
	void CloseServer();
	bool IsExists(std::string username,std::string password); // kiểm tra tài khoản để đăng nhập
	bool IsValid(std::string username); // kiểm tra tên để đăng ký
	void Run(); // Goị listen
	void WriteUserInfo(std::string username, std::string password);
	void SendToAll(std::string packet);
	void UpdateUserList();
};

std::vector<std::string> stringTokenizer(std::string input, char delim);
std::istream& safeGetline(std::istream& is, std::string& t);