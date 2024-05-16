#include "SurvivantEditor/PanelItems/PanelScript.h"

#include "SurvivantEditor/PanelItems/PanelButton.h"
#include "SurvivantEditor/PanelItems/PanelCheckbox.h"
#include "SurvivantEditor/PanelItems/PanelDisabledTextInput.h"
#include "SurvivantEditor/PanelItems/PanelDoubleInput.h"
#include "SurvivantEditor/PanelItems/PanelFloatInput.h"
#include "SurvivantEditor/PanelItems/PanelIntInput.h"
#include "SurvivantEditor/PanelItems/PanelTextBox.h"
#include "SurvivantEditor/PanelItems/PanelTextInput.h"
#include "SurvivantEditor/PanelItems/PanelTransformInput.h"
#include "SurvivantEditor/PanelItems/PanelVec2Input.h"
#include "SurvivantEditor/PanelItems/PanelVec3Input.h"

#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/Serialization/MathSerializers.h>

#include <SurvivantScripting/LuaScriptList.h>
#include <SurvivantScripting/LuaTypeRegistry.h>

#include <imgui.h>
#include <memory>

using namespace LibMath;

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvScripting;

namespace SvEditor::PanelItems
{
    namespace
    {
        void AddLuaTableToPanel(PanelScript::Items& p_items, sol::table p_table, uint8_t p_depth = 0);

        std::string GetDisplayName(const sol::object& p_key, const uint8_t p_depth)
        {
            static constexpr size_t KEY_OFFSET = 2;

            std::string displayName;

            if (p_depth > 0)
                displayName.append(KEY_OFFSET * p_depth, ' ');

            if (p_key.get_type() == sol::type::string)
            {
                const std::string& keyStr = p_key.as<const std::string&>();
                return displayName.append(p_depth == 0 ? keyStr : "." + keyStr);
            }

            if (p_key.get_type() == sol::type::number)
            {
                displayName.append("[");

                if (p_key.is<int>())
                    displayName.append(std::to_string(p_key.as<int>()));
                else
                    displayName.append(std::to_string(p_key.as<double>()));

                return displayName.append("]");
            }

            return {};
        }

        void AddLuaUserTypeToPanel(
            PanelScript::Items& p_items, sol::table p_table, const sol::object& p_key, const std::string& p_displayName)
        {
            std::string typeString = p_table[p_key]["__type"]["name"];

            if (typeString.empty())
                return;

            const auto&  luaTypes = LuaTypeRegistry::GetInstance();
            const TypeId typeId   = luaTypes.GetRegisteredTypeId(typeString);

            if (typeId == 0)
                return;

            if (typeId == LuaTypeRegistry::GetTypeId<Vector2>())
            {
                p_items.emplace_back(std::make_shared<PanelVec2Input>(p_displayName,
                    [p_table, p_key]() mutable -> Vector2& {
                        return *p_table[p_key].get<Vector2*>();
                    }
                ));
            }
            else if (typeId == LuaTypeRegistry::GetTypeId<Vector3>())
            {
                p_items.emplace_back(std::make_shared<PanelVec3Input>(p_displayName,
                    [p_table, p_key]() mutable -> Vector3& {
                        return *p_table[p_key].get<Vector3*>();
                    }
                ));
            }
            else if (typeId == LuaTypeRegistry::GetTypeId<Transform>())
            {
                p_items.emplace_back(std::make_shared<DefaultText>(p_displayName));
                p_items.emplace_back(std::make_shared<PanelTransformInput>(
                    [p_table, p_key]() mutable -> Transform& {
                        return *p_table[p_key].get<Transform*>();
                    }
                ));
            }
            else if (typeId == LuaTypeRegistry::GetTypeId<Degree>())
            {
                p_items.emplace_back(std::make_shared<PanelFloatInput>(p_displayName,
                    [p_table, p_key]() mutable
                    {
                        return p_table[p_key].get<Degree>().raw();
                    },
                    [p_table, p_key](const float p_value) mutable
                    {
                        p_table[p_key] = Degree(p_value);
                    }
                ));
            }
            else if (typeId == LuaTypeRegistry::GetTypeId<Radian>())
            {
                p_items.emplace_back(std::make_shared<PanelFloatInput>(p_displayName,
                    [p_table, p_key]() mutable
                    {
                        return p_table[p_key].get<Radian>().degree(true);
                    },
                    [p_table, p_key](const float p_value) mutable
                    {
                        p_table[p_key] = Radian(Degree(p_value));
                    }
                ));
            }
            else if (typeId == LuaTypeRegistry::GetTypeId<Quaternion>())
            {
                p_items.emplace_back(std::make_shared<PanelVec3Input>(p_displayName,
                    [p_table, p_key]() mutable -> Vector3
                    {
                        const TVector3<Degree> degrees(p_table[p_key].get<Quaternion>().toEuler(ERotationOrder::YXZ));
                        return { degrees.m_x.raw(), degrees.m_y.raw(), degrees.m_z.raw() };
                    },
                    [p_table, p_key](const Vector3& p_value) mutable
                    {
                        p_table[p_key] = Quaternion::fromEuler(TVector3<Degree>(p_value), ERotationOrder::YXZ);
                    }
                ));
            }
            else
            {
                // TODO: Add other known lua user types to script inspector panel
                p_items.emplace_back(std::make_shared<PanelDisabledTextInput>(p_displayName, typeString));
            }
        }

