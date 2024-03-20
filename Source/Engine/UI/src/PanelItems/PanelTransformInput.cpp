//PanelTransformInput.cpp

#include "SurvivantUI/PanelItems/PanelTransformInput.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
    PanelTransformInput::PanelTransformInput(
        Transform& p_transform,
        const Callback& p_callback) :
        m_transform(p_transform),
        m_callback(p_callback)
    {
        m_yawPitchRoll = m_transform.getRotation().toYawPitchRoll();
    }

    void PanelTransformInput::DisplayAndUpdatePanel()
    {
        static int flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

        auto position = m_transform.getPosition();
        auto rotation = m_transform.getRotation();
        auto scale =    m_transform.getScale();

        ImGui::Text("Position");
        ImGui::SameLine();
        ImGui::PushID(0);
        if (ImGui::InputFloat3("##", position.getArray(), "%3.f", flags))
        {
            m_transform.setPosition(position);

            if (m_callback)
                m_callback(&position, nullptr, nullptr);
        }
        ImGui::PopID();


        LibMath::Vector3 asDegree = ToVector3Degree(m_yawPitchRoll);

        ImGui::Text("Rotation");
        ImGui::SameLine();
        ImGui::PushID(1);
        if (ImGui::InputFloat3("##", asDegree.getArray(), "%3.f", flags))
        {
            LibMath::Vector3 currentDeg = asDegree;// - ToVector3Degree(m_yawPitchRoll); //m_yawPitchRoll hasnt been modified so still prev
            //LibMath::Vector3 diffDegree = asDegree - ToVector3Degree(m_yawPitchRoll); //m_yawPitchRoll hasnt been modified so still prev

            //add diff to current quat
            auto currentRad = ToVector3Radian(currentDeg);
            rotation = LibMath::Quaternion(currentRad);
            m_yawPitchRoll = currentRad; // m_yawPitchRoll += ToVector3Radian(diffDegree);

            m_transform.setRotation(rotation);

            if (m_callback)
                m_callback(nullptr, &rotation, nullptr);
        }
        ImGui::PopID();


        ImGui::Text("Scale   ");
        ImGui::SameLine();
        ImGui::PushID(2);
        if (ImGui::InputFloat3("##", scale.getArray(), "%3.f", flags))
        {
            m_transform.setScale(scale);

            if (m_callback)
                m_callback(nullptr, nullptr, &scale);
        }
        ImGui::PopID();
    }

    LibMath::Vector3 PanelTransformInput::ToVector3Degree(const LibMath::TVector3<LibMath::Radian>& p_radians)
    {
        return LibMath::Vector3(p_radians[0].degree(true), p_radians[1].degree(true), p_radians[2].degree(true));
    }

    LibMath::TVector3<LibMath::Radian> PanelTransformInput::ToVector3Radian(const LibMath::Vector3& p_degrees)
    {
        auto tmp = LibMath::TVector3<LibMath::Radian>(
            LibMath::Degree(std::fmodf(p_degrees[0], 360)),
            LibMath::Degree(std::fmodf(p_degrees[1], 360)),
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