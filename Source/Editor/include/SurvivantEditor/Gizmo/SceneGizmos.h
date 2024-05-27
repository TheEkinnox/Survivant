//SceneGizmos.h
#pragma once

#include "TransformGizmo.h"

#include <SurvivantApp/Core/RenderingContext.h>

#include <SurvivantCore/ECS/EntityHandle.h>

#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include <functional>

namespace SvEditor::Gizmo
{
	class SceneGizmos
	{
	public:
		using Context = std::weak_ptr<SvApp::Core::RenderingContext>;

		SceneGizmos(const Context& p_context);
		~SceneGizmos() = default;

		void RenderGizmos();

		bool UsingGizmo();

		TransformGizmo						m_transform;
	private:
		Context								m_context;
	};
}