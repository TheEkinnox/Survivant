//SceneGizmos.h
#pragma once

#include "TransformGizmo.h"
#include "OrientationGizmo.h"
#include "GridGizmo.h"

#include <SurvivantApp/Core/RenderingContext.h>

#include <SurvivantCore/ECS/EntityHandle.h>

#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include "Vector/Vector2.h"

#include <functional>


namespace SvEditor::Gizmo
{
	class SceneGizmos
	{
	public:
		using Context = std::weak_ptr<SvApp::Core::RenderingContext>;

		SceneGizmos(const Context& p_context);
		~SceneGizmos() = default;

		void RenderGizmos(const LibMath::Vector2& p_orientationPos, bool isSmallDisplay);

		bool UsingGizmo();

		TransformGizmo		m_transform;
		OrientationGizmo	m_orientation;
		GridGizmo			m_grid;
	private:
		Context				m_context;
	};
}