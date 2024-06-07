//PanelImage.cpp

#include "SurvivantEditor/PanelItems/PanelImage.h"

#include "SurvivantEditor/PanelItems/PanelButtonList.h"
#include "SurvivantCore/Debug/Assertion.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvEditor::PanelItems
{
	PanelImage::PanelImage(void* p_textureId, const LibMath::Vector2& p_size) :
		m_textureId(p_textureId),
		m_size(p_size)
	{}

	void PanelImage::DisplayAndUpdatePanel()
	{
		ImGui::Image(
			m_textureId,
			m_size == LibMath::Vector2::zero() ? ImGui::GetContentRegionAvail() : ImVec2(m_size[0], m_size[2]),
			ImVec2(0, 1),
			ImVec2(1, 0));
	}

	void PanelImage::SetSize(const LibMath::Vector2& p_size)
	{
		m_size = p_size;
	}

	void PanelItems::PanelImage::SetTexture(void* p_textureId)
	{
		m_textureId = p_textureId;
	}
}

