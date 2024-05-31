#pragma once
#include "SurvivantPhysics/Collider/ICollider.h"

#include <SurvivantCore/ECS/ComponentTraits.h>

namespace SvPhysics
{
    class CapsuleCollider final : public ICollider
    {
    public:
        LibMath::Vector3 m_axis   = LibMath::Vector3::up();
        float            m_height = 1.f;
        float            m_radius = .5f;

        /**
         * \brief Gets the capsule collider's bounding box
         * \return The capsule collider's bounding box
         */
        LibMath::BoundingBox GetBoundingBox() const override;

        /**
         * \brief Gets the capsule collider's bounding box while using the 'up' axis
         * \return The capsule collider's bounding box
         */
        LibMath::BoundingBox GetUpwardsBoundingBox() const;

        /**
         * \brief Gets the capsule collider's bounding sphere
         * \return The capsule collider's bounding sphere
         */
        LibMath::BoundingSphere GetBoundingSphere() const override;

        /**
         * \brief Serializes the capsule collider to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJsonImpl(SvCore::Serialization::JsonWriter& p_writer) const override;

        /**
         * \brief Deserializes the capsule collider from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJsonImpl(const SvCore::Serialization::JsonValue& p_json) override;

    private:
        /**
         * \brief Refreshes the internal PhysX capsule collider
         * \param p_transform The collider's transform
         */
        void Refresh(const LibMath::Transform* p_transform) override;
    };
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle&, SvPhysics::CapsuleCollider&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, SvPhysics::CapsuleCollider&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, SvPhysics::CapsuleCollider&);
}
