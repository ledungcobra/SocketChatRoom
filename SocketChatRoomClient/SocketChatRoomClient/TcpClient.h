#define _WIN32_WINNT _WIN32_WINNT_WIN10
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <map>
#include "FlagClientToServer.h"
#include "FlagServerToClient.h"
#include "Lock.h"
#pragma comment (lib,"ws2_32.lib")


static Lock _lock;

class TcpClient
{
private:
	int _serverPort;
	std::string _serverIpaddress;
	sockaddr_in _hint;
public:
	//TODO:
	bool _isActive;
	bool _isRunning;
	SOCKET _serverSocket;
	TcpClient();
	SOCKET CreateSocket();
	void SendPacketRaw(SOCKET serverSocket, std::string packet);
	bool AnalyzeAndProcess(SOCKET serverSocket, std::string packet);
	std::string ReceivePacket(SOCKET serverSocket);
	bool Connect();
	void CloseConnection();

    static TcpClient *GetInstance();
	void Run();
private:
    static TcpClient* _instance;


};

