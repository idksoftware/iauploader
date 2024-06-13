#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Common
{
	class FolderItem;
	class DirNode;
	class DirectoryVisitor;
	/// This class is used by the DirectoryVisitor. The DirectoryVisitor class will
	/// call the make function to make FolderVisitor objects when required.
	/// This must be used as a base class for new classes that the DirectoryVisitor uses to
	/// process folders the DirectoryVisitor class visits.
	///
	class FolderVisitor {
		friend class DirectoryVisitor;
		friend class DirNode;
	protected:
		virtual bool onStart(const char* path) { return true; };
		virtual bool onFile(const char* path) { return true; };
		virtual bool onDirectory(const char* path) { return true; };
		virtual bool onEnd() { return true; };
		virtual bool onError(const char* what) { return true; };
		virtual std::shared_ptr<FolderVisitor> make() { return (std::make_shared<FolderVisitor>()); }
	public:

		FolderVisitor() {};
		virtual ~FolderVisitor() {};
	};

	class DirectoryVisitor {
		std::shared_ptr<DirNode> m_dirNode;
		std::shared_ptr<FolderVisitor> m_folderVisitor;
		bool m_deleteFolderVisitor;
	public:
		/// Constructor
		/// @parm folderVisitor - pointer to FolderVisitor
		DirectoryVisitor(std::shared_ptr<FolderVisitor> folderVisitor, bool val = true);

		// Destructor
		virtual ~DirectoryVisitor();
		/// This Function processes the files under the root using the
		/// FolderVisitor class passed in the constructor
		bool process(const char* rootFolder);
		void deleteFolderVisitor(bool val = true) {
			m_deleteFolderVisitor = val;
		}
	};
};

