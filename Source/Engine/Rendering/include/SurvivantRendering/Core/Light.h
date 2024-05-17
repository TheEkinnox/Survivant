#pragma once
#include "SurvivantRendering/Core/Color.h"

#include <Transform.h>
#include <Matrix/Matrix4.h>
#include <Vector/Vector3.h>

namespace SvRendering::Core
{
    struct Light
    {
        /**
         * \brief The light's color (alpha is intensity)
         */
        Color m_color = Color::white;

        Light() = default;
        Light(const Color& p_color);

        Light(const Light&) = default;
        Light(Light&&)      = default;

        virtual ~Light() = default;

        Light& operator=(const Light&) = default;
        Light& operator=(Light&&)      = default;

        /**
         * \brief Creates a matrix storing the light's color in the first row,\n
         and the light type in the fourth row's fourth column
         * \return A matrix containing the light's data
         */
        virtual LibMath::Matrix4 GetMatrix(const LibMath::Transform* = nullptr) const;
    };

    struct DirectionalLight final : Light
    {
        LibMath::Vector3 m_direction;

        DirectionalLight() = default;
        DirectionalLight(const Light& p_light, const LibMath::Vector3& p_direction);

        DirectionalLight(const DirectionalLight&) = default;
        DirectionalLight(DirectionalLight&&)      = default;
        ~DirectionalLight() override              = default;

        DirectionalLight& operator=(const DirectionalLight&) = default;
        DirectionalLight& operator=(DirectionalLight&&)      = default;

        /**
         * \brief Creates a matrix storing the light's color in the first row,\n
         the direction in the remaining space of the first column,\n
         and the light type in the fourth row's fourth column
         * \param p_transform The (optional) transform to apply to the light
         * \return A matrix containing the light's data
         */
        LibMath::Matrix4 GetMatrix(const LibMath::Transform* p_transform = nullptr) const override;
    };

    struct Attenuation
    {
        float m_constant  = 1;
        float m_linear    = 0;
        float m_quadratic = 0;

        Attenuation() = default;
        explicit Attenuation(float p_range);
        Attenuation(float p_constant, float p_linear, float p_quadratic);
    };

    struct PointLight final : Light
    {
        LibMath::Vector3 m_position;
        Attenuation      m_attenuationData;

        PointLight() = default;
        PointLight(const Light& p_light, const LibMath::Vector3& p_position, const Attenuation& p_attenuationData);

        PointLight(const PointLight&) = default;
        PointLight(PointLight&&)      = default;
        ~PointLight() override        = default;

        PointLight& operator=(const PointLight&) = default;
        PointLight& operator=(PointLight&&)      = default;

        /**
         * \brief Creates a matrix storing the light's color in the first row,\n
         the position in the remaining space of the second column,\n
         the attenuation data in the remaining space of the third column,\n
         and the light type in the fourth row's fourth column
         * \param p_transform The (optional) transform to apply to the light
         * \return A matrix containing the light's data
         */
        LibMath::Matrix4 GetMatrix(const LibMath::Transform* p_transform = nullptr) const override;
    };

    struct Cutoff
    {
        float m_inner;
        float m_outer;
    };

    struct SpotLight final : Light
    {
        LibMath::Vector3 m_position;
        LibMath::Vector3 m_direction;

        Attenuation m_attenuationData;
        Cutoff      m_cutoff{ 0, 0 };

        SpotLight() = default;
        SpotLight(const Light&       p_light, const LibMath::Vector3& p_position, const LibMath::Vector3& p_direction,
                  const Attenuation& p_attenuation, const Cutoff&     p_cutoff);

        SpotLight(const SpotLight&) = default;
        SpotLight(SpotLight&&)      = default;
        ~SpotLight() override       = default;

        SpotLight& operator=(const SpotLight&) = default;
        SpotLight& operator=(SpotLight&&)      = default;

        /**
         * \brief Creates a matrix storing the light's color in the first row,\n
         the direction in the remaining space of the first column,\n
         the position in the remaining space of the second column,\n
         the attenuation data in the remaining space of the third column,\n
         the cutoff data in the remaining space of the fourth column,\n
         and the light type in the fourth row's fourth column
         * \param p_transform The (optional) transform to apply to the light
         * \return A matrix containing the light's data
         */
        LibMath::Matrix4 GetMatrix(const LibMath::Transform* p_transform = nullptr) const override;
    };
}
