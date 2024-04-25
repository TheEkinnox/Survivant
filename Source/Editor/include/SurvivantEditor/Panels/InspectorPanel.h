//InspectorPanel.h
#pragma once

#include "SurvivantEditor/Interfaces/IInspectorable.h"
#include "SurvivantEditor/Panels/Panel.h"
#include "SurvivantCore/Utility/UnusedIdGenerator.h"

#include <functional>
#include <memory>


namespace SvEditor::Panels
{
	using namespace Interfaces;

	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel();
		~InspectorPanel();

		static void SetInpectorInfo(const std::shared_ptr<IInspectorable>& p_selected, const std::string& p_resourceName);
		static void ClearInfo();

		virtual ERenderFlags Render()override;

		static constexpr char NAME[] = "Inspector";
	private:

		static inline std::shared_ptr<IInspectorable>	s_selected;
		static inline std::string						s_resourceName;
	};
}