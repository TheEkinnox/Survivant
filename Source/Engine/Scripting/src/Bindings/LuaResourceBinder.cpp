#include "SurvivantScripting/Bindings/LuaResourceBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantCore/Resources/ResourceRef.h>

#include <sol/state.hpp>

using namespace SvCore::Resources;

namespace SvScripting::Bindings
{
    void LuaResourceBinder::Bind(sol::state& p_luaState)
    {
        BindResourceRef(p_luaState);
        BindResourceManager(p_luaState);
    }

    void LuaResourceBinder::BindResourceRef(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Resource";

        sol::constructors<
            GenericResourceRef(),
            GenericResourceRef(const std::string&, const std::string&),
            GenericResourceRef(const GenericResourceRef&)
        > ctor;

        sol::usertype resourceType = p_luaState.new_usertype<GenericResourceRef>(
            typeName, ctor,
            sol::base_classes, sol::bases<ResourceRef<IResource>, ResourceRefBase>(),
            "isValid", sol::readonly_property(&GenericResourceRef::operator bool),
            "type", sol::readonly_property(&GenericResourceRef::GetType),
            "path", sol::readonly_property(&GenericResourceRef::GetPath),
            "fullPath", sol::readonly_property(&GenericResourceRef::GetFullPath),
            sol::meta_function::index, [&p_luaState](const GenericResourceRef& p_self, const sol::object& p_index) -> sol::object
            {
                const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.GetType());
                return typeInfo.ToLua(p_self.GetOrDefault(), p_luaState)[p_index];
            },
            sol::meta_function::new_index,
            [&p_luaState](const GenericResourceRef& p_self, const sol::object& p_key, sol::object p_value)
            {
                if (!p_self)
                    return;

                IResource* resource = p_self.Get();

                const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.GetType());

                sol::userdata data = typeInfo.ToLua(resource, p_luaState);

                data[p_key] = p_value;
                typeInfo.FromLua(resource, data);
            },
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const GenericResourceRef& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << "{ type: \"" << p_self.GetType() << "\", path: \"" << p_self.GetPath() << "\"}";
                    return oss.str();
                },
                [](const GenericResourceRef& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << "{ type: \"" << p_self.GetType() << "\", path: \"" << p_self.GetPath() << "\"}" << p_str;
                    return oss.str();
                }
            ),
            "self", sol::property(
                [&p_luaState](const GenericResourceRef& p_self) -> sol::userdata
                {
                    if (!p_self)
                        return make_object_userdata(p_luaState, sol::nil);

                    IResource* resource = p_self.Get();
                    return LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.GetType()).ToLua(resource, p_luaState);
                },
                [](const GenericResourceRef& p_self, const sol::userdata& p_value)
                {
                    if (!p_self)
                        return;

                    LuaTypeRegistry::GetInstance().GetTypeInfo(p_self.GetType()).FromLua(p_self.Get(), p_value);
                }
            )
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<GenericResourceRef>(typeName);
        return (void)typeInfo;
    }

    void LuaResourceBinder::BindResourceManager(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "ResourceManager";

        using Get = GenericResourceRef(ResourceManager::*)(const std::string&, const std::string&) const;
        using GetOrCreate = GenericResourceRef(ResourceManager::*)(const std::string&, const std::string&);

        sol::usertype logType = p_luaState.new_usertype<ResourceManager>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "Load", &ResourceManager::Create,
            "Get", static_cast<Get>(&ResourceManager::Get),
            "GetOrCreate", static_cast<GetOrCreate>(&ResourceManager::GetOrCreate),
            "Remove", &ResourceManager::Remove,
            "Clear", &ResourceManager::Clear,
            "searchPaths", sol::property(&ResourceManager::GetSearchPaths, &ResourceManager::SetSearchPaths),
            "AddSearchPath", &ResourceManager::AddSearchPath,
            "RemoveSearchPath", &ResourceManager::RemoveSearchPath,
            "GetFullPath", &ResourceManager::GetFullPath,
            "GetRelativePath", &ResourceManager::GetRelativePath
        );

        p_luaState[typeName] = &ResourceManager::GetInstance();
    }
}
