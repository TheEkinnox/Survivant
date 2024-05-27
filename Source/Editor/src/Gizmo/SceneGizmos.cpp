//SceneGizmos.cpp
#include "SurvivantEditor/Gizmo/SceneGizmos.h"

#include <SurvivantCore/Debug/Assertion.h>

#include "Vector/Vector3.h"
#include "Matrix.h"

#include <functional>

#include <imgui.h>
#include <ImGuizmo.h>

using namespace SvCore::ECS;
using namespace LibMath;

namespace SvEditor::Gizmo
{
	SceneGizmos::SceneGizmos(const Context& p_context) :
		m_context(p_context)
	{
		m_transform.SetEntity(p_context.lock()->s_editorSelectedEntity);
	}

	void SceneGizmos::RenderGizmos()
	{
		ASSERT(!m_context.expired(), "SceneGizmos context not set");

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto winPos = ImGui::GetWindowPos();
		auto winSize = ImGui::GetWindowSize();
		ImGuizmo::SetRect(winPos.x, winPos.y, winSize.x, winSize.y);

		auto [cam, cTrans] = m_context.lock()->GetCameraInfo();
		ASSERT(cam && cTrans, "Camera or Transform in SceneGizmos is nullptr");

		m_transform.SetEntity(m_context.lock()->s_editorSelectedEntity);
		m_transform.Render(cTrans->getWorldMatrix().inverse().transposed(), cam->GetProjection().transposed());
	}

	bool SceneGizmos::UsingGizmo()
	{
		return m_transform.IsUsing();
	}
}

