#include "pch.h"
#include "TcpServer.h"

SOCKET TcpServer::CreateSocket()
{
	//Khởi tạo socket
	SOCKET sock_server = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock_server != INVALID_SOCKET)	//Check xem cấp có bị lỗi không 
	{
		// Hoàn thiện cấu trúc hint ( báo là mình dùng kiểu protocol nào, hiện đang dùng IPv4 )
		this->hint.sin_family = AF_INET;
		this->hint.sin_port = htons(this->port_number);
		inet_pton(AF_INET, this->ipaddress.c_str(), &this->hint.sin_addr);

		// Bind ipaddress và port vào socket
		int bind_check = bind(sock_server, (sockaddr*)&this->hint, sizeof(this->hint));

		if (bind_check != SOCKET_ERROR) //check xem bind thành công không
		{
			// Báo winsock là socket đang đợi nghe tín hiệu
			int listen_check = listen(sock_server, SOMAXCONN); // SOMAXCONN là số connect tối đa có thể nghe được, là hằng số
			if (listen_check == SOCKET_ERROR) // check xem nghe có được không
			{
				std::cerr << "Can't listen to socket ! Error #" << WSAGetLastError() << std::endl; // Xuất ra báo lỗi
				return SOCKET_ERROR;
			}
		}
		else
		{
			std::cerr << "Can't bind IP and port to socket! Error #" << WSAGetLastError() << std::endl; // Xuất lỗi bind không được
			return SOCKET_ERROR;
		}
	}
	else
	{
		std::cerr << "Can't Create Socket ! Error # " << WSAGetLastError() << std::endl;
		WSACleanup(); // Gọi hàm để huỷ socket trong fd
	}

	return sock_server;

}

TcpServer::~TcpServer() // destructor
{
	// Đóng hết tất cả socket
	// Báo cho client biết là " dm bố đi ngủ "
	std::string shutdown_msg = "Server is shutting down. GOODBYE ! \n";
	while (master.fd_count > 0) // nghĩa là đang còn socket mở
	{
		// Lấy thông tin socket 
		SOCKET sock = master.fd_array[0]; //0 là vì mỗi lần đóng thì dồn lên, tương tự như queue

		//Gửi tin nhắn 
		send(sock, shutdown_msg.c_str(), shutdown_msg.size() + 1, 0);

		// Xoá socket đó khỏi fd và đóng hẳn socket đó 
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	WSACleanup();
	getchar();


}

//Khởi tạo winsock
bool TcpServer::Init()
{
	WSADATA data;
	WORD ver = MAKEWORD(2, 2); // version 2,2

	int winsock_check = WSAStartup(ver, &data);

	if (winsock_check != 0) // check xem có lỗi không
	{
		std::cerr << "Can't start winsock ! Error # " << WSAGetLastError << std::endl;
		return false;
	}

	return true;
}

bool TcpServer::Send(SOCKET socketclient, std::string message)
{
	if (!message.empty())
	{
		return send(socketclient, message.c_str(), message.size() + 1, 0);
	}
	return false;
}

void TcpServer::SendAllClientExcept(SOCKET socketclient, std::string message)
{
	if (!message.empty())
	{
		// Gửi tin nhắn thông qua các socket trong fd ( nhớ TRÁNH SOCKET DÙNG ĐỂ LISTEN )
		for (int i = 0; i < this->master.fd_count; i++)
		{
			SOCKET sock = this->master.fd_array[i];
			if ((sock != this->listening) && (sock != socketclient)) // Điều kiện tránh
			{
				send(sock, message.c_str(), message.size() + 1, 0);
			}
		}
	}
}

void TcpServer::Run()
{
	//Tạo socket dùng để nghe
	this->listening = this->CreateSocket();

	if (listening == INVALID_SOCKET) // check
	{
		std::cerr << "Error in Run(), counld't start the server !"<< std::endl;
		return;
	}

	// Cho socket nghe vào fd, cần thiết vì có nó mới nghe tín hiệu từ các client được
	FD_SET(this->listening, &this-> master);

	this->thread_running = true;
	std::cout << "Server is running !" << std::endl;
	
	AfxBeginThread(Thread, this);


}


// Bất kì tin nhắn nào của client vào server đều gọi hàm này

UINT Thread(LPVOID server)
{
	auto serv = (TcpServer*)server;
	while (serv->thread_running == true)
	{
		fd_set copy = serv->master; // Cần tạo copy master vì hàm khi gọi thì hàm select sẽ huỷ luôn fd đó nên bước này quan trọng vl
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Vòng lặp duyệt qua các kết nối đã có / các kết nối đang đợi
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i]; // Dùng copy

			if (sock == serv->listening)
			{
				// Cho client mới vào
				SOCKET client = accept(serv->listening, nullptr, nullptr);

				// Cho client mới vào danh sách các client đã kết nối
				FD_SET(client, &serv->master);

				// Gửi tin nhắn chào mừng client mới
				std::string welcommessage = "WELCOME TO THE AWSOME CHAT ROOM ! \r\n";
				send(client, welcommessage.c_str(), welcommessage.size() + 1, 0);

			}

			else // Xử lí tin nhắn yêu cầu của client 
			{
				// buffer
				char buffer[4096];
				ZeroMemory(buffer, 4096);

				//Nhận tin nhắn
				int bytesIn = recv(sock, buffer, 4096, 0);
				std::string cmd = std::string(buffer, bytesIn);
				// Cho client thoát
				if (cmd == "quit -all\r\n")
				{
					closesocket(sock);
					FD_CLR(sock, &serv->master);
				}

				//Thêm cái yêu càu khác phía dưới
				//...
				else
				{
					if (buffer[0] == '\\')
					{
						// command thoat
						if (cmd == "\\\\quit\r\n")
						{
							serv->thread_running = false;
							break;
						}
						continue;
					}

					if (serv->MessageRecieved != NULL)
					{
						serv->MessageRecieved(serv, sock, std::string(buffer, 0, bytesIn));

					}
				}
			}
		}
	}


	std::string msg = "Server is shutting down. Goodbye\r\n";

	while (serv->master.fd_count > 0)
	{
		// Lấy số socket
		SOCKET sock = serv->master.fd_array[0];

		// Gửi tin nhắn tạm biệt
		send(sock, msg.c_str(), (int)(msg.size() + 1), 0);

		// Xoá gửi master và đóng
		FD_CLR(sock, &serv->master);
		closesocket(sock);
	}

	FD_CLR(serv->listening, &serv->master);
	closesocket(serv->listening);
	std::cout << "Server stopped !" << std::endl;
	// Message to let users know what's happening.
	return 0;

}

void MessageReceived(TcpServer* listen_server, SOCKET client, std::string msg)
{
	// Phần login thêm vào đây

// Gửi tin nhắn đến tất cả client khác
	std::string strOut;
	strOut = strOut + "SOCKET #" + std::to_string(client) + ": " + msg + "\r";
	listen_server->SendAllClientExcept(client, strOut);

	//Đưa tin nhắn đó lên server
	std::cout << "SOCKET " << client << ": " << msg << "\r";
}







