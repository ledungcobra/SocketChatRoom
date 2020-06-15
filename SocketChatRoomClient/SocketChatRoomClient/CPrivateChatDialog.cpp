// CPrivateChatDialog.cpp : implementation file
//

#include "pch.h"
#include "SocketChatRoomClient.h"
#include "CPrivateChatDialog.h"
#include "afxdialogex.h"
#include <string>
#include "ConvertString.h"
#include "FlagClientToServer.h"
#include "TcpClient.h"


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
	DDX_Control(pDX, IDC_EDIT2, mMessageBox);
	DDX_Control(pDX, IDC_EDIT1, mEdtMessage);
}


BEGIN_MESSAGE_MAP(CPrivateChatDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CPrivateChatDialog::OnBnClickedSendUploadFile)
	ON_BN_CLICKED(IDC_BUTTON1, &CPrivateChatDialog::OnBnClickedSend)
	
END_MESSAGE_MAP()


// CPrivateChatDialog message handlers


void CPrivateChatDialog::OnOK()
{
	this->OnBnClickedSend();
}

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
	mMessageBox.GetWindowTextW(currentContent);
	currentContent += _partnerUsername + L":" + ConvertString::ConvertStringToCString(incommingMessage) +L"\r\n";
	mMessageBox.SetWindowTextW(currentContent);

}


void CPrivateChatDialog::OnBnClickedSend()
{
	std::string packet;
	CString message;
	mEdtMessage.GetWindowTextW(message);
	packet = std::to_string(static_cast<int>(FlagClientToServer::PrivateChat)) +
		'\0'+ConvertString::
		ConvertCStringToString(TcpClient::GetInstance()->_username)+'\0'+ ConvertString::
		ConvertCStringToString(_partnerUsername)+'\0'+ ConvertString::
		ConvertCStringToString(message)+'\0';
	CString buff;
	mMessageBox.GetWindowTextW(buff);
	buff += TcpClient::GetInstance()->_username + L": " + message+L"\r\n";
	mMessageBox.SetWindowTextW(buff);
	mEdtMessage.SetWindowTextW(L"");
	TcpClient::GetInstance()->SendPacketRaw(packet);



}
