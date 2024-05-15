#include "SurvivantRendering/Resources/Material.h"

#include "SurvivantRendering/RHI/ITexture.h"

#include <SurvivantCore/Debug/Assertion.h>

using namespace LibMath;
using namespace SvCore::Resources;
using namespace SvCore::Serialization;
using namespace SvRendering::Enums;
using namespace SvRendering::RHI;

namespace SvRendering::Resources
{
    Material::Material(const ResourceRef<IShader>& p_shader)
    {
        SetShader(p_shader);
    }

    bool Material::Load(const std::string& p_fileName)
    {
        return FromJson(LoadJsonFile(p_fileName));
    }

    bool Material::Save(const std::string& p_fileName)
    {
        JsonStringBuffer buffer;
        JsonWriter       writer(buffer);

        if (!ToJson(writer) || !ASSUME(writer.IsComplete(), "Failed to save material - Generated json is incomplete"))
            return false;

        std::ofstream fs(p_fileName);

        if (!CHECK(fs.is_open(), "Unable to open material file at path \"%s\"", p_fileName.c_str()))
            return false;

        fs << std::string_view(buffer.GetString(), buffer.GetLength());

        return CHECK(!fs.bad(), "Failed to write material to \"%s\"", p_fileName.c_str());
    }

    bool Material::ToJson(JsonWriter& p_writer) const
    {
        p_writer.StartObject();

        p_writer.Key("shader");
        if (!CHECK(m_shader.ToJson(p_writer), "Unable to serialize material - Failed to serialize shader"))
            return false;

        p_writer.Key("properties");
        p_writer.StartArray();

        for (const auto& [name, property] : m_properties)
        {
            p_writer.StartObject();

            p_writer.Key("name");
            p_writer.String(name.c_str(), static_cast<rapidjson::SizeType>(name.size()));

            p_writer.Key("type");
            p_writer.Uint(static_cast<uint8_t>(property.m_type));

            p_writer.Key("value");

            if (!CHECK(SerializePropertyValue(p_writer, property), "Unable to serialize material property \"%s\"", name.c_str()))
                return false;

            p_writer.EndObject();
        }

        p_writer.EndArray();
        return p_writer.EndObject();
    }

    bool Material::FromJson(const JsonValue& p_json)
    {
        if (!CHECK(p_json.IsObject(), "Unable to deserialize material - Json value should be an object"))
            return false;

        auto it = p_json.FindMember("shader");
        if (!CHECK(it != p_json.MemberEnd(), "Unable to deserialize material - Missing shader") || !m_shader.FromJson(it->value))
            return false;

        it = p_json.FindMember("properties");
        return CHECK(it != p_json.MemberEnd(), "Unable to deserialize material - Missing properties") &&
            DeserializeProperties(it->value);
    }

    IShader& Material::GetShader() const
    {
        ASSERT(m_shader, "Missing material shader");
        return *m_shader;
    }

    ResourceRef<IShader> Material::GetShaderRef() const
    {
        return m_shader;
    }

    void Material::SetShader(const ResourceRef<IShader>& p_shader)
    {
        m_shader = p_shader;
        m_properties.clear();

        for (auto [name, uniform] : m_shader->GetUniforms())
        {
            if (name.starts_with(ENGINE_UNIFORM_PREFIX))
                continue;

            m_properties[name] =
            {
                uniform.m_type,
                GetDefaultValue(uniform.m_type)
            };
        }
    }

    const Material::Property& Material::GetProperty(const std::string& p_name) const
    {
        const auto it = m_properties.find(p_name);
        ASSERT(it != m_properties.end(), "Unable to find material property \"%s\"", p_name.c_str());
        return it->second;
    }

    Material::Property& Material::GetProperty(const std::string& p_name)
    {
        const auto it = m_properties.find(p_name);
        ASSERT(it != m_properties.end(), "Unable to find material property \"%s\"", p_name.c_str());
        return it->second;
    }

    const std::unordered_map<std::string, Material::Property>& Material::GetProperties() const
    {
        return m_properties;
    }

    std::unordered_map<std::string, Material::Property>& Material::GetProperties()
    {
        return m_properties;
    }

