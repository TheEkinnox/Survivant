//ColliderGizmo.cpp
#include "SurvivantEditor/Gizmo/ColliderGizmo.h"

#include "SurvivantPhysics/Collider/BoxCollider.h"
#include "SurvivantPhysics/Collider/CapsuleCollider.h"
#include "SurvivantPhysics/Collider/SphereCollider.h"

#include "Transform.h"
#include "Vector.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <functional>

using namespace SvCore::ECS;
using namespace LibMath;
using namespace SvPhysics;

namespace SvEditor::Gizmo
{
	void ColliderGizmo::Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Matrix4& p_vp, const LibMath::Transform& /*p_camTrans*/)
	{
		Transform* eTrans = m_entity.Get<Transform>();

		if (!eTrans)
			return;

		//ImGuizmo::Manipulate();

		//behind cam
		//Vector4 camSpacePosition = p_vp * Vector4(0.f, 0.f, 0.f, 1.f);
		//if (camSpacePosition.m_z < 0.001f)
		//	return;

		ImVec2 size = ImGui::GetWindowSize();
		ImVec2 winPos = ImGui::GetWindowPos();

		Vector4 copy = p_vp * Vector4(eTrans->getWorldPosition(), 1.f);
		copy *= 0.5f / copy.m_w;
		copy += Vector4(0.5f, 0.5f, 0.f, 0.f);
		copy.m_y = 1.f - copy.m_y;
		copy.m_x *= size.x;
		copy.m_y *= size.y;
		copy.m_x += winPos.x;
		copy.m_y += winPos.y;
		ImVec2 origin = ImVec2(copy.m_x, copy.m_y);
		
		ImDrawList* drawList =  ImGui::GetWindowDrawList();
		drawList->AddCircleFilled(origin, 30.f, ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 0.f, 0.f, 1.f)));


		Matrix4 worldMat = eTrans ? eTrans->getWorldMatrix().transposed() : Matrix4(1);
		BoxCollider* box = m_entity.Get<BoxCollider>();

		//check box

		static BoxCollider oldBox;
		BoundingBox newBox = box->GetBoundingBox();

		ImGuizmo::Manipulate(p_view.getArray(), p_proj.getArray(), ImGuizmo::BOUNDS,
			ImGuizmo::MODE::LOCAL, worldMat.getArray(), nullptr, nullptr, 
			reinterpret_cast<float*>(&newBox));



		bool oldUsing = m_isUsing;
		m_isUsing = ImGuizmo::IsUsing();

		if (!oldUsing && m_isUsing)
		{
			oldBox = *box;
		}

		if (m_isUsing)
		{
			Vector3 pos; Quaternion rot; Vector3 scl;
			Transform::decomposeMatrix(worldMat.transposed(), pos, rot, scl);

			box->m_size = (scl / eTrans->getScale()) * oldBox.m_size;
			//box->m_offset = (pos - eTrans->getPosition())/3 + oldBox.m_offset;
		}



		//ImVec2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVP);
		//ImVec2 worldDirSSpace = worldToPos(dirAxis * gContext.mScreenFactor, gContext.mMVP);
		//drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], gContext.mStyle.TranslationLineThickness);

	}

	void ColliderGizmo::SetEntity(const SvCore::ECS::EntityHandle& p_entity)
	{
		m_entity = p_entity;
	}


}