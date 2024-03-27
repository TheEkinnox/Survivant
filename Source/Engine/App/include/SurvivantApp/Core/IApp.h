//SvApp.h
#pragma once

namespace SvApp::Core
{
	class IApp
	{
	public:
		IApp();
		virtual ~IApp();

		virtual void Init() = 0;
		virtual void Run() = 0;
	};
}