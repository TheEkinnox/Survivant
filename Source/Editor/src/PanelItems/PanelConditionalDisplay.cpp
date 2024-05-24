#include "SurvivantEditor/PanelItems/PanelConditionalDisplay.h"

namespace SvEditor::PanelItems
{
    PanelConditionalDisplay::PanelConditionalDisplay(Display&& p_displayOn, Display&& p_displayOff, const bool& p_state)
        : PanelConditionalDisplay(std::move(p_displayOn), std::move(p_displayOff),
            GetStateFunc([p_state]() -> const bool& {
                return p_state;
            }))
    {
    }

    PanelConditionalDisplay::PanelConditionalDisplay(Display&& p_displayOn, Display&& p_displayOff, GetStateFunc p_getState)
        : m_getState(std::move(p_getState)), m_displayOn(std::move(p_displayOn)), m_displayOff(std::move(p_displayOff))
    {
    }

    bool PanelConditionalDisplay::GetState() const
    {
        return m_getState();
    }

    void PanelConditionalDisplay::DisplayAndUpdatePanel()
    {
        if (GetState())
            m_displayOn->DisplayAndUpdatePanel();
        else
            m_displayOff->DisplayAndUpdatePanel();
    }
}
