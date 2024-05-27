//SceneGizmos.h
#pragma once

#include <SurvivantApp/Core/RenderingContext.h>

#include <SurvivantCore/ECS/EntityHandle.h>

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

	private:
		void SetTransformGizmo();

		Context		m_context;
	};
}