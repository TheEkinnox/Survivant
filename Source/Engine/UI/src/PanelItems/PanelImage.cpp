//PanelImage.cpp

#include "SurvivantUI/PanelItems/PanelImage.h"

#include "SurvivantUI/PanelItems/PanelButtonList.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace SvUI::PanelItems
{
	PanelImage::PanelImage(intptr_t p_textureId, const LibMath::Vector2& p_size) :
		m_textureId(p_textureId),
		m_size(p_size)
	{
	}

	void PanelImage::DisplayAndUpdatePanel()
	{
		ImGui::Image(
			(void*)m_textureId, 
			m_size == LibMath::Vector2::zero() ? ImGui::GetContentRegionAvail() : ImVec2(m_size[0], m_size[2]),
			ImVec2(0, 1), 
			ImVec2(1, 0));
	}

	void PanelImage::SetSize(const LibMath::Vector2& p_size)
	{
		m_size = p_size;
	}

	void PanelImage::SetTexture(intptr_t p_textureId)
	{
		m_textureId = p_textureId;
	}
}

