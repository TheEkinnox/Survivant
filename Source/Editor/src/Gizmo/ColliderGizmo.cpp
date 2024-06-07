//ColliderGizmo.cpp
#include "SurvivantEditor/Gizmo/ColliderGizmo.h"

#include "SurvivantPhysics/Collider/BoxCollider.h"
#include "SurvivantPhysics/Collider/CapsuleCollider.h"
#include "SurvivantPhysics/Collider/SphereCollider.h"

#include "Transform.h"
#include "Vector.h"
#include "Trigonometry.h"
#include "Matrix/Matrix3.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <functional>

using namespace SvCore::ECS;
using namespace LibMath;
using namespace SvPhysics;

namespace SvEditor::Gizmo
{
	void ColliderGizmo::Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp, const LibMath::Transform& p_camTrans)
	{
		Transform* eTrans = m_entity.Get<Transform>();

		if (!m_sphUsing && !m_capUsing)
			RenderBoxCollider(p_view, p_proj, p_vp, eTrans? *eTrans : Transform());

		if (!m_boxUsing && !m_capUsing)
			RenderSphCollider(p_view, p_proj, p_vp, eTrans? *eTrans : Transform(), p_camTrans);

		if (!m_boxUsing && !m_sphUsing)
			RenderCapCollider(p_view, p_proj, p_vp, eTrans? *eTrans : Transform());
	}

	void ColliderGizmo::SetEntity(const SvCore::ECS::EntityHandle& p_entity)
	{
		m_entity = p_entity;
	}

	bool ColliderGizmo::IsUsing()
	{
		return m_boxUsing || m_capUsing || m_sphUsing;
	}

	Vector2 ColliderGizmo::GetOrigin(const LibMath::Matrix4& p_vp, const LibMath::Vector3& p_worldPos)
	{
		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 winPos = ImGui::GetWindowPos();

		Vector4 copy = p_vp * Vector4(p_worldPos, 1.f);
		copy *= 0.5f / copy.m_w;
		copy += Vector4(0.5f, 0.5f, 0.f, 0.f);
		copy.m_y = 1.f - copy.m_y;
		copy.m_x *= size.x;
		copy.m_y *= size.y;
		copy.m_x += winPos.x;
		copy.m_y += winPos.y;

		return Vector2(copy.m_x, copy.m_y);
	}

	void ColliderGizmo::RenderBoxCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, 
		const LibMath::Matrix4& /*p_vp*/, const LibMath::Transform& p_entityTrans)
	{
		BoxCollider* box = m_entity.Get<BoxCollider>();
		if (!box)
			return;

		//check box
		BoxCollider newBox = *box;
		newBox.m_offset = Vector3::zero();
		BoundingBox newCollider = newBox.GetBoundingBox();

		auto test = p_entityTrans;
		test.translate(box->m_offset);
		Matrix4 worldMat = test.getWorldMatrix().transposed();

		ImGuizmo::Manipulate(p_view.getArray(), p_proj.getArray(), ImGuizmo::BOUNDS,
			ImGuizmo::MODE::LOCAL, worldMat.getArray(), nullptr, nullptr,
			reinterpret_cast<float*>(&newCollider));

		bool oldUsing = m_boxUsing;
		m_boxUsing = ImGuizmo::IsUsing();

		if (!oldUsing && m_boxUsing)
			m_oldBox = *box;

		if (m_boxUsing)
		{
			Vector3 pos; Quaternion rot; Vector3 scl;
			Transform::decomposeMatrix(worldMat.transposed(), pos, rot, scl);

			box->m_size = m_oldBox.m_size * (2.f * (scl / test.getScale() - 1.f) + 1.f);
		}
	}

	void ColliderGizmo::RenderSphCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& /*p_vp*/, 
		const LibMath::Transform& p_entityTrans, const LibMath::Transform& /*p_camTrans*/)
	{		
		SphereCollider* sph = m_entity.Get<SphereCollider>();
		if (!sph)
			return;

		//check box
		SphereCollider newSph = *sph;
		newSph.m_offset = Vector3(0, FLT_MAX, 0);
		BoundingBox newCollider = newSph.GetBoundingBox();

		auto eTrans = p_entityTrans;
		eTrans.translate(sph->m_offset);
		Matrix4 worldMat = eTrans.getWorldMatrix().transposed();

		ImGuizmo::Manipulate(p_view.getArray(), p_proj.getArray(), ImGuizmo::BOUNDS,
			ImGuizmo::MODE::LOCAL, worldMat.getArray(), nullptr, nullptr,
			reinterpret_cast<float*>(&newCollider));

		bool oldUsing = m_sphUsing;
		m_sphUsing = ImGuizmo::IsUsing();

		if (!oldUsing && m_sphUsing)
			m_oldSph = *sph;

		if (m_sphUsing)
		{
			Vector3 pos; Quaternion rot; Vector3 scl;
			Transform::decomposeMatrix(worldMat.transposed(), pos, rot, scl);

			float newMax;
			//if (/* clicked on corner ||| */ scl.magnitudeSquared() >= eTrans.getScale().magnitudeSquared())
			newMax = LibMath::max(scl.m_x, scl.m_z);
			//else
			//	newMax = LibMath::min(scl.m_x, scl.m_z);

			Vector3 oldScl = eTrans.getScale();
			float oldMax = LibMath::max(oldScl.m_x, oldScl.m_z);

			//box->m_size = m_oldBox.m_size * (2.f * (scl / test.getScale() - 1.f) + 1.f);
			sph->m_radius = m_oldSph.m_radius * (2.f * (newMax / oldMax - 1.f) + 1.f);
		}


		
		//auto scale = trans.getWorldScale();
		//float radius = sph->m_radius * LibMath::max(LibMath::max(scale.m_x, scale.m_y), scale.m_z);

		//Vector2 origin = GetOrigin(p_vp, trans.getWorldPosition());

		//Vector3 axisX = radius * Vector3::up();
		//axisX.rotate(p_camTrans.getRotation());

		//Vector3 axisY = radius * Vector3::right();
		//axisY.rotate(p_camTrans.getRotation());

		////first
		////Vector2 W = GetOrigin(p_vp, trans.getWorldPosition() - axisX + axisY);// - origin;
		////Vector2 X = GetOrigin(p_vp, trans.getWorldPosition() + axisX + axisY);// - origin;
		////Vector2 Z = GetOrigin(p_vp, trans.getWorldPosition() + axisX - axisY);// - origin;
		////Vector2 Y = GetOrigin(p_vp, trans.getWorldPosition() - axisX - axisY);// - origin;

		//Vector2 A = GetOrigin(p_vp, trans.getWorldPosition() + axisX);
		//Vector2 B = GetOrigin(p_vp, trans.getWorldPosition() + axisY);
		//
		//ImDrawList* drawList = ImGui::GetWindowDrawList();

		//float radA = (A - origin).magnitude();
		//float radB = (B - origin).magnitude();
		//drawList->AddEllipse(ImVec2(origin.m_x, origin.m_y), radA, radB, ElipseColor);

		////drawList->AddCircle(ImVec2(origin.m_x, origin.m_y), radA, radB, ElipseColor);

		//drawList->AddCircleFilled(ImVec2(A.m_x, A.m_y), 10.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f)));
		//drawList->AddCircleFilled(ImVec2(B.m_x, B.m_y), 10.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f)));
		////drawList->AddCircleFilled(ImVec2(Y.m_x, Y.m_y), 10.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f)), 64);
		////drawList->AddCircleFilled(ImVec2(Z.m_x, Z.m_y), 10.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f)), 64);
	}

	void ColliderGizmo::RenderCapCollider(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& /*p_vp*/, 
		const LibMath::Transform& p_entityTrans)
	{
		CapsuleCollider* cap = m_entity.Get<CapsuleCollider>();
		if (!cap)
			return;

		//check box
		CapsuleCollider newCap = *cap;
		newCap.m_offset = Vector3(0.f, 0.f, 0.f);
		BoundingBox newCollider = cap->GetUpwardsBoundingBox();

		auto eTrans = p_entityTrans;
		eTrans.translate(cap->m_offset);
		eTrans.rotate(Quaternion::fromTo(Vector3::up(), cap->m_axis)); //rotatse with capsule axis

		Matrix4 worldMat = eTrans.getWorldMatrix().transposed();

		ImGuizmo::Manipulate(p_view.getArray(), p_proj.getArray(), ImGuizmo::BOUNDS,
			ImGuizmo::MODE::LOCAL, worldMat.getArray(), nullptr, nullptr,
			reinterpret_cast<float*>(&newCollider));

		bool oldUsing = m_capUsing;
		m_capUsing = ImGuizmo::IsUsing();

		if (!oldUsing && m_capUsing)
			m_oldCap = *cap;

		if (m_capUsing)
		{
			Vector3 pos; Quaternion rot; Vector3 scl;
			Transform::decomposeMatrix(worldMat.transposed(), pos, rot, scl);

			float newRadMax;
			if (scl.magnitudeSquared() >= eTrans.getScale().magnitudeSquared())
				newRadMax = LibMath::max(scl.m_x, scl.m_z);
			else
				newRadMax = LibMath::min(scl.m_x, scl.m_z);


			Vector3  oldScl = eTrans.getScale();
			float oldMax = LibMath::max(oldScl.m_x, oldScl.m_z);

			cap->m_radius = LibMath::max(0.f, m_oldCap.m_radius * (2.f * (newRadMax / oldMax - 1.f) + 1.f));

			auto height = (m_oldCap.m_height + 2.f * m_oldCap.m_radius) * (2.f * (scl.m_y / oldScl.m_y - 1.f) + 1.f);
			cap->m_height = LibMath::max(0.f, height - 2.f * cap->m_radius);
		}
	}


}