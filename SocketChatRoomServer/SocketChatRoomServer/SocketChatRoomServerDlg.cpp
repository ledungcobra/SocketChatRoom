// SocketChatRoomServerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SocketChatRoomServer.h"
#include "SocketChatRoomServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ConvertString.h"
#include "TcpServer.h"



SocketChatRoomServerDlg::SocketChatRoomServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SOCKETCHATROOMSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void SocketChatRoomServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, mListBox);
	DDX_Control(pDX, IDC_EDIT1, mEdtLog);
}

BEGIN_MESSAGE_MAP(SocketChatRoomServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CSocketChatRoomServerDlg message handlers

BOOL SocketChatRoomServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.
#ifdef _DEBUG
	if (!AllocConsole())
		AfxMessageBox(_T("Failed to create the console!"), MB_ICONEXCLAMATION);
#endif

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
	SetWindowText(L"Server");
	TcpServer::GetInstance()->SetDialog(this);
	TcpServer::GetInstance()->Run();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void SocketChatRoomServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_CLOSE)
	{
		auto i = AfxMessageBox(_T("Do you want to close this server"), 1, 1);
		if (i == IDOK) {
			try {
				_cwprintf(L"Number of threads: %d", TcpServer::GetInstance()->flagRunningThread.size());
				
				
				for (auto it = TcpServer::GetInstance()->flagRunningThread.begin(); it != TcpServer::GetInstance()->flagRunningThread.end(); it++) {

					it->second = false;

				}
				TcpServer::GetInstance()->flagRunningThread.clear();

				TcpServer::GetInstance()->CloseServer();
				//TODO: Tương tự Turn off server

				OnDestroy();
			}
			catch (...) {

			}
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

void SocketChatRoomServerDlg::OnPaint()
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
HCURSOR SocketChatRoomServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void SocketChatRoomServerDlg::UpdateLogBox(std::string message)
{
	if (message == "") return;
	CString buff;

	mEdtLog.GetWindowText(buff);
	buff += ConvertString::DecodeStringToCString(message) + L"\r\n";
	mEdtLog.SetWindowText(buff);
}

void SocketChatRoomServerDlg::UpdateActiveUserListView()
{
	mListBox.ResetContent();
	for (auto it = TcpServer::GetInstance()->listUser.begin(); it != TcpServer::GetInstance()->listUser.end(); it++) {
		if (it->second != "") {
			this->mListBox.AddString(ConvertString::DecodeStringToCString(it->second));
		}
		
	}
}
