#pragma once
#include "SurvivantEditor/PanelItems/PanelInputBase.h"

namespace SvEditor::PanelItems
{
    class PanelSeparator final : public Interfaces::IPanelable
    {
    public:
        PanelSeparator() = default;
        ~PanelSeparator() override = default;

        void DisplayAndUpdatePanel() override;
    };
}
