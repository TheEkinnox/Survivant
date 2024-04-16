//InspectorPanel.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
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

		//TODO : change to resource
		void SetInpectorInfo(const std::shared_ptr<IPanelable>& p_info);
		void ClearInfo();

		virtual ERenderFlags Render()override;

		static constexpr char NAME[] = "Inspector";
	private:

		std::shared_ptr<IPanelable> m_info;
	};
}