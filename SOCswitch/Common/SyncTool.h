#pragma once
#include <string>
#include <memory>
#include <vector>

namespace Common
{
	class CFtpClient;
	class VirtualFileSystem;
	class RemoteInfoCache;
	class FtpSession;

	class SyncJob
	{
		std::string m_path;

	public:
		SyncJob(const char* path);
		~SyncJob() = default;

		const char* getFilepath() { return m_path.c_str(); };
	};
	class JobList : public std::vector<std::shared_ptr<SyncJob>> {};

	class SyncTool {
		bool newJob(const char* path);
		JobList m_list;
		std::shared_ptr<CFtpClient>	m_ftpclient{ nullptr };
		std::shared_ptr<FtpSession> m_session{ nullptr };
		std::shared_ptr<RemoteInfoCache> m_remoteInfoCache{ nullptr };
		std::string m_rootPath;
		int m_numFilesProcessed{ 0 };
	public:
		enum class Result
		{
			NewFile,
			NoChange,
			Changed,
			Error,
			Unknown
		};
		SyncTool();
		virtual ~SyncTool() = default;

		bool init(const char* user, const char* pw, const char* ip);
		bool tryRelogin();
		SyncTool::Result processFile(const char* path, std::shared_ptr<VirtualFileSystem> vfs);
		JobList& getJobList() { return m_list; };
		int getNumFilesProcessed() { return m_numFilesProcessed; };
		void resetNumFilesProcessed() { m_numFilesProcessed = 0; };
		bool doList();
	};
};