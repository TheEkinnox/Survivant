#include "SurvivantScripting/Bindings/LuaECSBinder.h"

#include "SurvivantCore/ECS/Components/TagComponent.h"

#include "SurvivantScripting/LuaContext.h"
#include "SurvivantScripting/LuaTypeRegistry.h"

#include <sol/state.hpp>

using namespace SvCore::ECS;
using namespace SvCore::Utility;

namespace SvScripting::Bindings
{
    bool LuaECSBinder::ComponentHandle::operator==(const ComponentHandle& p_other) const
    {
        return m_typeId == p_other.m_typeId && m_owner == p_other.m_owner;
    }

    LuaECSBinder::ComponentHandle::operator bool() const
    {
        return m_owner && m_owner.GetScene()->GetStorage(m_typeId).Contains(m_owner);
    }

    void LuaECSBinder::Bind(sol::state& p_luaState)
    {
        BindTag(p_luaState);
        BindScene(p_luaState);
        BindEntity(p_luaState);
        BindComponent(p_luaState);
        BindScriptHandle(p_luaState);
        BindComponentSearchOrigin(p_luaState);
    }

    void LuaECSBinder::BindTag(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Tag";

        sol::usertype tagType = p_luaState.new_usertype<TagComponent>(
            typeName,
            sol::meta_function::construct, sol::overload(
                []
                {
                    return TagComponent{};
                },
                [](const std::string& p_tag)
                {
                    return TagComponent{ p_tag };
                }
            ),
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const TagComponent& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self.m_tag;
                    return oss.str();
                },
                [](const TagComponent& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self.m_tag << p_str;
                    return oss.str();
                }
            ),
            "value", &TagComponent::m_tag
        );

        tagType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<TagComponent>(typeName);
        return (void)typeInfo;
    }

    void LuaECSBinder::BindScene(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Scene";

        sol::usertype sceneType = p_luaState.new_usertype<Scene>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "Create", sol::overload(
                sol::resolve<EntityHandle()>(&Scene::Create),
                sol::resolve<EntityHandle(Entity)>(&Scene::Create),
                [](Scene& p_self, const EntityHandle& p_hint)
                {
                    return p_self.Create(p_hint);
                }
            ),
            "Destroy", sol::overload(
                &Scene::Destroy,
                [](Scene& p_self, const EntityHandle& p_handle)
                {
                    return p_self.Destroy(p_handle);
                }
            ),
            "IsValid", sol::overload(
                &Scene::IsValid,
                [](const Scene& p_self, const EntityHandle& p_handle)
                {
                    return p_self.IsValid(p_handle);
                }
            ),
            "entities", sol::readonly_property(
                [](Scene& p_self) -> std::vector<EntityHandle>
                {
                    std::vector<EntityHandle> out;

                    const auto& storage = p_self.GetStorage<Entity>();
                    out.reserve(storage.size());

                    for (const auto& entity : storage)
                        out.emplace_back(&p_self, entity);

                    return out;
                }
            ),
            "Contains", sol::overload(
                &Scene::Contains,
                [](const Scene& p_self, const EntityHandle& p_handle)
                {
                    return p_self.Contains(p_handle);
                }
            )
        );

        sceneType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Scene>(typeName);
        return (void)typeInfo;
    }

    void LuaECSBinder::BindComponent(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Component";

        sol::usertype componentType = p_luaState.new_usertype<ComponentHandle>(
            typeName,
            "isValid", sol::readonly_property(&ComponentHandle::operator bool),
            sol::meta_function::index, [&p_luaState](const ComponentHandle& p_self, const sol::object& p_index) -> sol::object
            {
                if (!p_self.m_owner)
                    return sol::nil;

                void* component = p_self.m_owner.GetScene()->GetStorage(p_self.m_typeId).FindRaw(p_self.m_owner);
                return LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId).ToLua(component, p_luaState)[p_index];
            },
            sol::meta_function::new_index,
            [&p_luaState](const ComponentHandle& p_self, const sol::object& p_key, sol::object p_value)
            {
                if (!p_self.m_owner)
                    return;

                void* component = p_self.m_owner.GetScene()->GetStorage(p_self.m_typeId).FindRaw(p_self.m_owner);

                if (!component)
                    return;

                const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId);

                sol::userdata data = typeInfo.ToLua(component, p_luaState);

                data[p_key] = p_value;
                typeInfo.FromLua(component, data);
            },
            "Set", [](const ComponentHandle& p_self, const sol::userdata& p_value)
            {
                if (!p_self.m_owner)
                    return;

                void* component = p_self.m_owner.GetScene()->GetStorage(p_self.m_typeId).FindRaw(p_self.m_owner);

                if (!component)
                    return;

                const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId);
                typeInfo.FromLua(component, p_value);
            }
        );

        componentType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ComponentHandle>(typeName);
        return (void)typeInfo;
    }

    void LuaECSBinder::BindScriptHandle(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Script";

        sol::usertype componentType = p_luaState.new_usertype<LuaContext::ScriptHandle>(
            typeName,
            "isValid", sol::readonly_property(&LuaContext::ScriptHandle::operator bool),
            sol::meta_function::index, [](LuaContext::ScriptHandle& p_self, const sol::object& p_index) -> sol::object
            {
                if (!p_self)
                    return sol::nil;

                const sol::optional<sol::object> out = p_self.m_table[p_index];
                return out.value_or(sol::nil);
            },
            sol::meta_function::new_index, [](LuaContext::ScriptHandle& p_self, const sol::object& p_key, sol::object p_value)
            {
                if (p_self)
                    p_self.m_table[p_key] = p_value;
            }
        );

        componentType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<LuaContext::ScriptHandle>(typeName);
        return (void)typeInfo;
    }

    void LuaECSBinder::BindComponentSearchOrigin(sol::state& p_luaState)
    {
        using ESearchOrigin = EntityHandle::EComponentSearchOrigin;
        static constexpr const char* typeName = "EComponentSearchOrigin";

        p_luaState.new_enum(typeName,
            "ROOT", ESearchOrigin::ROOT,
            "PARENT", ESearchOrigin::PARENT,
            "CHILDREN", ESearchOrigin::CHILDREN
        );

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ESearchOrigin>(typeName);
        return (void)typeInfo;
    }
}
