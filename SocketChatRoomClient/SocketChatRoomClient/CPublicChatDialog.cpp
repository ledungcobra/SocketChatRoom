// CPublicChatDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CPublicChatDialog.h"
#include "afxdialogex.h"


// CPublicChatDialog dialog

IMPLEMENT_DYNAMIC(CPublicChatDialog, CDialogEx)

CPublicChatDialog::CPublicChatDialog(CWnd* pParent,CString username)
	: CDialogEx(IDD_PUBLIC_CHAT, pParent)
{
	this->_username = username;

}

CPublicChatDialog::~CPublicChatDialog()
{
}

void CPublicChatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Control(pDX, IDD_LIST2, mActiveUsersList2);
}

void CPublicChatDialog::OnDestroy()
{
	_cwprintf(_T("avcxasdasd"));
}

BOOL CPublicChatDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	mActiveUsersList2.AddString(_T("1"));
	mActiveUsersList2.AddString(_T("2dasjkhdjkasdhajksdh"));
	mActiveUsersList2.AddString(_T("3dasjkhdjkasdhajksdh"));
	mActiveUsersList2.AddString(_T("4dasjkhdjkasdhajksdh"));
	mActiveUsersList2.AddString(_T("5dasjkhdjkasdhajksdh"));
	mActiveUsersList2.AddString(_T("6dasjkhdjkasdhajksdh"));
	mActiveUsersList2.AddString(_T("7321312"));
	mActiveUsersList2.AddString(_T("8qeqwe1314524"));
	mActiveUsersList2.AddString(_T("9qeqeqwe"));
	mActiveUsersList2.AddString(_T("10eewewew"));
	mActiveUsersList2.AddString(_T("11qewew"));
	mActiveUsersList2.AddString(_T("12aaa"));
	mActiveUsersList2.AddString(_T("13aaa"));
	mActiveUsersList2.AddString(_T("15aaa"));
	return 0; 
}


BEGIN_MESSAGE_MAP(CPublicChatDialog, CDialogEx)
	ON_LBN_SELCHANGE(IDD_PUBLIC_CHAT, &CPublicChatDialog::OnLbnSelchangePublicChat)
END_MESSAGE_MAP()


// CPublicChatDialog message handlers


void CPublicChatDialog::OnLbnSelchangePublicChat()
{
	// TODO: Add your control notification handler code here
}
