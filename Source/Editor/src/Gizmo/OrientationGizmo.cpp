//OrientationGizmo.h
#include "SurvivantEditor/Gizmo/OrientationGizmo.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <functional>

namespace SvEditor::Gizmo
{
	OrientationGizmo::OrientationGizmo(const LibMath::Vector2& p_size) :
		m_size(p_size)
	{}
	
	void OrientationGizmo::Render(LibMath::Matrix4& p_view, const LibMath::Matrix4& /*p_proj*/, const LibMath::Vector2& p_pos)
	{
		static auto Color = ImGui::GetColorU32(ImGuiCol_Header);

		ImGuizmo::ViewManipulate(
			p_view.getArray(), /*p_proj.getArray(), ImGuizmo::OPERATION::BOUNDS, ImGuizmo::MODE::LOCAL, */
			8.0f, 
			ImVec2(p_pos.m_x - m_size.m_x, p_pos.m_y), 
			ImVec2(m_size.m_x, m_size.m_y), 
			Color);

		m_isUsing = ImGuizmo::IsUsing();
	}


	bool OrientationGizmo::IsUsing()
	{
		return m_isUsing;
	}
}

