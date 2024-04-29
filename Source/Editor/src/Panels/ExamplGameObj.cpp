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

    bool ExamplResource::Open()
    {
        ISelectable::Open();

        return false;
    }

    bool ExamplResource::Select()
    {
        ISelectable::Select();
        return false;
    }

    void ExamplResource::DisplayAndUpdatePopupMenu()
    {
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

        //can display other items
    }

    void ExamplGameObj::AddComponentCallback(ExamplGameObj* /*p_this*/)
    {
        //static std::shared_ptr<Transform> trans = std::make_shared<Transform>();
        //p_this->m_aditionalComponents.push_back({ "Transform", std::make_shared<PanelTransformInput>(*trans.get()) });
    }
}