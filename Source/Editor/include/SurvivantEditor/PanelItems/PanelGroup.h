#pragma once
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <Vector/Vector2.h>

#include <memory>
#include <vector>

namespace SvEditor::PanelItems
{
    class PanelGroup : public Interfaces::IPanelable
    {
    public:
        using Items = std::vector<std::shared_ptr<IPanelable>>;

        struct GroupSettings
        {
            std::string      m_label;
            LibMath::Vector2 m_offset = LibMath::Vector2::zero();

            bool m_isCollapsible = true;
            bool m_isCollapsed   = false;
        };

        PanelGroup(Items p_items, GroupSettings p_settings);

        void DisplayAndUpdatePanel() override;

    private:
        Items     m_items;
        GroupSettings m_settings;
        bool          m_defaultOpen;
    };
}
