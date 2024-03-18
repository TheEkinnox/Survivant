//UI.h
#pragma once

#include "SurvivantCore/Events/Event.h"

//foward declaration
struct ImFont;

namespace SvUI::Interfaces
{
	class IUI
	{
	public:
		class DebugEvent : public ::Core::Event<const char*> {};

	protected:
		virtual void StartFrameUpdate() = 0;
		virtual void EndFrameUpdate() = 0;
	};
}
