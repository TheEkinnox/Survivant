//OrientationGizmo.h
#pragma once

#include <functional>

#include <SurvivantRendering/Core/Color.h>

#include "Vector/Vector2.h"
#include "Matrix/Matrix4.h"
#include "Transform.h"

namespace SvEditor::Gizmo
{
	class OrientationGizmo
	{
	public:
		OrientationGizmo(const LibMath::Vector2& p_size = { 10, 10 });
		~OrientationGizmo() = default;
		
		void Render(LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Vector2& p_pos);

		bool IsUsing();

	private:
		bool				m_isUsing = false;
		LibMath::Vector2	m_size;
		LibMath::Matrix4	m_prevMat;
	};
}