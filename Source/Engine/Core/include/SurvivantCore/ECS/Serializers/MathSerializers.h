#pragma once
#include "SurvivantCore/ECS/ComponentRegistry.h"

#include <Transform.h>
#include <Vector.h>

namespace SvCore::ECS
{
    /* Transform */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Transform&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Transform&, const rapidjson::Value&, Scene*);

    /* Degree */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Degree&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Degree&, const rapidjson::Value&, Scene*);

    /* Radian */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Radian&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Radian&, const rapidjson::Value&, Scene*);

    /* ERotationOrder */
    template <>
    bool ComponentRegistry::ToJson(
        const LibMath::ERotationOrder& p_component, rapidjson::Writer<rapidjson::StringBuffer>& p_writer, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::ERotationOrder& p_out, const rapidjson::Value& p_json, Scene*);

    /* Quaternion */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Quaternion&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Quaternion&, const rapidjson::Value&, Scene*);

    /* Vector2 */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Vector2&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Vector2&, const rapidjson::Value&, Scene*);

    /* Vector3 */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Vector3&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Vector3&, const rapidjson::Value&, Scene*);

    /* Vector4 */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Vector4&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Vector4&, const rapidjson::Value&, Scene*);

    /* Matrix3 */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Matrix3&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Matrix3&, const rapidjson::Value&, Scene*);

    /* Matrix4 */
    template <>
    bool ComponentRegistry::ToJson(const LibMath::Matrix4&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(LibMath::Matrix4&, const rapidjson::Value&, Scene*);
}
