#pragma once

//#include <windows.h>
//#include <WinSock.h>
#include <Ws2tcpip.h>
#include <string>
#include <vector>
#include <filesystem>

namespace Common
{
	class TimeDate {

		time_t m_Time;
		tm m_timePtr;
		int m_msec{ -1 };
	public:
		TimeDate() = default;
		TimeDate(time_t t);
		TimeDate(const char* str);
		TimeDate(std::filesystem::file_time_type time);
		virtual ~TimeDate() = default;

		bool operator==(const TimeDate& td) const;
		bool operator<=(const TimeDate& td) const;

		int getSec() const {
			return m_timePtr.tm_sec;
		}

		int getMin() const {
			return m_timePtr.tm_min;
		}

		int getHour() const {
			return m_timePtr.tm_hour;
		}

		int getDay() const {
			return m_timePtr.tm_mday;
		}

		int getMonth() const {
			return m_timePtr.tm_mon + 1;
		}

		int getYear() const {
			return m_timePtr.tm_year + 1900;
		}
		void print();
	};


	typedef struct
	{
		bool	m_isDir;
		char	m_fileName[MAX_PATH + 2];
		__int64	m_fileSize;
		TimeDate m_mod;
	}FtpFile;

	class Logger;

	class CFtpClient
	{

		Logger& m_logger;
	public:
		CFtpClient();
		~CFtpClient();

		bool		connectToHost(const std::string& serverIP, int serverPort = 21);
		bool		login(const std::string& user, const std::string& password);
		bool		cd(const std::string& path);
		bool		deleteFile(const std::string& path);
		bool		rmdir(const std::string& path);
		bool		mkdir(const std::string& path);
		std::string		pwd();
		bool		list(const std::string& path, std::vector<FtpFile*>& fileList);
		bool		mlist(const std::string& path, std::vector<FtpFile*>& fileList);
		bool		put(const std::string& remoteFile, const std::string& localFile);
		void		get(const std::string& remoteFile, const std::string& localFile);
		bool		quit();
		bool		sendRawCommand(const char* cmd, const char* data, char expectCode);
		int			getStatusCode() { return m_statusCode; }
		std::string		getDesc() { return m_desc; }

	private:

		SOCKET	createSocket(uint32_t serverIP, uint16_t port);
		bool	readResponse(char* resCode, std::string& str);
		void	initSocket();
		bool	sendCommand(const char* cmd, const char* data);
		bool	getResponseResult(char code, std::string& respStr);
		bool	parsePasvData(uint32_t* prmServerIP, uint16_t* prmPort);
		bool	openDataChannel(uint32_t serverIP, uint16_t port);
		int		parseListLines(char* msg, int bytes, std::vector<FtpFile*>& fileList);
		int		parseMLstLines(char* msg, int bytes, std::vector<FtpFile*>& fileList);
		void	parseFtpFileInfo(char* msg, std::vector<FtpFile*>& fileList);
		void	parseFtpMLstFileInfo(char* msg, std::vector<FtpFile*>& fileList);

		SOCKET	m_commandsocket;
		SOCKET	m_dataSocket{ INVALID_SOCKET };
		int 	m_statusCode;
		std::string  m_desc;
	};
};

