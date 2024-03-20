//ConsolePanel.h
#pragma once

#include "SurvivantUI/Interfaces/ITextable.h"
#include "SurvivantUI/Interfaces/IUI.h"
#include "SurvivantUI/MenuItems/Menu.h"
#include "SurvivantUI/PanelItems/PanelTextInput.h"
#include "SurvivantUI/PanelItems/PanelTextBox.h"
#include "SurvivantUI/PanelItems/PanelButtonList.h"
#include "SurvivantUI/Panels/Panel.h"

#include <functional>

//foward declaration
struct ImVec4;

namespace SvUI::Panels
{
	using namespace PanelItems;
	using namespace MenuItems;

	class ConsolePanel : public Panel
	{
	public:
		enum class ELogType
		{
			DEFAULT_LOG = 1 << 0,
			DEBUG_LOG = 1 << 1,
			WARNING_LOG = 1 << 2,
			ERROR_LOG = 1 << 3,
			//COMMAND_LOG	=	1 << 4
		};

		struct LogInfo
		{
			ELogType m_type;
			std::string m_message;
		};

		ConsolePanel();
		~ConsolePanel();

		ERenderFlags Render() override;

		void AddConsoleLog(const LogInfo& p_log);

	private:
		class LogText : public Interfaces::ITextable
		{
		public:
			LogText(const LogInfo& p_logInfo);
			~LogText() = default;

			void DisplayAndUpdatePanel()override;
			std::string GetString(size_t p_len = 0)const override;
			size_t GetLength()const override;

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

		void TextInputCallback(PanelTextInput& p_textInput);

		static constexpr char NAME[] = "DebugLog";
		static inline int s_panelCount = 0;

		std::string								m_inputBuffer;
		PanelTextInput							m_input;
		PanelTextBox							m_textBox;
		PanelButtonList							m_buttons;
		Menu									m_filterMenu;
		//cant use vector<bool> bcs not container
		std::vector<unsigned char>				m_currentFilters;
		IUI::DebugEvent::ListenerId			m_eventHandle;
	};
}