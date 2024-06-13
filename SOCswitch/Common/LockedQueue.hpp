/*/*================================================================================
SECURITY CLASSIFICATION: Airbus Defence and Space Company Confidential
--------------------------------------------------------------------------------
Project     : Zephyr 8b SOC
File name   : LcckedQueue.hpp
Description : Provides a Asynchcronous access to a std::queue, defines the Message object 
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
#include <string>
#include <queue>
#include <boost/asio.hpp>


using namespace boost::asio::ip;

struct Message
{

	std::string message;
	udp::endpoint recivedFrom;
	std::string time;


};

class LockedQueue {


	std::mutex mutex;
	std::queue<Message> queue;


public:

	/// <summary>
	/// inserts a message into the queue 
	/// </summary>
	/// <param name="value"></param>
	void push(Message value)
	{
		std::unique_lock<std::mutex> lock(mutex);
		queue.push(value);
	};

	/// <summary>
	/// 
	/// </summary>
	/// <returns> returns the last value inserted in the queue</returns>
	Message pop()
	{
		std::unique_lock<std::mutex> lock(mutex);
		Message value;
		std::swap(value, queue.front());
		queue.pop();
		return value;
	};

	bool empty() {

		std::unique_lock<std::mutex> lock(mutex);
		return queue.empty();
	}



};