//SceneGizmos.cpp
#include "SurvivantEditor/Gizmo/SceneGizmos.h"

#include <SurvivantCore/Debug/Assertion.h>

#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"

#include <functional>

#include <imgui.h>
#include <ImGuizmo.h>

using namespace SvCore::ECS;
using namespace LibMath;

namespace SvEditor::Gizmo
{
	SceneGizmos::SceneGizmos(const Context& p_context) :
		m_context(p_context),
		m_orientation(Vector2(200, 200))
	{
		m_transform.SetEntity(p_context.lock()->s_editorSelectedEntity);
	}

	void SceneGizmos::RenderGizmos(const LibMath::Vector2& p_orientationPos, bool isSmallDisplay)
	{	
		ASSERT(!m_context.expired(), "SceneGizmos context not set");

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto winPos = ImGui::GetWindowPos();
		auto winSize = ImGui::GetWindowSize();
		ImGuizmo::SetRect(winPos.x, winPos.y, winSize.x, winSize.y);

		auto [cam, cTrans] = m_context.lock()->GetCameraInfo();
		ASSERT(cam && cTrans, "Camera or Transform in SceneGizmos is nullptr");

		auto copyCProj = cam->GetProjection().transposed();
		auto copyCTrans = cTrans->getMatrix();
		float det = copyCTrans.determinant();
		copyCTrans = copyCTrans.coMatrix();
		copyCTrans /= det;

		Radian yaw = cTrans->getRotation().toYawPitchRoll().m_y;
		yaw.wrap(true);
		yaw = yaw * (yaw < Radian(0.f) ? -1.f : 1.f);
		float percent = LibMath::sin(yaw);
		m_grid.Render(*cam, copyCTrans, copyCProj, cTrans->getPosition(), percent);

		m_transform.SetEntity(m_context.lock()->s_editorSelectedEntity);
		m_transform.Render(copyCTrans, copyCProj);

		m_collider.SetEntity(m_context.lock()->s_editorSelectedEntity);
		m_collider.Render(copyCTrans, copyCProj, (*cam)->GetViewProjection(), *cTrans);

		
		if (!isSmallDisplay)
		{
			m_orientation.Render(copyCTrans, copyCProj, p_orientationPos + Vector2(winPos.x, winPos.y));
			//det = copyCTrans.determinant();
			//copyCTrans = copyCTrans.coMatrix();
			//copyCTrans /= det;
			//cTrans->setMatrix(copyCTrans);
		}
	}

	bool SceneGizmos::UsingGizmo()
	{
		return m_transform.IsUsing() || m_orientation.IsUsing();
	}
}

