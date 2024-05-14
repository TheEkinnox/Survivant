//BuildManager.h
#pragma once

#include "BuildConfig.h"

#include <string>

namespace SvEditor::RuntimeBuild
{
	class BuildManager
	{
	public:
		BuildManager() = default;
		~BuildManager() = default;

		static BuildManager& GetInstance();

		void CreateBuild(const std::string& p_buildFileName, const BuildConfig& p_setup);
		bool RunBuild(const std::string& p_buildFileName);
	};
}


