#pragma once

#ifdef _WIN32
// windsock 1.0 #include <Winsock.h>
#include <Winsock2.h> // Windsock 2
#include <stdio.h>
#else
#define SOCKET int
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
namespace Common
{
	class UDPOut
	{
		static struct sockaddr_in m_si_other;
		static SOCKET m_socketId;
		static int m_socketLength;
		static const char* m_address;
		static int m_port;
		static bool init();
		static bool m_socklibStarted;
		static bool m_udpEnabled;
		static bool m_serverMode;
	public:
		UDPOut();
		~UDPOut();
		static bool enableUDPOutput(int port, const char* address);
		static bool enableUDPOutput(int port);
		static bool printf(const char* fmt, ...);
		static bool out(const char* out);
		static bool isEnabled() { return m_udpEnabled; };
	};
}


