#include "SurvivantScripting/LuaScript.h"

#include <sol/state.hpp>

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

        std::ifstream file(p_fileName, std::ios::binary | std::ios::ate);

        if (!CHECK(file.is_open(), "Unable to load script - Failed to open file at path \"%s\"", p_fileName.c_str()))
            return false;

        const std::ifstream::pos_type length = file.tellg();
        file.seekg(0, std::ios::beg);

        m_source.resize(length);
        file.read(m_source.data(), length);
        file.close();

        return !m_source.empty();
    }

    std::string_view LuaScript::GetSource() const
    {
        return m_source;
    }

    LuaScript::OrderT LuaScript::GetExecutionOrder() const
    {
        return m_executionOrder;
    }
}
