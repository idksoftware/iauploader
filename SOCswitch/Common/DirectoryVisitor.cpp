#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <sstream>


#if defined  _WIN64 || defined WIN32
//#include <sysstat.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <filesystem>
#include "DirectoryVisitor.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif
namespace Common
{
	/**
	 * This class is the directory node that is used to
	 * create a folder path that the DirectoryVisitor uses
	 * to navigate the directory
	 */
	class DirNode {
		std::string m_dirpath;
		std::shared_ptr<DirNode> m_dirNode;
		std::shared_ptr<FolderVisitor> m_folderVisitor;
		std::string m_errorStr;
		bool m_error{ false };

	public:
		DirNode(std::shared_ptr<DirNode> dirNode, const char* dirpath) {
			m_dirNode = dirNode;
			m_dirpath = dirpath;
			m_folderVisitor = nullptr;
		}

		DirNode(std::shared_ptr<DirNode> dirNode, const char* dirpath, std::shared_ptr<FolderVisitor> folderVisitor) {
			m_dirNode = dirNode;
			m_dirpath = dirpath;
			m_folderVisitor = folderVisitor;
			m_folderVisitor->onStart(dirpath);
		}
		DirNode(const DirNode& x) {
		}

		~DirNode() {}

		bool process();

		std::string& getErrorString() { return m_errorStr; }
		bool isError() { return m_error; };
	};



	DirectoryVisitor::DirectoryVisitor(std::shared_ptr<FolderVisitor> folderVisitor, bool val) {

		m_dirNode = nullptr;
		m_folderVisitor = folderVisitor;
		m_deleteFolderVisitor = val;

	}

	DirectoryVisitor::~DirectoryVisitor() {}


	bool DirectoryVisitor::process(const char* rf) {
		std::string rootFolder = rf;
		if (m_folderVisitor != 0) {
			m_dirNode = std::make_shared<DirNode>(nullptr, rootFolder.c_str(), m_folderVisitor->make());
		}
		else {
			m_dirNode = std::make_shared<DirNode>(nullptr, rootFolder.c_str());
		}
		if (m_dirNode->process() == false) {
			return false;
		}
		return true;
	}

	bool DirNode::process()
	{
		std::string path = m_dirpath;
		try {
			for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
			{

				std::filesystem::path p = dir_entry.path();
				std::string filename = p.string();

				if (std::filesystem::is_regular_file(p) == true) {
					if (m_folderVisitor) {
						m_folderVisitor->onFile(filename.c_str());
					}
				}
				else {
					if (m_folderVisitor) {
						m_folderVisitor->onDirectory(filename.c_str());
					}
					if (m_folderVisitor != 0) {
						m_dirNode = std::make_shared<DirNode>(nullptr, filename.c_str(), m_folderVisitor->make());
					}
					else {
						m_dirNode = std::make_shared<DirNode>(nullptr, filename.c_str());
					}
					m_dirNode->process();
				}
			}
			m_folderVisitor->onEnd();
		}
		catch (std::exception& e) {

			m_errorStr = e.what();
			m_error = true;
			std::string errorStr = "Error in path: \"" + path + "\" " + m_errorStr;
			m_folderVisitor->onError(errorStr.c_str());
			return false;
		}
		return true;
	}
};
