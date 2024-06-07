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
	template<class T, class RefT = SvCore::Resources::ResourceRef<T>>
	class PanelResourceSelector : public PanelInputBase<RefT>
	{
	public:
		using Ref = typename PanelInputBase<RefT>::GetRefFunc;
		using Copy = typename PanelInputBase<RefT>::GetCopyFunc;
		using Callback = typename PanelInputBase<RefT>::Callback;

		PanelResourceSelector(
			const std::string& p_name,
			const Ref& p_resourceRef,
			const Callback& p_callback = Callback());
		PanelResourceSelector(
			const std::string& p_name,
			const Ref& p_resourceRef,
			bool p_displayResource,
			const Callback& p_callback = Callback());
		~PanelResourceSelector() override = default;

		PanelResourceSelector(const PanelResourceSelector& p_other);
		PanelResourceSelector(PanelResourceSelector&& p_other) noexcept;
		PanelResourceSelector& operator=(const PanelResourceSelector& p_other);
		PanelResourceSelector& operator=(PanelResourceSelector&& p_other) noexcept;

		void DisplayAndUpdatePanel() override;

	private:
		virtual void FetchAllResources();

		std::string								m_name;
		bool									m_displayResource;

	protected:
		std::shared_ptr<PanelPopupMenuButton>	m_allResources;
	};
}

#include "SurvivantEditor/PanelItems/PanelResourceSelector.inl"