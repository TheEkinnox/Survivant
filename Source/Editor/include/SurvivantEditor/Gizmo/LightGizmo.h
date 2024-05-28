//TransformGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Matrix/Matrix4.h"

#include <functional>

namespace SvEditor::Gizmo
{
	class LightGizmo
	{
	public:
		LightGizmo() = default;
		~LightGizmo() = default;

		void Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj);

		bool IsUsing();

		void SetEntity(const SvCore::ECS::EntityHandle& p_entity);

	private:
		SvCore::ECS::EntityHandle	m_entity;
		bool m_isUsing = false;
	};
}