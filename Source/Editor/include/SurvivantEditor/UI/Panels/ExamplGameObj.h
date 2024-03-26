//ExamplGameObj.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"
#include "SurvivantEditor/UI/Interfaces/ISelectable.h"
#include "SurvivantEditor/UI/Panels/Panel.h"
#include "SurvivantEditor/UI/PanelItems/PanelButton.h"

#include <memory>
#include <functional>


namespace SvEditor::UI::Panels
{
	using namespace Interfaces;
	using namespace PanelItems;

	class ExamplResource : public IPanelable, public ISelectable
	{
	public:
		ExamplResource() = default;
		~ExamplResource() = default;

		// Inherited via IPanelable
		void DisplayAndUpdatePanel() override;

		// Inherited via ISelectable
		const std::string& GetIcon() override;
		const std::string& GetName() override;
		bool InvokeDoubleClick() override;
		void DisplayAndUpdatePopupMenu() override;
		bool GetSelectedState() override;
		void SetSelectedState(bool p_isSelected) override;

	protected:
		std::string m_name = "None";
		std::string m_resourceLogo = "Tp";
		std::string m_resourceName = "Resource";

		bool m_isSelected;
	};

	class ExamplGameObj : public ExamplResource
	{
	public:
		ExamplGameObj();
		~ExamplGameObj() = default;

		void DisplayAndUpdatePanel() override;
		//void AddComponent();

	private:
		static void AddComponentCallback(ExamplGameObj* p_this);

		struct ExampleComponent
		{
			std::string					m_name;
			std::shared_ptr<IPanelable> m_panelable;
		};

		//components
		std::vector<ExampleComponent> m_defaultComponents;
		std::vector<ExampleComponent> m_aditionalComponents;

		PanelButton m_addComponentButton;
	};
}