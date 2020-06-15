﻿#define _WIN32_WINNT _WIN32_WINNT_WIN10
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
#pragma comment (lib,"ws2_32.lib")


static Lock _lock;

class TcpClient
{
	
private:
	int _serverPort;
	std::string _serverIpaddress;
	sockaddr_in _hint;
	CPublicChatDialog* _publicChatDialog = nullptr;
	std::map<std::string,CPrivateChatDialog*> _mapPrivateChatDialog;
	CSignUpLogInDlg* _signUpLogInDlg = nullptr;

public:
	//TODO:
	bool _isActive;
	bool _isRunning;
	SOCKET _serverSocket;

	SOCKET CreateSocket();
	void SendPacketRaw( std::string packet);
	//TODO: Thay đổi _isRunning nếu ngắt kết nối
	bool AnalyzeAndProcess( std::string packet);
	std::string ReceivePacket();
	bool Connect();
	void CloseConnection();

    static TcpClient *GetInstance();
	void Run();
private:
    static TcpClient* _instance;
	TcpClient();

public:
	//CPublicChatDialog* GetPublicChatDialog();
	//CPrivateChatDialog* GetPrivateChatDialog();
	//CSignUpLogInDlg* GetSignUpLogInDlg();
	void SetDialog(CDialog* dialog);
	void ShowSignUpLoginDialog();

	CPrivateChatDialog* CreatePrivateChatDlg(CString _partnerUsername);
	



};


std::vector<std::string> stringTokenizer(std::string input, char delim);
