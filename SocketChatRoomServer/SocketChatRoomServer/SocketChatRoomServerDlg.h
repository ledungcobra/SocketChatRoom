
// SocketChatRoomServerDlg.h : header file
//

#pragma once
#include "User.h"
#include <list>
#include <vector>
#include "TcpServer.h"
#define PORT 54000
#define WM_SOCKET WM_USER+1

// CSocketChatRoomServerDlg dialog
class CSocketChatRoomServerDlg : public CDialogEx
{
// Construction
public:
	CSocketChatRoomServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SOCKETCHATROOMSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CButton mBTNSWITCH;
	afx_msg void OnBnClickedSwitch();
	CListBox mListBox;

private:
	TcpServer *_server;
	bool _isRunning = false;
	



//public:
//	char* ConvertToChar(const CString& s);
//	void mSend(SOCKET sk, CString command);
//	int mRecv(SOCKET sk, CString& command);

//public:
//	SOCKET sockServer, sockCLient, flag, sclient;
//	struct Client {
//		SOCKET sockClient;
//		User user;
//	};
//	struct sockaddr_in serverAdd;
//	int msgType;
//	int buffLength, t, lenguser, flagsend, kq, count_sock;
//	int number_Socket;
//	int first_send;
//	std::vector<Client*> _listClient;
//
//	void TurnOnServer() {
//		//INIT socket
//		sockServer = socket(AF_INET, SOCK_STREAM, 0);
//		serverAdd.sin_family = AF_INET;
//		serverAdd.sin_port = htons(PORT);
//		serverAdd.sin_addr.s_addr = htonl(INADDR_ANY);
//		bind(sockServer, (SOCKADDR*)&serverAdd, sizeof(serverAdd));
//		listen(sockServer, 5);
//		int err = WSAAsyncSelect(sockServer, m_hWnd, WM_SOCKET,FD_READ|FD_ACCEPT|FD_CLOSE);
//		if (err)
//			MessageBox(_T("Cant call WSAAsyncSelect"));
//
//		number_Socket = 0;
//		
//
//
//	}
//	LRESULT SockMsg(WPARAM wParam, LPARAM lParam) {
//		if (WSAGETSELECTERROR(lParam)) {
//			closesocket(lParam);
//		}
//		switch (WSAGETSELECTEVENT(lParam)) {
//		case FD_ACCEPT: {
//			auto s = accept(wParam, NULL, NULL);
//			Client a;
//			a.sockClient = s;
//			_listClient.push_back(&a);
//			number_Socket = _listClient.size();
//			break;
//			}
//			
//
//		case FD_READ:
//			int post = -1, dpos = -1;
//
//			for (int i = 0; i < _listClient.size(); i++) {
//				if (_listClient[i]->sockClient == wParam) {
//					post = i;
//				}
//			}
//			CString temp;
//			if (mRecv(wParam, temp) < 0)
//				break;
//			_cwprintf(temp);
//
//		}
//	}

	
public:
	CEdit mEdtLog;
};
