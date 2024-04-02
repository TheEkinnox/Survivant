//PanelTextBox.cpp

#include "SurvivantEditor/UI/PanelItems/PanelTextBox.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::UI::PanelItems
{
    using namespace Interfaces;

    void PanelTextBox::DisplayAndUpdatePanel()
    {
        auto& textList = m_filters.empty() ? m_items : m_filteredItems;

        //const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0 /*-footer_height_to_reserve*/), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

            //copy
            if (m_copy)
                ImGui::LogToClipboard();

            for (auto& text : textList)
            {
                text->DisplayAndUpdatePanel();
            }

            if (m_copy)
                ImGui::LogFinish();

            //Scrolling
            if (m_scrollToBottom || (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);

            m_copy = false;
            m_scrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();


    }

    void PanelTextBox::SetFilters(const FilterSet& p_filters)
    {
        for (auto& filter : p_filters)
        {
            //group all filters with the same lenght together
            auto& set = m_filters.try_emplace(filter.length(), FilterSet()).first->second;
            set.insert(filter);
        }

        UpdateFilteredItems();
    }

    void PanelTextBox::AddFilter(const std::string& p_filter)
    {
        //insert filter with others of same lenght
        auto& set = m_filters.try_emplace(p_filter.length(), FilterSet()).first->second;
        set.insert(p_filter);

        UpdateFilteredItems();
    }

    void PanelTextBox::RemoveFilter(const std::string& p_filter)
    {
        const auto& itt = m_filters.find(p_filter.length());

        //does nothing if does not contain filter
        if (itt == m_filters.end())
            return;

        //erase filter
        itt->second.erase(p_filter);

        //if set is empty, remove it from the map
        if (itt->second.empty())
            m_filters.erase(itt);

        UpdateFilteredItems();
    }

    void PanelTextBox::AddOrRemoveFilter(const std::string& p_filter)
    {
        const auto& itt = m_filters.find(p_filter.length());

        //add if doesnt contain
        if (itt == m_filters.end() || !itt->second.contains(p_filter))
        {
            auto& set = m_filters.try_emplace(p_filter.length(), FilterSet()).first->second;
            set.insert(p_filter);
        }
        //remove if contains
        else
            itt->second.erase(p_filter);

        UpdateFilteredItems();
    }

    void PanelTextBox::AddItem(const std::shared_ptr<ITextable>& item, bool p_willScrollToBottom)
    {
        m_items.push_back(item);

        if (IsFilteredItem(item.get()))
            m_filteredItems.push_back(item);

        m_scrollToBottom = p_willScrollToBottom;
    }

    void PanelTextBox::Clear()
    {
        m_items.clear();
        m_filteredItems.clear();
    }

    void PanelTextBox::Copy()
    {
        m_copy = true;
    }

    bool PanelTextBox::IsFilteredItem(const ITextable* p_item)
    {
        if (m_filters.empty())
            return false;

        bool isFiltered = false;
        size_t itemLen = p_item->GetLength();

        for (auto& pair : m_filters)
        {
            size_t filterLen = pair.first;

            if (isFiltered || itemLen < filterLen)
                continue;

            //isFiltered if item is equal to a filter
            auto& set = pair.second;
            isFiltered = set.contains(p_item->GetString(filterLen));
        }

        return isFiltered;
    }

    void PanelTextBox::UpdateFilteredItems()
    {
        m_filteredItems.clear();

        if (m_filters.empty())
            return;

        for (auto& item : m_items)
        {
            //add item if filtered
            if (IsFilteredItem(item.get()))
                m_filteredItems.push_back(item);
        }

    }
}