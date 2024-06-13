#include "RemoteInfoCache.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include "FtpClient.h"
#include <chrono>

namespace Common
{

    static int splitPath(std::vector<std::string>& directories, const char* path)
    {
        std::string filePath = path;
        size_t position = 0, currentPosition = 0;

        while (currentPosition != -1)
        {
            currentPosition = filePath.find_first_of('/', position);
            directories.push_back(filePath.substr(position, currentPosition - position));
            position = currentPosition + 1;
        }


        return 0;
    }


    bool RemoteFileInfo::isChanged(std::filesystem::path& p) const
    {
        std::string name = p.filename().string();
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        if (size != m_size)
        {
            return true;
        }
       
        TimeDate tdMod(modTime);

        if (tdMod == m_modTime)
        {
            return true;
        }
        return false;
    }

    void RemoteInfoCache::init()
    {


    }

    bool RemoteInfoCache::insertDirectory(const char* path)
    {
        std::string p = path;
        std::shared_ptr<RemoteFolderNode> currentDirNode = insertDirectory(p);

        if (currentDirNode == nullptr)
        {
            return false;
        }
        return true;
    }

    void RemoteInfoCache::scanNode(std::string& path, std::shared_ptr<RemoteFolderNode>& folder)
    {
        std::vector<FtpFile*> fileList;
        if (m_ftpclient->list(path.c_str(), fileList) == false) {
            //logger.log(LOG_OK, AALTOLogNoticeType::ERR, "Ftp failed listing Folder: %s", folder.c_str());
        }
        //logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Ftp listing Folder: %s", folder.c_str());
        for (size_t i = 0; i < fileList.size(); i++)
        {
            if (fileList[i]->m_isDir) {
                std::shared_ptr<RemoteFolderNode> node = std::make_shared<RemoteFolderNode>(fileList[i]->m_fileName);
                folder->getFolder()[fileList[i]->m_fileName] = node;
            }
        }
        fileList.clear();
        if (m_ftpclient->mlist(path.c_str(), fileList) == false) {
            //logger.log(LOG_OK, AALTOLogNoticeType::ERR, "Ftp failed listing Folder: %s", folder.c_str());
        }
        //logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Ftp listing Folder: %s", folder.c_str());
        for (size_t i = 0; i < fileList.size(); i++)
        {
            std::shared_ptr<RemoteFileNode> node = std::make_shared<RemoteFileNode>(fileList[i]->m_fileName, fileList[i]->m_mod, fileList[i]->m_fileSize);
            folder->getFolder()[fileList[i]->m_fileName] = node;
            //delete fileList[i];
        }
        fileList.clear();
        folder->scanned();
    }

    std::shared_ptr<RemoteFolderNode> RemoteInfoCache::insertDirectory(std::string& path)
    {

        if (path.empty()) {
            path = "/";
            if (!m_rootFolder->hasBeenScanned()) {
                scanNode(path, m_rootFolder);
            }
            return m_rootFolder;
        }
        std::string curpath = ".";
        std::vector<std::string> directories;
        splitPath(directories, path.c_str());
        bool first = true;
        std::shared_ptr<RemoteFolderNode> currentNode = nullptr;
        for (auto it = directories.begin(); it != directories.end(); it++) {

            std::string name = *it;
            if (name.empty()) {
                continue;
            }
            if (first) {
                auto it = m_rootFolder->getFolder().find(name);
                if (it == m_rootFolder->getFolder().end())
                {
                    std::shared_ptr<RemoteFolderNode> node = std::make_shared<RemoteFolderNode>(name.c_str());
                    m_rootFolder->getFolder()[name] = node;
                    if (m_ftpclient->mkdir(name) == false) {
                        // error
                    }
                    if (m_ftpclient->cd(name) == false) {
                        // error
                    }
                    currentNode = node;
                }
                else {
                    if (RemoteNodeType::Folder != it->second->getType())
                    {
                        return nullptr;
                    }
                    if (m_ftpclient->cd(name) == false) {
                        // error
                    }
                    currentNode = std::static_pointer_cast<RemoteFolderNode>(it->second);
                    scanNode(curpath, currentNode);
                }

                first = false;
            }
            else {
                if (RemoteNodeType::Folder != currentNode->getType())
                {
                    return nullptr;
                }
                std::shared_ptr<RemoteFolderNode> folderNode = std::static_pointer_cast<RemoteFolderNode>(currentNode);
                std::map<std::string, std::shared_ptr<RemoteNode>>& folders = folderNode->getFolder();
                auto it = folders.find(name);
                if (it == folders.end())
                {
                    std::shared_ptr<RemoteFolderNode> node = std::make_shared<RemoteFolderNode>(name.c_str());
                    std::map<std::string, std::shared_ptr<RemoteNode>>& folders = currentNode->getFolder();
                    folders[name] = node;
                    if (m_ftpclient->mkdir(name) == false) {
                        // error
                    }
                    if (m_ftpclient->cd(name) == false) {
                        // error
                    }

                    currentNode = node;
                }
                else
                {
                    currentNode = std::static_pointer_cast<RemoteFolderNode>(it->second);
                    if (m_ftpclient->cd(name) == false) {
                        // error
                    }

                    scanNode(curpath, currentNode);
                }
            }

        }
        if (m_ftpclient->cd("/") == false) {
            // error
        }

        return currentNode;
    }

