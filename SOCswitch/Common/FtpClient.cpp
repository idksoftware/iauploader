
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <chrono>
#include <iostream>
#include <filesystem>
#include <chrono>
#include "FtpClient.h"
#include "logger.h"

#pragma comment(lib,"ws2_32.lib")


#define SOCKET_READ_TIMEOUT_SEC 10

using namespace std::chrono_literals;

namespace Common
{
	CFtpClient::CFtpClient()
		: m_commandsocket(INVALID_SOCKET), m_logger(Logger::getLogger())
	{

		initSocket();
	}

	CFtpClient::~CFtpClient()
	{
		if (m_commandsocket != INVALID_SOCKET)
		{
			closesocket(m_commandsocket);
		}
		WSACleanup();
	}

	bool CFtpClient::connectToHost(const std::string& serverIP, int serverPort/* =21 */)
	{
		
		char		respCode[4] = { 0 };
		std::string		resStr;
		uint32_t	ip = inet_addr(serverIP.c_str());
		m_commandsocket = this->createSocket(ntohl(ip), serverPort);
		if (m_commandsocket == INVALID_SOCKET)
		{
			return false;
		}
		bool result = this->readResponse(respCode, resStr);
		if (!result)
		{
			return false;
		}
		if (respCode[0] != '2')
		{
			return false;
		}
		return true;
	}

	bool	CFtpClient::login(const std::string& user, const std::string& password)
	{
		std::string		respStr;
		bool		result = this->sendCommand("user", user.c_str());
		if (!result)
		{
			goto ERR;
		}
		result = this->getResponseResult('3', respStr);
		if (!result)
		{
			goto ERR;
		}
		result = this->sendCommand("pass", password.c_str());
		if (!result)
		{
			goto ERR;
		}
		return this->getResponseResult('2', respStr);
	ERR:
		return false;
	}

	bool CFtpClient::cd(const std::string& path)
	{
		std::string		respStr;
		if (!sendCommand("cwd", path.c_str()))
		{
			return false;
		}
		return getResponseResult('2', respStr);
	}

	bool CFtpClient::deleteFile(const std::string& path)
	{
		std::string		respStr;

		if (!sendCommand("dele", path.c_str()))
		{
			return false;
		}
		return getResponseResult('2', respStr);
	}

	bool CFtpClient::rmdir(const std::string& path)
	{
		std::string		respStr;
		if (!sendCommand("rmd", path.c_str()))
		{
			return false;
		}
		return this->getResponseResult('2', respStr);
	}

	bool CFtpClient::mkdir(const std::string& path)
	{
		std::string		respStr;

		if (!sendCommand("mkd", path.c_str()))
		{
			return false;
		}
		return getResponseResult('2', respStr);
	}

	std::string	CFtpClient::pwd()
	{
		std::string		respStr;
		if (!sendCommand("pwd", ""))
		{
			return std::string();
		}
		getResponseResult('2', respStr);

		size_t		left = respStr.find("\"");
		size_t		right = respStr.rfind("\"");
		return respStr.substr(left + 1, right - left - 1);
	}

	bool CFtpClient::mlist(const std::string& path, std::vector<FtpFile*>& fileList)
	{
		uint16_t	port = 0;
		uint32_t	serverIP = 0;
		if (!parsePasvData(&serverIP, &port))
		{
			return false;
		}
		if (!openDataChannel(serverIP, port))
		{
			return false;
		}
		if (path[0] == '.') {
			this->sendCommand("MLSD", nullptr);
		}
		else {
			this->sendCommand("MLSD", path.c_str());
		}
		char recvBuf[4096] = { 0 };
		int	 unParseBytes = 0;
		int	 readBytes = 0;
		int	 totalBytes = 0;
		while ((readBytes = recv(m_dataSocket, recvBuf + unParseBytes, 4095 - unParseBytes, 0)) > 0)
		{
			totalBytes = unParseBytes + readBytes;
			recvBuf[totalBytes] = 0;
			if (strstr(recvBuf, "\r\n") != nullptr)
			{
				int parseBytes = parseMLstLines(recvBuf, totalBytes, fileList);
				if (totalBytes - parseBytes > 0)
				{
					memcpy(recvBuf, recvBuf + parseBytes, totalBytes - parseBytes);
				}
				unParseBytes = totalBytes - parseBytes;
			}
		}
		closesocket(m_dataSocket);
		return true;
	}

