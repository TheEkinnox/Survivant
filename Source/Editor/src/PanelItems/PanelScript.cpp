#include "SurvivantEditor/PanelItems/PanelScript.h"

#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelCheckbox.h"
#include "SurvivantEditor/PanelItems/PanelDoubleInput.h"
#include "SurvivantEditor/PanelItems/PanelIntInput.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/PanelItems/PanelVec2Input.h"
#include "SurvivantEditor/PanelItems/PanelVec3Input.h"

#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/Serialization/MathSerializers.h>

#include <SurvivantScripting/LuaScriptList.h>
#include <SurvivantScripting/LuaTypeRegistry.h>

#include <imgui.h>

using namespace LibMath;

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvScripting;

namespace SvEditor::PanelItems
{
    namespace
    {
        bool AddLuaUserTypeToPanel(
            PanelScript::Items& p_items, sol::table p_table, const std::string& p_key, const std::string& p_displayName)
        {
            const std::string typeString = p_table[p_key]["__type"]["name"];

            if (typeString.empty())
                return false;

            const auto&  luaTypes = LuaTypeRegistry::GetInstance();
            const TypeId typeId   = luaTypes.GetRegisteredTypeId(typeString);

            if (typeId == 0)
                return false;

            if (typeId == LuaTypeRegistry::GetTypeId<Vector2>())
            {
                p_items.emplace_back(std::make_shared<PanelVec2Input>(p_displayName,
                    [p_table, p_key]() mutable -> Vector2& {
                        return *p_table[p_key].get<Vector2*>();
                    }
                ));

                return true;
            }

            if (typeId == LuaTypeRegistry::GetTypeId<Vector3>())
            {
                p_items.emplace_back(std::make_shared<PanelVec3Input>(p_displayName,
                    [p_table, p_key]() mutable -> Vector3& {
                        return *p_table[p_key].get<Vector3*>();
                    }
                ));

                return true;
            }

            if (typeId == LuaTypeRegistry::GetTypeId<Transform>())
            {
                p_items.emplace_back(std::make_shared<PanelTransformInput>(
                    [p_table, p_key]() mutable -> Transform& {
                        return *p_table[p_key].get<Transform*>();
                    }
                ));

                return true;
            }

            return false;
        }

        void AddLuaTableToPanel(PanelScript::Items& p_items, sol::table p_table, const std::string& p_prefix = {})
        {
            using namespace SvCore::Serialization;
            using namespace SvCore::Utility;

            for (auto [key, value] : p_table)
            {
                if (key.get_type() != sol::type::string)
                    continue;

                const std::string keyStr = key.as<const std::string&>();

                if (keyStr.empty() || keyStr[0] == '_' || LuaScriptList::s_ignoredFields.contains(keyStr))
                    continue;

                switch (value.get_type())
                {
                case sol::type::string:
                {
                    p_items.emplace_back(std::make_shared<PanelTextInput>(p_prefix + keyStr,
                        [p_table, keyStr]() mutable
                        {
                            return p_table[keyStr].get<std::string&>();
                        },
                        [p_table, keyStr](const std::tuple<PanelTextInput*>& p_textInput) mutable
                        {
                            const std::string& str = std::get<0>(p_textInput)->GetText();
                            p_table[keyStr]        = str;
                        }
                    ));

                    break;
                }
                case sol::type::number:
                {
                    if (value.is<int>())
                        p_items.emplace_back(std::make_shared<PanelIntInput>(p_prefix + keyStr,
                            [p_table, keyStr]() mutable
                            {
                                return p_table[keyStr].get<int>();
                            },
                            [p_table, keyStr](const int p_value) mutable
                            {
                                p_table[keyStr] = p_value;
                            }
                        ));
                    else
                        p_items.emplace_back(std::make_shared<PanelDoubleInput>(p_prefix + keyStr,
                            [p_table, keyStr]() mutable
                            {
                                return p_table[keyStr].get<double>();
                            },
                            [p_table, keyStr](const double p_value) mutable
                            {
                                p_table[keyStr] = p_value;
                            }
                        ));

                    break;
                }
                case sol::type::boolean:
                {
                    p_items.emplace_back(std::make_shared<PanelCheckbox>(p_prefix + keyStr,
                        [p_table, keyStr]() mutable
                        {
                            return p_table[keyStr].get<bool>();
                        },
                        [p_table, keyStr](const bool p_value) mutable
                        {
                            p_table[keyStr] = p_value;
                        }
                    ));

                    break;
                }
                case sol::type::table:
                {
                    AddLuaTableToPanel(p_items, value.as<sol::table>(), p_prefix + keyStr + ".");
                    break;
                }
                case sol::type::userdata:
                {
                    if (!AddLuaUserTypeToPanel(p_items, p_table, keyStr, p_prefix + keyStr))
                        AddLuaTableToPanel(p_items, value.as<sol::table>(), p_prefix + keyStr + ".");
                    break;
                }
                case sol::type::lightuserdata:
                case sol::type::none:
                case sol::type::lua_nil:
                case sol::type::thread:
                case sol::type::function:
                case sol::type::poly:
                default:
                    break;
                }
            }
        }
    }

    PanelScript::PanelScript(LuaScriptHandle p_script)
        : m_script(std::move(p_script)), m_isActive(true)
    {
        ComponentRegistry::EntitiesMap entitiesMap;
        AddLuaTableToPanel(m_items, m_script.m_table);
    }

    void PanelScript::DisplayAndUpdatePanel()
    {
        if (!m_script)
            return;

        static constexpr ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;

        const bool wasActive = m_isActive;

        if (m_items.empty())
            ImGui::CollapsingHeader(m_script.m_name.c_str(), &m_isActive, flags | ImGuiTreeNodeFlags_Leaf);
        else if (ImGui::CollapsingHeader(m_script.m_name.c_str(), &m_isActive, flags))
            for (const auto& item : m_items)
                item->DisplayAndUpdatePanel();

        if (wasActive && !m_isActive)
        {
            m_script.m_owner.Get<LuaScriptList>()->Remove(m_script.m_name);
            m_script = {};
        }
    }

    bool PanelScript::IsValid() const
    {
        return m_isActive && m_script;
    }

    LuaScript::OrderT PanelScript::GetExecutionOrder() const
    {
        return m_script ? m_script.m_script->GetExecutionOrder() : static_cast<LuaScript::OrderT>(0);
    }
}
