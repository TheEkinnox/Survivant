#pragma once
#include "SurvivantPhysics/Collider/ICollider.h"

#include <SurvivantCore/ECS/ComponentTraits.h>

namespace SvPhysics
{
    class SphereCollider final : public ICollider
    {
    public:
        float m_radius = 1.f;

        /**
         * \brief Gets the sphere collider's bounding box
         * \return The sphere collider's bounding box
         */
        LibMath::BoundingBox GetBoundingBox() const override;

        /**
         * \brief Gets the sphere collider's bounding sphere
         * \return The sphere collider's bounding sphere
         */
        LibMath::BoundingSphere GetBoundingSphere() const override;

        /**
         * \brief Serializes the sphere collider to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJsonImpl(SvCore::Serialization::JsonWriter& p_writer) const override;

        /**
         * \brief Deserializes the sphere collider from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJsonImpl(const SvCore::Serialization::JsonValue& p_json) override;

    private:
        /**
         * \brief Refreshes the internal PhysX sphere collider
         * \param p_transform The collider's transform
         */
        void Refresh(const LibMath::Transform* p_transform) override;
    };
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle&, SvPhysics::SphereCollider&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, SvPhysics::SphereCollider&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, SvPhysics::SphereCollider&);
}
