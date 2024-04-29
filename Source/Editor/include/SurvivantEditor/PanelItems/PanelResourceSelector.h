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
		using Ref = PanelInputBase<SvCore::Resources::ResourceRef<T>>::GetRefFunc;
		using Callback = PanelInputBase<SvCore::Resources::ResourceRef<T>>::Callback;

		PanelResourceSelector(
			const std::string& p_name,
			const Ref& p_resourceRef,
			const Callback& p_callback = Callback());
		~PanelResourceSelector() = default;

		PanelResourceSelector(const PanelResourceSelector& p_other);
		PanelResourceSelector(PanelResourceSelector&& p_other);
		PanelResourceSelector<T>& operator=(const PanelResourceSelector& p_other);

		virtual void DisplayAndUpdatePanel() override;

	private:
		void GetAllResources();

		std::string								m_name;
		std::shared_ptr<PanelPopupMenuButton>	m_allResources;
	};
}

#include "SurvivantEditor/PanelItems/PanelResourceSelector.inl"