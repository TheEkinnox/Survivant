#include "SurvivantEditor/PanelItems/PanelScriptList.h"

#include <SurvivantScripting/LuaScriptList.h>

#include <imgui.h>

using namespace LibMath;

using namespace SvCore::ECS;
using namespace SvCore::Resources;

using namespace SvScripting;

namespace SvEditor::PanelItems
{
    namespace
    {
        bool CompareExecOrder(const PanelScript::Items::value_type& p_a, const PanelScript::Items::value_type& p_b)
        {
            const auto a = std::dynamic_pointer_cast<PanelScript>(p_a);
            const auto b = std::dynamic_pointer_cast<PanelScript>(p_b);

            return a->GetExecutionOrder() < b->GetExecutionOrder();
        }
    }

    PanelScriptList::PanelScriptList(const EntityHandle& p_entity, const size_t p_priority)
        : PanelComponent(ComponentRegistry::GetInstance().GetRegisteredTypeName<LuaScriptList>(), {}, p_priority),
        m_entity(p_entity)
    {
        m_newScriptSelector = std::make_unique<PanelResourceSelector<LuaScript>>(
            "Add Script ",
            []() -> ResourceRef<LuaScript>& {
                static ResourceRef<LuaScript> empty;
                empty = {};
                return empty;
            },
            false,
            [this](const ResourceRef<LuaScript>& p_script)
            {
                LuaScriptList* scriptList = m_entity.Get<LuaScriptList>();

                if (!scriptList)
                    scriptList = &m_entity.Make<LuaScriptList>();

                const std::string& script = p_script.GetPath();

                if (!scriptList->Contains(script))
                    AddScript(scriptList->Add(script));
                else
                    SV_LOG_WARNING("Entity %s already contains script \"%s\"", m_entity.GetDisplayName().c_str(), script.c_str());
            }
        );

        Init();
    }

    void PanelScriptList::DisplayAndUpdatePanel()
    {
        if (!m_entity)
            return;

        DisplayList();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        DisplayAddScript();
    }

    void PanelScriptList::DisplayList()
    {
        if (!m_entity || m_items.empty())
            return;

        ImGui::PushID(m_name.c_str());
        ImGui::SeparatorText("Scripts");

        size_t currentIndex = 0;
        for (auto it = m_items.begin(); it != m_items.end(); ++currentIndex)
        {
            const auto script = std::dynamic_pointer_cast<PanelScript>(*it);

            if (!script->IsValid())
            {
                it = m_items.erase(it);
                continue;
            }

            if (it != m_items.begin())
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            script->DisplayAndUpdatePanel();

            if (script->IsValid())
                ++it;
            else
                it = m_items.erase(it);
        }

        ImGui::PopID();
    }

    void PanelScriptList::DisplayAddScript() const
    {
        if (!m_entity)
            return;

        m_newScriptSelector->DisplayAndUpdatePanel();
    }

    void PanelScriptList::Init()
    {
        const LuaScriptList* scripts = m_entity.Get<LuaScriptList>();

        if (!scripts)
            return;

        m_items.reserve(scripts->size());

        for (const auto& name : *scripts)
            AddScript(scripts->Get(name));
    }

    void PanelScriptList::AddScript(const LuaScriptHandle& p_script)
    {
        if (!p_script)
            return;

        const auto first = m_items.begin();
        const auto last  = m_items.end();

        const auto script    = std::make_shared<PanelScript>(p_script);
        const auto execOrder = script->GetExecutionOrder();

        if (first == last || execOrder == 0)
            m_items.push_back(script);
        else if (std::dynamic_pointer_cast<PanelScript>(*first)->GetExecutionOrder() < execOrder)
            m_items.insert(first, script);
        else
            m_items.insert(std::ranges::upper_bound(m_items, script, &CompareExecOrder), script);
    }
}
