#include "SyncWorker.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "Logger.h"
#include "FtpClient.h"
#include "TargetsList.h"
#include "VirtualFileSystem.h"
#include "SyncTool.h"

using namespace std::chrono_literals;

namespace Common
{

	bool SyncWorker::m_running = true;
	std::string SyncWorker::m_user;
	std::string SyncWorker::m_password;
	std::string SyncWorker::m_ip;
	std::vector<std::string> SyncWorker::m_folders;

	SyncWorker::SyncWorker()
	{

	}

	void SyncWorker::init(const char* user, const char* pw, const char* ip)
	{
		m_user = user;
		m_password = pw;
		m_ip = ip;
		
	}

	bool SyncWorker::addFolder(const char* folder)
	{
		m_folders.push_back(folder);
		// check valid folder
		return true;
	}

	bool SyncWorker::Start()
	{
		m_running = true;
		Logger& logger = Logger::getLogger();

		SyncTool syncTool;
		if (syncTool.init(m_user.c_str(), m_password.c_str(), m_ip.c_str()) == false)
		{
			logger.log(1234, AALTOLogNoticeType::FATAL, "Cannot start an FTP session");
			return false;
		}

		for (auto folder : m_folders) 
		{
			TargetsList targetsList(syncTool);
			logger.log(1234, AALTOLogNoticeType::STATUS, "Processing local files");
			if (targetsList.process(folder.c_str()) == false) {
				return false;
			}
			logger.log(1234, AALTOLogNoticeType::STATUS, "Local files processed: %d", syncTool.getNumFilesProcessed());
			syncTool.resetNumFilesProcessed();
			syncTool.doList();
		}
		return true;
	}


};