#include "SurvivantScripting/LuaTypeRegistry.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include <Matrix.h>

using namespace LibMath;

namespace SvScripting::Bindings
{
    template <length_t Rows, length_t Cols, typename DataT>
    void BindMatrixType(const std::string& p_typeName, sol::state& p_luaState)
    {
        using MatT = TMatrix<Rows, Cols, DataT>;

        sol::constructors<
            MatT(),
            MatT(DataT),
            MatT(const MatT&)
        > ctor;

        sol::usertype matType = p_luaState.new_usertype<MatT>(
            p_typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const MatT& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self;
                    return oss.str();
                },
                [](const MatT& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self << p_str;
                    return oss.str();
                }
            ),
            sol::meta_function::unary_minus, [](const MatT& p_self)
            {
                return -p_self;
            },
            sol::meta_function::multiplication, sol::overload(
                [](const MatT& p_self, const DataT p_scalar)
                {
                    return p_self * p_scalar;
                },
                [](const MatT& p_self, const MatT& p_other)
                {
                    return p_self * p_other;
                }
            ),
            sol::meta_function::division, sol::overload(
                [](const MatT& p_self, const DataT p_scalar)
                {
                    return p_self / p_scalar;
                },
                [](const MatT& p_self, const MatT& p_other)
                {
                    return p_self / p_other;
                }
            ),
            sol::meta_function::call, sol::resolve<DataT(length_t, length_t) const>(&MatT::operator()),
            "rowCount", sol::readonly_property(&MatT::getRowCount),
            "columnCount", sol::readonly_property(&MatT::getColumnCount),
            "size", sol::readonly_property(&MatT::getSize),
            "identity", sol::readonly_property([]
            {
                return MatT(DataT(1));
            }),
            "zero", sol::readonly_property([]
            {
                return MatT();
            }),
            "Get", sol::resolve<DataT(length_t, length_t) const>(&MatT::operator()),
            "Set", [](MatT& p_self, const length_t p_row, const length_t p_col, const DataT p_value)
            {
                p_self(p_row, p_col) = p_value;
            },
            "isIdentity", sol::readonly_property(&MatT::isIdentity),
            "determinant", sol::readonly_property(&MatT::determinant),
            "Cofactor", &MatT::cofactor,
            "Minor", &MatT::minor,
            "transposed", sol::readonly_property(&MatT::transposed),
            "coMatrix", sol::readonly_property(&MatT::coMatrix),
            "adjugate", sol::readonly_property(&MatT::adjugate),
            "inverse", sol::readonly_property(&MatT::inverse),
            "GetIndex", &MatT::getIndex
        );

        matType["__type"]["name"] = p_typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<MatT>(p_typeName);
        return (void)typeInfo;
    }

    void LuaMathBinder::BindMatrix(sol::state& p_luaState)
    {
        BindMatrixType<3, 3, float>("Matrix3", p_luaState);

        static constexpr const char* mat4Name = "Matrix4";
        BindMatrixType<4, 4, float>(mat4Name, p_luaState);

        {
            sol::table meta = p_luaState.get<sol::table>(mat4Name);
            meta.set_function("Translation", sol::overload(
                    sol::resolve<Matrix4(float, float, float)>(&translation),
                    sol::resolve<Matrix4(const Vector3&)>(&translation)
                )
            );

            meta.set_function("Scaling", sol::overload(
                    sol::resolve<Matrix4(float, float, float)>(&scaling),
                    sol::resolve<Matrix4(const Vector3&)>(&scaling)
                )
            );

            meta.set_function("Rotation", sol::overload(
                    sol::resolve<Matrix4(const Quaternion&)>(&rotation),
                    sol::resolve<Matrix4(const Radian&, const Vector3&)>(&rotation),
                    [](const Degree& p_angle, const Vector3& p_axis)
                    {
                        return rotation(p_angle, p_axis);
                    }
                )
            );

            meta.set_function("RotationFromTo", &LibMath::rotationFromTo<float>);
            meta.set_function("OrthographicProjection", &LibMath::orthographicProjection<float>);
            meta.set_function("PerspectiveProjection", &LibMath::perspectiveProjection<float>);
            meta.set_function("LookAt", &LibMath::lookAt<float>);
            meta.set_function("ToEuler",
                [](const Matrix4& p_mat, const ERotationOrder p_order)
                -> std::tuple<Radian, Radian, Radian>
                {
                    TVector3<Radian> angles = toEuler(p_mat, p_order);
                    return { angles.m_x, angles.m_y, angles.m_z };
                }
            );
        }
    }
}
