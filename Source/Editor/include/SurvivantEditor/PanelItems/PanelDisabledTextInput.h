#pragma once
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <string>

namespace SvEditor::PanelItems
{
    class PanelDisabledTextInput : public Interfaces::IPanelable
    {
    public:
        explicit PanelDisabledTextInput(std::string p_name, std::string p_string);
        ~PanelDisabledTextInput() override = default;

        void DisplayAndUpdatePanel() override;

    private:
        std::string m_name;
        std::string m_string;
    };
}
