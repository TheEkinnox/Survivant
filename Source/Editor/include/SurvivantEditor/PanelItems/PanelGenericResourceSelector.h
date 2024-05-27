//PanelGenericResourceSelector.h
#pragma once

#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/PanelItems/PanelUniqueSelection.h"

#include <SurvivantCore/Resources/ResourceRef.h>

namespace SvEditor::PanelItems
{
    class PanelGenericResourceSelector final
        : public PanelResourceSelector<SvCore::Resources::IResource, SvCore::Resources::GenericResourceRef>
    {
    public:
        PanelGenericResourceSelector(
            const std::string& p_name,
            const Ref&         p_resourceRef,
            const Callback&    p_callback = Callback());

        PanelGenericResourceSelector(
            std::string     p_name,
            const Ref&      p_resourceRef,
            bool            p_displayResource,
            const Callback& p_callback = Callback());

        ~PanelGenericResourceSelector() override = default;

        PanelGenericResourceSelector(const PanelGenericResourceSelector& p_other);
        PanelGenericResourceSelector(PanelGenericResourceSelector&& p_other) noexcept;

        PanelGenericResourceSelector& operator=(const PanelGenericResourceSelector& p_other)     = default;
        PanelGenericResourceSelector& operator=(PanelGenericResourceSelector&& p_other) noexcept = default;

        void DisplayAndUpdatePanel() override;

    private:
        void FetchResourceTypes();
        void FetchAllResources() override;
        void FetchTypedResources();

        std::string                           m_label;
        std::vector<std::string>              m_resourceTypes;
        std::shared_ptr<PanelUniqueSelection> m_resourceTypeSelection;

        int m_currentType;
    };
}
