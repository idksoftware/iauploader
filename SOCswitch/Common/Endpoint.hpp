#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <boost/asio.hpp>
#include <string>

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <map>
#include <thread>
#include <atomic>
#include <iostream>
#include <array>

using boost::asio::ip::udp;

class Endpoint
{
   
    private:
     
        std::string ipAddress;
        unsigned short port;


    public:
        // Constructor

        Endpoint() : ipAddress(""), port(0) {}

        Endpoint(std::string ip, unsigned short p) : ipAddress(ip), port(p) {}

        // Copy Constructor
        Endpoint(const Endpoint& other) : ipAddress(other.ipAddress), port(other.port) {}

        // Getter methods
        inline std::string getIpAddress() const {
            return ipAddress;
        }

        int getPort() const {
            return port;
        }

   

        // Comparison operators
        inline bool operator==(const Endpoint& other) const {

            return (ipAddress == other.ipAddress) && (port == other.port);
        }

        inline bool operator!=(const Endpoint& other) const {
            return !(*this == other);
        }
       
    
        inline udp::endpoint getEndpoint() const
        {

            boost::asio::ip::address addr = boost::asio::ip::address::from_string(ipAddress);
            return udp::endpoint(addr, port);


        }
   

};

#endif












