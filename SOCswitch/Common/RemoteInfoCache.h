#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include <filesystem>
#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <time.h>
#include "FtpClient.h"



namespace Common
{

	class FTpClient;

	enum class RemoteNodeType {
		File,
		Folder,
		Unknown
	};

	class RemoteNode {
	public:
		RemoteNode() = default;
		~RemoteNode() = default;

		virtual RemoteNodeType getType() = 0;
	};

	class RemoteFileInfo;
	class VirtualFileSystem;

	class RemoteFileNode : public RemoteNode {
		std::string m_filename;
		TimeDate m_modTime;
		uintmax_t m_size{ 0 };
		friend class RemoteFileInfo;
	public:
		RemoteFileNode(const char* filename, TimeDate mod, uintmax_t s)
			: m_filename(filename), m_modTime(mod), m_size(s) {};
		~RemoteFileNode() = default;
		RemoteNodeType getType() override { return RemoteNodeType::File; };
		void update(std::filesystem::file_time_type mod, uintmax_t s)
		{
			//m_modTime = mod;
			m_size = s;
		}
		TimeDate getModTime() { return m_modTime; };
		uintmax_t getSize() { return m_size; };
	};

	class RemoteFolderNode : public RemoteNode {
		
		std::string m_foldername;
		std::map<std::string, std::shared_ptr<RemoteNode>> m_folder;
		bool m_scanned{ false };
	public:
		RemoteFolderNode(const char* foldername) : m_foldername(foldername) {};
		~RemoteFolderNode() = default;
		
		RemoteNodeType getType() override { return RemoteNodeType::Folder; };
		std::map<std::string, std::shared_ptr<RemoteNode>>& getFolder() { return m_folder; };
		bool hasBeenScanned() { return m_scanned; };
		void scanned() { m_scanned = true; };
	};

	class RemoteInfo {
		bool m_error{ false };
	public:
		RemoteInfo() = default;
		~RemoteInfo() = default;

		virtual RemoteNodeType getType() = 0;
		void setError() { m_error = true; };
		bool isError() { return m_error; };
	};

	class RemoteFileInfo : public RemoteInfo {
		std::string m_filename;
		TimeDate m_modTime;
		uintmax_t m_size{ 0 };
		friend class RemoteInfoCache;
		RemoteFileInfo() {};
		RemoteFileInfo(const char* filename) : m_filename(filename) {};
		RemoteFileInfo(std::shared_ptr<RemoteFileNode> n) : m_filename(n->m_filename), m_modTime(n->m_modTime), m_size(n->m_size) {};
	public:

		~RemoteFileInfo() = default;
		RemoteNodeType getType() override { return RemoteNodeType::File; };
		bool isChanged(std::filesystem::path& p) const;
	};

	class RemoteFolderInfo : public RemoteInfo {
		std::string m_filename;
	public:
		RemoteFolderInfo(const char* filename) : m_filename(filename) {};
		~RemoteFolderInfo() = default;
		RemoteNodeType getType() override { return RemoteNodeType::File; };
	};

	class RemoteInfoCache
	{

		std::shared_ptr<CFtpClient>	m_ftpclient;
		void init();
		std::shared_ptr<RemoteFolderNode> m_rootFolder;

		std::shared_ptr<RemoteFolderNode> insertDirectory(std::string& path);

	public:
		RemoteInfoCache(std::shared_ptr<CFtpClient> ftpclient);
		~RemoteInfoCache() = default;

		bool insertDirectory(const char* path);
		bool insertFile(const char* parentPath, const char* filename, std::filesystem::file_time_type time, uintmax_t size);

		bool findDirectory(const char* path);
		bool findFile(const char* path);
		std::shared_ptr<RemoteFileNode> getFile(const char* path);
		RemoteFileInfo getFileInfo(const char* path);
		bool updateFile(const char* path);

		bool listFolder(std::vector<std::string>& list, const char* path);
		void scanNode(std::string& path,std::shared_ptr<RemoteFolderNode>& folder);
	};

};