//PanelSelectionBox.cpp

#include "SurvivantUI/PanelItems/PanelSelectionBox.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    void PanelSelectionBox::SetSelectionSize(float p_width, float p_height)
    {
        m_width = p_width;
        m_height = p_height;
    }

    void PanelSelectionBox::DisplayAndUpdatePanel()
    {
        static float spacingX = 10;
        static auto Padding = ImVec2(15, 15);
        ImGuiStyle& style = ImGui::GetStyle();
        float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        ImGuiSelectableFlags    sFlags = ImGuiSelectableFlags_None;
        auto                    cFlags = ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize;

        size_t count = m_elements.size();

        for (size_t i = 0; i < count; i++)
        {
            auto item = m_elements[i].get();
            bool isSelected = m_currentSelection.contains(item);
            auto isBreak = false; auto open = false;

            ImGui::BeginChild(std::string("##" + std::to_string(i)).c_str(), ImVec2(m_width + Padding.x, m_height + Padding.y), cFlags);

            auto cursorBefore = ImGui::GetCursorPos();
            ImGui::PushID(static_cast<int>(i));
            open = ImGui::Selectable("##", isSelected, sFlags, ImVec2(m_width, m_height));
            ImGui::PopID();
            auto cursorAfter = ImGui::GetCursorPos();

            bool doubleClicked = ImGui::GetMouseClickedCount(0) == 2 && ImGui::IsItemHovered();
            float last_button_x2 = ImGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + m_width; // Expected position if next button was on same line

            ImGui::SetCursorPos(cursorBefore);
            isBreak = item->DisplayAndUpdateSelection(m_width, m_height, doubleClicked);

            ImGui::EndChild();
            //ImGui::GetID();
            if (i + 1 < count && next_button_x2 < window_visible_x2)
            {
                ImGui::SameLine();
            }

            if (open)
            {
                if (isSelected)
                    m_currentSelection.erase(item);
                else
                    m_currentSelection.insert(item);
            }

            if (isBreak)
                break;
        }
    }

    void PanelSelectionBox::SetSelectionBoxable(const std::vector<std::shared_ptr<ISelectionBoxable>>& p_elements)
    {
        m_elements = p_elements;
    }

    void PanelSelectionBox::DisplayCenteredText(const std::string& p_text, float p_maxWidth)
    {
        //float win_width = ImGui::GetWindowSize().x;
        float text_width = ImGui::CalcTextSize(p_text.c_str()).x;

        // offset cursor
        auto cursorPos = ImGui::GetCursorPos();// (ImVec2(cursorPos.x + 10, cursorPos.y + 10));

        float text_indentation = (p_maxWidth - text_width) * 0.5f;

        // if iconTxt is too long to be drawn on one line, `text_indentation` can
        // become too small or even negative, so we check a minimum indentation
        float min_indentation = 0.0f;
        if (text_indentation <= min_indentation) {
            text_indentation = min_indentation;
        }

        ImGui::SameLine(text_indentation);
        ImGui::PushTextWrapPos(p_maxWidth - text_indentation);
        ImGui::TextWrapped(p_text.c_str());
        ImGui::PopTextWrapPos();
    }
}