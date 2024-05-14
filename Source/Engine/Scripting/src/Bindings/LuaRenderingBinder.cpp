#include "SurvivantScripting/Bindings/LuaRenderingBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantRendering/Components/ModelComponent.h>

#include <sol/state.hpp>

using namespace SvCore::Resources;
using namespace SvRendering::Components;
using namespace SvRendering::Resources;

namespace SvScripting::Bindings
{
    void LuaRenderingBinder::Bind(sol::state& p_luaState)
    {
        BindModelComponent(p_luaState);
    }

    void LuaRenderingBinder::BindModelComponent(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Model";

        sol::usertype resourceType = p_luaState.new_usertype<ModelComponent>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "model", sol::property(
                [](const ModelComponent& p_self)
                {
                    return GenericResourceRef(p_self.m_model);
                },
                [](ModelComponent& p_self, const GenericResourceRef& p_ref)
                {
                    p_self.m_model = p_ref;
                }
            ),
            "material", sol::property(
                [](const ModelComponent& p_self)
                {
                    return GenericResourceRef(p_self.m_material);
                },
                [](ModelComponent& p_self, const GenericResourceRef& p_ref)
                {
                    p_self.m_material = p_ref;
                }
            ),
            "layerMask", &ModelComponent::m_layerMask
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<ModelComponent>(typeName);
        return (void)typeInfo;
    }
}
