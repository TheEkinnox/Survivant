//InspectorPanel.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantCore/Utility/UnusedIdGenerator.h"

#include <functional>
#include <memory>


namespace SvEditor::UI::Panels
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

	private:
		static constexpr char NAME[] = "Inspector";
		static inline SvCore::Utility::UnusedIdGenerator s_idGenerator;

		std::shared_ptr<IPanelable> m_info;
	};
}