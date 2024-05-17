//PanelButtonList.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"
#include "SurvivantEditor/PanelItems/PanelButton.h"

namespace SvEditor::PanelItems
{
    class PanelButtonList : Interfaces::IPanelable
    {
    public:
        PanelButtonList()           = default;
        ~PanelButtonList() override = default;

        void DisplayAndUpdatePanel() override;

        std::vector<PanelButton> m_buttons;
    };
}