	bool CFtpClient::list(const std::string& path, std::vector<FtpFile*>& fileList)
	{
		uint16_t	port = 0;
		uint32_t	serverIP = 0;
		if (!parsePasvData(&serverIP, &port))
		{
			return false;
		}
		if (!openDataChannel(serverIP, port))
		{
			return false;
		}
		if (path[0] == '.') {
			this->sendCommand("List", nullptr);
		}
		else {
			this->sendCommand("List", path.c_str());
		}
		char recvBuf[4096] = { 0 };
		int	 unParseBytes = 0;
		int	 readBytes = 0;
		int	 totalBytes = 0;
		while ((readBytes = recv(m_dataSocket, recvBuf + unParseBytes, 4095 - unParseBytes, 0)) > 0)
		{
			totalBytes = unParseBytes + readBytes;
			recvBuf[totalBytes] = 0;
			if (strstr(recvBuf, "\r\n") != nullptr)
			{
				int parseBytes = parseListLines(recvBuf, totalBytes, fileList);
				if (totalBytes - parseBytes > 0)
				{
					memcpy(recvBuf, recvBuf + parseBytes, totalBytes - parseBytes);
				}
				unParseBytes = totalBytes - parseBytes;
			}
		}
		closesocket(m_dataSocket);
		return true;
	}

