#include "SurvivantPhysics/Collider/ICollider.h"

#include "SurvivantPhysics/PhysicsContext.h"

#include <PxShape.h>

using namespace SvCore::Serialization;

namespace SvPhysics
{
    void ICollider::Refresh(const LibMath::Transform*)
    {
        using namespace physx;

        const PxShapeFlags flags = m_pxShape->getFlags() & ~(PxShapeFlag::eTRIGGER_SHAPE | PxShapeFlag::eSIMULATION_SHAPE);
        m_pxShape->setFlags(flags | (m_isTrigger ? PxShapeFlag::eTRIGGER_SHAPE : PxShapeFlag::eSIMULATION_SHAPE));

        PxMaterial* mat = &SV_SERVICE(PhysicsContext).GetMaterial(m_material.GetOrDefault());
        m_pxShape->setMaterials(&mat, 1);
    }

    bool ICollider::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("material");
        if (!m_material.ToJson(p_writer))
            return false;

        p_writer.Key("offset");
        if (!::ToJson(m_offset, p_writer))
            return false;

        p_writer.Key("is_trigger");
        if (!CHECK(p_writer.Bool(m_isTrigger), "Failed to write collider trigger flag"))
            return false;

        return ToJsonImpl(p_writer) && CHECK(p_writer.EndObject(), "Failed to serialize collider");
    }

    bool ICollider::FromJson(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize collider - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("material");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize collider - Missing material"))
            return false;

        if (!m_material.FromJson(it->value))
            return false;

        it = p_json.FindMember("offset");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize collider - Missing offset"))
            return false;

        if (!::FromJson(m_offset, it->value))
            return false;

        it = p_json.FindMember("is_trigger");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize collider - Missing trigger flag"))
            return false;

        if (!CHECK(it->value.IsBool(), "Unable to deserialize collider trigger flag - Json value should be a boolean"))
            return false;

        m_isTrigger = it->value.GetBool();

        return FromJsonImpl(p_json);
    }
}
