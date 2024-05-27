//TransformGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Matrix/Matrix4.h"

#include "imgui.h"
#include "ImGuizmo.h"

#include <functional>

namespace SvEditor::Gizmo
{
	class TransformGizmo
	{
	public:
		TransformGizmo() = default;
		~TransformGizmo() = default;

		void Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj);


		bool IsUsing();

		void SetEntity(const SvCore::ECS::EntityHandle& p_entity);
		void SetOperation(const ImGuizmo::OPERATION& p_operation);

	private:
		void DrawNoTransform();

		SvCore::ECS::EntityHandle	m_entity;
		ImGuizmo::OPERATION			m_operation = ImGuizmo::OPERATION::TRANSLATE;
		bool m_isUsing = false;
	};
}