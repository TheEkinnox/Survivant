#pragma once
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <functional>
#include <memory>

namespace SvEditor::PanelItems
{
    class PanelConditionalDisplay : public Interfaces::IPanelable
    {
    public:
        using Display = std::unique_ptr<IPanelable>;
        using GetStateFunc = std::function<bool()>;

        PanelConditionalDisplay(Display&& p_displayOn, Display&& p_displayOff, const bool& p_state);

        PanelConditionalDisplay(Display&& p_displayOn, Display&& p_displayOff, GetStateFunc p_getState);

        bool GetState() const;

        void DisplayAndUpdatePanel() override;

    private:
        GetStateFunc m_getState;
        Display      m_displayOn;
        Display      m_displayOff;
    };
}
