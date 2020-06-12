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


	DDX_Control(pDX, IDD_LIST2, mActiveUsersList);
}




BOOL CPublicChatDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	mActiveUsersList.AddString(_T("1"));
	mActiveUsersList.AddString(_T("2dasjkhdjkasdhajksdh"));
	mActiveUsersList.AddString(_T("3dasjkhdjkasdhajksdh"));
	mActiveUsersList.AddString(_T("4dasjkhdjkasdhajksdh"));
	mActiveUsersList.AddString(_T("5dasjkhdjkasdhajksdh"));
	mActiveUsersList.AddString(_T("6dasjkhdjkasdhajksdh"));
	mActiveUsersList.AddString(_T("7321312"));
	mActiveUsersList.AddString(_T("8qeqwe1314524"));
	mActiveUsersList.AddString(_T("9qeqeqwe"));
	mActiveUsersList.AddString(_T("10eewewew"));
	mActiveUsersList.AddString(_T("11qewew"));
	mActiveUsersList.AddString(_T("12aaa"));
	mActiveUsersList.AddString(_T("13aaa"));
	mActiveUsersList.AddString(_T("15aaa"));
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
