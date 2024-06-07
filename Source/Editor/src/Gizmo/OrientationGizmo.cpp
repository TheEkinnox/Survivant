//OrientationGizmo.h
#include "SurvivantEditor/Gizmo/OrientationGizmo.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <functional>

using namespace LibMath;

namespace SvEditor::Gizmo
{
	OrientationGizmo::OrientationGizmo(const LibMath::Vector2& p_size) :
		m_size(p_size)
	{}
	
	void OrientationGizmo::Render(LibMath::Matrix4& p_view, const LibMath::Matrix4& p_proj, const LibMath::Vector2& p_pos)
	{
		static auto Color = ImGui::GetColorU32(ImGuiCol_Header);
		static Matrix4 mat = Matrix4(1.f);

		ImGuizmo::ViewManipulate(
			p_view.getArray(), 
			p_proj.getArray(), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::LOCAL, mat.getArray(),
			8.0f, 
			ImVec2(p_pos.m_x - m_size.m_x, p_pos.m_y), 
			ImVec2(m_size.m_x, m_size.m_y), 
			Color);
	}


	bool OrientationGizmo::IsUsing()
	{
		return m_isUsing;
	}
}

