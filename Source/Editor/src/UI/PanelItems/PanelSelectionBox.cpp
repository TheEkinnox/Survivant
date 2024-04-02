//PanelSelectionBox.cpp

#include "SurvivantEditor/UI/PanelItems/PanelSelectionBox.h"

#include "SurvivantEditor/UI/Core/UIManager.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::PanelItems
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

        size_t i = 0;
        for (auto& itemPair : m_elements)
        {
            auto item = itemPair.second.get();
            bool isSelected = item->GetSelectedState();
            auto isBreak = false; auto open = false;

            ImGui::BeginChild(std::string("##" + std::to_string(i)).c_str(), ImVec2(m_width + Padding.x, m_height + Padding.y), cFlags);

            auto cursorBefore = ImGui::GetCursorPos();
            ImGui::PushID(static_cast<int>(i));
            open = ImGui::Selectable("##", isSelected, sFlags, ImVec2(m_width, m_height));
            ImGui::PopID();
            auto cursorAfter = ImGui::GetCursorPos();

            bool doubleClicked = ImGui::GetMouseClickedCount(0) == 2 && ImGui::IsItemHovered();
            //bool rightClicked = ImGui::GetMouseClickedCount(1) == 1 && ImGui::IsItemHovered();
            float last_button_x2 = ImGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + style.ItemSpacing.x + m_width; // Expected position if next button was on same line

            //cant be after bcs popup
            item->DisplayAndUpdatePopupMenu();

            ImGui::SetCursorPos(cursorBefore);
            isBreak = DisplaySelectable(item, doubleClicked);

            ImGui::EndChild();

            if (i + 1 < count && next_button_x2 < window_visible_x2)
                ImGui::SameLine();

            if (open)
            {
                SV_CURRENT_UI()->SetSelected(item);
                item->SetSelectedState(true);
            }

            if (isBreak)
                break;

            i++;
        }
    }

    void PanelSelectionBox::SetSelectionBoxable(const MAP& p_elements)
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

    bool PanelSelectionBox::DisplaySelectable(ISelectable* p_item, bool p_doubleClicked)
    {
        static auto font = SV_CURRENT_UI()->GetIconFont();
        auto cursorPos = ImGui::GetCursorPos();
        std::string iconTxt = p_item->GetIcon();

        ImGui::PushFont(font);
        ImVec2 sz = ImGui::CalcTextSize(iconTxt.c_str());
        ImGui::PopFont();
        //float canvasWidth = ImGui::GetWindowContentRegionWidth();
        float canvasWidth = m_width - 16;
        float origScale = font->Scale;
        font->Scale = canvasWidth / sz.x;
        ImGui::SetCursorPos({ cursorPos.x + 8, cursorPos.y + 8 });

        ImGui::PushFont(font);
        ImGui::Text("%s", iconTxt.c_str());
        ImGui::PopFont();
        font->Scale = origScale;

        cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPos({ cursorPos.x + 4, cursorPos.y + 4 });

        ImGui::PushTextWrapPos(m_width - 4);
        ImGui::TextWrapped(p_item->GetName().c_str());
        ImGui::PopTextWrapPos();

        if (p_doubleClicked)
            return p_item->InvokeDoubleClick();

        return false;
    }
}