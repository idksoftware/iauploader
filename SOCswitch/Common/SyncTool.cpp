#include <filesystem>
#include "SyncTool.h"
#include "VirtualFileSystem.h"
#include "RemoteInfoCache.h"

#include "FtpClient.h"
#include "Logger.h"

namespace Common
{
    class FtpSession {
        std::string m_user;
        std::string m_password;
        std::string m_ipAddress;
    public:
        FtpSession(const char* user, const char* pw, const char* ip) :
            m_user{ user }, m_password{ pw }, m_ipAddress{ ip } {};

        ~FtpSession() = default;

        const char* getUser() { return m_user.c_str(); };
        const char* getPassword() { return m_password.c_str(); };
        const char* getIPAddress() { return m_ipAddress.c_str(); };
    };


    SyncJob::SyncJob(const char* path) : m_path(path) {};

    bool SyncTool::newJob(const char* path)
    {
        m_list.push_back(std::make_shared<SyncJob>(path));
        return true;
    }

    SyncTool::SyncTool(const char* rootPath) : m_rootPath(rootPath)
    {
        m_ftpclient = std::make_shared<CFtpClient>();
        m_remoteInfoCache = std::make_shared<RemoteInfoCache>(m_ftpclient);
    }

    SyncTool::SyncTool()
    {
        m_ftpclient = std::make_shared<CFtpClient>();
        m_remoteInfoCache = std::make_shared<RemoteInfoCache>(m_ftpclient);
    }

    bool SyncTool::init(const char* user, const char* pw, const char* ip)
    {
        if (m_session == nullptr) {
            m_session = std::make_shared<FtpSession>(user, pw, ip);
        }
        if (m_ftpclient->connectToHost(ip) == false)
        {
            return false;
        }
        if (m_ftpclient->login(user, pw) == false)
        {
            return false;
        }
        
        return true;
    }

    bool SyncTool::tryRelogin()
    {
        
        if (m_ftpclient->connectToHost(m_session->getIPAddress()) == false)
        {
            return false;
        }
        if (m_ftpclient->login(m_session->getUser(), m_session->getPassword()) == false)
        {
            return false;
        }
        return true;
    }

    SyncTool::Result SyncTool::processFile(const char* path, std::shared_ptr<VirtualFileSystem> vfs)
    {
        Logger& logger = Logger::getLogger();

        if (std::filesystem::is_regular_file(path) == false)
        {
            return Result::Error;
        }
        m_numFilesProcessed++;
        logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Processing local - file: %s", path);
        std::filesystem::path p = path;
        std::string name = p.filename().string();
        std::filesystem::file_time_type modTime = last_write_time(p);
        uintmax_t size = file_size(p);
        if (vfs->findFile(path) == false)
        {
            if (vfs->insertFile(path) == false) {
                return Result::Error;
            }
            newJob(path);
            logger.log(LOG_OK, AALTOLogNoticeType::INFO, "New file Job - file: %s", path);
            return Result::NewFile;
        }
        else
        {

            VFSFileInfo vfsFileInfo = vfs->getFileInfo(path);
            if (vfsFileInfo.isError())
            {
                return Result::Error;
            }
            if (vfsFileInfo.isChanged(p) == true)
            {
                newJob(path);
                logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Changed file Job - file: %s", path);
                vfs->updateFile(path);
                return Result::Changed;
            }
        }
        return Result::NoChange;
    }

    bool SyncTool::doList()
    {

        Logger& logger = Logger::getLogger();
       
        if (m_list.size() == 0) {
            return true;
        }
        logger.log(LOG_OK, AALTOLogNoticeType::STATUS, "Job processing Jobs: %d", m_list.size());
        for (auto ii : m_list)
        {
            SyncJob job = *ii;
            //printf("%s\n", job.getFilepath());
            logger.log(LOG_OK, AALTOLogNoticeType::STATUS, "Ftp processing job - file: %s", job.getFilepath());
            std::string fullPath = job.getFilepath();
            std::string remotePath = fullPath.substr(m_rootPath.length() + 1, fullPath.length() - (m_rootPath.length() + 1));

            std::filesystem::path fp = fullPath;
            std::string fullParentPath = fp.parent_path().generic_string();
            std::string remoteParentPath = fullParentPath.substr(m_rootPath.length(), fullParentPath.length() - (m_rootPath.length()));
            
            std::filesystem::file_time_type modTime = last_write_time(fp);
            uintmax_t size = file_size(fp);
            if (m_remoteInfoCache->insertFile(remoteParentPath.c_str(), fp.filename().string().c_str(), modTime, size)) {
                
                if (m_ftpclient->put(remotePath.c_str(), fullPath.c_str()) == false) {
                    logger.log(LOG_OK, AALTOLogNoticeType::ERR, "Ftp failed for file: %s, trying re-logong on", job.getFilepath());
                    if (tryRelogin() == false) {
                        logger.log(LOG_OK, AALTOLogNoticeType::ERR, "Ftp failed sending file: %s", job.getFilepath());
                    }
                    else {
                        if (m_ftpclient->put(remotePath.c_str(), fullPath.c_str()) == false) {
                            logger.log(LOG_OK, AALTOLogNoticeType::ERR, "Ftp failed sending file: %s", job.getFilepath());
                        }
                    }
                }
                logger.log(LOG_OK, AALTOLogNoticeType::INFO, "Ftp sending file: %s", job.getFilepath());
            }
            logger.log(LOG_OK, AALTOLogNoticeType::FINE, "Ftp Not sending file: %s", job.getFilepath());
            
            
        }
        m_list.clear();
        return true;
    }
};
