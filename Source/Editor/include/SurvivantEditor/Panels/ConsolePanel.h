//ConsolePanel.h
#pragma once

#include "SurvivantEditor/Interfaces/ITextable.h"
#include "SurvivantCore/Events/Event.h"
#include "SurvivantEditor/MenuItems/Menu.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTextBox.h"
#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantEditor/PanelItems/PanelPopupMenuButton.h"
#include "SurvivantEditor/Panels/Panel.h"

#include <SurvivantCore/Debug/Logger.h>

//foward declaration
struct ImVec4;

namespace SvEditor::Panels
{
	using namespace PanelItems;
	using namespace MenuItems;

	class ConsolePanel : public Panel
	{
	public:
		using ELogType = SvCore::Debug::ELogType;
		using LogInfo = SvCore::Debug::LogInfo;

		ConsolePanel();
		~ConsolePanel() override;

		ERenderFlags Render() override;

		void AddConsoleLog(const LogInfo& p_log);

		static constexpr char NAME[] = "DebugLog";

	private:
		class LogText : public Interfaces::ITextable
		{
		public:
			LogText(const LogInfo& p_logInfo);
			~LogText() = default;

			// Inherited via ITextable
			void	DisplayAndUpdatePanel()override;
			std::string			GetString(size_t p_len = 0)const override;
			const std::string&	GetString() override;
			size_t				GetLength()const override;

		private:
			static constexpr char SPACER[] = "";

			LogInfo m_logInfo;
			size_t m_length;
		};

		static std::string LogTypeToString(ELogType p_type);

		/// <summary>
		/// returns a color associated with the LogType, or nullptr if there are none
		/// </summary>
		/// <param name="p_type"></param>
		/// <param name="p_color">[out]</param>
		static void LogTypeColor(ELogType p_type, ImVec4* p_color);

		void TextInputCallback(const std::tuple<PanelTextInput*>& p_textInput);

		static inline int s_panelCount = 0;

		PanelTextBox							m_textBox;
		PanelButtonList							m_buttons;
		PanelPopupMenuButton					m_popUp;
		//cant use vector<bool> bcs not container
		std::vector<unsigned char>						m_currentFilters;
		SvCore::Events::Event<LogInfo>::ListenerId		m_eventHandle;
	};
}