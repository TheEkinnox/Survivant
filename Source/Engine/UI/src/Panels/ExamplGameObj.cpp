//ExamplGameObj.cpp

#include "SurvivantUI/Panels/ExamplGameObj.h"
#include "SurvivantUI/Core/UIManager.h"
#include "SurvivantUI/PanelItems/PanelTransformInput.h"



#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
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

        ExamplResource::DisplayAndUpdatePanel();
        ImGui::Separator();

        for (auto& item : m_defaultComponents)
        {
            bool open = ImGui::CollapsingHeader(item.m_name.c_str(), flags);
            if (open)
                item.m_panelable->DisplayAndUpdatePanel();
        }

        for (auto& item : m_aditionalComponents)
        {
            bool open = ImGui::CollapsingHeader(item.m_name.c_str(), flags);
            ImGui::SameLine();

            ImGui::Text("add menu popup here");

            if (open)
                item.m_panelable->DisplayAndUpdatePanel();
        }

        ImGui::Separator();

        m_addComponentButton.DisplayAndUpdatePanel();
        //can display other items
    }

    void ExamplGameObj::AddComponentCallback(ExamplGameObj* p_this)
    {
        p_this->m_aditionalComponents.push_back({ "Transform", std::make_shared<PanelTransformInput>() });
    }
}