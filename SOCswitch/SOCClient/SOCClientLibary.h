#pragma once
#include <string>
#include <Ws2tcpip.h>

class SOCClientLibary
{
	static std::string  m_configPath;
	static std::string  m_homePath;
	static bool m_configured;
	static void setConfigured() { m_configured = true; };
	static void setConfigPath(const char* configPath) { m_configPath = configPath; }
	WSAData data;
public:
	SOCClientLibary() = default;
	~SOCClientLibary() = default;
	
	static bool initalise();

	static bool InitRunFTPSync(const char* user = nullptr, const char* pw = nullptr, const char* ip = nullptr);
	static bool FTPSyncFolder(const char* folder);
	static bool startFTPSync();

	static bool stopFTPSync();

	static bool enableUDPEvents();

	static bool shutdown();
};

