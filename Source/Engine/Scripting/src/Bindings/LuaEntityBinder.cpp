#include "SurvivantScripting/LuaComponentHandle.h"
#include "SurvivantScripting/LuaScriptList.h"
#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaECSBinder.h"

#include <sol/state.hpp>

using namespace SvCore::ECS;

namespace SvScripting::Bindings
{
    void LuaECSBinder::BindEntity(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Entity";

        static const auto getComponent = [](const EntityHandle& p_self, const std::string& p_type) -> LuaComponentHandle
        {
            if (p_type.empty())
                return {};

            ComponentRegistry& components = ComponentRegistry::GetInstance();

            if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            return { p_self, components.GetTypeInfo(p_type).m_typeId };
        };

        static const auto getInParent = [](const EntityHandle& p_self, const std::string& p_type) -> LuaComponentHandle
        {
            if (!p_self || p_type.empty())
                return {};

            ComponentRegistry& components = ComponentRegistry::GetInstance();

            if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            const auto typeId = components.GetTypeInfo(p_type).m_typeId;

            LuaComponentHandle current{ p_self, typeId };

            if (current)
                return current;

            EntityHandle parent = p_self.GetParent();

            while (parent)
            {
                if ((current = { parent, typeId }))
                    return current;

                parent = parent.GetParent();
            }

            return {};
        };

        static const auto getInChildren = [](const EntityHandle& p_self, const std::string& p_type) -> LuaComponentHandle
        {
            if (!p_self || p_type.empty())
                return {};

            ComponentRegistry& components = ComponentRegistry::GetInstance();

            if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                return {};

            const auto typeId = components.GetTypeInfo(p_type).m_typeId;

            LuaComponentHandle current{ p_self, typeId };

            if (current)
                return current;

            const std::vector<EntityHandle> children = p_self.GetChildren();

            for (const EntityHandle& child : children)
            {
                if ((current = { child, typeId }))
                    return current;
            }

            return {};
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
            "parent", sol::property(&EntityHandle::GetParent, &EntityHandle::SetParent),
            "nextSibling", sol::readonly_property(&EntityHandle::GetNextSibling),
            "previousSibling", sol::readonly_property(&EntityHandle::GetPreviousSibling),
            "childCount", sol::readonly_property(&EntityHandle::GetChildCount),
            "GetChild", &EntityHandle::GetChild,
            "children", sol::readonly_property(&EntityHandle::GetChildren),
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
            "RemoveScript", [](EntityHandle& p_self, const std::string& p_name)
            {
                if (LuaScriptList* script = p_self.Get<LuaScriptList>())
                    script->Remove(p_name);
            },
            "Has", [](const EntityHandle& p_self, const std::string& p_type)
            {
                if (!p_self || p_type.empty())
                    return false;

                ComponentRegistry& components = ComponentRegistry::GetInstance();

                if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                    return false;

                const auto typeId = components.GetTypeInfo(p_type).m_typeId;
                return p_self.GetScene()->GetStorage(typeId).Contains(p_self.GetEntity());
            },
            "Get", getComponent,
            "GetOrCreate", [](const EntityHandle& p_self, const std::string& p_type) -> LuaComponentHandle
            {
                if (p_type.empty())
                    return {};

                ComponentRegistry& components = ComponentRegistry::GetInstance();

                if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                    return {};

                const ComponentTypeInfo& typeInfo = components.GetTypeInfo(p_type);

                if (p_self.GetScene()->GetStorage(typeInfo.m_typeId).GetOrCreateRaw(p_self))
                    return { p_self, typeInfo.m_typeId };

                return {};
            },
            "GetInParent", getInParent,
            "GetInChildren", getInChildren,
            "GetInHierarchy",
            [](const EntityHandle& p_self, const std::string& p_type, const EntityHandle::EComponentSearchOrigin p_searchOrigin)
            -> LuaComponentHandle
            {
                switch (p_searchOrigin)
                {
                case EntityHandle::EComponentSearchOrigin::ROOT:
                {
                    return getInChildren(p_self.GetRoot(), p_type);
                }
                case EntityHandle::EComponentSearchOrigin::PARENT:
                {
                    if (const LuaComponentHandle component = getInParent(p_self, p_type))
                        return component;

                    return getInChildren(p_self, p_type);
                }
                case EntityHandle::EComponentSearchOrigin::CHILDREN:
                {
                    if (const LuaComponentHandle component = getInChildren(p_self, p_type))
                        return component;

                    return getInParent(p_self, p_type);
                }
                default:
                    ASSERT(false, "Invalid component search origin");
                    return {};
                }
            },
            "Remove", [](const EntityHandle& p_self, const std::string& p_type)
            {
                if (!p_self || p_type.empty())
                    return;

                ComponentRegistry& components = ComponentRegistry::GetInstance();

                if (!CHECK(components.Contains(p_type), "Unkown component type \"%s\"", p_type.c_str()))
                    return;

                const auto typeId = components.GetTypeInfo(p_type).m_typeId;
                p_self.GetScene()->GetStorage(typeId).Remove(p_self);
            },
            "componentCount", sol::readonly_property(&EntityHandle::GetComponentCount),
            "components", sol::readonly_property([](EntityHandle& p_self) -> std::vector<LuaComponentHandle>
            {
                const auto ids = p_self.GetComponentIds();

                std::vector<LuaComponentHandle> components;

                for (auto id : ids)
                    components.emplace_back(p_self, id);

                return components;
            })
        );

        handleType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<EntityHandle>(typeName);
        return (void)typeInfo;
    }
}
