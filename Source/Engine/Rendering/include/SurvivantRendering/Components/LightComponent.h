#pragma once
#include "SurvivantRendering/Core/Light.h"
#include "SurvivantRendering/Enums/ELightType.h"

#include <SurvivantCore/ECS/ComponentRegistry.h>

namespace SvRendering::Components
{
    struct LightComponent
    {
        Enums::ELightType m_type;

        Core::Light            m_ambient;
        Core::DirectionalLight m_directional;
        Core::PointLight       m_point;
        Core::SpotLight        m_spot;

        LightComponent(Core::Light p_ambient = Core::Light());
        LightComponent(Core::DirectionalLight p_directional);
        LightComponent(Core::PointLight p_point);
        LightComponent(Core::SpotLight p_spot);

        LightComponent(const LightComponent& p_other)     = default;
        LightComponent(LightComponent&& p_other) noexcept = default;
        ~LightComponent()                                 = default;

        LightComponent& operator=(const LightComponent& p_other)     = default;
        LightComponent& operator=(LightComponent&& p_other) noexcept = default;
    };

    /**
     * \brief Serializes the given ambient light to json
     * \param p_light The light to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializeAmbient(const Core::Light& p_light, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given ambient light from json
     * \param p_out The output light
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializeAmbient(Core::Light& p_out, const rapidjson::Value& p_json);

    /**
     * \brief Serializes the given directional light to json
     * \param p_light The light to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializeDirectional(const Core::DirectionalLight& p_light, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given directional light from json
     * \param p_out The output light
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializeDirectional(Core::DirectionalLight& p_out, const rapidjson::Value& p_json);

    /**
     * \brief Serializes the given attenuation data to json
     * \param p_attenuation The attenuation data to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializeAttenuation(const Core::Attenuation& p_attenuation, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given attenuation data from json
     * \param p_out The output attenuation data
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializeAttenuation(Core::Attenuation& p_out, const rapidjson::Value& p_json);

    /**
     * \brief Serializes the given point light to json
     * \param p_light The light to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializePoint(const Core::PointLight& p_light, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given point light from json
     * \param p_out The output light
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializePoint(Core::PointLight& p_out, const rapidjson::Value& p_json);

    /**
     * \brief Serializes the given cutoff data to json
     * \param p_cutoff The cutoff data to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializeCutoff(const Core::Cutoff& p_cutoff, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given cutoff data from json
     * \param p_out The output cutoff data
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializeCutoff(Core::Cutoff& p_out, const rapidjson::Value& p_json);

    /**
     * \brief Serializes the given spot light to json
     * \param p_light The light to serialize
     * \param p_writer The output json writer
     * \return True on success. False otherwise
     */
    bool SerializeSpot(const Core::SpotLight& p_light, rapidjson::Writer<rapidjson::StringBuffer>& p_writer);

    /**
     * \brief Deserializes the given spot light from json
     * \param p_out The output light
     * \param p_json The input json data
     * \return True on success. False otherwise
     */
    bool DeserializeSpot(Core::SpotLight& p_out, const rapidjson::Value& p_json);
}

namespace SvCore::ECS
{
    template <>
    bool ComponentRegistry::ToJson(
        const SvRendering::Components::LightComponent&, rapidjson::Writer<rapidjson::StringBuffer>&, const EntitiesMap&);

    template <>
    bool ComponentRegistry::FromJson(
        SvRendering::Components::LightComponent&, const rapidjson::Value&, Scene*);
}
