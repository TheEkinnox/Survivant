//GridGizmo.cpp
#include "SurvivantEditor/Gizmo/GridGizmo.h"

#include "SurvivantRendering/Components/CameraComponent.h"

#include "Transform.h"
#include "Arithmetic.h"
#include "Trigonometry.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <functional>


using namespace SvCore::ECS;
using namespace LibMath;
using namespace SvRendering::Components;

namespace SvEditor::Gizmo
{
    void GridGizmo::Render(const SvRendering::Components::CameraComponent& p_cam,
        const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj,
        const LibMath::Vector3& p_pos, float p_percent)
    {
        static const Matrix4 Mat = Matrix4(1.f);
        static const float HeightMult = 0.08f;

        //sin(fovy/2) = width / 2 dist => width = 2 * dist * sin(fovy/2)
        float viewWidth = 2 * p_cam.GetPerspectiveFar() * LibMath::sin(p_cam.GetFovY() * 0.5f);
        float aspect = p_cam.GetAspect();

        float mult = LibMath::clamp((LibMath::abs(p_pos.m_y) * HeightMult + p_percent) / 2.f, 0.001f, 1.f);

        Matrix4 newView = (p_view.transposed() * translation(
            (float)((int)p_pos.m_x), 0.f, (float)((int)p_pos.m_z))).transposed();

        ImGuizmo::DrawGrid(newView.getArray(), p_proj.getArray(), Mat.getArray(),
            (float)(int)(mult * viewWidth * (aspect > 1.f ? aspect : 1.f / aspect)));
    }
}