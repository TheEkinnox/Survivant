#pragma once
#include "SurvivantPhysics/Material.h"

#include <SurvivantCore/Resources/ResourceRef.h>
#include <SurvivantCore/Serialization/MathSerializers.h>

#include <Geometry/BoundingBox.h>
#include <Geometry/BoundingSphere.h>

namespace physx
{
    class PxShape;
}

namespace SvPhysics
{
    class ICollider
    {
    public:
        SvCore::Resources::ResourceRef<Material> m_material;
        LibMath::Vector3                         m_offset;
        bool                                     m_isTrigger = false;

        /**
         * \brief Creates a copy of the given collider
         * \param p_other The collider to copy
         */
        ICollider(const ICollider& p_other) = default;

        /**
         * \brief Creates a move copy of the given collider
         * \param p_other The collider to move
         */
        ICollider(ICollider&& p_other) noexcept = default;

        /**
         * \brief Destroys the collider
         */
        virtual ~ICollider() = default;

        /**
         * \brief Assigns a copy of the given collider to this one
         * \param p_other The collider to copy
         * \return A reference to the modified collider
         */
        ICollider& operator=(const ICollider& p_other) = default;

        /**
         * \brief Moves the given collider into this one
         * \param p_other The collider to move
         * \return A reference to the modified collider
         */
        ICollider& operator=(ICollider&& p_other) noexcept = default;

        /**
         * \brief Gets the capsule collider's bounding box
         * \return The capsule collider's bounding box
         */
        virtual LibMath::BoundingBox GetBoundingBox() const = 0;

        /**
         * \brief Gets the capsule collider's bounding sphere
         * \return The capsule collider's bounding sphere
         */
        virtual LibMath::BoundingSphere GetBoundingSphere() const = 0;

        /**
         * \brief Serializes the box collider to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        bool ToJson(SvCore::Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the box collider from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        bool FromJson(const SvCore::Serialization::JsonValue& p_json);

    protected:
        friend class PhysicsContext;
        physx::PxShape* m_pxShape;

        ICollider() = default;

        /**
         * \brief Refreshes the internal PhysX collider
         * \param p_transform The collider's transform
         */
        virtual void Refresh(const LibMath::Transform* p_transform) = 0;

        /**
         * \brief Serializes the inherited collider information to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise
         */
        virtual bool ToJsonImpl(SvCore::Serialization::JsonWriter& p_writer) const = 0;

        /**
         * \brief Deserializes the inherited collider information from json
         * \param p_json The input json value
         * \return True on success. False otherwise
         */
        virtual bool FromJsonImpl(const SvCore::Serialization::JsonValue& p_json) = 0;
    };
}
