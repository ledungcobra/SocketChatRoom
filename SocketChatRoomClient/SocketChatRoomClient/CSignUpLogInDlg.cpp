
// SocketChatRoomClientDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SocketChatRoomClient.h"
#include "CSignUpLogInDlg.h"
#include "afxdialogex.h"
#include "CPublicChatDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "TcpClient.h"
#include "ConvertString.h"


// CAboutDlg dialog used for App About

//class CAboutDlg : public CDialogEx
//{
//public:
//	CAboutDlg();
//
//// Dialog Data
//#ifdef AFX_DESIGN_TIME
//	enum { IDD = IDD_ABOUTBOX };
//#endif
//
//	protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//
//// Implementation
//protected:
//	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnBnClickedLogin();
//};
//
//CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
//{
//}
//
//void CAboutDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CDialogEx::DoDataExchange(pDX);
//}
//
//BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//
//END_MESSAGE_MAP()
//
//
//// CSocketChatRoomClientDlg dialog



CSignUpLogInDlg::CSignUpLogInDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_SIGNUP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


}

void CSignUpLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, mEdtUsername);
	DDX_Control(pDX, IDC_EDIT2, mEdtPassword);
}

BEGIN_MESSAGE_MAP(CSignUpLogInDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CSignUpLogInDlg::OnBnClickedSignUp)
	ON_BN_CLICKED(IDC_BUTTON1, &CSignUpLogInDlg::OnBnClickedLogIn)
	ON_MESSAGE(SIGNUP_SUCCESS_MSG,&CSignUpLogInDlg::SignUpSuccess)
	ON_MESSAGE(LOGIN_SUCCESS_MSG, &CSignUpLogInDlg::LoginSuccess)
END_MESSAGE_MAP()


// CSocketChatRoomClientDlg message handlers

BOOL CSignUpLogInDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	TcpClient::GetInstance()->SetDialog(this);
	TcpClient::GetInstance()->Run();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSignUpLogInDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		auto i = AfxMessageBox(_T("Do you wanna disconnect to this server"), 1, 1);
		if (i == IDOK) {

			std::string packet = "";
			packet += std::to_string(static_cast<int>(FlagClientToServer::Disconnect_To_Server)) + '\0';
			TcpClient::GetInstance()->SendPacketRaw(packet);
			OnDestroy();

		}


	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSignUpLogInDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSignUpLogInDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CSignUpLogInDlg::OnBnClickedSignUp()
{
	// TODO:
	std::string packet = "";
	CString username;
	CString password;
	
	mEdtUsername.GetWindowTextW(username);
	mEdtPassword.GetWindowTextW(password);

	packet += std::to_string(static_cast<int>(FlagClientToServer::SignUp)) + '\0' + ConvertString::ConvertCStringToString(username) + '\0' + ConvertString::ConvertCStringToString(password) + '\0';
	TcpClient::GetInstance()->SendPacketRaw(packet);

	


}


void CSignUpLogInDlg::OnBnClickedLogIn()
{

	
	// TODO: Add your control notification handler code here
	//CPublicChatDialog *dialog = new CPublicChatDialog(nullptr, CString("ABC"));

	//TEST
	//EndDialog(IDOK);
	//dialog.ShowWindow(SW_NORMAL);
	//CPublicChatDialog*dialog = new CPublicChatDialog(nullptr,_T("ABC"));
	//dialog->Create(IDD_PUBLIC_CHAT);
	//dialog->ShowWindow(SW_SHOWNORMAL);
	std::string packet = "";
	CString username;
	CString password;

	mEdtUsername.GetWindowTextW(username);
	mEdtPassword.GetWindowTextW(password);

	packet += std::to_string(static_cast<int>(FlagClientToServer::Login)) + '\0' + ConvertString::ConvertCStringToString(username) + '\0' + ConvertString::ConvertCStringToString(password) + '\0';


	TcpClient::GetInstance()->SendPacketRaw(packet);
	

	
}

void CSignUpLogInDlg::FailSignUp()
{
	AfxMessageBox(L"Sign Up unsuccessfully please check your username or password and try again");

}

void CSignUpLogInDlg::FailLogin()
{
	AfxMessageBox(L"Login unsuccessfully please check your username or password and try it again");
}


void CSignUpLogInDlg::AccountAlreadyUsed()
{
	AfxMessageBox(L"Account has been logged in by another device");
}

LRESULT CSignUpLogInDlg::LoginSuccess(WPARAM wParam, LPARAM lParam)
{
	//TODO:
	CString username;
	mEdtUsername.GetWindowTextW(username);
	ShowWindow(SW_HIDE);

	CPublicChatDialog* p = new CPublicChatDialog(nullptr, username);
	auto client = TcpClient::GetInstance();
	p->Create(IDD_PUBLIC_CHAT);
	p->ShowWindow(SW_SHOWNORMAL);
	client->SetDialog(p);

	return 0;
	

}

LRESULT CSignUpLogInDlg::SignUpSuccess(WPARAM wParam, LPARAM lParam)
{
	CString username;
	mEdtUsername.GetWindowTextW(username);
	ShowWindow(SW_HIDE);

	CPublicChatDialog* p = new CPublicChatDialog(nullptr,username);
	auto client = TcpClient::GetInstance();
	p->Create(IDD_PUBLIC_CHAT);
	p->ShowWindow(SW_SHOWNORMAL);
	client->SetDialog(p);

	
	return 0;

}
