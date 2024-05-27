//SceneGizmos.cpp
#include "SurvivantEditor/Gizmo/SceneGizmos.h"

#include <SurvivantCore/Debug/Assertion.h>

#include "Vector/Vector3.h"
#include "Transform.h"
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
	{}

	void SceneGizmos::RenderGizmos()
	{
		ASSERT(!m_context.expired(), "SceneGizmos context not set");

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		auto& selected = m_context.lock()->s_editorSelectedEntity;

		auto winPos = ImGui::GetWindowPos();
		auto winSize = ImGui::GetWindowSize();
		ImGuizmo::SetRect(winPos.x, winPos.y, winSize.x, winSize.y);

		auto [cam, cTrans] = m_context.lock()->GetCameraInfo();
		ASSERT(cam && cTrans, "Camera or Transform in SceneGizmos is nullptr");


		//Transform trans;
		//trans.setPosition(Vector3(0, 0, -1.f));
		//auto camView = cTrans->getWorldMatrix().inverse();
		//auto camProj = perspectiveProjection(Radian(90), 3.0f/4.0f, 0.1f, 50.0f);
		//auto entityTrans = trans.getMatrix();

		//ImGuizmo::Manipulate(
		//	camView.getArray(),
		//	camProj.getArray(),
		//	ImGuizmo::OPERATION::TRANSLATE ,
		//	ImGuizmo::WORLD, entityTrans.getArray());


		if (selected.GetEntity() != NULL_ENTITY)
		{
			auto eTrans = selected.Get<Transform>();

			if (eTrans)
			{
				ImGuizmo::Manipulate(
					cTrans->getWorldMatrix().inverse().transposed().getArray(),
					cam->GetProjection().transposed().getArray(),
					ImGuizmo::OPERATION::TRANSLATE,
					ImGuizmo::LOCAL, eTrans->getWorldMatrix().transposed().getArray());
			}
		}
	}

	void SceneGizmos::SetTransformGizmo()
	{
	}
}

