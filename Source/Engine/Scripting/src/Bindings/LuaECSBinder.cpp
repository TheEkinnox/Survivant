#include "SurvivantScripting/Bindings/LuaECSBinder.h"

#include "SurvivantScripting/LuaScriptHandle.h"
#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantCore/ECS/ComponentHandle.h>
#include <SurvivantCore/ECS/Components/TagComponent.h>

#include <sol/state.hpp>

using namespace SvCore::ECS;
using namespace SvCore::Utility;

namespace SvScripting::Bindings
{
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

                void* component = p_self.Get();
                return LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId).ToLua(component, p_luaState)[p_index];
            },
            sol::meta_function::new_index,
            [&p_luaState](const ComponentHandle& p_self, const sol::object& p_key, sol::object p_value)
            {
                if (!p_self.m_owner)
                    return;

                void* component = p_self.Get();

                if (!component)
                    return;

                const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId);

                sol::userdata data = typeInfo.ToLua(component, p_luaState);

                data[p_key] = p_value;
                typeInfo.FromLua(component, data);
            },
            "owner", sol::readonly_property(
                [](const ComponentHandle& p_self)
                {
                    return p_self ? p_self.m_owner : EntityHandle{};
                }
            ),
            "self", sol::property(
                [&p_luaState](const ComponentHandle& p_self) -> sol::userdata
                {
                    if (!p_self.m_owner)
                        return make_object_userdata(p_luaState, sol::nil);

                    return LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId).ToLua(p_self.Get(), p_luaState);
                },
                [](const ComponentHandle& p_self, const sol::userdata& p_value)
                {
                    if (!p_self)
                        return;

                    LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.m_typeId).FromLua(p_self.Get(), p_value);
                }
            )
        );

        componentType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ComponentHandle>(typeName);
        return (void)typeInfo;
    }

    void LuaECSBinder::BindScriptHandle(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Script";

        sol::usertype componentType = p_luaState.new_usertype<LuaScriptHandle>(
            typeName,
            "isValid", sol::readonly_property(&LuaScriptHandle::operator bool),
            sol::meta_function::index, [](LuaScriptHandle& p_self, const sol::object& p_index) -> sol::object
            {
                if (!p_self)
                    return sol::nil;

                const sol::optional<sol::object> out = p_self.m_table[p_index];
                return out.value_or(sol::nil);
            },
            sol::meta_function::new_index, [](LuaScriptHandle& p_self, const sol::object& p_key, sol::object p_value)
            {
                if (p_self)
                    p_self.m_table[p_key] = p_value;
            },
            "owner", sol::readonly_property(
                [](const LuaScriptHandle& p_self)
                {
                    return p_self ? p_self.m_owner : EntityHandle{};
                }
            ),
            "table", sol::readonly_property(
                [](const LuaScriptHandle& p_self) -> sol::table
                {
                    if (!p_self)
                        return sol::nil;

                    return p_self.m_table;
                }
            )
        );

        componentType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<LuaScriptHandle>(typeName);
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
