//PanelResourceSelector.h
#pragma once

#include "SurvivantCore/Resources/ResourceRef.h"
#include "SurvivantCore/Resources/ResourceManager.h"
#include "SurvivantEditor/PanelItems/PanelInputBase.h"
#include "SurvivantEditor/PanelItems/PanelPopupMenuButton.h"

#include "Vector/Vector3.h"

#include <functional>

namespace SvEditor::PanelItems
{
	template<class T>
	class PanelResourceSelector : public PanelInputBase<SvCore::Resources::ResourceRef<T>>
	{
	public:
		using Ref = PanelInputBase<SvCore::Resources::ResourceRef<T>>::Value;
		using Callback = PanelInputBase<SvCore::Resources::ResourceRef<T>>::Callback;

		PanelResourceSelector(
			const std::string& p_name,
			Ref p_resourceRef,
			const Callback& p_callback = Callback());

		virtual void DisplayAndUpdatePanel() override;

	private:
		void UpdatePopup();

		std::string				m_items;
		PanelPopupMenuButton	m_allResources;
	};
}

#include "SurvivantEditor/PanelItems/PanelResourceSelector.inl"