//ColliderGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Matrix/Matrix4.h"
#include "Transform.h"

#include <functional>

namespace SvEditor::Gizmo
{
	class ColliderGizmo
	{
	public:
		ColliderGizmo() = default;
		~ColliderGizmo() = default;

		void Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp, const LibMath::Transform& p_camTrans);

		//bool IsUsing();

		void SetEntity(const SvCore::ECS::EntityHandle& p_entity);
		//void SetOperation(const ImGuizmo::OPERATION& p_operation);

	private:
		//void BoxCollider();

		SvCore::ECS::EntityHandle	m_entity;
		bool m_isUsing = false;
	};
}