        void AddLuaTableToPanel(PanelScript::Items& p_items, sol::table p_table, const uint8_t p_depth)
        {
            using namespace SvCore::Serialization;
            using namespace SvCore::Utility;

            for (auto [key, value] : p_table)
            {
                std::string displayName = GetDisplayName(key, p_depth);

                if (displayName.empty() || displayName[0] == '_' || LuaScriptList::s_ignoredFields.contains(displayName))
                    continue;

                switch (value.get_type())
                {
                case sol::type::none:
                case sol::type::lua_nil:
                {
                    p_items.emplace_back(std::make_shared<PanelDisabledTextInput>(displayName,
                        sol::type_name(nullptr, value.get_type())));
                    break;
                }
                case sol::type::string:
                {
                    p_items.emplace_back(std::make_shared<PanelTextInput>(displayName,
                        [p_table, key]
                        {
                            return p_table[key].get<std::string>();
                        },
                        [p_table, key](const std::tuple<PanelTextInput*>& p_textInput) mutable
                        {
                            const std::string& str = std::get<0>(p_textInput)->GetText();
                            p_table[key]           = str;
                        }
                    ));

                    break;
                }
                case sol::type::number:
                {
                    if (value.is<int>())
                        p_items.emplace_back(std::make_shared<PanelIntInput>(displayName,
                            [p_table, key]
                            {
                                return p_table[key].get<int>();
                            },
                            [p_table, key](const int p_value) mutable
                            {
                                p_table[key] = p_value;
                            }
                        ));
                    else
                        p_items.emplace_back(std::make_shared<PanelDoubleInput>(displayName,
                            [p_table, key]
                            {
                                return p_table[key].get<double>();
                            },
                            [p_table, key](const double p_value) mutable
                            {
                                p_table[key] = p_value;
                            }
                        ));

                    break;
                }
                case sol::type::boolean:
                {
                    p_items.emplace_back(std::make_shared<PanelCheckbox>(displayName,
                        [p_table, key]
                        {
                            return p_table[key].get<bool>();
                        },
                        [p_table, key](const bool p_value) mutable
                        {
                            p_table[key] = p_value;
                        }
                    ));

                    break;
                }
                case sol::type::userdata:
                {
                    AddLuaUserTypeToPanel(p_items, p_table, key, displayName);
                    break;
                }
                case sol::type::table:
                {
                    ASSERT(~p_depth != 0, "Max lua table depth (%u) reached - Unable to add sub table \"%s\"",
                        static_cast<uint8_t>(~0), displayName.c_str());

                    p_items.emplace_back(std::make_shared<DefaultText>(displayName));
                    AddLuaTableToPanel(p_items, value.as<sol::table>(), p_depth + 1);
                    break;
                }
                case sol::type::lightuserdata:
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
