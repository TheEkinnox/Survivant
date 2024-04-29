#include "SurvivantScripting/LuaScript.h"

#include <SurvivantCore/Serialization/Serializer.h>
#include <SurvivantCore/Utility/FileSystem.h>

#include <sol/state.hpp>

using namespace SvCore::Serialization;
using namespace SvCore::Utility;

namespace SvScripting
{
    LuaScript::LuaScript()
        : m_executionOrder(0)
    {
    }

    bool LuaScript::Load(const std::string& p_fileName)
    {
        if (!CHECK(!p_fileName.empty(), "Attempted to load script from empty path"))
            return false;

        {
            std::ifstream file(p_fileName, std::ios::binary | std::ios::ate);

            if (!CHECK(file.is_open(), "Unable to load script - Failed to open file at path \"%s\"", p_fileName.c_str()))
                return false;

            const std::ifstream::pos_type length = file.tellg();
            file.seekg(0, std::ios::beg);

            m_source.resize(length);
            file.read(m_source.data(), length);
        }

        return CHECK(!m_source.empty(), "Unable to load script - Empty source file") && LoadMeta(GetMetaPath(p_fileName));
    }

    bool LuaScript::Save(const std::string& p_fileName)
    {
        return SaveMeta(GetMetaPath(p_fileName));
    }

    std::string_view LuaScript::GetSource() const
    {
        return m_source;
    }

    LuaScript::OrderT LuaScript::GetExecutionOrder() const
    {
        return m_executionOrder;
    }

    void LuaScript::SetExecutionOrder(const OrderT p_executionOrder)
    {
        m_executionOrder = p_executionOrder;
    }

    bool LuaScript::LoadMeta(const std::string& p_path)
    {
        if (!PathExists(p_path))
            return true;

        JsonDocument json = LoadJsonFile(p_path);

        const auto it = json.FindMember("order");

        if (!CHECK(it != json.MemberEnd(), "Unable to deserialize lua script meta data - Missing execution order"))
            return false;

        if (!CHECK(it->value.IsInt(), "Unable to deserialize lua script execution order - Json value should be an int"))
            return false;

        m_executionOrder = static_cast<OrderT>(it->value.GetInt());

        return true;
    }

    bool LuaScript::SaveMeta(const std::string& p_path) const
    {
        std::ofstream fs(p_path);

        if (!CHECK(fs.is_open(), "Unable to open lua script meta file at path \"%s\"", p_path.c_str()))
            return false;

        JsonOStream    jos(fs);
        JsonFileWriter writer(jos);

        writer.StartObject();

        writer.Key("order");
        writer.Int(m_executionOrder);

        if (!writer.EndObject() || !ASSUME(writer.IsComplete(), "Failed to save lua script meta - Generated json is incomplete"))
            return false;

        return CHECK(!fs.bad(), "Failed to write lua script meta data to \"%s\"", p_path.c_str());
    }
}
