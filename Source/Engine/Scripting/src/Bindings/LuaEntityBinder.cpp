#include "SurvivantScripting/LuaScriptList.h"
#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaECSBinder.h"

#include <SurvivantCore/ECS/ComponentHandle.h>

using namespace SvCore::ECS;

namespace SvScripting::Bindings
{
    void LuaECSBinder::BindEntity(sol::state& p_luaState)
    {
        using SearchOrigin = EntityHandle::EComponentSearchOrigin;

        static constexpr const char* typeName = "Entity";

        static const auto hasComponent = [](const EntityHandle& p_self, const std::string& p_type)
        {
            if (!p_self || p_type.empty())
                return false;

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return false;

            return p_self.Has(typeRegistry.GetRegisteredTypeId(p_type));
        };

        static const auto getComponent = [](const EntityHandle& p_self, const std::string& p_type) -> ComponentHandle
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.Get(typeRegistry.GetRegisteredTypeId(p_type));
        };

        static const auto getOrCreate = [](const EntityHandle& p_self, const std::string& p_type) -> ComponentHandle
        {
            if (!p_self || p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetOrCreate(typeRegistry.GetRegisteredTypeId(p_type));
        };

        static const auto getInParent = [](const EntityHandle& p_self, const std::string& p_type) -> ComponentHandle
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetInParent(typeRegistry.GetRegisteredTypeId(p_type));
        };

        static const auto getInChildren = [](const EntityHandle& p_self, const std::string& p_type) -> ComponentHandle
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetInChildren(typeRegistry.GetRegisteredTypeId(p_type));
        };

        static const auto getInHierarchy
        {
            [](const EntityHandle& p_self, const std::string& p_type, const SearchOrigin p_searchOrigin)
            -> ComponentHandle
            {
                if (p_type.empty())
                    return {};

                const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

                if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                    return {};

                return p_self.GetInHierarchy(typeRegistry.GetRegisteredTypeId(p_type), p_searchOrigin);
            }
        };

        static const auto removeComponent = [](const EntityHandle& p_self, const std::string& p_type)
        {
            if (!p_self || p_type.empty())
                return;

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return;

            const auto typeId = typeRegistry.GetRegisteredTypeId(p_type);
            p_self.Remove(typeId);
        };

        sol::usertype handleType = p_luaState.new_usertype<EntityHandle>(
            typeName,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const EntityHandle& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const EntityHandle& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            "isValid", sol::readonly_property(&EntityHandle::operator bool),
            "scene", sol::readonly_property(&EntityHandle::GetScene),
            "Copy", &EntityHandle::Copy,
            "Destroy", &EntityHandle::Destroy,
            "root", sol::readonly_property(&EntityHandle::GetRoot),
            "parent", sol::property(&EntityHandle::GetParent, sol::resolve<void(EntityHandle)>(&EntityHandle::SetParent)),
            "nextSibling", sol::readonly_property(&EntityHandle::GetNextSibling),
            "previousSibling", sol::readonly_property(&EntityHandle::GetPreviousSibling),
            "childCount", sol::readonly_property(&EntityHandle::GetChildCount),
            "GetChild", &EntityHandle::GetChild,
            "AddChild", &EntityHandle::AddChild,
            "children", sol::readonly_property(&EntityHandle::GetChildren),
            "SetParent", sol::overload(
                sol::resolve<void(EntityHandle)>(&EntityHandle::SetParent),
                sol::resolve<void(EntityHandle, bool)>(&EntityHandle::SetParent)
            ),
            "HasScript", [](const EntityHandle& p_self, const std::string& p_name) -> bool
            {
                const LuaScriptList* script = p_self.Get<LuaScriptList>();
                return script ? script->Contains(p_name) : false;
            },
            "GetScript", [](const EntityHandle& p_self, const std::string& p_name) -> LuaScriptHandle
            {
                const LuaScriptList* script = p_self.Get<LuaScriptList>();
                return script ? script->Get(p_name) : LuaScriptHandle{};
            },
            "AddScript", [](EntityHandle& p_self, const std::string& p_name) -> LuaScriptHandle
            {
                LuaScriptList* script = p_self.Get<LuaScriptList>();

                std::unordered_map<std::string, sol::table> scripts;

                if (!script)
                    script = &p_self.Make<LuaScriptList>();

                return script->Add(p_name);
            },
            "RequireScript", [](EntityHandle& p_self, const std::string& p_name) -> LuaScriptHandle
            {
                LuaScriptList* scriptList = p_self.Get<LuaScriptList>();

                std::unordered_map<std::string, sol::table> scripts;

                if (!scriptList)
                    scriptList = &p_self.Make<LuaScriptList>();

                return scriptList->Contains(p_name) ? scriptList->Get(p_name) : scriptList->Add(p_name);
            },
            "RemoveScript", [](EntityHandle& p_self, const std::string& p_name)
            {
                if (LuaScriptList* script = p_self.Get<LuaScriptList>())
                    script->Remove(p_name);
            },
            "Has", sol::overload(
                hasComponent,
                [](const EntityHandle& p_self, const sol::table& p_type)
                {
                    if (!p_type.valid())
                        return false;

                    return hasComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "Get", sol::overload(
                getComponent,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return getComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetOrCreate", sol::overload(
                getOrCreate,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return getOrCreate(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInParent", sol::overload(
                getInParent,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return getInParent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInChildren", sol::overload(
                getInChildren,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return getInChildren(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInHierarchy", sol::overload(
                getInHierarchy,
                [](const EntityHandle& p_self, const sol::table& p_type, const SearchOrigin p_searchOrigin)
                -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return getInHierarchy(p_self, p_type["__type"]["name"].get_or<std::string>({}), p_searchOrigin);
                }
            ),
            "Remove", sol::overload(
                removeComponent,
                [](const EntityHandle& p_self, const sol::table& p_type)
                {
                    if (!p_type.valid())
                        return;

                    removeComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                },
                [](const EntityHandle& p_self, ComponentHandle& p_component)
                {
                    if (!CHECK(p_component.m_owner == p_self, "Attempted to remove component from non-owner entity"))
                        return;

                    p_component.Destroy();
                }
            ),
            "componentCount", sol::readonly_property(&EntityHandle::GetComponentCount),
            "components", sol::readonly_property(&EntityHandle::GetComponentHandles)
        );

        handleType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EntityHandle>(typeName);
        return (void)typeInfo;
    }
}
