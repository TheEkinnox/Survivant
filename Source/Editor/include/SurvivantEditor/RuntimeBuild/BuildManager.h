//BuildManager.h
#pragma once

#include <SurvivantApp/Core/BuildConfig.h>

#include <string>

namespace SvEditor::RuntimeBuild
{
    class BuildManager
    {
    public:
        BuildManager()  = default;
        ~BuildManager() = default;

        static BuildManager& GetInstance();

        std::string CreateBuild(const std::string& p_buildName, SvApp::Core::BuildConfig p_setup);
        std::string CreateAndRunBuild(const std::string& p_buildName, const SvApp::Core::BuildConfig& p_setup);
        void        RunBuild(std::string p_buildFilePath);

    private:
#ifdef _DEBUG
        static constexpr const char* RUNTIME_BUILD_LOCAL_PATH = SV_RUNTIME_DIR "\\debug\\" SV_RUNTIME_EXE;
#else
        static constexpr const char* RUNTIME_BUILD_LOCAL_PATH = SV_RUNTIME_DIR "\\release\\" SV_RUNTIME_EXE;
#endif
        static constexpr const char* BUILD_CONFIG_FILE_NAME = "buildConfig.txt";
        static constexpr const char* ASSETS_DIR_LOCAL_PATH  = "assets";
    };
}
