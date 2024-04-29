//ConsolePanel.cpp

#include "SurvivantEditor/Panels/ConsolePanel.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantCore/Debug/Logger.h"
#include "SurvivantEditor/MenuItems/MenuCheckBox.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::Panels
{
    ConsolePanel::ConsolePanel() :
        m_filterMenu("Filters"),
        m_input("Input", m_inputBuffer, std::bind(&ConsolePanel::TextInputCallback, this, std::placeholders::_1))
    {
        static size_t NumLogTypes = 5;
        m_name = GetUniqueName(NAME, s_panelCount);
        s_panelCount++;

        m_buttons.m_buttons.emplace_back("Clear", [this]() { m_textBox.Clear(); });
        m_buttons.m_buttons.emplace_back("Copy", [this]() { m_textBox.Copy(); });

        m_currentFilters = std::vector<unsigned char>(NumLogTypes, static_cast<unsigned char>(false));
        for (size_t i = 0; i < NumLogTypes; i++)
        {
            ELogType type = ELogType(1 << i);
            //convert stored uchar to bool ref
            auto& boolRef = *(bool*)&(m_currentFilters[i]);

            m_filterMenu.m_items.emplace_back(
                std::make_unique<MenuCheckBox>(LogTypeToString(type), boolRef));
        }

        //debug event
        //

        m_eventHandle = SvCore::Debug::Logger::GetInstance().m_onPrint.AddListener(
            [this](const LogInfo& m_message)
            {
                if (this == nullptr)
                    return;

                m_textBox.AddItem(
                    std::make_shared<ConsolePanel::LogText>(m_message));
            });
    }

    ConsolePanel::~ConsolePanel()
    {
        s_panelCount--;
        SvCore::Debug::Logger::GetInstance().m_onPrint.RemoveListener(m_eventHandle);
    }

    Panel::ERenderFlags ConsolePanel::Render()
    {
        bool open;

        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        m_buttons.DisplayAndUpdatePanel();
        ImGui::Separator();

        std::vector<unsigned char> lastFilters = m_currentFilters;
        m_filterMenu.DisplayAndUpdatePanel();
        ImGui::Separator();

        for (size_t i = 0; i < m_currentFilters.size(); i++)
        {
            if (lastFilters[i] != m_currentFilters[i])
            {
                ELogType type = ELogType(1 << i);
                m_textBox.AddOrRemoveFilter(LogTypeToString(type));
            }
        }

        m_input.DisplayAndUpdatePanel();
        ImGui::Separator();
        m_textBox.DisplayAndUpdatePanel();

        ImGui::End();

        auto flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags::CLOSE;

        return flags;
    }

    void ConsolePanel::AddConsoleLog(const LogInfo& p_log)
    {
        m_textBox.AddItem(std::make_shared<LogText>(p_log));
    }

    std::string ConsolePanel::LogTypeToString(ELogType p_type)
    {
        switch (p_type)
        {
        case ConsolePanel::ELogType::DEBUG_LOG:
            return "[Debug]   ";
        case ConsolePanel::ELogType::WARNING_LOG:
            return "[Warning] ";
        case ConsolePanel::ELogType::ERROR_LOG:
            return "[ERROR]   ";
            //case UI::ConsolePanel::ELogType::COMMAND_LOG:
            //    return "[Command] ";
        case ConsolePanel::ELogType::DEFAULT_LOG:
        default:
            return "          ";
        }
    }

    void ConsolePanel::LogTypeColor(ELogType p_type, ImVec4* p_color)
    {
        switch (p_type)
        {
        case ConsolePanel::ELogType::DEBUG_LOG:
            *p_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            break;
        case ConsolePanel::ELogType::WARNING_LOG:
            *p_color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
            break;
        case ConsolePanel::ELogType::ERROR_LOG:
            *p_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
            //case UI::ConsolePanel::ELogType::COMMAND_LOG:
            //    *p_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
                //break;
        case ConsolePanel::ELogType::DEFAULT_LOG:
        default:
            p_color = nullptr;
            break;
        }
    }

    void ConsolePanel::TextInputCallback(const std::tuple<PanelTextInput*>& p_textInput)
    {
        auto& [input] = p_textInput;
        m_textBox.AddItem(std::make_shared<LogText>(LogInfo{ ELogType::WARNING_LOG, input->GetText()}), true);
        input->Clear();
    }

    ConsolePanel::LogText::LogText(const LogInfo& p_logInfo) :
        m_logInfo(p_logInfo)
    {
        m_length = GetString().length();
    }

    void ConsolePanel::LogText::DisplayAndUpdatePanel()
    {
        ImVec4 color;
        LogTypeColor(m_logInfo.m_type, &color);
        bool has_color = &color != nullptr;

        if (has_color)
            ImGui::PushStyleColor(ImGuiCol_Text, color);

        ImGui::TextUnformatted(LogTypeToString(m_logInfo.m_type).c_str());

        if (has_color)
            ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::TextUnformatted((SPACER + m_logInfo.m_message).c_str());
    }

    std::string ConsolePanel::LogText::GetString(size_t p_len)const
    {
        if (p_len == 0)
            return std::string((LogTypeToString(m_logInfo.m_type) + SPACER + m_logInfo.m_message));

        auto tmp = std::string((LogTypeToString(m_logInfo.m_type) + SPACER + m_logInfo.m_message), 0, p_len);
        return tmp;
    }

    size_t ConsolePanel::LogText::GetLength()const
    {
        return m_length;
    }

    const std::string& ConsolePanel::LogText::GetString()
    {
        static std::string tmp;
        tmp = std::string((LogTypeToString(m_logInfo.m_type) + SPACER + m_logInfo.m_message));
        return tmp;
    }
}