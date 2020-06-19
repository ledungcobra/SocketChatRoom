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
#include "CPrivateChatDialog.h"
#include "CPublicChatDialog.h"
#include "CSignUpLogInDlg.h"
#include <fstream>
#include <memory>
#include "ConvertString.h"
#include <map>
#include <iomanip>
#pragma comment (lib,"ws2_32.lib")
#define RAWSIZE 6000000
static Lock _lock;

class TcpClient
{
private:
	int _serverPort;
	std::string _serverIpaddress;
	sockaddr_in _sockAddr;
	CPublicChatDialog* _publicChatDialog = nullptr;
	CSignUpLogInDlg* _signUpLogInDlg = nullptr;
	static std::shared_ptr<TcpClient> _instance;
	TcpClient();
	// dùng cho đọc và ghi file
	CString _filePath = L"";
public:
	std::map<std::string, std::shared_ptr<CPrivateChatDialog> > _mapPrivateChatDialog; //
	bool isRunning; // kiểm tra có đang chạy hay không
	SOCKET serverSocket; // socket client 
	CString username; // tên của client 
	SOCKET CreateSocket(); // tạo socket 
	void SendPacketRaw( std::string packet); // gửi gói tin thô
	bool AnalyzeAndProcess( std::string packet); // phân tích và xử lí
	std::string ReceivePacket(); // nhận gói tin 
	bool Connect(); // kết nối với server
	void CloseConnection(); // đóng kết nối
    static std::shared_ptr<TcpClient> GetInstance(); // lấy thể hiện
	void Run(); // chạy client
	void SetDialog(CDialog* dialog); //
	void ShowSignUpLoginDialog(); //
	CPrivateChatDialog* CreatePrivateChatDlg(CString partnerUsername);
	void SetIPAddress(std::string ipaddr);
	void Init();
	~TcpClient();
};


std::vector<std::string> stringTokenizer(std::string input, char delim);
std::vector<std::string> stringTokenizer(std::string input, char delim, int lim);
long long fileSize(CString add);


//thread
UINT ReceiveThreadFunc(LPVOID param);