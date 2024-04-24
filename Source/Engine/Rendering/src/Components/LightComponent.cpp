#include "SurvivantRendering/Components/LightComponent.h"

using namespace LibMath;
using namespace SvRendering::Enums;

namespace SvRendering::Components
{
    LightComponent::LightComponent(Core::Light p_ambient)
        : m_type(ELightType::AMBIENT), m_ambient(std::move(p_ambient))
    {
    }

    LightComponent::LightComponent(Core::DirectionalLight p_directional)
        : m_type(ELightType::DIRECTIONAL), m_directional(std::move(p_directional))
    {
    }

    LightComponent::LightComponent(Core::PointLight p_point)
        : m_type(ELightType::POINT), m_point(std::move(p_point))
    {
    }

    LightComponent::LightComponent(Core::SpotLight p_spot)
        : m_type(ELightType::SPOT), m_spot(std::move(p_spot))
    {
    }

    bool SerializeAmbient(const Core::Light& p_light, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        const std::string colorString = Vector4(p_light.m_color).string();
        p_writer.Key("color");
        p_writer.String(colorString.c_str(), static_cast<rapidjson::SizeType>(colorString.size()));

        return CHECK(p_writer.EndObject());
    }

    bool DeserializeAmbient(Core::Light& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize ambient light - Json value should be an object"))
            return false;

        const auto it = p_json.FindMember("color");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize ambient light color"))
            return false;

        std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));

        Vector4 colorVec;
        iss >> colorVec;
        p_out.m_color = colorVec;

        return true;
    }

    bool SerializeDirectional(const Core::DirectionalLight& p_light, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        std::string vecString = Vector4(p_light.m_color).string();
        p_writer.Key("color");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        vecString = p_light.m_direction.string();
        p_writer.Key("direction");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        return CHECK(p_writer.EndObject());
    }

    bool DeserializeDirectional(Core::DirectionalLight& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize directional light - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("color");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize directional light color"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));

            Vector4 colorVec;
            iss >> colorVec;
            p_out.m_color = colorVec;
        }

        it = p_json.FindMember("direction");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize directional light direction"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> p_out.m_direction;
        }

        return true;
    }

    bool SerializeAttenuation(const Core::Attenuation& p_attenuation, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        p_writer.Key("constant");
        p_writer.Double(p_attenuation.m_constant);

        p_writer.Key("linear");
        p_writer.Double(p_attenuation.m_linear);

        p_writer.Key("quadratic");
        p_writer.Double(p_attenuation.m_quadratic);

        return CHECK(p_writer.EndObject());
    }

    bool DeserializeAttenuation(Core::Attenuation& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize light attenuation - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("constant");

        if (!CHECK(it != p_json.MemberEnd() && it->value.Is<float>(), "Unable to deserialize light constant attenuation"))
            return false;

        p_out.m_constant = it->value.Get<float>();

        it = p_json.FindMember("linear");

        if (!CHECK(it != p_json.MemberEnd() && it->value.Is<float>(), "Unable to deserialize light linear attenuation"))
            return false;

        p_out.m_linear = it->value.Get<float>();

        it = p_json.FindMember("quadratic");

        if (!CHECK(it != p_json.MemberEnd() && it->value.Is<float>(), "Unable to deserialize light quadratic attenuation"))
            return false;

        p_out.m_quadratic = it->value.Get<float>();

        return true;
    }

    bool SerializePoint(const Core::PointLight& p_light, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        std::string vecString = Vector4(p_light.m_color).string();
        p_writer.Key("color");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        vecString = p_light.m_position.string();
        p_writer.Key("position");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        p_writer.Key("attenuation");

        if (!SerializeAttenuation(p_light.m_attenuationData, p_writer))
            return false;

        return CHECK(p_writer.EndObject());
    }

    bool DeserializePoint(Core::PointLight& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize point light - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("color");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize point light color"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));

            Vector4 colorVec;
            iss >> colorVec;
            p_out.m_color = colorVec;
        }

        it = p_json.FindMember("position");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize point light position"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> p_out.m_position;
        }

        it = p_json.FindMember("attenuation");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize point light - attenuation not found"))
            return false;

        return DeserializeAttenuation(p_out.m_attenuationData, it->value);
    }

    bool SerializeCutoff(const Core::Cutoff& p_cutoff, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        p_writer.Key("inner");
        p_writer.Double(p_cutoff.m_inner);

        p_writer.Key("outer");
        p_writer.Double(p_cutoff.m_outer);

        return CHECK(p_writer.EndObject());
    }

    bool DeserializeCutoff(Core::Cutoff& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize light cutoff - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("inner");

        if (!CHECK(it != p_json.MemberEnd() && it->value.Is<float>(), "Unable to deserialize light inner cuttoff"))
            return false;

        p_out.m_inner = it->value.Get<float>();

        it = p_json.FindMember("outer");

        if (!CHECK(it != p_json.MemberEnd() && it->value.Is<float>(), "Unable to deserialize light outer cuttoff"))
            return false;

        p_out.m_outer = it->value.Get<float>();

        return true;
    }

    bool SerializeSpot(const Core::SpotLight& p_light, SvCore::Serialization::JsonWriter& p_writer)
    {
        p_writer.StartObject();

        std::string vecString = Vector4(p_light.m_color).string();
        p_writer.Key("color");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        vecString = p_light.m_position.string();
        p_writer.Key("position");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        vecString = p_light.m_direction.string();
        p_writer.Key("direction");
        p_writer.String(vecString.c_str(), static_cast<rapidjson::SizeType>(vecString.size()));

        p_writer.Key("attenuation");

        if (!SerializeAttenuation(p_light.m_attenuationData, p_writer))
            return false;

        p_writer.Key("cutoff");

        if (!SerializeCutoff(p_light.m_cutoff, p_writer))
            return false;

        return CHECK(p_writer.EndObject());
    }

    bool DeserializeSpot(Core::SpotLight& p_out, const SvCore::Serialization::JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize spot light - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("color");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize spot light color"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));

            Vector4 colorVec;
            iss >> colorVec;
            p_out.m_color = colorVec;
        }

        it = p_json.FindMember("position");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize spot light position"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> p_out.m_position;
        }

        it = p_json.FindMember("direction");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize spot light direction"))
            return false;

        {
            std::istringstream iss(std::string(it->value.GetString(), it->value.GetStringLength()));
            iss >> p_out.m_direction;
        }

        it = p_json.FindMember("attenuation");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize spot light - attenuation not found"))
            return false;

        if (!DeserializeAttenuation(p_out.m_attenuationData, it->value))
            return false;

        it = p_json.FindMember("cutoff");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize spot light - cutoff not found"))
            return false;

        return DeserializeCutoff(p_out.m_cutoff, it->value);
    }
}

