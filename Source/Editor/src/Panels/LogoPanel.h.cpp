//LogoPanel.h
#include "SurvivantEditor/Panels/LogoPanel.h"

#include "SurvivantCore/Resources/ResourceManager.h"

#include "SurvivantRendering/RHI/ITexture.h"
#include "SurvivantRendering/Enums/EPixelDataFormat.h"

#include "imgui.h"

using namespace SvRendering::RHI;
using namespace SvRendering::Enums;
using namespace SvCore::Resources;

namespace SvEditor::Panels
{
    LogoPanel::ERenderFlags LogoPanel::Render()
    {
        //static auto logo = SV_SERVICE(ResourceManager).Load<ITexture>("textures/game/armor-plating1_albedo.png");
        static auto logo = SV_SERVICE(ResourceManager).Load<ITexture>("SurvivantLogo.png");

        bool open = true;
        //fullscreen + undockable/cant move
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse;

        if (ImGui::Begin("##", &open, windowFlags))
        {
            m_image.SetTexture(logo->GetHandle());
            m_image.DisplayAndUpdatePanel();
        }

        ImGui::PopStyleVar(2);

        ImGui::End();


        return LogoPanel::ERenderFlags();
    }

}