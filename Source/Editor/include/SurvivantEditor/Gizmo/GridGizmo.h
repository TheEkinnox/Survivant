//TransformGizmo.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include "Matrix/Matrix4.h"

#include <functional>

namespace SvEditor::Gizmo
{
	class GridGizmo
	{
	public:
		GridGizmo() = default;
		~GridGizmo() = default;

		void Render(const LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj);
		
	private:
		void DrawNoTransform();
	};
}