    RemoteInfoCache::RemoteInfoCache(std::shared_ptr<CFtpClient> ftpclient)
    {
        m_ftpclient = ftpclient;
        m_rootFolder = std::make_shared<RemoteFolderNode>("/");
    }

    bool RemoteInfoCache::insertFile(const char* parentPath, const char* filename, std::filesystem::file_time_type time, uintmax_t size)
    {
        std::string path = parentPath;
        std::shared_ptr<RemoteFolderNode> currentDirNode = insertDirectory(path);
        std::map<std::string, std::shared_ptr<RemoteNode>>& folders = currentDirNode->getFolder();
        TimeDate modLocal(time);
        auto it = folders.find(filename);
        if (it == folders.end())
        {
            std::shared_ptr<RemoteFileNode> node = std::make_shared<RemoteFileNode>(filename, modLocal, size);
            folders[filename] = node;
        }
        else {
            RemoteNodeType type = it->second->getType();
            if (type != RemoteNodeType::File) {
                return false;
            }
            std::shared_ptr<RemoteNode> n = it->second;
            std::shared_ptr<RemoteFileNode> fileNode = std::static_pointer_cast<RemoteFileNode>(n);
            if (fileNode->getModTime() <= modLocal) {
                return true;
            }
            if (fileNode->getSize() != size) {
                return true;
            }
        }
        return false;
    }

    bool RemoteInfoCache::findDirectory(const char* path)
    {
        if (std::filesystem::is_directory(path) == false)
        {
            return false;
        }

        std::vector<std::string> directories;
        splitPath(directories, path);
        bool first = true;
        std::shared_ptr<RemoteNode> currentNode = nullptr;
        for (auto it = directories.begin(); it != directories.end(); it++) {
            std::string name = *it;

            //  if(m.find(s1)!=m.end()){
            if (first) {
                auto it = m_rootFolder->getFolder().find(name);
                if (it == m_rootFolder->getFolder().end())
                {
                    return false;
                }
                currentNode = it->second;
                first = false;
            }
            else {
                if (RemoteNodeType::Folder != currentNode->getType())
                {
                    return false;
                }
                std::shared_ptr<RemoteFolderNode> folderNode = std::static_pointer_cast<RemoteFolderNode>(currentNode);
                std::map<std::string, std::shared_ptr<RemoteNode>>& folders = folderNode->getFolder();
                auto it = folders.find(name);
                if (it == folders.end())
                {
                    return false;
                }
                currentNode = it->second;

            }

        }

        return true;
    }

    bool RemoteInfoCache::findFile(const char* path)
    {
        if (std::filesystem::is_regular_file(path) == false)
        {
            return false;
        }
        std::filesystem::path p = path;
        std::string name = p.filename().string();
        std::string parent = p.parent_path().string();
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        std::shared_ptr<RemoteFolderNode> currentDirNode = insertDirectory(parent);
        std::map<std::string, std::shared_ptr<RemoteNode>>& folders = currentDirNode->getFolder();
        auto it = folders.find(name);
        if (it == folders.end())
        {
            return false;
        }
        return true;
    }

    std::shared_ptr<RemoteFileNode> RemoteInfoCache::getFile(const char* path)
    {
        if (std::filesystem::is_regular_file(path) == false)
        {
            return nullptr;
        }
        std::filesystem::path p = path;
        std::string name = p.filename().string();
        std::string parent = p.parent_path().string();
        std::shared_ptr<RemoteFolderNode> currentDirNode = insertDirectory(parent);
        std::map<std::string, std::shared_ptr<RemoteNode>>& folders = currentDirNode->getFolder();
        auto it = folders.find(name);
        if (it == folders.end())
        {
            return nullptr;
        }
        if (RemoteNodeType::File != it->second->getType())
        {
            return nullptr;
        }
        std::shared_ptr<RemoteFileNode> fileNode = std::static_pointer_cast<RemoteFileNode>(it->second);
        return fileNode;
    }

    bool RemoteInfoCache::updateFile(const char* path)
    {
        std::shared_ptr<RemoteFileNode> fileNode = getFile(path);
        if (fileNode == nullptr)
        {
            return false;
        }
        std::filesystem::path p = path;
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        fileNode->update(modTime, size);
        return true;
    }

    RemoteFileInfo RemoteInfoCache::getFileInfo(const char* path)
    {
        std::shared_ptr<RemoteFileNode> fileNode = getFile(path);
        RemoteFileInfo RemoteFileInfo(fileNode);
        return RemoteFileInfo;
    }

    bool RemoteInfoCache::listFolder(std::vector < std::string>& list, const char* path)
    {
        if (std::filesystem::is_directory(path) == false)
        {
            return false;
        }
        std::string p = path;
        std::shared_ptr<RemoteFolderNode> currentDirNode = insertDirectory(p);
        std::map<std::string, std::shared_ptr<RemoteNode>>& folders = currentDirNode->getFolder();
        for (auto it : folders)
        {
            list.push_back(it.first);
        }
        return  true;
    }
}