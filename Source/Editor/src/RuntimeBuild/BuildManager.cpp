//BuildManager.cpp
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include "SurvivantCore/Utility/LeanWindows.h"
#include "SurvivantCore/Utility/FileSystem.h"

#include <tchar.h>
#include <Lmcons.h>
#include <filesystem>


namespace SvEditor::RuntimeBuild
{
    BuildManager::BuildManager()
    {
        std::string userName;
        userName.resize(UNLEN + 1);
        DWORD userNameLen = UNLEN + 1;

        if (!GetUserNameA(userName.data(), &userNameLen))
        {
            SV_LOG_ERROR("Can't get User Name to create Build");
            return;
        }
        userName.resize(userNameLen - 1); //userNameLen includes \0

        m_downloadPath = DefaultDownloadPath;
        size_t start_pos = m_downloadPath.find(DefaultUserName);
        ASSERT(start_pos != std::string::npos);
        m_downloadPath.replace(start_pos, DefaultUserName.length(), userName);
    }

    BuildManager& BuildManager::GetInstance()
    {
        static BuildManager s_instance;

        return s_instance;
    }

    std::string BuildManager::CreateBuild(const std::string& p_buildName, BuildConfig p_setup)
    {
        using namespace SvCore::Utility;
        namespace fs = std::filesystem;

        if (!PathExists(m_downloadPath))
        {
            SV_LOG_ERROR("Can't Create Build, Build path to Download directory doesn't exist: %s", m_downloadPath.c_str());
            return {};
        }

        std::string buildDirPath = m_downloadPath + "\\" + p_buildName;
        if (!PathExists(buildDirPath) && CreateDirectoryA(buildDirPath.data(), NULL))
        {
            SV_LOG_ERROR("Can't Create Build, can't create directory for Build: %s", buildDirPath.c_str());
            return {};
        }
 
        if (!fs::is_empty(buildDirPath)) 
        {
            SV_LOG_ERROR("Can't Create Build, Directory not Empty: %s", buildDirPath.c_str());
            return {};
        }

        //std::string buildFilePath = buildDirPath + "\\" + p_buildName + ".exe";
        //if (!CopyFileA(RuntimeBuildFileName.data(), buildFilePath.data(), TRUE))
        //{
        //    SV_LOG_ERROR("Can't Create Build, Can't copy file: %s", buildFilePath.c_str());
        //    return {};
        //}

        fs::path assetsSource = std::filesystem::current_path() / AssetsDirName;
        fs::path assetsPath = fs::path(buildDirPath) / AssetsDirName;
        try
        {
            fs::create_directories(assetsPath); // Recursively create assetsPath directory
            fs::copy(assetsSource, assetsPath, fs::copy_options::recursive);
        }
        catch (std::exception& e)
        {
            SV_LOG_ERROR("Can't Create Build, can't copy Assets folder: %s", e.what());
        }

        if (!p_setup.Save(buildDirPath))
        {
            SV_LOG_ERROR("Can't Create Build, can't save BuildConfig: %s", buildDirPath.c_str());
            return {};
        }

        return {};
    }

    std::string BuildManager::CreateAndRunBuild(const std::string& p_buildName, BuildConfig p_setup)
    {
        auto path = CreateBuild(p_buildName, p_setup);

        if (path.empty())
            return {};

        RunBuild(path);
        return path;
    }

    void BuildManager::RunBuild(std::string p_buildFilePath)
    {
        LPSTR path = p_buildFilePath.data();

        STARTUPINFOA  si;
        PROCESS_INFORMATION  pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if(!CreateProcessA(
            NULL, path, NULL,
            NULL, NULL, FALSE, 0, NULL,
            &si, &pi))  
        {
            SV_LOG_ERROR("Can't run build (%s): CreateProcess failed (%d).", p_buildFilePath.c_str(), GetLastError());
            return;
        }

        // Wait until child process exits.
        WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}