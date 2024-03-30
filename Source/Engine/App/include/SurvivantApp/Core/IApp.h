//SvApp.h
#pragma once

namespace SvApp::Core
{
	class IApp
	{
	public:
		struct WorldContext
		{
			int WorldType;

		};




		IApp();
		virtual ~IApp();

		virtual void Init() = 0;
		virtual void Run() = 0;

		//static CreateNewWorldContext(EWorldType::Type WorldType);
	};
}