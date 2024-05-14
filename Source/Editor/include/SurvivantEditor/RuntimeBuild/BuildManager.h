//BuildManager.h
#pragma once

#include "BuildConfig.h"
#include <SurvivantCore/Resources/ResourceRef.h>

#include <string>

namespace SvEditor::RuntimeBuild
{
	class BuildManager
	{
	public:
		BuildManager();
		~BuildManager() = default;

		static BuildManager& GetInstance();

		std::string		CreateBuild(const std::string& p_buildName, BuildConfig p_setup);
		std::string		CreateAndRunBuild(const std::string& p_buildName, BuildConfig p_setup);
		void			RunBuild(std::string p_buildFilePath);

	private:

		static constexpr std::string_view RuntimeBuildFileName = "SurvivantRuntime.exe";
		static constexpr std::string_view AssetsDirName = "assets";
		static constexpr std::string_view DefaultDownloadPath = "C:\\Users\\USER_NAME\\Downloads";
		static inline std::string DefaultUserName = "USER_NAME"; //cant be constexpr bcs need to be std::string 

		std::string m_downloadPath;
	};
}


