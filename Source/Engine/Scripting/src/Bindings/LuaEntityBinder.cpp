#include "SurvivantScripting/LuaScriptList.h"
#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaECSBinder.h"

#include <SurvivantCore/ECS/ComponentHandle.h>

using namespace SvCore::ECS;

namespace SvScripting::Bindings
{
    namespace
    {
        using SearchOrigin = EntityHandle::EComponentSearchOrigin;

        bool HasComponent(const EntityHandle& p_self, const std::string& p_type)
        {
            if (!p_self || p_type.empty())
                return false;

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return false;

            return p_self.Has(typeRegistry.GetRegisteredTypeId(p_type));
        }

        ComponentHandle GetComponent(const EntityHandle& p_self, const std::string& p_type)
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.Get(typeRegistry.GetRegisteredTypeId(p_type));
        }

        ComponentHandle GetOrCreate(const EntityHandle& p_self, const std::string& p_type)
        {
            if (!p_self || p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetOrCreate(typeRegistry.GetRegisteredTypeId(p_type));
        }

        ComponentHandle GetInParent(const EntityHandle& p_self, const std::string& p_type)
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetInParent(typeRegistry.GetRegisteredTypeId(p_type));
        }

        ComponentHandle GetInChildren(const EntityHandle& p_self, const std::string& p_type)
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetInChildren(typeRegistry.GetRegisteredTypeId(p_type));
        }

        ComponentHandle GetInHierarchy(const EntityHandle& p_self, const std::string& p_type, const SearchOrigin p_searchOrigin)
        {
            if (p_type.empty())
                return {};

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return p_self.GetInHierarchy(typeRegistry.GetRegisteredTypeId(p_type), p_searchOrigin);
        }

        void RemoveComponent(const EntityHandle& p_self, const std::string& p_type)
        {
            if (!p_self || p_type.empty())
                return;

            const LuaTypeRegistry& typeRegistry = LuaTypeRegistry::GetInstance();

            if (!CHECK(typeRegistry.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return;

            const auto typeId = typeRegistry.GetRegisteredTypeId(p_type);
            p_self.Remove(typeId);
        }

        LuaScriptHandle GetScript(const EntityHandle& p_self, const std::string& p_name)
        {
            const LuaScriptList* script = p_self.Get<LuaScriptList>();
            return script ? script->Get(p_name) : LuaScriptHandle{};
        }

        LuaScriptHandle GetScriptInParent(const EntityHandle& p_self, const std::string& p_name)
        {
            if (LuaScriptHandle script = GetScript(p_self, p_name))
                return script;

            EntityHandle parent = p_self.GetParent();

            while (parent)
            {
                if (LuaScriptHandle script = GetScript(parent, p_name))
                    return script;

                parent = parent.GetParent();
            }

            return {};
        }

        LuaScriptHandle GetScriptInChildren(const EntityHandle& p_self, const std::string& p_name)
        {
            if (const LuaScriptHandle script = GetScript(p_self, p_name))
                return script;

            for (const EntityHandle& child : p_self)
            {
                if (const LuaScriptHandle script = GetScriptInChildren(child, p_name))
                    return script;
            }

            return {};
        }

        LuaScriptHandle GetScriptInHierarchy(
            const EntityHandle& p_self, const std::string& p_name, const SearchOrigin p_searchOrigin)
        {
            switch (p_searchOrigin)
            {
            case SearchOrigin::ROOT:
            {
                return GetScriptInChildren(p_self.GetRoot(), p_name);
            }
            case SearchOrigin::PARENT:
            {
                if (const LuaScriptHandle script = GetScriptInParent(p_self, p_name))
                    return script;

                return GetScriptInChildren(p_self, p_name);
            }
            case SearchOrigin::CHILDREN:
            {
                if (const LuaScriptHandle script = GetScriptInChildren(p_self, p_name))
                    return script;

                return GetScriptInParent(p_self, p_name);
            }
            default:
                ASSERT(false, "Invalid component search origin");
                return {};
            }
        }
    }

    void LuaECSBinder::BindEntity(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Entity";

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
            "GetScript", &GetScript,
            "GetScriptInParent", &GetScriptInParent,
            "GetScriptInChildren", &GetScriptInChildren,
            "GetScriptInHierarchy", &GetScriptInHierarchy,
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
                &HasComponent,
                [](const EntityHandle& p_self, const sol::table& p_type)
                {
                    if (!p_type.valid())
                        return false;

                    return HasComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "Get", sol::overload(
                &GetComponent,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return GetComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetOrCreate", sol::overload(
                &GetOrCreate,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return GetOrCreate(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInParent", sol::overload(
                &GetInParent,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return GetInParent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInChildren", sol::overload(
                &GetInChildren,
                [](const EntityHandle& p_self, const sol::table& p_type) -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return GetInChildren(p_self, p_type["__type"]["name"].get_or<std::string>({}));
                }
            ),
            "GetInHierarchy", sol::overload(
                &GetInHierarchy,
                [](const EntityHandle& p_self, const sol::table& p_type, const SearchOrigin p_searchOrigin)
                -> ComponentHandle
                {
                    if (!p_type.valid())
                        return {};

                    return GetInHierarchy(p_self, p_type["__type"]["name"].get_or<std::string>({}), p_searchOrigin);
                }
            ),
            "Remove", sol::overload(
                &RemoveComponent,
                [](const EntityHandle& p_self, const sol::table& p_type)
                {
                    if (!p_type.valid())
                        return;

                    RemoveComponent(p_self, p_type["__type"]["name"].get_or<std::string>({}));
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