    void Material::Bind() const
    {
        ASSERT(m_shader, "Failed to bind material - Missing shader");
        m_shader->Bind();

        for (const auto& [name, property] : m_properties)
            BindProperty(name, property);
    }

    std::any Material::GetDefaultValue(const EShaderDataType p_dataType)
    {
        switch (p_dataType)
        {
        case EShaderDataType::BOOL:
            return false;
        case EShaderDataType::INT:
            return 0;
        case EShaderDataType::UNSIGNED_INT:
            return 0u;
        case EShaderDataType::FLOAT:
            return 0.f;
        case EShaderDataType::VEC2:
            return Vector2();
        case EShaderDataType::VEC3:
            return Vector3();
        case EShaderDataType::VEC4:
            return Vector4();
        case EShaderDataType::MAT3:
            return Matrix3();
        case EShaderDataType::MAT4:
            return Matrix4();
        case EShaderDataType::TEXTURE:
            return ResourceRef<ITexture>();
        case EShaderDataType::UNKNOWN:
        default:
            ASSERT(false, "Failed to get default value - Unkown data type");
            return {};
        }
    }

    void Material::BindProperty(const std::string& p_name, const Property& p_property) const
    {
        ASSERT(m_shader, "Unable to bind material property - No shader");

        switch (p_property.m_type)
        {
        case EShaderDataType::BOOL:
            m_shader->SetUniformInt(p_name, std::any_cast<bool>(p_property.m_value));
            break;
        case EShaderDataType::INT:
            m_shader->SetUniformInt(p_name, std::any_cast<int>(p_property.m_value));
            break;
        case EShaderDataType::UNSIGNED_INT:
            m_shader->SetUniformUInt(p_name, std::any_cast<uint32_t>(p_property.m_value));
            break;
        case EShaderDataType::FLOAT:
            m_shader->SetUniformFloat(p_name, std::any_cast<float>(p_property.m_value));
            break;
        case EShaderDataType::VEC2:
            m_shader->SetUniformVec2(p_name, std::any_cast<const Vector2&>(p_property.m_value));
            break;
        case EShaderDataType::VEC3:
            m_shader->SetUniformVec3(p_name, std::any_cast<const Vector3&>(p_property.m_value));
            break;
        case EShaderDataType::VEC4:
            m_shader->SetUniformVec4(p_name, std::any_cast<const Vector4&>(p_property.m_value));
            break;
        case EShaderDataType::MAT3:
            m_shader->SetUniformMat3(p_name, std::any_cast<const Matrix3&>(p_property.m_value));
            break;
        case EShaderDataType::MAT4:
            m_shader->SetUniformMat4(p_name, std::any_cast<const Matrix4&>(p_property.m_value));
            break;
        case EShaderDataType::TEXTURE:
        {
            m_shader->SetUniformTexture(p_name, std::any_cast<const ResourceRef<ITexture>&>(p_property.m_value).GetOrDefault());
            break;
        }
        case EShaderDataType::UNKNOWN:
        default:
            ASSERT(false, "Unknown uniform type");
            return;
        }
    }

    bool Material::SerializePropertyValue(JsonWriter& p_writer, const Property& p_property)
    {
        switch (p_property.m_type)
        {
        case EShaderDataType::BOOL:
            return p_writer.Bool(std::any_cast<bool>(p_property.m_value));
        case EShaderDataType::INT:
            return p_writer.Int(std::any_cast<int>(p_property.m_value));
        case EShaderDataType::UNSIGNED_INT:
            return p_writer.Uint(std::any_cast<uint32_t>(p_property.m_value));
        case EShaderDataType::FLOAT:
            return p_writer.Double(std::any_cast<float>(p_property.m_value));
        case EShaderDataType::VEC2:
        {
            const std::string str = std::any_cast<Vector2>(p_property.m_value).string();
            return p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
        }
        case EShaderDataType::VEC3:
        {
            const std::string str = std::any_cast<Vector3>(p_property.m_value).string();
            return p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
        }
        case EShaderDataType::VEC4:
        {
            const std::string str = std::any_cast<Vector4>(p_property.m_value).string();
            return p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
        }
        case EShaderDataType::MAT3:
        {
            const std::string str = std::any_cast<Matrix3>(p_property.m_value).string();
            return p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
        }
        case EShaderDataType::MAT4:
        {
            const std::string str = std::any_cast<Matrix4>(p_property.m_value).string();
            return p_writer.String(str.c_str(), static_cast<rapidjson::SizeType>(str.size()));
        }
        case EShaderDataType::TEXTURE:
            return std::any_cast<const ResourceRef<ITexture>&>(p_property.m_value).ToJson(p_writer);
        case EShaderDataType::UNKNOWN:
        default:
            return false;
        }
    }

