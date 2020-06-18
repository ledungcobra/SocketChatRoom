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
#define TOKENIZERLIMIT 10
#define USERINFO "data.bin"
#define DOWNLOADFORMAT L".TCPD"
#define DOWNLOADTOCLEAR 10 

static Lock _lock; // khóa singleton


class TcpServer
{
private:

	int _port ; // số port
	std::string _ipAddress; // địa chỉ ip
	sockaddr_in _hint; // 

public:
	// biến public :)))
	SocketChatRoomServerDlg *serverDlg; 
	SOCKET listeningSocket;
	bool isRunning;
	std::map<SOCKET, std::string> listUser; // Lưu đang on
	static std::map<SOCKET, bool> flagRunningThread;

	// Khởi tạo -> Run -> Listen -> receive -> analyzeAndProcess -> SendPacketRaw
	
	SOCKET CreateSocket(); //Tạo socket nghe
	void SendPacketRaw(SOCKET clientSocket, std::string packet ); // gửi gói tin dạng thô
	bool AnalyzeAndProcess(SOCKET clientSocket,std::string packet); // phân tích và xử lí gói tin
	std::string ReceivePacket(SOCKET clientSocket); // nhân gói tin và trả về string
	bool Listen(); // Gọi receivePacket
	void CloseServer(); // đóng server
	bool IsExists(std::string username,std::string password); // kiểm tra tài khoản để đăng nhập
	bool IsValid(std::string username); // kiểm tra tên để đăng ký
	void Run(); // Goị listen
	void WriteUserInfo(std::string username, std::string password); // ghi thông tin user xuống file data
	void SendToAll(std::string packet); // gửi tới tất cả các client
	void SendToAllExcept(std::string packet,std::string exception); // gửi tất trừ ai đó :))
	void UpdateUserList(); // cập nhật danh sách user
	void RemoveUserFromActiveList(SOCKET clientSocket); // xóa user khỏi dang sách online
	static TcpServer* GetInstance(); // lấy thể hiện
	void SetDialog(SocketChatRoomServerDlg* dlg); // liên quan tới dialog 
	~TcpServer(); // destructor :))
private:
	static TcpServer* _instance;
	TcpServer(); // constructor

};

std::vector<std::string> stringTokenizer(std::string input, char delim); // string tokenizer 
std::istream& safeGetline(std::istream& is, std::string& t); // lấy chuổi từ file mà không dính \r \n v.v kiểu vậy

std::vector<std::string> stringTokenizer(std::string input, char delim, bool flag);
// thread
UINT ListeningThreadFunc(LPVOID serv); // nghe client kết nối
UINT ReceiveAndSend(LPVOID params); // nhận packet và xử lí


struct Param
{
	SOCKET clientSocket;
	TcpServer* tcpServer;
};