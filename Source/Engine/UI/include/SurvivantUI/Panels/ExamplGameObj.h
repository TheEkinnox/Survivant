//ExamplGameObj.h
#pragma once

#include "SurvivantUI/Interfaces/IPanelable.h"
#include "SurvivantUI/Panels/Panel.h"
#include "SurvivantUI/PanelItems/PanelButton.h"

#include <memory>
#include <functional>


namespace SvUI::Panels
{
	using namespace PanelItems;

	class ExamplResource :public Interfaces::IPanelable
	{
	public:
		ExamplResource() = default;
		~ExamplResource() = default;

		void DisplayAndUpdatePanel() override;

	protected:
		std::string m_name = "None";
		std::string m_resourceLogo = "Tmp";
		std::string m_resourceName = "Default";
		//private:
		//	static void OpenCallback();
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
