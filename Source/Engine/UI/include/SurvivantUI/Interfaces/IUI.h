//UI.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantUI/Interfaces/ISelectable.h"

#include <memory>

//foward declaration
struct ImFont;

namespace SvUI::Interfaces
{
	class IUI
	{
	public:
		class DebugEvent : public ::Core::Event<const char*> {};

		virtual ImFont* GetFontDefault() = 0;
		virtual ImFont* GetIconFont() = 0;

		virtual ISelectable*	GetSelected() = 0;
		virtual void			SetSelected(ISelectable* p_selected = nullptr) = 0;

	protected:
		virtual void StartFrameUpdate() = 0;
		virtual void EndFrameUpdate() = 0;

	};
}
