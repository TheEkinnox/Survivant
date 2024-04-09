//PanelImage.h
#pragma once

#include "SurvivantEditor/UI/Interfaces/IPanelable.h"

#include "Vector/Vector2.h"

#include <functional>

namespace SvEditor::UI::PanelItems
{
	using namespace Interfaces;

	class PanelImage : public IPanelable
	{
	public:
		PanelImage(intptr_t& p_textureId, const LibMath::Vector2& p_size = LibMath::Vector2::zero());
		~PanelImage() = default;

		// Inherited via IPanelable
		void DisplayAndUpdatePanel() override;

		void SetSize(const LibMath::Vector2& p_size);
		void SetTexture(intptr_t& p_textureId);

	private:
		//change to world current scene ptr
		intptr_t*			m_textureId;
		LibMath::Vector2	m_size;
	};
}
