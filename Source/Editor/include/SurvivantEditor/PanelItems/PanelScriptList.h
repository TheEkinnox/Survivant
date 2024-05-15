#pragma once
#include "SurvivantEditor/PanelItems/PanelComponent.h"
#include "SurvivantEditor/PanelItems/PanelResourceSelector.h"
#include "SurvivantEditor/PanelItems/PanelScript.h"

#include <SurvivantCore/ECS/EntityHandle.h>

namespace SvEditor::PanelItems
{
    class PanelScriptList final : public PanelComponent
    {
    public:
        PanelScriptList(const std::string& p_name, const SvCore::ECS::EntityHandle& p_entity, size_t p_priority = 0);
        ~PanelScriptList() override = default;

        void DisplayAndUpdatePanel() override;

    private:
        static constexpr int PADDING = 12;

        void Init();
        void AddScript(const SvScripting::LuaScriptHandle& p_script);

        SvCore::ECS::EntityHandle m_entity;

        std::unique_ptr<PanelResourceSelector<SvScripting::LuaScript>> m_newScriptSelector;
    };
}