    bool Material::DeserializeProperties(const JsonValue& p_json)
    {
        m_properties.clear();

        if (!CHECK(p_json.IsArray(), "Unable to deserialize material properties - Json value should be an object"))
            return false;

        for (const auto& jsonProperty : p_json.GetArray())
        {
            auto it = jsonProperty.FindMember("name");
            if (!CHECK(it != jsonProperty.MemberEnd() && it->value.IsString(), "Unable to deserialize material property name"))
                return false;

            std::string name = it->value.GetString();

            Property property;
            it = jsonProperty.FindMember("type");
            if (!CHECK(it != jsonProperty.MemberEnd() && it->value.IsUint(), "Unable to deserialize material property type"))
                return false;

            property.m_type = static_cast<EShaderDataType>(it->value.GetUint());

            it = jsonProperty.FindMember("value");
            if (!DeserializePropertyValue(it->value, property))
                return false;

            m_properties[name] = property;
        }

        return true;
    }

    bool Material::DeserializePropertyValue(const JsonValue& p_json, Property& p_out)
    {
        switch (p_out.m_type)
        {
        case EShaderDataType::BOOL:
        {
            if (!CHECK(p_json.IsBool(), "Unable to deserialize boolean material property value"))
                return false;

            p_out.m_value = p_json.GetBool();
            return true;
        }
        case EShaderDataType::INT:
        {
            if (!CHECK(p_json.IsInt(), "Unable to deserialize integer material property value"))
                return false;

            p_out.m_value = p_json.GetInt();
            return true;
        }
        case EShaderDataType::UNSIGNED_INT:
        {
            if (!CHECK(p_json.IsUint(), "Unable to deserialize unsigned integer material property value"))
                return false;

            p_out.m_value = p_json.GetUint();
            return true;
        }
        case EShaderDataType::FLOAT:
        {
            if (!CHECK(p_json.IsFloat(), "Unable to deserialize float material property value"))
                return false;

            p_out.m_value = p_json.GetFloat();
            return true;
        }
        case EShaderDataType::VEC2:
        {
            if (!CHECK(p_json.IsString(), "Unable to deserialize Vector2 material property value"))
                return false;

            std::istringstream iss(p_json.GetString());

            Vector2 vec;
            iss >> vec;

            p_out.m_value = vec;
            return true;
        }
        case EShaderDataType::VEC3:
        {
            if (!CHECK(p_json.IsString(), "Unable to deserialize Vector3 material property value"))
                return false;

            std::istringstream iss(p_json.GetString());

            Vector3 vec;
            iss >> vec;

            p_out.m_value = vec;
            return true;
        }
        case EShaderDataType::VEC4:
        {
            if (!CHECK(p_json.IsString(), "Unable to deserialize Vector4 material property value"))
                return false;

            std::istringstream iss(p_json.GetString());

            Vector4 vec;
            iss >> vec;

            p_out.m_value = vec;
            return true;
        }
        case EShaderDataType::MAT3:
        {
            if (!CHECK(p_json.IsString(), "Unable to deserialize Matrix3 material property value"))
                return false;

            std::istringstream iss(p_json.GetString());

            Matrix3 vec;
            iss >> vec;

            p_out.m_value = vec;
            return true;
        }
        case EShaderDataType::MAT4:
        {
            if (!CHECK(p_json.IsString(), "Unable to deserialize Matrix4 material property value"))
                return false;

            std::istringstream iss(p_json.GetString());

            Matrix4 vec;
            iss >> vec;

            p_out.m_value = vec;
            return true;
        }
        case EShaderDataType::TEXTURE:
        {
            ResourceRef<ITexture> texture;

            if (!texture.FromJson(p_json))
                return false;

            p_out.m_value = texture;
            return true;
        }
        case EShaderDataType::UNKNOWN:
        default:
            return false;
        }
    }
}
