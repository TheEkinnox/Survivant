//InspectorPanel.cpp

#include "SurvivantUI/Panels/InspectorPanel.h"

#include "SurvivantUI/Panels/ExamplGameObj.h"
#include "SurvivantUI/Panels/Panel.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::Panels
{
    InspectorPanel::InspectorPanel()
    {
        m_name = GetUniqueName(NAME, s_idGenerator.GetUnusedId());

        m_info = std::make_shared<ExamplGameObj>();
    }

    InspectorPanel::~InspectorPanel()
    {
    }

    void InspectorPanel::SetInpectorInfo(const std::shared_ptr<IPanelable>& p_info)
    {
        m_info = p_info;
    }

    void InspectorPanel::ClearInfo()
    {
        m_info.reset();
    }

    Panel::ERenderFlags InspectorPanel::Render()
    {
        static bool open = true;

        if (!ImGui::Begin(m_name.c_str(), &open))
        {
            ImGui::End();
            return Panel::ERenderFlags();
        }

        m_info->DisplayAndUpdatePanel();

        ImGui::End();

        auto flags = ERenderFlags();
        if (!open)
            flags = ERenderFlags::CLOSE;

        return flags;
    }
}