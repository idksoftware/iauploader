/*/*================================================================================
SECURITY CLASSIFICATION: Airbus Defence and Space Company Confidential
--------------------------------------------------------------------------------
Project     : Zephyr 8b SOC
File name   : UDPInstance.h
Description : Provides a easy to use UDP Client and Server combination.
--------------------------------------------------------------------------------
Compiler : Microsoft Visual C++ 2022
Host : Windows 11
--------------------------------------------------------------------------------
Written by : Shpresim Muja
Creation Date : Mar 2024
--------------------------------------------------------------------------------
Copyright AALTO HAPS Ltd 2024.
The Software and Data files within this product are the property of Airbus
Defence and Space Limited, and it is prohibited for the Software and/or Data
files to be reproduced, disassembled, reverse engineered and/or revealed to
third parties without the prior written permission of Airbus Defence and Space
Limited in writing.
================================================================================*/
#pragma once

#include "LockedQueue.hpp"
#include "Endpoint.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>





using namespace boost::asio::ip;
using namespace boost;




const size_t UDPINSTANCE_BUFFER_SIZE = 2048;


class UDPInstance
{
public:

	

	UDPInstance(unsigned short Port);

	//returns true if a new message exists in the queue
	bool HasMessages();


	/// <summary>
	/// returns the last message recieved and stored in the 
	/// </summary>
	/// <returns></returns>
	Message PopMessage();

	/// <summary>
	/// Adds an Endpoint to the Clients map 
	/// </summary>
	/// <param name="name"> The name that will be used to access it </param>
	/// <param name="endpoint">The loaction of the rerciver </param>
	void AddClient(const std::string& name, const Endpoint& endpoint);


	/// <summary>
	/// Sends an std::string message to a client referenced by name after it has been looked up in Clients map
	/// </summary>
	/// <param name="message">the message to be sent </param>
	/// <param name="name"> name of the Endpoint stored in tClients map</param>
	void SendToClient(const std::string& message, const std::string& name);

	/// <summary>
	/// Sends a message to all endpoints stored in the Clients map
	/// </summary>
	/// <param name="message"></param>
	void SendToAll(const std::string& message);

	void RunAsync();

	//Closes the sockets, the service_thread and the io_service
	void Close();

	Endpoint GetSenderEndpoint() const;

	Endpoint GetReciverEndpoint() const;

	size_t GetClientCount() const;
	
	bool terminate;

	bool IsError() { return isError; };
	int  ReciveSocketPort() { return reciveSocketPort; }
	int  SendSocketPort() { return sendSocketPort; };

	~UDPInstance();
private:
	//background service
	int reciveSocketPort{ -1 };
	int sendSocketPort{ -1 };
	bool isError{ true };
	udp::socket * reciveSocket; // this will be initilised if it a is a server that sends and recieves.
	udp::socket * sendSocket;
	asio::io_service ioservice;
	std::array<char, UDPINSTANCE_BUFFER_SIZE> recieveBuffer;
	std::string sendBuffer;
	//port and iadress of the 
	udp::endpoint instanceEndpoint[2];
	udp::endpoint remoteEndpoint;
	std::thread service_thread;
	std::unordered_map<std::string, Endpoint> Clients;
	
	void start_receive();
	void handle_receive(const std::error_code& error, std::size_t bytes_transferred);
	void run_service();
	void handle_send(std::string /*message*/, const std::error_code& /*error*/, std::size_t /*bytes_transferred*/) {}
	

	LockedQueue MessageQueue;



};

