//PanelTransformInput.cpp

#include "SurvivantUI/PanelItems/PanelTransformInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    PanelTransformInput::PanelTransformInput(
        const LibMath::Vector3& p_position,
        const LibMath::Quaternion& p_rotation,
        const LibMath::Vector3& p_scale,
        const Callback& p_callback) :
        m_position(p_position),
        m_rotation(p_rotation),
        m_scale(p_scale),
        m_callback(p_callback)
    {
        m_yawPitchRoll = m_rotation.toYawPitchRoll();
    }

    void PanelTransformInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        ImGui::Text("Position");
        ImGui::SameLine();
        if (ImGui::InputFloat3("##", m_position.getArray(), "%3.f", flags))
            m_callback(&m_position, nullptr, nullptr);

        LibMath::Vector3 asDegree = ToVector3Degree(m_yawPitchRoll);

        ImGui::Text("Rotation");
        ImGui::SameLine();
        if (ImGui::InputFloat3("##", asDegree.getArray(), "%3.f", flags))
        {
            LibMath::Vector3 diffDegree = asDegree - ToVector3Degree(m_yawPitchRoll); //m_yawPitchRoll hasnt been modified so still prev

            //add diff to current quat
            auto diffRad = ToVector3Radian(diffDegree);
            m_rotation += LibMath::Quaternion(diffRad);
            m_yawPitchRoll = m_rotation.toYawPitchRoll(); // m_yawPitchRoll += ToVector3Radian(diffDegree);

            m_callback(nullptr, &m_rotation, nullptr);
        }

        ImGui::Text("Scale   ");
        ImGui::SameLine();
        if (ImGui::InputFloat3("##", m_scale.getArray(), "%3.f", flags))
            m_callback(nullptr, nullptr, &m_scale);
    }

    LibMath::Vector3 PanelTransformInput::ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians)
    {
        return LibMath::Vector3(p_radians[0].radian(), p_radians[1].radian(), p_radians[2].radian());
    }

    LibMath::TVector3<LibMath::Radian> PanelTransformInput::ToVector3Radian(const LibMath::Vector3& p_degrees)
    {
        return LibMath::TVector3<LibMath::Radian>(
            LibMath::Radian(p_degrees[0]),
            LibMath::Radian(p_degrees[1]),
            LibMath::Radian(p_degrees[2]));
    }
}