	bool CFtpClient::put(const std::string& remoteFile, const std::string& localFile)
	{
		std::string respStr;
		uint32_t	serverIP = 0;
		uint16_t  port = 0;
		if (!parsePasvData(&serverIP, &port))
		{
			return false;
		}
		if (!openDataChannel(serverIP, port))
		{
			return false;
		}
		
		char* msg = (char*)malloc(1024 * 1024);
		if (msg == nullptr)
		{
			return false;
		}
		sendCommand("STOR", remoteFile.c_str());
		if (getResponseResult('1', respStr) == false) {
			return false;
		}

		HANDLE	tmpFileHandle = ::CreateFile(localFile.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (tmpFileHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}

		DWORD		tmpFileSize = ::GetFileSize(tmpFileHandle, nullptr);
		DWORD		bytesSend = 0;
		while (bytesSend <= tmpFileSize)
		{
			DWORD bytesRead = 0;
			ReadFile(tmpFileHandle, msg, 1024 * 1024, &bytesRead, nullptr);
			if (bytesRead == 0)
			{
				break;
			}
			bytesSend += bytesRead;
			send(m_dataSocket, msg, bytesRead, 0);
		}
		::CloseHandle(tmpFileHandle);
		free(msg);
		closesocket(m_dataSocket);
		return getResponseResult('2', respStr);
	}

	void CFtpClient::get(const std::string& remoteFile, const std::string& localFile)
	{
		std::string respStr;
		uint32_t	serverIP = 0;
		uint16_t  port = 0;
		if (!parsePasvData(&serverIP, &port))
		{
			return;
		}
		if (!openDataChannel(serverIP, port))
		{
			return;
		}
		sendCommand("RETR", remoteFile.c_str());
		getResponseResult('1', respStr);

		char msg[2332] = { 0 };
		int	bytes = 0;
		FILE* fp = nullptr;
		fopen_s(&fp, localFile.c_str(), "wb");
		while ((bytes = recv(m_dataSocket, msg, 2332, 0)) > 0)
		{
			fwrite(msg, bytes, 1, fp);
		}
		fclose(fp);
		closesocket(m_dataSocket);
		this->getResponseResult('2', respStr);
	}

	bool CFtpClient::quit()
	{
		std::string		respStr;
		bool	result = this->sendCommand("quit", "");
		if (!result)
		{
			return false;
		}
		return this->getResponseResult('2', respStr);
	}

	bool CFtpClient::sendRawCommand(const char* cmd, const char* data, char expectCode)
	{
		std::string		respStr;
		if (!this->sendCommand(cmd, data))
		{
			return false;
		}
		return this->getResponseResult(expectCode, respStr);
	}

	SOCKET CFtpClient::createSocket(uint32_t serverIP, uint16_t port)
	{
		SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == INVALID_SOCKET)
		{
			return INVALID_SOCKET;
		}
		sockaddr_in		sockaddr;
		memset(&sockaddr, 0, sizeof(sockaddr));
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(port);
		//sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(serverIP);
		sockaddr.sin_addr.s_addr = htonl(serverIP);
		char msg[128] = { 0 };
		InetNtop(AF_INET, &(sockaddr.sin_addr.s_addr), msg, 128);
		if (connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(struct sockaddr)) == SOCKET_ERROR)
		{
			closesocket(sockfd);
			int m_LastError = WSAGetLastError();
			return INVALID_SOCKET;
		}
		return sockfd;
	}

	std::vector<std::string> split_string(const std::string& str,
		const std::string& delimiter, bool last = false)
	{
		std::vector<std::string> strings;

		std::string::size_type pos = 0;
		std::string::size_type prev = 0;
		while ((pos = str.find(delimiter, prev)) != std::string::npos)
		{
			strings.push_back(str.substr(prev, pos - prev));
			prev = pos + delimiter.size();
		}

		// To get the last substring (or only, if delimiter is not found)
		if (last) {
			strings.push_back(str.substr(prev));
		}
		return strings;
	}

	bool CFtpClient::readResponse(char* resCode, std::string& str)
	{
		char		msgBuf[4096 + 2] = { 0 };
		char		codeStr[4] = { 0 };
		str.clear();
		int		bytes = recv(m_commandsocket, msgBuf, 4096, 0);
		if (bytes <= 0)
		{
			return false;
		}
		msgBuf[bytes] = 0;
		
		

		std::string strBuf = msgBuf;
		std::vector<std::string> lines = split_string(strBuf, "\r\n");

		std::string msg = lines[lines.size() - 1];
		for (auto i : lines) {

			m_logger.log(1234, AALTOLogNoticeType::TRACE, "FTP Response: %s", i.c_str());
		}

		std::string numStr = msg.substr(0, 3);
		m_statusCode = std::stoi(numStr);
		m_desc = msg.substr(3, msg.size() - 3);
		memcpy(resCode, numStr.c_str(), 4);
		str = m_desc;
		return true;
	}

	void CFtpClient::initSocket()
	{
		//WSADATA		wsaData;
		//WSAStartup(MAKEWORD(2, 2), &wsaData);
		m_commandsocket = socket(AF_INET, SOCK_STREAM, 0);
		struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		//DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
		setsockopt(m_commandsocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	}


	bool CFtpClient::sendCommand(const char* cmd, const char* data)
	{
		std::string		sendData = cmd;
		if (data != nullptr) {
			sendData.append(" ").append(data);
		}
		m_logger.log(1234, AALTOLogNoticeType::TRACE, "FTP Send %s", sendData.c_str());
		sendData.append("\r\n");
		size_t bytes = send(m_commandsocket, sendData.c_str(), (int)sendData.length(), 0);

		

		if (bytes == SOCKET_ERROR) {
			//m_logger.log(1234, AALTOLogNoticeType::ERR, "Failed to send command - Data: %s ", sendData);
			return false;
		}
		return (bytes == sendData.length());
	}

	bool CFtpClient::getResponseResult(char code, std::string& respStr)
	{
		char respCode[4] = { 0 };
		std::this_thread::sleep_for(50ms);
		if (!readResponse(respCode, respStr))
		{
			return false;
		}
		if (respCode[0] != code)
		{
			m_logger.log(1234, AALTOLogNoticeType::ERR, "Failed to get a successfull responce - Code: %s Message: %s", respCode, respStr.c_str());
			return false;
		}
		return true;
	}

	bool CFtpClient::parsePasvData(uint32_t* prmServerIP, uint16_t* prmPort)
	{
		char	msg[128] = { 0 };
		std::string respStr;
		bool result = this->sendCommand("Type", "I");
		if (!result)
		{
			return false;
		}
		
		getResponseResult('2', respStr);
		result = sendCommand("PASV", "");
		getResponseResult('2', respStr);

		if (!result)
		{
			return false;
		}
		int		ip[4];
		int		port[2];
		//std::cout << 
		size_t		left = respStr.find("(");
		size_t		right = respStr.find(")");
		std::string  portStr = respStr.substr(left + 1, right - left - 1);
		sscanf_s(portStr.c_str(), "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1],
			&ip[2], &ip[3], &port[0], &port[1]);
		*prmPort = (port[0] * 256) + port[1]; // (p1 * 256) + p2

		*prmServerIP = (ip[0] * 256 * 256 * 256) + (ip[1] * 256 * 256) + (ip[2] * 256) + ip[3];

		InetNtop(AF_INET, prmServerIP, msg, 128);

		return true;
	}

	bool CFtpClient::openDataChannel(uint32_t serverIP, uint16_t port)
	{
		m_dataSocket = this->createSocket(serverIP, port);
		if (m_dataSocket == INVALID_SOCKET)
		{
			return false;
		}
		struct timeval timeout;
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		//DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
		setsockopt(m_dataSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
		return true;
	}

	int CFtpClient::parseListLines(char* msg, int bytes, std::vector<FtpFile*>& fileList)
	{
		int		curPos = 0;
		char* tokEndPos = nullptr;
		char* tokStartPos = msg;
		while ((tokEndPos = strstr(tokStartPos, "\r\n")) != nullptr)
		{
			*tokEndPos = 0;
			parseFtpFileInfo(tokStartPos, fileList);
			tokStartPos = tokEndPos + 2;
		}
		return tokStartPos - msg;
	}

	int CFtpClient::parseMLstLines(char* msg, int bytes, std::vector<FtpFile*>& fileList)
	{
		int		curPos = 0;
		char* tokEndPos = nullptr;
		char* tokStartPos = msg;
		while ((tokEndPos = strstr(tokStartPos, "\r\n")) != nullptr)
		{
			*tokEndPos = 0;
			parseFtpMLstFileInfo(tokStartPos, fileList);
			tokStartPos = tokEndPos + 2;
		}
		return tokStartPos - msg;
	}

	void CFtpClient::parseFtpMLstFileInfo(char* msg, std::vector<FtpFile*>& fileList)
	{
		std::string line = msg;

		std::vector<std::string> items = split_string(line, ";", true);
		if (items.size() != 5) {
			return;
		}
		std::string type = items[0].substr(5, items[0].length() - 5);
		std::string size = items[1].substr(5, items[1].length() - 5);
		std::string modify = items[2].substr(7, items[2].length() - 7);
		std::string perms = items[3].substr(5, items[3].length() - 5);
		std::string name = items[4].substr(1, items[4].length() - 1);

		int fileSize = std::stoi(size);
		TimeDate timeDate(modify.c_str());

		FtpFile* fileInfo = new FtpFile;
		fileInfo->m_isDir = (type.compare("file") != 0);
		fileInfo->m_fileSize = fileSize;
		strcpy(fileInfo->m_fileName, name.c_str());
		TimeDate mod(modify.c_str());
		fileInfo->m_mod = mod;
		fileList.push_back(fileInfo);

	}

	void CFtpClient::parseFtpFileInfo(char* msg, std::vector<FtpFile*>& fileList)
	{
		FtpFile* fileInfo = new FtpFile;
		if (msg[0] == '-')
		{
			fileInfo->m_isDir = FALSE;
		}
		else
		{
			fileInfo->m_isDir = TRUE;
		}

		char* tok = strchr(msg, ' ');
		tok = strchr(tok + 1, ' ');
		tok = strchr(tok + 1, ' ');
		tok = strchr(tok + 1, ' ');
		while (*tok == ' ')
		{
			tok++;
		}
		char* byteEnd = strchr(tok, ' ');
		*byteEnd = 0;
		fileInfo->m_fileSize = _atoi64(tok);

		tok = strchr(byteEnd + 1, ' ');
		//tok = strchr(tok + 1, ' ');
		tok = strchr(tok + 1, ' ');

		tok = strchr(tok + 1, ' ');
		memset(fileInfo->m_fileName, 0, sizeof(fileInfo->m_fileName));
		int	bytes = strlen(tok + 1) > MAX_PATH ? MAX_PATH : strlen(tok + 1);
		memcpy(fileInfo->m_fileName, tok + 1, bytes);
		fileList.push_back(fileInfo);
	}

	TimeDate::TimeDate(time_t t) {
		m_Time = t;
		time_t tt = m_Time;
		localtime_s(&m_timePtr, &tt);
	}

	TimeDate::TimeDate(const char* str) {
		std::string data = str;

		time_t rawtime;
		time(&rawtime);
		localtime_s(&m_timePtr, &rawtime);
		std::string tmp;
		tmp = data.substr(0, 4);
		m_timePtr.tm_year = strtol(tmp.c_str(), 0, 10);
		m_timePtr.tm_year = m_timePtr.tm_year - 1900;
		tmp = data.substr(4, 2);
		m_timePtr.tm_mon = strtol(tmp.c_str(), 0, 10);
		m_timePtr.tm_mon = m_timePtr.tm_mon - 1;
		tmp = data.substr(6, 2);
		m_timePtr.tm_mday = strtol(tmp.c_str(), 0, 10);
		tmp = data.substr(8, 2);
		m_timePtr.tm_hour = strtol(tmp.c_str(), 0, 10);
		tmp = data.substr(10, 2);
		m_timePtr.tm_min = strtol(tmp.c_str(), 0, 10);
		tmp = data.substr(12, 2);
		m_timePtr.tm_sec = strtol(tmp.c_str(), 0, 10);
		m_Time = mktime(&m_timePtr);
		size_t delim = data.find_first_of('.');
		if (delim != std::string::npos) {
			tmp = data.substr(delim + 1, data.length() - delim + 1);
			m_msec = strtol(tmp.c_str(), 0, 10);
		}
	}

	TimeDate::TimeDate(std::filesystem::file_time_type time)
	{
		auto ftsys = std::chrono::utc_clock::to_sys(std::chrono::file_clock::to_utc(time));
		m_Time = std::chrono::system_clock::to_time_t(ftsys);
		localtime_s(&m_timePtr, &m_Time);
	}

	bool TimeDate::operator==(const TimeDate& td) const
	{
		return (m_Time == td.m_Time);
	}

	bool TimeDate::operator<=(const TimeDate& td) const
	{
		return (m_Time <= td.m_Time);
	}

	void TimeDate::print()
	{
		printf("%d:%d:%d-%d/%d/%d", m_timePtr.tm_hour, m_timePtr.tm_min, m_timePtr.tm_sec,
			m_timePtr.tm_mday, m_timePtr.tm_mon + 1, m_timePtr.tm_year + 1900);
	}

}