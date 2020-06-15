
// SocketChatRoomServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CSocketChatRoomServerApp:
// See SocketChatRoomServer.cpp for the implementation of this class
//

class CSocketChatRoomServerApp : public CWinApp
{
public:
	CSocketChatRoomServerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButton1();
};

extern CSocketChatRoomServerApp theApp;
