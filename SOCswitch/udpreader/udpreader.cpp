// udpreader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include "udpsocket.h"

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

/*
int main()
{
    try
    {
        WSASession Session;
        UDPSocket Socket;
        std::string data = "SocClient:127.0.0.1:562";
        char buffer[100];
        while (1) {
            printf("Sending: %s\n", data.c_str());
            Socket.SendTo("127.0.0.1", 562, data.c_str(), data.size());
            Sleep(1000);
            Socket.RecvFrom(buffer, 562);
            std::cout << "Received: " << buffer << std::endl;
        }
        Socket.RecvFrom(buffer, 562);
        std::cout << buffer;
    }
    catch (std::exception& ex)
    {
        std::cout << ex.what();
    }
    char c;
    std::cin >> c;
}
*/
int main()
{
    try
    {
        WSASession Session;
        UDPSocket Socket;
        char buffer[100];

        Socket.Bind(560);
        while (1)
        {
            

            std::string input = Socket.Recv();
            printf("Event: %s\n", input.c_str());
            //std::reverse(std::begin(input), std::end(input));
            //Socket.SendTo(add, input.c_str(), input.size());
         
        }
    }
    catch (std::system_error& e)
    {
        std::cout << e.what();
    }
}