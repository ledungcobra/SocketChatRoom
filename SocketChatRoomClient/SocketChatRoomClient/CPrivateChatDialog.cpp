// CPrivateChatDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CPrivateChatDialog.h"
#include "afxdialogex.h"


// CPrivateChatDialog dialog

IMPLEMENT_DYNAMIC(CPrivateChatDialog, CDialogEx)

CPrivateChatDialog::CPrivateChatDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(ID_PRIVATE_CHAT, pParent)
{

}

CPrivateChatDialog::~CPrivateChatDialog()
{
}

void CPrivateChatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPrivateChatDialog, CDialogEx)
END_MESSAGE_MAP()


// CPrivateChatDialog message handlers
