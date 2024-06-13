/*================================================================================
SECURITY CLASSIFICATION: Airbus Defence and Space Company Confidential
--------------------------------------------------------------------------------
Project     : Zephyr 8b SOC
File name   : base.h
Version     : 0.0.2
Description : Provides all commonly used constants macros etc.
--------------------------------------------------------------------------------
Compiler : Microsoft Visual C++ 2022
Host : Windows 11
--------------------------------------------------------------------------------
Written by : Natasha Chia
Creation Date : Jan 2024
--------------------------------------------------------------------------------
Copyright AALTO HAPS Ltd 2024.
The Software and Data files within this product are the property of Airbus
Defence and Space Limited, and it is prohibited for the Software and/or Data
files to be reproduced, disassembled, reverse engineered and/or revealed to
third parties without the prior written permission of Airbus Defence and Space
Limited in writing.
================================================================================*/
#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"

//Constants used in Soc Switch modules

#define MAX_NODES				(100) // Maximum Number of Local nodes.
#define MAX_REMOTENODES			(100) // Maximum Number of remote nodes.
#define MAX_CONNECTIONS			(100) // Maximum UDP Connections established.
#define MAX_CHANNELS			(100) // Maximum Number of channels.
#define MAX_ROUTES				(100) // Maximum Number of Routes.
#define MAX_NODE_ID				(100) // Maximum Node IDs used to differentiate local nodes.
#define MAX_REMOTENODE_ID       (100) // Maximum Remote Node IDs to differentiate the remote nodes.
#define MAX_CHANNEL_ID          (100) // Maximum channel IDs used to differentiate channels.

// Enable and Disable of Main Functions

#if ENCRYPTED  
#define AES128_ENCRYPTION // If Encryption is enabled, AES128 is selected as the default encryption
#else
#define ENCRYPT_OFF
#endif

// Definition of main exceptions and errors
#ifndef INVALID_SOCKET
#define INVALID_SOCKET			(0xFFF)
#endif


#ifndef SOCKET_ERROR
#define SOCKET_ERROT			(-1)
#endif


//Macros used
#define listofnodes(x) nodeCollection(std::vector<DataModel::CNode::nodeParams::nodeId> nodeIdsList)   {return nodeIdsList;}

#define isValid(x) if (x) > MAX_NODES || MAX_REMOTENODES || MAX_CONNECTIONS || MAX_CHANNELS || MAX_ROUTES ? 0:1

//Time definitions
const auto soc_MS = boost::posix_time::milliseconds(60 * 1000);



//type definitions
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef unsigned char Byte;
typedef void* p_void;
typedef const void* p_const_void;
typedef std::size_t size_type;

// Port States
enum class portStates
{
	PORT_CLOSED = 0,
	PORT_OPEN,
	PORT_INITIALISING,


	// Flow control values
	FC_NONE = 0,
	FC_CTS_FLOW_CONTROL,
	FC_DSR_FLOW_CONTROL,
	FC_DSR_AND_CTS_FLOW_CONTROL,
	FC_RS485_TOGGLE_MODE,
};



enum class scdMsgType {
	cpptojson,
	jsontocpp,

};

enum class statusRoute {
	nodeOK,
	nodeNOK,
	rnodeOK,
	rnodeNOK,
	sourceConnectionOK,
	targetConnectionOK,
	sourceConnectionNOK,
	targetConnectionNOK,
};

class Base
{
public:

	Base(Base& const b);
	~Base();

private:




};