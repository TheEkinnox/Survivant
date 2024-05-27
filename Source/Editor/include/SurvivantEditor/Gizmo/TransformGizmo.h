//TransformGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include <functional>

namespace SvEditor::Gizmo
{
	class TransformGizmo
	{
	public:
		TransformGizmo() = default;
		~TransformGizmo() = default;

		void Render();

	private:
		SvCore::ECS::EntityHandle m_entity;
	};
}