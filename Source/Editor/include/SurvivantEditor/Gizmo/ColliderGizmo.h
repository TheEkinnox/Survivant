//ColliderGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>
#include <SurvivantPhysics/Collider/BoxCollider.h>
#include <SurvivantPhysics/Collider/SphereCollider.h>
#include <SurvivantPhysics/Collider/CapsuleCollider.h>

#include "Matrix/Matrix4.h"
#include "Transform.h"
#include "Vector/Vector2.h"

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
		
	private:
		LibMath::Vector2 GetOrigin(const LibMath::Matrix4& p_vp, const LibMath::Vector3& p_worldPos);

		void RenderBoxCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp, 
			const LibMath::Transform& p_entityTrans);
		void RenderSphCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp, 
			const LibMath::Transform& p_entityTrans, const LibMath::Transform& p_camTrans);
		void RenderCapCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp,
			const LibMath::Transform& p_entityTrans);
		
		SvCore::ECS::EntityHandle	m_entity;
		SvPhysics::BoxCollider		m_oldBox;
		SvPhysics::SphereCollider	m_oldSph;
		SvPhysics::CapsuleCollider	m_oldCap;

		bool m_boxUsing = false;
		bool m_sphUsing = false;
		bool m_capUsing = false;
	};
}