//TransformGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Matrix/Matrix4.h"
#include "Vector/Vector3.h"

#include <functional>

namespace SvRendering::Components
{
	class CameraComponent;
}


namespace SvEditor::Gizmo
{
	class GridGizmo
	{
	public:
		GridGizmo() = default;
		~GridGizmo() = default;

		void Render(const SvRendering::Components::CameraComponent& p_cam, 
			const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, 
			const LibMath::Vector3& p_pos, float p_percent);

	};
}