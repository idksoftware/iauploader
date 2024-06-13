#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include <filesystem>
#include <string>
#include <chrono>


namespace Common
{
	enum class VFSNodeType {
		File,
		Folder,
		Unknown
	};

	class VFSNode {
	public:
		VFSNode() = default;
		~VFSNode() = default;

		virtual VFSNodeType getType() = 0;
	};

	class VFSFileInfo;
	class VirtualFileSystem;

	class VFSFileNode : public VFSNode {
		std::string m_filename;
		std::filesystem::file_time_type m_modTime;
		uintmax_t m_size{ 0 };
		friend class VFSFileInfo;
	public:
		VFSFileNode(const char* filename, std::filesystem::file_time_type mod, uintmax_t s)
			: m_filename(filename), m_modTime(mod), m_size(s) {};
		~VFSFileNode() = default;
		VFSNodeType getType() override { return VFSNodeType::File; };
		void update(std::filesystem::file_time_type mod, uintmax_t s)
		{
			m_modTime = mod;
			m_size = s;
		}
	};

	class VFSFolderNode : public VFSNode {
		uint64_t m_vfsFolderID;
		std::string m_foldername;
		std::map<std::string, std::shared_ptr<VFSNode>> m_folder;

	public:
		VFSFolderNode(uint64_t vfsFolderID, const char* foldername) : m_foldername(foldername), m_vfsFolderID(vfsFolderID) {};
		VFSFolderNode(const char* foldername) : m_foldername(foldername) {};
		~VFSFolderNode() = default;
		uint64_t getID() { return m_vfsFolderID; };
		VFSNodeType getType() override { return VFSNodeType::Folder; };
		std::map<std::string, std::shared_ptr<VFSNode>>& getFolder() { return m_folder; };
	};

	class VFSInfo {
		bool m_error{ false };
	public:
		VFSInfo() = default;
		~VFSInfo() = default;

		virtual VFSNodeType getType() = 0;
		void setError() { m_error = true; };
		bool isError() { return m_error; };
	};

	class VFSFileInfo : public VFSInfo {
		std::string m_filename;
		std::filesystem::file_time_type m_modTime;
		uintmax_t m_size{ 0 };
		friend class VirtualFileSystem;
		VFSFileInfo() {};
		VFSFileInfo(const char* filename) : m_filename(filename) {};
		VFSFileInfo(std::shared_ptr<VFSFileNode> n) : m_filename(n->m_filename), m_modTime(n->m_modTime), m_size(n->m_size) {};
	public:

		~VFSFileInfo() = default;
		VFSNodeType getType() override { return VFSNodeType::File; };
		bool isChanged(std::filesystem::path& p) const;
	};

	class VFSFolderInfo : public VFSInfo {
		std::string m_filename;
	public:
		VFSFolderInfo(const char* filename) : m_filename(filename) {};
		~VFSFolderInfo() = default;
		VFSNodeType getType() override { return VFSNodeType::File; };
	};

	class VirtualFileSystem
	{
		std::map<uint64_t, std::shared_ptr<VFSFolderNode>> m_folders;
		static uint64_t m_sequenceNumber;

		void init();
		std::map<std::string, std::shared_ptr<VFSNode>> m_rootFolder;

		std::shared_ptr<VFSFolderNode> insertDirectory(const std::string& path);

	public:
		VirtualFileSystem() = default;
		~VirtualFileSystem() = default;

		bool insertDirectory(const char* path);
		bool insertFile(const char* path);

		bool findDirectory(const char* path);
		bool findFile(const char* path);
		std::shared_ptr<VFSFileNode> getFile(const char* path);
		VFSFileInfo getFileInfo(const char* path);
		bool updateFile(const char* path);

		bool listFolder(std::vector<std::string>& list, const char* path);

	};

};