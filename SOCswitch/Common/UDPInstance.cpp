/*/*================================================================================
SECURITY CLASSIFICATION: Airbus Defence and Space Company Confidential
--------------------------------------------------------------------------------
Project     : Zephyr 8b SOC
File name   : UDPInstance.h
Description : Memeber definitions for UDPIntance.h.
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


#include "UDPInstance.h"



//retunrs a hh/mm//ss//ms string of the time when the function is called from the system clock.
std::string get_system_time()
{

	boost::posix_time::ptime date_time = boost::posix_time::microsec_clock::universal_time();


	std::stringstream ss;

	//converts datetime to a stringstreram
	ss << date_time.time_of_day();

	//returns the stringstream as a string
	return ss.str();


}
//initialises a new UDP Instance with a recive Port and a send port one number higher than the recive port
UDPInstance::UDPInstance(unsigned short Port)
{

	bool SocketsReady = false;

	while (!SocketsReady)
	{
		try
		{

			reciveSocketPort = Port;
			reciveSocket = new udp::socket(ioservice, udp::endpoint(udp::v4(), Port));
			sendSocketPort = Port + 1;
			sendSocket = new udp::socket(ioservice, udp::endpoint(udp::v4(), Port + 1));

			//std::cout << "Tried to start server at Port : " << Port << "and Sender at Port :" << Port + 1 << " Worked!" << std::endl;
			isError = false;
			SocketsReady = true;
		}
		catch(std::exception ex)
		{

			
			//std::cout << "Tried to start server at Port :" << Port << "and Sender at Port :" << Port + 1 << " Failed!" << "Error: " << ex.what() << std::endl;
			Port += 2;


		}


	}



	instanceEndpoint[0] = reciveSocket->local_endpoint();
	instanceEndpoint[1] = sendSocket->local_endpoint();

	

	//std::cout << "Server Waitng at IpAdress: " << instanceEndpoint[0].address() << " at Port: " << Port << std::endl;
	//std::cout << "Sender Started at IpAdress: " << instanceEndpoint[0].address() << " at Port: " << Port + 1 << std::endl;


}


bool UDPInstance::HasMessages()
{
	return !MessageQueue.empty();
}

Message UDPInstance::PopMessage()
{
	return MessageQueue.pop();
}


void UDPInstance::AddClient(const std::string& name,const  Endpoint &endpoint)
{


	Clients.insert(std::make_pair(name, endpoint));


}

void UDPInstance::SendToClient(const std::string& message,const std::string& name)
{
	//checks if key-value pair exists in map

	if (Clients.find(name) != Clients.end())
	{
		sendSocket->send_to(asio::buffer(message), Clients[name].getEndpoint());
	}
	else
	{
		std::cout << "Object does not exist in the map." << std::endl;
	}



	
}

void UDPInstance::SendToAll(const std::string& message)
{


	//iterates through the map of endpoints and sends to each one of them
	for(std::unordered_map<std::string, Endpoint>::iterator it = Clients.begin(); it != Clients.end(); ++it)
	{
		SendToClient(message, it->first);
		
	}


}
//runs the server and the io service in a separate thread waiting to recive
void UDPInstance::RunAsync()
{
	service_thread = std::thread([this]
	{

			run_service();

	});



}

//joins the main the thread to the main thread and stops the io service

void UDPInstance::Close()
{

	reciveSocket->close();
	sendSocket->close();
	service_thread.join();
}

Endpoint UDPInstance::GetSenderEndpoint() const
{


	Endpoint endpoint(instanceEndpoint[1].address().to_string(),instanceEndpoint[1].port());

	return endpoint;
}
//returns 
Endpoint UDPInstance::GetReciverEndpoint() const
{

	Endpoint endpoint(instanceEndpoint[0].address().to_string(), instanceEndpoint[0].port());

	return endpoint;
}


size_t UDPInstance::GetClientCount() const 
{

	return Clients.size();
}

// 

void UDPInstance::start_receive()
{

	reciveSocket->async_receive_from(asio::buffer(recieveBuffer), remoteEndpoint,
		[this](std::error_code ec, std::size_t bytes_recvd) 
		{ 

			if (ec)
			{
				std::cout << ec.message() << std::endl;
			}
			
			
			this->handle_receive(ec, bytes_recvd); 
		});


}



void UDPInstance::handle_receive(const std::error_code& error, std::size_t bytes_transferred)
{
	if (!error)
	{
		try {

			Message message;
			
		
			message.time = get_system_time();
			message.message = recieveBuffer.data();
			message.recivedFrom = remoteEndpoint;

			
			this->MessageQueue.push(message);

			

		
		}
		catch (std::exception ex) {
			std::cout << "handle_receive: Error parsing incoming message:" << ex.what() << std::endl;;
		}
		catch (...) {
			std::cout << "handle_receive: Unknown error while parsing incoming message" << std::endl;
		}
	}
	//flushes the buffer


	recieveBuffer = std::array<char,UDPINSTANCE_BUFFER_SIZE>();
	
	start_receive();


}

void UDPInstance::run_service()
{

	start_receive();

	



	while (!ioservice.stopped()) {
		try {
			
			ioservice.run();
		}
		catch (const std::exception& e) {

			std::cout << "Instance exception: " <<  e.what() << std::endl; ;
		}
		catch (...) {
			std::cout << "Instance exception: unknown" << std::endl;
		}
	}




}	





UDPInstance::~UDPInstance()
{



	service_thread.join();
	delete reciveSocket;
	delete sendSocket;
	
}