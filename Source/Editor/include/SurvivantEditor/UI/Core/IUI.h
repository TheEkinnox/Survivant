//UI.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/UI/Interfaces/ISelectable.h"

#include "Vector/Vector4.h"

#include <memory>

#ifndef SV_CURRENT_UI
#define SV_CURRENT_UI() SvEditor::UI::Core::IUI::GetCurrentUI()
#endif //SV_CURRENT_UI

//foward declaration
struct ImFont;

namespace SvEditor::UI::Core
{
	class IUI
	{
	public:
		class DebugEvent : public SvCore::Events::Event<const char*> {};

		virtual ImFont* GetFontDefault() = 0;
		virtual ImFont* GetIconFont() = 0;

		virtual Interfaces::ISelectable*	GetSelected() = 0;
		virtual void						SetSelected(Interfaces::ISelectable* p_selected = nullptr) = 0;

		static IUI* GetCurrentUI();

	protected:
		virtual void StartFrameUpdate() = 0;
		virtual void EndFrameUpdate() = 0;

		static inline IUI* m_currentUI = nullptr;

		static LibMath::Vector4 GetColorFromString(const std::string& p_string);
		static LibMath::Vector4 size_tToRGBA(size_t p_value);
	};
}
