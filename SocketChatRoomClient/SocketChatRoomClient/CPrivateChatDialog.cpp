﻿// CPrivateChatDialog.cpp : implementation file
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
	OnDestroy();
}

void CPrivateChatDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, mMessageBox);
	DDX_Control(pDX, IDC_EDIT1, mEdtMessage);
}

BOOL CPrivateChatDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(_partnerUsername);
	return 0;
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
	if (check == IDOK)
	{
		CString filePath = fileDlg.GetPathName();
		_cwprintf(fileDlg.GetPathName());


		// đọc file lên
		std::ifstream file(filePath, std::ios::binary);
		if (file.is_open())
		{

			std::ostringstream ostrm;
			long long size = fileSize(filePath); // kich thuoc theo byte


			if (size > 5242880 || size < 1048576)
			{
				AfxMessageBox(L"File must be between 2 and 5 MB");
				return;
			}
			else
			{
				// ghi vào packet 

				ostrm << file.rdbuf();

				std::string content = std::string(ostrm.str());

				std::string username = ConvertString::EncodeCStringToString(this->_partnerUsername);

				// lấy tên file

				int curPos = 0;
				CString resToken = filePath.Tokenize(_T("\\"), curPos);
				while (filePath.Find(L"\\", curPos) != -1)
				{
					OutputDebugString(resToken);

					resToken = filePath.Tokenize(_T("\\"), curPos);
				}
				OutputDebugString(resToken);
				resToken = filePath.Tokenize(_T("\\"), curPos);
				std::string filename = ConvertString::EncodeCStringToString(resToken);

				// gửi đi desc
				std::string file_desc = std::to_string(static_cast<int>(FlagClientToServer::Send_File_Descriptor)) + '\0' + username + '\0' + filename + '\0' + std::to_string(size) + '\0'; // username + filename + filesize
				TcpClient::GetInstance()->SendPacketRaw(file_desc);
				// gửi đi content
				std::string file_content = std::to_string(static_cast<int>(FlagClientToServer::Send_Content)) + '\0' + username + '\0' + filename + '\0' + std::to_string(size) + '\0' + content + '\0'; // username + filename + filesize +content
				TcpClient::GetInstance()->SendPacketRaw(file_content);
				file.close();
			}
		}
	}
}



void CPrivateChatDialog::UpdateChatView(std::string incommingMessage)
{

	CString currentContent;
	mMessageBox.GetWindowTextW(currentContent);
	currentContent += _partnerUsername + L":" + ConvertString::DecodeStringToCString(incommingMessage) +L"\r\n";
	mMessageBox.SetWindowTextW(currentContent);


}


void CPrivateChatDialog::OnBnClickedSend()
{
	std::string packet;
	CString message;
	mEdtMessage.GetWindowTextW(message);
	if (message == L"") {
		return;
	}
	message = ConvertString::EmojiConverter(message);
	packet = std::to_string(static_cast<int>(FlagClientToServer::PrivateChat)) +
		'\0'+ConvertString::
		EncodeCStringToString(TcpClient::GetInstance()->username)+'\0'+ ConvertString::
		EncodeCStringToString(_partnerUsername)+'\0'+ ConvertString::
		EncodeCStringToString(message)+'\0';
	CString buff;
	mMessageBox.GetWindowTextW(buff);
	buff += TcpClient::GetInstance()->username + L": " + message+L"\r\n";
	mMessageBox.SetWindowTextW(buff);
	mEdtMessage.SetWindowTextW(L"");
	TcpClient::GetInstance()->SendPacketRaw(packet);
}


