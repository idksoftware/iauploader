#pragma once
#include "DirectoryVisitor.h"
#include "VirtualFileSystem.h"
#include "SyncTool.h"
#include "Logger.h"

class VirtualFileSystem;
namespace Common
{
	class FolderDir : public FolderVisitor {

		static std::shared_ptr<VirtualFileSystem> m_virtualFileSystem;
		std::string m_path;
		SyncTool& m_syncTool;
	protected:
		virtual bool onStart(const char* path) {
			
			Logger& logger = Logger::getLogger();
			logger.log(LOG_OK, AALTOLogNoticeType::FINE, "Starting reading folder: \"%s\"", path);
			//printf("Starting reading folder: \"%s\"\n", path);
			return true;//m_importList->onStart(path);
		};

		virtual bool onFile(const char* path) {
			Logger& logger = Logger::getLogger();
			logger.log(LOG_OK, AALTOLogNoticeType::FINE, "Reading file: %s", path);
			SyncTool::Result res = m_syncTool.processFile(path, m_virtualFileSystem);
			switch (res)
			{
			case SyncTool::Result::Changed:
				logger.log(LOG_OK, AALTOLogNoticeType::INFO, "file changed: \"%s\"", path);
				break;
			case SyncTool::Result::NewFile:
				logger.log(LOG_OK, AALTOLogNoticeType::INFO, "new file: \"%s\"", path);
				break;
			}
			//std::string tmp = path;
			//std::string filename = SAUtils::getFilename(tmp);
			logger.log(LOG_OK, AALTOLogNoticeType::FINE, "File: \"%s\"", path);
			//printf( "file: \"%s\"\n", path);
			return true;
		};
		virtual bool onDirectory(const char* path) {
			Logger& logger = Logger::getLogger();
			logger.log(LOG_OK, AALTOLogNoticeType::FINE, "Now reading folder:  %s", path);
			m_path = path;
			//printf("Now reading folder:  %s\n", path);
			m_virtualFileSystem->insertDirectory(path);
			return true;
		};
		virtual bool onEnd() {
			Logger& logger = Logger::getLogger();
			logger.log(LOG_OK, AALTOLogNoticeType::FINE, "Completed reading folder: %s", m_path.c_str());
			//printf("Completed reading folder\n");
			return true;//m_importList->onEnd();
		};
		virtual bool onError(const char* error) {
			Logger& logger = Logger::getLogger();
			logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Completed reading folder: %s", m_path.c_str());
			logger.log(LOG_OK, AALTOLogNoticeType::ERR, error);
			return true; // m_importList->onEnd();
		};
		virtual std::shared_ptr<FolderVisitor> make() {
			return (std::make_shared<FolderDir>(m_syncTool));
		}
	public:
		FolderDir(SyncTool& syncTool);
		virtual ~FolderDir() {};
		//std::shared_ptr<ImageSets> getImageSets() {
		//	return m_importList->getImageSets();
		//}
		static void destroy() {
			//m_importList = nullptr;
		}

		int getFileCount() {
			//return m_importList->getFileCount();
			return 0;
		}

		int getFolderCount() {
			//return m_importList->getFolderCount();
			return 0;
		}
		std::shared_ptr<VirtualFileSystem> getVFS() { return m_virtualFileSystem; };
	};

	class TargetsList
	{

		std::shared_ptr<FolderDir> m_folderDir;
	public:
		TargetsList(SyncTool& syncTool) : m_folderDir(std::make_shared<FolderDir>(syncTool)) {}
		virtual ~TargetsList() = default;

		/// This function processes all the folders under the root folder and creates
		/// a number of Images sets, one per folder found.
		bool process(const char* path);

		/// This returns the resulting Image Sets 
		std::shared_ptr<FolderDir> getVFS() { return m_folderDir; };

	};
};