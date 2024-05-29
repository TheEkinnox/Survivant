//BuildManager.cpp
#include "SurvivantEditor/RuntimeBuild/BuildManager.h"

#include "SurvivantCore/Utility/LeanWindows.h"
#include "SurvivantCore/Utility/FileSystem.h"
#include <filesystem>

#ifdef _WIN32
#include <shlobj.h>
#else
static_assert(false, "Non windows builds are not supported")
#endif

namespace SvEditor::RuntimeBuild
{
    namespace
    {
        const std::string& GetDownloadsPath()
        {
            static std::string downloadsPath;
            static bool        isInitialized = false;

            if (isInitialized)
                return downloadsPath;

            PWSTR out;
            if (S_OK != SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &out))
                return downloadsPath; // Empty by default

            downloadsPath = std::filesystem::path(out).string();
            CoTaskMemFree(out);

            isInitialized = true;
            return downloadsPath;
        }
    }

    BuildManager& BuildManager::GetInstance()
    {
        static BuildManager instance;
        return instance;
    }

    std::string BuildManager::CreateBuild(const std::string& p_buildName, SvApp::Core::BuildConfig p_setup)
    {
        using namespace SvCore::Utility;
        namespace fs = std::filesystem;

        static const auto& downloadPath = GetDownloadsPath();

        if (!PathExists(downloadPath))
        {
            SV_LOG_ERROR("Can't Create Build, Path to Download directory doesn't exist: %s", downloadPath.c_str());
            return {};
        }

        const std::string buildDirPath = AppendPath(downloadPath, p_buildName);
        std::error_code   err;

        if (!(fs::create_directories(buildDirPath.c_str(), err) || err.value() == 0))
        {
            SV_LOG_ERROR("Can't Create Build, can't create directory for Build at \"%s\": %s",
                buildDirPath.c_str(), err.message().c_str());

            return {};
        }

        if (!fs::is_empty(buildDirPath, err))
        {
            SV_LOG_ERROR("Can't Create Build, Directory not Empty: %s", buildDirPath.c_str());
            return {};
        }

        if (err.value() != 0)
        {
            SV_LOG_ERROR("Can't Create Build, Failed to check if directory \"%s\" is empty: %s",
                buildDirPath.c_str(), err.message().c_str());

            return {};
        }

        std::string buildFilePath = buildDirPath + "\\" + p_buildName + ".exe";
        if (!(fs::copy_file(RUNTIME_BUILD_LOCAL_PATH, buildFilePath, err) || err.value() == 0))
        {
            SV_LOG_ERROR("Can't Create Build, Can't copy runtime from \"%s\" to \"%s\": %s",
                RUNTIME_BUILD_LOCAL_PATH, buildFilePath.c_str(), err.message().c_str());

            return {};
        }

        const fs::path assetsSource = ASSETS_DIR_LOCAL_PATH;
        const fs::path assetsPath   = fs::path(buildDirPath) / ASSETS_DIR_LOCAL_PATH;

        // Recursively create assetsPath directory
        if (!(fs::create_directories(assetsPath, err) || err.value() == 0))
        {
            SV_LOG_ERROR("Can't Create Build, can't create Assets folder at \"%s\": %s", assetsPath.c_str(), err.message().c_str());
            return {};
        }

        fs::copy(assetsSource, assetsPath, fs::copy_options::recursive, err);
        if (err.value() != 0)
        {
            SV_LOG_ERROR("Can't Create Build, can't copy Assets folder from \"%s\" to \"%s\": %s",
                assetsSource.c_str(), assetsPath.c_str(), err.message().c_str());

            return {};
        }

        const std::string configPath = AppendPath(buildDirPath, BUILD_CONFIG_FILE_NAME);
        if (!p_setup.Save(configPath))
        {
            SV_LOG_ERROR("Can't Create Build, can't save BuildConfig: %s", configPath.c_str());
            return {};
        }

        SV_LOG("Successfully created Build: %s", buildFilePath.c_str());
        return buildFilePath;
    }

    std::string BuildManager::CreateAndRunBuild(const std::string& p_buildName, SvApp::Core::BuildConfig p_setup)
    {
        auto path = CreateBuild(p_buildName, p_setup);

        if (path.empty())
            return {};

        RunBuild(path);
        return path;
    }

    void BuildManager::RunBuild(std::string p_buildFilePath)
    {
        p_buildFilePath.resize(MAX_PATH);

        STARTUPINFOA        si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (!CreateProcessA(
            NULL, p_buildFilePath.data(), NULL,
            NULL, NULL, FALSE, 0, NULL,
            &si, &pi))
        {
            SV_LOG_ERROR("Can't run build (%s): CreateProcess failed (%d).", p_buildFilePath.c_str(), GetLastError());
            return;
        }

        //Wait until child process exits.
        //WaitForSingleObject(pi.hProcess, INFINITE);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        SV_LOG("Successfully run Build: %s", p_buildFilePath.c_str());
    }
}
