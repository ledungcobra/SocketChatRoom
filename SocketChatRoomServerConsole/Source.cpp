// EXAMPLE OF USAGE

#include <iostream>
#include <string>
#include "Tcpserver.h"

using namespace std;

int main()
{
	string input;
	TcpServer* server = new TcpServer("127.0.0.1", 54000, MessageReceived);

	if (server->Init())
	{
		server->Run();
	}
	else
	{
		cout << "Fail to init server" << endl;
	}
	delete server;
	cin.get();
	return 0;
}