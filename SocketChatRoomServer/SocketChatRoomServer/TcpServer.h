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
#include "SocketChatRoomServerDlg.h"
#include "Lock.h"
#pragma comment (lib,"ws2_32.lib")
#define RAWSIZE 6000000


static CMutex containerLock;
static Lock _lock;
class TcpServer
{
private:

	int _port ;
	std::string _ipAddress;
	sockaddr_in _hint;
	std::vector<std::string> _container;

public:
	//TODO:
	SocketChatRoomServerDlg *_serverDlg;
	SOCKET _listeningSocket;
	bool _isRunning;
	std::map<SOCKET, std::string> _listUser; // Lưu đang on
	// Khởi tạo -> Run -> Listen -> receive -> analyzeAndProcess -> SendPacketRaw
	
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
	void SendToAllExcept(std::string packet,std::string exception);
	void UpdateUserList();
	void RemoveUserFromActiveList(SOCKET clientSocket);
	static std::map<SOCKET, bool> _flagRunningThread;
	static TcpServer* GetInstance();
	~TcpServer();
	void SetDialog(SocketChatRoomServerDlg* dlg);
	void refreshContainer();
	int getContainerSize();
	bool checkContainer();
private:
	static TcpServer* _instance;
	TcpServer();

};

std::vector<std::string> stringTokenizer(std::string input, char delim);
std::vector<std::string> stringTokenizer(std::string input, char delim, int limit);
std::istream& safeGetline(std::istream& is, std::string& t);