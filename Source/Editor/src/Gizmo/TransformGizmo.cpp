//TransformGizmo.cpp
#include "SurvivantEditor/Gizmo/TransformGizmo.h"

#include "Transform.h"

#include <functional>


using namespace SvCore::ECS;
using namespace LibMath;

namespace SvEditor::Gizmo
{
	void TransformGizmo::Render(
		const LibMath::Matrix4& p_view, 
		const LibMath::Matrix4& p_proj)
	{
		if (m_entity.GetEntity() == NULL_ENTITY)
			return;

		Transform* eTrans = m_entity.Get<Transform>();

		if (!eTrans)
			return;

		auto mat = eTrans->getMatrix().transposed();
		ImGuizmo::Manipulate(p_view.getArray(), p_proj.getArray(), m_operation,
			ImGuizmo::LOCAL, mat.getArray());

		m_isUsing = ImGuizmo::IsUsing();
		if (m_isUsing)
			eTrans->setMatrix(mat.transposed());
		

	}

	void TransformGizmo::DrawNoTransform()
	{
		//TODO : TransformGizmo::DrawNoTransform()
		//drawList->AddCircleFilled(gContext.mScreenSquareCenter, gContext.mStyle.CenterCircleSize, colors[0], 32);
	}

	bool TransformGizmo::IsUsing()
	{
		return m_isUsing;
	}

	void TransformGizmo::SetEntity(const SvCore::ECS::EntityHandle& p_entity)
	{
		m_entity = p_entity;
	}

	void TransformGizmo::SetOperation(const ImGuizmo::OPERATION& p_operation)
	{
		m_operation = p_operation;
	}
}