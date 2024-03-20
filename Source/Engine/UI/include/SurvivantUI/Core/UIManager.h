//UIManager.h
#pragma once

#include "SurvivantUI/Interfaces/IUI.h"
#include <memory>

#include "Vector/Vector4.h"

#ifndef SV_CURRENT_UI
#define SV_CURRENT_UI() SvUI::Core::UIManager::GetInstance().GetCurrentUI()
#endif //SV_CURRENT_UI

namespace SvUI::Core
{
	using namespace Interfaces;

	class UIManager
	{
	public:
		friend IUI;

		UIManager();
		UIManager(UIManager const&) = delete;
		void operator=(UIManager const&) = delete;
		
		static UIManager& GetInstance();

		void SetCurrentUI(IUI* p_ui);
		IUI * const GetCurrentUI();

		static LibMath::Vector4 GetColorFromString(const std::string& p_string);
		

	private:
		static LibMath::Vector4 size_tToRGBA(size_t p_value);

		IUI* m_currentUI;
	};
}