namespace SvCore::ECS
{
    using namespace SvRendering::Components;

    template <>
    bool ComponentRegistry::ToJson(
        const LightComponent& p_component, SvCore::Serialization::JsonWriter& p_writer, const EntitiesMap&)
    {
        p_writer.StartObject();

        const std::string typeString = LightTypeToString(ELightType::AMBIENT);

        p_writer.Key("type");
        p_writer.String(typeString.c_str(), static_cast<rapidjson::SizeType>(typeString.size()));

        p_writer.Key("data");

        switch (p_component.m_type)
        {
        case ELightType::AMBIENT:
            return SerializeAmbient(p_component.m_ambient, p_writer) && CHECK(p_writer.EndObject());
        case ELightType::DIRECTIONAL:
            return SerializeDirectional(p_component.m_directional, p_writer);
        case ELightType::POINT:
            return SerializePoint(p_component.m_point, p_writer) && CHECK(p_writer.EndObject());
        case ELightType::SPOT:
            return SerializeSpot(p_component.m_spot, p_writer) && CHECK(p_writer.EndObject());
        default:
            return ASSUME(false, "Unsupported light type") && false;
        }
    }

    template <>
    bool ComponentRegistry::FromJson(LightComponent& p_out, const SvCore::Serialization::JsonValue& p_json, Scene*)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize light - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("type");

        if (!CHECK(it != p_json.MemberEnd() && it->value.IsString(), "Unable to deserialize light - Invalid type string"))
            return false;

        p_out.m_type = StringToLightType(std::string(it->value.GetString(), it->value.GetStringLength()));

        it = p_json.FindMember("data");

        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize light - Light data not found"))
            return false;

        switch (p_out.m_type)
        {
        case ELightType::AMBIENT:
            return DeserializeAmbient(p_out.m_ambient, it->value);
        case ELightType::DIRECTIONAL:
            return DeserializeDirectional(p_out.m_directional, it->value);
        case ELightType::POINT:
            return DeserializePoint(p_out.m_point, it->value);
        case ELightType::SPOT:
            return DeserializeSpot(p_out.m_spot, it->value);
        default:
            return ASSUME(false, "Unsupported light type") && false;
        }
    }
}
