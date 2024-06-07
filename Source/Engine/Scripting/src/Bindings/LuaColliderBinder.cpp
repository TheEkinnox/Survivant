#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaPhysicsBinder.h"

#include <SurvivantPhysics/Collider/BoxCollider.h>
#include <SurvivantPhysics/Collider/CapsuleCollider.h>
#include <SurvivantPhysics/Collider/SphereCollider.h>

using namespace LibMath;
using namespace SvCore::Resources;
using namespace SvPhysics;

namespace SvScripting::Bindings
{
    void LuaPhysicsBinder::BindICollider(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Collider";

        sol::usertype resourceType = p_luaState.new_usertype<ICollider>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "isTrigger", &ICollider::m_isTrigger,
            "material", sol::property(
                [](const ICollider& p_self)
                {
                    return GenericResourceRef(p_self.m_material);
                },
                [](ICollider& p_self, const GenericResourceRef& p_material)
                {
                    p_self.m_material = p_material;
                }
            ),
            "offset", &ICollider::m_offset,
            "boundingBox", sol::readonly_property(&ICollider::GetBoundingBox),
            "boundingSphere", sol::readonly_property(&ICollider::GetBoundingSphere)
        );

        resourceType["__type"]["name"] = typeName;
    }

    void LuaPhysicsBinder::BindBoxCollider(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "BoxCollider";

        sol::usertype resourceType = p_luaState.new_usertype<BoxCollider>(
            typeName,
            sol::base_classes, sol::bases<ICollider>(),
            "size", &BoxCollider::m_size
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<BoxCollider>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindSphereCollider(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "SphereCollider";

        sol::usertype resourceType = p_luaState.new_usertype<SphereCollider>(
            typeName,
            sol::base_classes, sol::bases<ICollider>(),
            "radius", &SphereCollider::m_radius
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<SphereCollider>(typeName);
        return (void)typeInfo;
    }

    void LuaPhysicsBinder::BindCapsuleCollider(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "CapsuleCollider";

        sol::usertype resourceType = p_luaState.new_usertype<CapsuleCollider>(
            typeName,
            sol::base_classes, sol::bases<ICollider>(),
            "axis", &CapsuleCollider::m_axis,
            "height", &CapsuleCollider::m_height,
            "radius", &CapsuleCollider::m_radius
        );

        resourceType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<CapsuleCollider>(typeName);
        return (void)typeInfo;
    }
}
