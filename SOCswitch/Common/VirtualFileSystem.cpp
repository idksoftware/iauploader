#include "VirtualFileSystem.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>


namespace Common
{

    int splitPath(std::vector<std::string>& directories, const char* path)
    {
        std::string filePath = path;
        size_t position = 0, currentPosition = 0;

        while (currentPosition != -1)
        {
            currentPosition = filePath.find_first_of('\\', position);
            directories.push_back(filePath.substr(position, currentPosition - position));
            position = currentPosition + 1;
        }


        return 0;
    }

    uint64_t VirtualFileSystem::m_sequenceNumber = 0;


    bool VFSFileInfo::isChanged(std::filesystem::path& p) const
    {
        std::string name = p.filename().string();
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        if (size != m_size)
        {
            return true;
        }
        if (modTime != m_modTime)
        {
            return true;
        }
        return false;
    }

    void VirtualFileSystem::init()
    {


    }

    bool VirtualFileSystem::insertDirectory(const char* path)
    {
        std::string p = path;
        std::shared_ptr<VFSFolderNode> currentDirNode = insertDirectory(p);

        if (currentDirNode == nullptr)
        {
            return false;
        }
        return true;
    }

    std::shared_ptr<VFSFolderNode> VirtualFileSystem::insertDirectory(const std::string& path)
    {


        std::vector<std::string> directories;
        splitPath(directories, path.c_str());
        bool first = true;
        std::shared_ptr<VFSFolderNode> currentNode = nullptr;
        for (auto it = directories.begin(); it != directories.end(); it++) {

            uint64_t sequenceNumber = m_sequenceNumber++;
            std::string name = *it;


            if (first) {
                auto it = m_rootFolder.find(name);
                if (it == m_rootFolder.end())
                {
                    std::shared_ptr<VFSFolderNode> node = std::make_shared<VFSFolderNode>(name.c_str());
                    m_rootFolder[name] = node;
                    currentNode = node;
                }
                else {
                    if (VFSNodeType::Folder != it->second->getType())
                    {
                        return nullptr;
                    }
                    currentNode = std::static_pointer_cast<VFSFolderNode>(it->second);
                }

                first = false;
            }
            else {
                if (VFSNodeType::Folder != currentNode->getType())
                {
                    return nullptr;
                }
                std::shared_ptr<VFSFolderNode> folderNode = std::static_pointer_cast<VFSFolderNode>(currentNode);
                std::map<std::string, std::shared_ptr<VFSNode>>& folders = folderNode->getFolder();
                auto it = folders.find(name);
                if (it == folders.end())
                {
                    std::shared_ptr<VFSFolderNode> node = std::make_shared<VFSFolderNode>(name.c_str());
                    std::map<std::string, std::shared_ptr<VFSNode>>& folders = currentNode->getFolder();
                    folders[name] = node;
                    currentNode = node;
                }
                else
                {
                    currentNode = std::static_pointer_cast<VFSFolderNode>(it->second);
                }
            }

        }

        return currentNode;
    }

    bool VirtualFileSystem::insertFile(const char* path)
    {
        if (std::filesystem::is_regular_file(path) == false)
        {
            return false;
        }
        std::filesystem::path p = path;
        std::string name = p.filename().string();
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        std::string parent = p.parent_path().string();
        std::shared_ptr<VFSFolderNode> currentDirNode = insertDirectory(parent);
        std::shared_ptr<VFSFileNode> node = std::make_shared<VFSFileNode>(name.c_str(), modTime, size);
        std::map<std::string, std::shared_ptr<VFSNode>>& folders = currentDirNode->getFolder();
        folders[name] = node;
        return true;
    }

    bool VirtualFileSystem::findDirectory(const char* path)
    {
        if (std::filesystem::is_directory(path) == false)
        {
            return false;
        }

        std::vector<std::string> directories;
        splitPath(directories, path);
        bool first = true;
        std::shared_ptr<VFSNode> currentNode = nullptr;
        for (auto it = directories.begin(); it != directories.end(); it++) {

            uint64_t sequenceNumber = m_sequenceNumber++;
            std::string name = *it;

            //  if(m.find(s1)!=m.end()){
            if (first) {
                auto it = m_rootFolder.find(name);
                if (it == m_rootFolder.end())
                {
                    return false;
                }
                currentNode = it->second;
                first = false;
            }
            else {
                if (VFSNodeType::Folder != currentNode->getType())
                {
                    return false;
                }
                std::shared_ptr<VFSFolderNode> folderNode = std::static_pointer_cast<VFSFolderNode>(currentNode);
                std::map<std::string, std::shared_ptr<VFSNode>>& folders = folderNode->getFolder();
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

    bool VirtualFileSystem::findFile(const char* path)
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
        std::shared_ptr<VFSFolderNode> currentDirNode = insertDirectory(parent);
        std::map<std::string, std::shared_ptr<VFSNode>>& folders = currentDirNode->getFolder();
        auto it = folders.find(name);
        if (it == folders.end())
        {
            return false;
        }
        return true;
    }

    std::shared_ptr<VFSFileNode> VirtualFileSystem::getFile(const char* path)
    {
        if (std::filesystem::is_regular_file(path) == false)
        {
            return nullptr;
        }
        std::filesystem::path p = path;
        std::string name = p.filename().string();
        std::string parent = p.parent_path().string();
        std::shared_ptr<VFSFolderNode> currentDirNode = insertDirectory(parent);
        std::map<std::string, std::shared_ptr<VFSNode>>& folders = currentDirNode->getFolder();
        auto it = folders.find(name);
        if (it == folders.end())
        {
            return nullptr;
        }
        if (VFSNodeType::File != it->second->getType())
        {
            return nullptr;
        }
        std::shared_ptr<VFSFileNode> fileNode = std::static_pointer_cast<VFSFileNode>(it->second);
        return fileNode;
    }

    bool VirtualFileSystem::updateFile(const char* path)
    {
        std::shared_ptr<VFSFileNode> fileNode = getFile(path);
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

    VFSFileInfo VirtualFileSystem::getFileInfo(const char* path)
    {
        std::shared_ptr<VFSFileNode> fileNode = getFile(path);
        VFSFileInfo vfsFileInfo(fileNode);
        return vfsFileInfo;
    }

    bool VirtualFileSystem::listFolder(std::vector < std::string>& list, const char* path)
    {
        if (std::filesystem::is_directory(path) == false)
        {
            return false;
        }
        std::string p = path;
        std::shared_ptr<VFSFolderNode> currentDirNode = insertDirectory(p);
        std::map<std::string, std::shared_ptr<VFSNode>>& folders = currentDirNode->getFolder();
        for (auto it : folders)
        {
            list.push_back(it.first);
        }
        return  true;
    }
}