//PanelButton.h
#pragma once

#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <SurvivantCore/Events/Event.h>

#include <functional>
#include <string>

namespace SvEditor::PanelItems
{
    class PanelButton : public Interfaces::IPanelable
    {
    public:
        class OnButtonPressEvent : public SvCore::Events::Event<> {};

        PanelButton() : PanelButton("")
        {
        }

        PanelButton(std::string p_name, const OnButtonPressEvent::EventDelegate& p_callback = nullptr)
            : m_name(std::move(p_name))
        {
            if (p_callback != nullptr)
                m_callback.AddListener(p_callback);
        }

        PanelButton(std::string p_name, OnButtonPressEvent p_event)
            : m_name(std::move(p_name)), m_callback(std::move(p_event))
        {
        }

        ~PanelButton() override = default;

        void DisplayAndUpdatePanel() override;

        void AddCallback(const OnButtonPressEvent::EventDelegate& p_callback)
        {
            m_callback.AddListener(p_callback);
        }

    private:
        std::string        m_name;
        OnButtonPressEvent m_callback;
    };
}
