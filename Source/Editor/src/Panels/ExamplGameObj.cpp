//ExamplGameObj.cpp

#include "SurvivantEditor/Panels/ExamplGameObj.h"

#include "SurvivantCore/Events/EventManager.h"
#include "SurvivantEditor/Core/IUI.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/MenuItems/PopupMenu.h"
#include "SurvivantEditor/MenuItems/MenuButton.h"


#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::Panels
{
    void ExamplResource::DisplayAndUpdatePanel()
    {
        //static PanelButton s_open = PanelButton("Open", );
        static auto font = SV_CURRENT_UI()->GetFontDefault();

        auto cursorPos = ImGui::GetCursorPos();

        ImGui::PushFont(font);
        ImGui::Text("%s", m_resourceLogo.c_str());
        ImGui::PopFont();

        ImGui::SameLine();
        ImGui::Text(m_name.c_str());

        ImGui::SameLine();
        ImGui::Text((std::string("(") + m_resourceName + ")").c_str());
    }

    const std::string& ExamplResource::GetIcon()
    {
        return m_resourceLogo;
    }

    const std::string& ExamplResource::GetName()
    {
        return m_name;
    }

    bool ExamplResource::InvokeOpen()
    {
        SV_EVENT_MANAGER().Invoke<Core::IUI::DebugEvent>("Clicked Resource");
        return false;
    }

    bool ExamplResource::InvokeSelected()
    {
        return false;
    }

    void ExamplResource::DisplayAndUpdatePopupMenu()
    {
        SV_EVENT_MANAGER().Invoke<Core::IUI::DebugEvent>("No PopupMenu");
    }

    bool ExamplResource::GetSelectedState()
    {
        return m_isSelected;
    }

    void ExamplResource::SetSelectedState(bool p_isSelected)
    {
        m_isSelected = p_isSelected;
    }

    ExamplGameObj::ExamplGameObj() :
        m_addComponentButton(std::string("Add Component"))
    {
        auto tmp = std::bind(&ExamplGameObj::AddComponentCallback, this);
        m_addComponentButton.AddCallback(tmp);
        AddComponentCallback(this);
    }

    void ExamplGameObj::DisplayAndUpdatePanel()
    {
        static ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen;

        MenuItems::PopupMenu tmpPopup;
        tmpPopup.m_items.emplace_back(std::make_unique<MenuItems::MenuButton>("Test", [](char) {}));

        ExamplResource::DisplayAndUpdatePanel();
        ImGui::Separator();

        for (auto& item : m_defaultComponents)
        {
            bool open = ImGui::CollapsingHeader(item.m_name.c_str(), flags);

            if (open)
                item.m_panelable->DisplayAndUpdatePanel();
        }

        //ImGuiContext& g = *GImGui;
        //ImGuiLastItemData last_item_backup = g.LastItemData;
        //float button_size = g.FontSize;
        //float button_x = ImMax(g.LastItemData.Rect.Min.x, g.LastItemData.Rect.Max.x - g.Style.FramePadding.x - button_size);
        //float button_y = g.LastItemData.Rect.Min.y + g.Style.FramePadding.y;
        //ImGuiID close_button_id = GetIDWithSeed("#CLOSE", NULL, id);
        //if (CloseButton(close_button_id, ImVec2(button_x, button_y)))
        //    *p_visible = false;
        //g.LastItemData = last_item_backup;

        for (auto& item : m_aditionalComponents)
        {
            bool button = true;
            bool open = ImGui::CollapsingHeader(item.m_name.c_str(), &button,  flags);
            if (!button)
                tmpPopup.m_isOpen = true;

            tmpPopup.DisplayAndUpdatePanel();

            if (open)
                item.m_panelable->DisplayAndUpdatePanel();
        }

        ImGui::Separator();

        m_addComponentButton.DisplayAndUpdatePanel();
        //can display other items
    }

    void ExamplGameObj::AddComponentCallback(ExamplGameObj* p_this)
    {
        static std::shared_ptr<Transform> trans = std::make_shared<Transform>();
        p_this->m_aditionalComponents.push_back({ "Transform", std::make_shared<PanelTransformInput>(*trans.get()) });
    }
}