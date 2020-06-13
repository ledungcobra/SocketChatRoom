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
#pragma comment (lib,"ws2_32.lib")
class TcpClient
{
private:
	int _serverPort;
	std::string _serverIpaddress;
	sockaddr_in _hint;
public:
	bool _isActive;
	SOCKET _clientSocket;
	TcpClient();
	SOCKET CreateSocket();
	void SendPacketRaw(SOCKET serverSocket, std::string packet);
	bool AnalyzeAndProcess(SOCKET serverSocket, std::string packet);
	std::string ReceivePacket(SOCKET serverSocket);
	bool Connect();
	void CloseConnection();
};

