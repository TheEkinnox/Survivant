//PanelTransformInput.cpp

#include "SurvivantEditor/PanelItems/PanelTransformInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
    PanelTransformInput::PanelTransformInput(
        const GetRefFunc& p_getRef, const TransformCallback& p_callback) :
        BasePanelTransformInput(p_getRef, p_callback)
    {
        m_yawPitchRoll = GetRef().getRotation().toYawPitchRoll();
    }

    PanelTransformInput::PanelTransformInput(
        const BasePanelTransformInput::GetCopyFunc& p_getCopy, const TransformCallback& p_callback) :
        BasePanelTransformInput(p_getCopy, p_callback)
    {
    }

    void PanelTransformInput::DisplayAndUpdatePanel()
    {
        static auto speedSlider = 1.0f;
        static auto minSlider = -FLT_MAX;
        static auto maxSlider = FLT_MAX;

        auto& trans = GetRef();
        auto position = trans.getPosition();
        auto rotation = trans.getRotation();
        auto scale =    trans.getScale();

        ImGui::Text("Position");
        ImGui::SameLine();
        ImGui::PushID(0);
        if (ImGui::DragFloat3("##", position.getArray(), speedSlider, minSlider, maxSlider))
        {
            trans.setPosition(position);

            if (m_callback)
                m_callback({ &position, nullptr, nullptr });
        }
        ImGui::PopID();


        LibMath::Vector3 asDegree = YPRToDegree(m_yawPitchRoll);

        ImGui::Text("Rotation");
        ImGui::SameLine();
        ImGui::PushID(1);
        if (ImGui::DragFloat3("##", asDegree.getArray(), speedSlider, minSlider, maxSlider))
        {
            //LibMath::Vector3 diffDegree = asDegree - YPRToDegree(m_yawPitchRoll); //m_yawPitchRoll hasnt been modified so still prev

            //add diff to current quat
            auto currentRad = DegreeToYPR(asDegree);
            rotation = LibMath::Quaternion(currentRad);
            m_yawPitchRoll = currentRad; // m_yawPitchRoll += DegreeToYPR(diffDegree);

            trans.setRotation(rotation);

            if (m_callback)
                m_callback({ nullptr, &rotation, nullptr });
        }
        ImGui::PopID();


        ImGui::Text("Scale   ");
        ImGui::SameLine();
        ImGui::PushID(2);
        if (ImGui::DragFloat3("##", scale.getArray(), speedSlider, minSlider, maxSlider))
        {
            if (scale.m_x == 0)
                scale.m_x = FLT_MIN;
            if (scale.m_y == 0)
                scale.m_y = FLT_MIN;
            if (scale.m_z == 0)
                scale.m_z = FLT_MIN;

            trans.setScale(scale);

            if (m_callback)
                m_callback({ nullptr, nullptr, &scale });
        }
        ImGui::PopID();
    }

    LibMath::Vector3 PanelTransformInput::YPRToDegree(const LibMath::TVector3<LibMath::Radian>& p_radians)
    {
        return LibMath::Vector3(p_radians[1].degree(true), p_radians[0].degree(true), p_radians[2].degree(true));
    }

    LibMath::TVector3<LibMath::Radian> PanelTransformInput::DegreeToYPR(const LibMath::Vector3& p_degrees)
    {
        auto tmp = LibMath::TVector3<LibMath::Radian>(
            LibMath::Degree(std::fmodf(p_degrees[1], 360)),
            LibMath::Degree(std::fmodf(p_degrees[0], 360)),
            LibMath::Degree(std::fmodf(p_degrees[2], 360)));

        return tmp;
    }

    //void PanelTransformInput::ResetVector3ValIfNan(LibMath::Vector3& p_value)
    //{
    //    if (std::isnan(p_value[0]))
    //        p_value[0] = 0;
    //    if (std::isnan(p_value[1]))
    //        p_value[1] = 0;
    //    if (std::isnan(p_value[2]))
    //        p_value[2] = 0;
    //}
}