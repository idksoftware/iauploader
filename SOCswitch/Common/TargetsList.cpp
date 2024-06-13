#include "TargetsList.h"
#include "VirtualFileSystem.h"
#include "SyncTool.h"

namespace Common
{
	std::shared_ptr<VirtualFileSystem> FolderDir::m_virtualFileSystem = nullptr;

	FolderDir::FolderDir(SyncTool& syncTool) : m_syncTool(syncTool) {

		if (m_virtualFileSystem == nullptr) {
			m_virtualFileSystem = std::make_shared<VirtualFileSystem>();
		}
	};

	bool TargetsList::process(const char* path) {

		DirectoryVisitor directoryVisitor(m_folderDir);
		if (directoryVisitor.process(path) == false) {
			return false;
		}
		return true;
	}

};