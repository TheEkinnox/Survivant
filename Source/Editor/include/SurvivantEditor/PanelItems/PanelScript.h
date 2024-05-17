#pragma once
#include "SurvivantEditor/Interfaces/IPanelable.h"

#include <SurvivantScripting/LuaScriptHandle.h>

#include <memory>

namespace SvEditor::PanelItems
{
    class PanelScript final : public Interfaces::IPanelable
    {
    public:
        using Items = std::vector<std::shared_ptr<IPanelable>>;

        PanelScript(SvScripting::LuaScriptHandle p_script);
        ~PanelScript() override = default;

        void DisplayAndUpdatePanel() override;
        bool IsValid() const;

        SvScripting::LuaScript::OrderT GetExecutionOrder() const;

    private:
        SvScripting::LuaScriptHandle m_script;
        Items                        m_items;
        bool                         m_isActive;
    };
}
