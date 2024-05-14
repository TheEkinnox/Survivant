#pragma once
#include "SurvivantCore/Serialization/Serializer.h"

#include <Transform.h>
#include <Vector.h>

#include <Geometry/BoundingBox.h>
#include <Geometry/BoundingSphere.h>

namespace SvCore::Serialization
{
    /* Transform */
    template <>
    bool ToJson(const LibMath::Transform&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Transform&, const JsonValue&);

    /* Degree */
    template <>
    bool ToJson(const LibMath::Degree&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Degree&, const JsonValue&);

    /* Radian */
    template <>
    bool ToJson(const LibMath::Radian&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Radian&, const JsonValue&);

    /* Quaternion */
    template <>
    bool ToJson(const LibMath::Quaternion&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Quaternion&, const JsonValue&);

    /* Vector2 */
    template <>
    bool ToJson(const LibMath::Vector2&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Vector2&, const JsonValue&);

    /* Vector3 */
    template <>
    bool ToJson(const LibMath::Vector3&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Vector3&, const JsonValue&);

    /* Vector4 */
    template <>
    bool ToJson(const LibMath::Vector4&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Vector4&, const JsonValue&);

    /* Matrix3 */
    template <>
    bool ToJson(const LibMath::Matrix3&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Matrix3&, const JsonValue&);

    /* Matrix4 */
    template <>
    bool ToJson(const LibMath::Matrix4&, JsonWriter&);

    template <>
    bool FromJson(LibMath::Matrix4&, const JsonValue&);

    /* Bounding Box */
    template <>
    bool ToJson(const LibMath::BoundingBox&, JsonWriter&);

    template <>
    bool FromJson(LibMath::BoundingBox&, const JsonValue&);

    /* Bounding Sphere */
    template <>
    bool ToJson(const LibMath::BoundingSphere&, JsonWriter&);

    template <>
    bool FromJson(LibMath::BoundingSphere&, const JsonValue&);
}
