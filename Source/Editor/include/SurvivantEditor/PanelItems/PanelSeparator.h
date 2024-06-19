#pragma once
#include "SurvivantEditor/PanelItems/PanelInputBase.h"

#include <string>

namespace SvEditor::PanelItems
{
    class PanelSeparator final : public Interfaces::IPanelable
    {
    public:
        PanelSeparator(std::string p_text = {});
        ~PanelSeparator() override = default;

        void DisplayAndUpdatePanel() override;

    private:
        std::string m_text;
    };
}
