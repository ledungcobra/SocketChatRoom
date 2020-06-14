// CPrivateChatDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CPrivateChatDialog.h"
#include "afxdialogex.h"
#include <string>
#include "ConvertString.h"


// CPrivateChatDialog dialog

IMPLEMENT_DYNAMIC(CPrivateChatDialog, CDialogEx)

CPrivateChatDialog::CPrivateChatDialog(CWnd* pParent, CString partnerUsername)
	: CDialogEx(ID_PRIVATE_CHAT, pParent)
{
	this->_partnerUsername = partnerUsername;

}

CPrivateChatDialog::~CPrivateChatDialog()
{
}

void CPrivateChatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, mEdtChatView);
}


BEGIN_MESSAGE_MAP(CPrivateChatDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CPrivateChatDialog::OnBnClickedSendUploadFile)
END_MESSAGE_MAP()


// CPrivateChatDialog message handlers


void CPrivateChatDialog::OnBnClickedSendUploadFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE);
	int check = fileDlg.DoModal();
	if (check == IDOK) {

		std::string filePath = ConvertString::ConvertCStringToString(fileDlg.GetPathName());
		_cwprintf(fileDlg.GetPathName());


	}
	
}

void CPrivateChatDialog::UpdateChatView(std::string incommingMessage)
{

	CString currentContent;
	mEdtChatView.GetWindowTextW(currentContent);
	currentContent += "\r\n";
	currentContent += _partnerUsername + L":" + ConvertString::ConvertStringToCString(incommingMessage);
	mEdtChatView.SetWindowTextW(currentContent);

}
