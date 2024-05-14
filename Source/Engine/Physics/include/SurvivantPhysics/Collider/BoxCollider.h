#pragma once
#include "SurvivantPhysics/Collider/ICollider.h"

#include <SurvivantCore/ECS/ComponentTraits.h>

namespace SvPhysics
{
    class BoxCollider final : public ICollider
    {
    public:
        LibMath::Vector3 m_size = LibMath::Vector3::one();

        /**
         * \brief Gets the box collider's bounding box
         * \return The box collider's bounding box
         */
        LibMath::BoundingBox GetBoundingBox() const override;

        /**
         * \brief Gets the box collider's bounding sphere
         * \return The box collider's bounding sphere
         */
        LibMath::BoundingSphere GetBoundingSphere() const override;

        /**
         * \brief Serializes the box collider to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJsonImpl(SvCore::Serialization::JsonWriter& p_writer) const override;

        /**
         * \brief Deserializes the box collider from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJsonImpl(const SvCore::Serialization::JsonValue& p_json) override;

    private:
        /**
         * \brief Refreshes the internal PhysX box collider
         * \param p_transform The collider's transform
         */
        void Refresh(const LibMath::Transform* p_transform) override;
    };
}

namespace SvCore::ECS
{
    template <>
    void ComponentTraits::OnAdd(EntityHandle&, SvPhysics::BoxCollider&);

    template <>
    void ComponentTraits::OnRemove(EntityHandle&, SvPhysics::BoxCollider&);

    template <>
    void ComponentTraits::OnChange(EntityHandle&, SvPhysics::BoxCollider&);
}
