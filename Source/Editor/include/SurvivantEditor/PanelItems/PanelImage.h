//PanelImage.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"

#include "Vector/Vector2.h"

#include <functional>

namespace SvEditor::PanelItems
{
	using namespace Interfaces;

	class PanelImage : public IPanelable
	{
	public:
		PanelImage(void* p_textureId = nullptr, const LibMath::Vector2& p_size = LibMath::Vector2::zero());
		~PanelImage() = default;

		// Inherited via IPanelable
		void DisplayAndUpdatePanel() override;

		void SetSize(const LibMath::Vector2& p_size);
		//change to world current scene ptr
		void SetTexture(void* p_textureId);

	private:
		void*				m_textureId;
		LibMath::Vector2	m_size;
	};
}
