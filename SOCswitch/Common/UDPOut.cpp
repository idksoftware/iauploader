

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include "UDPOut.h"
#include <stdio.h>
#ifdef _WIN32
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
#define SOCKET_ERROR -1
#endif

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

#define BUFLEN 512	//Max length of buffer

namespace Common
{

	struct sockaddr_in UDPOut::m_si_other;
	SOCKET UDPOut::m_socketId;
	int UDPOut::m_socketLength;
	const char* UDPOut::m_address;
	int UDPOut::m_port;
	bool UDPOut::m_socklibStarted = false;
	bool UDPOut::m_udpEnabled = false;
	bool UDPOut::m_serverMode = false;


	UDPOut::UDPOut()
	{
	}

	bool UDPOut::init() {
		/*
		//Construct the server sockaddr_ structure
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
		//-----------------------------------------------------------------
		// This line is correct because you want to bind an address on
		// you machine
		//-----------------------------------------------------------------
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		//-----------------------------------------------------------------
		// This is the port on your machine that you are going to bind.
		// This doesn't matter because the server just returns to the port
		// it received the request on.  Port zero is any port.
		//-----------------------------------------------------------------
		//myaddr.sin_port=htons(9045);
		m_addr.sin_port = htons(0);

		//Create the socket
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0))<0) {
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
		}

		if (bind(sock, (struct sockaddr *) &m_addr, sizeof(m_addr))<0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
		}
		// Use inet_pton to turn 131.156.145.90 into an integer used in the
		// socket functions
		//-----------------------------------------------------------------
		InetPton(AF_INET, "131.156.145.90", &m_addr.sin_addr.s_addr);
		//-----------------------------------------------------------------
		// Here is where the port matters, because you know the server is
		// listening on that port.  So we must set the port number here
		// for the out going message.
		//-----------------------------------------------------------------
		m_addr.sin_port = htons(9045);
		//-----------------------------------------------------------------
		// I don't know where the exact string is but this returns a message
		//-----------------------------------------------------------------
		std::string s("12345678910:5/15:300.00:Visa");

		//send the message to server
		if (sendto(sock, s.c_str(), s.size(), 0, (struct sockaddr *)&m_addr, sizeof(myaddr)) != s.size()) {
		perror("Mismatch in number of bytes sent");
		exit(EXIT_FAILURE);
		}

		//Receive the datagram back from server
		int addrLength(sizeof(myaddr)), received(0);
		char buffer[256] = { 0 };
		if ((received = recvfrom(sock, buffer, 256, 0, (sockaddr *)&m_addr, (socklen_t*)&addrLength)) < 0) {
		perror("Mismatch in number of bytes received");
		exit(EXIT_FAILURE);
		}
		buffer[received] = '\0';
		std::cout << "Server (" << inet_ntoa(myaddr.sin_addr) << ") echoed: " << buffer << std::endl;
		closesocket(sock);
		*/

		m_socketLength = sizeof(m_si_other);



		//create socket
		if ((m_socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			//printf("socket() failed with error code : %d", WSAGetLastError());
			return false;
		}

		//setup address structure
		if (m_serverMode) {

			m_si_other.sin_family = AF_INET;
			m_si_other.sin_port = htons(m_port);
			m_si_other.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr
			memset(m_si_other.sin_zero, '\0', sizeof m_si_other.sin_zero);

			// Bind socket with address struct
			bind(m_socketId, (struct sockaddr*)&m_si_other, sizeof(m_si_other));
		}
		else {
			memset((char*)&m_si_other, 0, sizeof(m_si_other));
			m_si_other.sin_family = AF_INET;
			m_si_other.sin_port = htons(m_port);
#ifdef _WIN32
			m_si_other.sin_addr.S_un.S_addr = inet_addr(m_address);
#else
			m_si_other.sin_addr.s_addr = inet_addr(m_address);
#endif
		}
		//start communication
		return true;
	}

	bool UDPOut::enableUDPOutput(int port) {

		m_port = port;
		m_serverMode = false;
		m_address = "127.0.0.1";
		if (init() == false) {
			return false;
		}
		m_udpEnabled = true;
		return true;
	}

	bool UDPOut::enableUDPOutput(int port, const char* address) {

		m_port = port;
		m_address = address;

		if (init() == false) {
			return false;
		}
		m_udpEnabled = true;
		return true;
	}

	bool UDPOut::printf(const char* format, ...) {
		if (m_udpEnabled == false) return true;
		char message[512];
		va_list args;
		va_start(args, format);
#ifdef _WIN32
		vsprintf_s(message, format, args);
#else
		vsprintf(message, format, args);
#endif
		va_end(args);
		if (out(message) == false) {
			return false;
		}
		return true;
	}

	bool UDPOut::out(const char* out) {
		if (m_udpEnabled == false) return true;
		//send the message
		int len = (int)strlen(out);
		if (sendto(m_socketId, out, len, 0, (struct sockaddr*)&m_si_other, m_socketLength) == SOCKET_ERROR)
		{
			//printf("sendto() failed with error code : %d", WSAGetLastError());
			return false;
		}
		return true;
	}


	UDPOut::~UDPOut()
	{
#ifdef _WIN32
		closesocket(m_socketId);
#else
		close(m_socketId);
#endif
	}
};
