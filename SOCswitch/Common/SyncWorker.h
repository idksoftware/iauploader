#pragma once
#include <memory>
#include <string>
#include <vector>


namespace Common
{
	class SyncWorker
	{
		static bool m_running;
		static std::string m_user;
		static std::string m_password;
		static std::string m_ip;
		
		static std::vector<std::string> m_folders;
	public:
		SyncWorker();

		~SyncWorker() = default;
		static void init(const char* user, const char* pw, const char* ip);
		static bool addFolder(const char* folder);
		static bool Start();
		static void Stop() { m_running = false; };
	};

};