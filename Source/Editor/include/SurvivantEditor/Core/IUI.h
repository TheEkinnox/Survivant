//UI.h
#pragma once

#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/Interfaces/ISelectable.h"

#include "Vector/Vector4.h"

#include <memory>

#ifndef SV_CURRENT_UI
#define SV_CURRENT_UI() SvEditor::Core::IUI::GetCurrentUI()
#endif //SV_CURRENT_UI

//forward declaration
struct ImFont;

namespace SvEditor::Core
{
	class IUI
	{
	public:
		virtual         ~IUI() = default;
		virtual ImFont* GetFontDefault() = 0;
		virtual ImFont* GetIconFont() = 0;

		virtual Interfaces::ISelectable*	GetSelected() = 0;

		static IUI* GetCurrentUI();

	protected:
		virtual void StartFrameUpdate() = 0;
		virtual void EndFrameUpdate() = 0;

		static inline IUI* m_currentUI = nullptr;

		static LibMath::Vector4 GetColorFromString(const std::string& p_string);
		static LibMath::Vector4 size_tToRGBA(size_t p_value);
	};
}
