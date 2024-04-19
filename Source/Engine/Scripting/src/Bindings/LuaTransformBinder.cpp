#include "SurvivantScripting/LuaContext.h"
#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include <Transform.h>

#include <sol/state.hpp>

using namespace LibMath;

namespace SvScripting::Bindings
{
    void LuaMathBinder::BindTransform(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Transform";

        sol::constructors<
            Transform(),
            Transform(Matrix4x4),
            Transform(const Vector3&, const Quaternion&, const Vector3&),
            Transform(const Transform&)
        > ctor;

        sol::usertype transformType = p_luaState.new_usertype<Transform>(
            typeName, ctor,
            sol::meta_function::to_string, [](const Transform& p_self)
            {
                std::ostringstream oss;
                oss << '{' << p_self.getPosition() << ',' << p_self.getRotation() << ',' << p_self.getScale() << '}';
                return oss.str();
            },
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const Transform& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << '{' << p_self.getPosition() << ',' << p_self.getRotation() << ',' << p_self.getScale() << '}';
                    return oss.str();
                },
                [](const Transform& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << '{' << p_self.getPosition() << ',' << p_self.getRotation() << ',' << p_self.getScale() << '}' << p_str;
                    return oss.str();
                }
            ),
            "hasParent", sol::readonly_property(&Transform::hasParent),
            "parent", sol::property(
                sol::resolve<Transform*()>(&Transform::getParent),
                [](Transform& p_self, Transform* p_parent)
                {
                    (void)p_self.setParent(p_parent);
                    return p_self;
                }
            ),
            "SetParent", sol::overload(
                &Transform::setParent,
                [](Transform& p_self, Transform* p_parent)
                {
                    return p_self.setParent(p_parent);
                }
            ),
            /* Local */
            "forward", sol::readonly_property(&Transform::forward),
            "back", sol::readonly_property(&Transform::back),
            "right", sol::readonly_property(&Transform::right),
            "left", sol::readonly_property(&Transform::left),
            "up", sol::readonly_property(&Transform::up),
            "down", sol::readonly_property(&Transform::down),
            "position", sol::property(&Transform::getPosition, &Transform::setPosition),
            "rotation", sol::property(&Transform::getRotation, &Transform::setRotation),
            "scale", sol::property(&Transform::getScale, &Transform::setScale),
            "matrix", sol::property(&Transform::getMatrix, &Transform::setMatrix),
            "inverse", sol::readonly_property(&Transform::inverse),
            "GetEuler", [](const Transform& p_self, const ERotationOrder p_order)
            -> std::tuple<Radian, Radian, Radian>
            {
                TVector3<Radian> out = p_self.getEuler(p_order);
                return { out.m_x, out.m_y, out.m_z };
            },
            "SetEuler", [](
            Transform& p_self, const Radian& p_x, const Radian& p_y, const Radian& p_z, const ERotationOrder p_rotationOrder)
            {
                return p_self.setEuler({ p_x, p_y, p_z }, p_rotationOrder);
            },
            "GetAll", [](const Transform& p_self)
            -> std::tuple<Vector3, Quaternion, Vector3>
            {
                return { p_self.getPosition(), p_self.getRotation(), p_self.getScale() };
            },
            "SetAll", &Transform::setAll,
            "Translate", &Transform::translate,
            "Rotate", sol::resolve<Transform&(const Quaternion&)>(&Transform::rotate),
            "Scale", &Transform::scale,
            "Invert", &Transform::invert,
            /* World */
            "worldForward", sol::readonly_property(&Transform::worldForward),
            "worldBack", sol::readonly_property(&Transform::worldBack),
            "worldRight", sol::readonly_property(&Transform::worldRight),
            "worldLeft", sol::readonly_property(&Transform::worldLeft),
            "worldUp", sol::readonly_property(&Transform::worldUp),
            "worldDown", sol::readonly_property(&Transform::worldDown),
            "worldPosition", sol::property(&Transform::getWorldPosition, &Transform::setWorldPosition),
            "worldRotation", sol::property(&Transform::getWorldRotation, &Transform::setWorldRotation),
            "worldScale", sol::property(&Transform::getWorldScale, &Transform::setWorldScale),
            "worldMatrix", sol::property(&Transform::getWorldMatrix, &Transform::setWorldMatrix),
            "worldInverse", sol::readonly_property(&Transform::inverseWorld),
            "GetWorldEuler", [](const Transform& p_self, const ERotationOrder p_order)
            -> std::tuple<Radian, Radian, Radian>
            {
                TVector3<Radian> out = p_self.getWorldEuler(p_order);
                return { out.m_x, out.m_y, out.m_z };
            },
            "SetWorldEuler",
            [](Transform& p_self, const Radian& p_x, const Radian& p_y, const Radian& p_z, const ERotationOrder p_order)
            {
                return p_self.setWorldEuler({ p_x, p_y, p_z }, p_order);
            },
            "GetAllWorld", [](const Transform& p_self)
            -> std::tuple<Vector3, Quaternion, Vector3>
            {
                return { p_self.getWorldPosition(), p_self.getWorldRotation(), p_self.getWorldScale() };
            },
            "SetAllWorld", &Transform::setAllWorld,
            "TranslateWorld", &Transform::worldTranslate,
            "RotateWorld", sol::resolve<Transform&(const Quaternion&)>(&Transform::worldRotate),
            "ScaleWorld", &Transform::worldScale,
            "InvertWorld", &Transform::invertWorld
        );

        {
            sol::table meta = p_luaState.get<sol::table>(typeName);
            meta.set("Interpolate", &Transform::interpolate);
            meta.set("InterpolateWorld", &Transform::interpolateWorld);
            meta.set("GenerateMatrix", &Transform::generateMatrix);
            meta.set("DecomposeMatrix", [](const Matrix4x4& p_matrix)
                -> std::tuple<Vector3, Quaternion, Vector3>
                {
                    Vector3    position, scale;
                    Quaternion rotation;

                    Transform::decomposeMatrix(p_matrix, position, rotation, scale);
                    return { position, rotation, scale };
                }
            );
        }

        transformType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<Transform>(typeName);
        return (void)typeInfo;
    }
}
