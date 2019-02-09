#pragma once

#include <map>
#include <string>

#ifdef _WIN32
#include <WinSock2.h>
#else
#include "sys/socket.h"
#endif

class Stream;

class Server {
public:
	virtual ~Server();

	virtual bool Open() = 0;

	virtual bool Process() = 0;
	SOCKET GetSocket() { return Sock; }

	void SetHost(const char* host);
	void SetPort(unsigned int port);

	virtual void StreamDisconnected(Stream* stream) {};
	
protected:
	Server();
	virtual Stream* GetNewStream(unsigned int ip, unsigned short port) = 0;

	SOCKET Sock;
	unsigned int Port;
	unsigned int Host;

private:
	static unsigned int InitializeCount;

};