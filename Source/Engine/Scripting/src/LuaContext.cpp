#include "SurvivantScripting/LuaContext.h"

#include "SurvivantCore/Utility/FileSystem.h"

#include "SurvivantScripting/FunctionNames.h"
#include "SurvivantScripting/LuaScriptList.h"
#include "SurvivantScripting/Bindings/LuaECSBinder.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"

#include <SurvivantCore/Resources/ResourceManager.h>

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

namespace SvScripting
{
    bool LuaContext::ScriptHandle::operator<(const ScriptHandle& p_other) const
    {
        return (m_script && p_other.m_script && m_script->GetExecutionOrder() < p_other.m_script->GetExecutionOrder())
            || m_name.compare(p_other.m_name) < 0
            || m_script.GetPath().compare(p_other.m_script.GetPath()) < 0
            || m_owner.GetEntity() < p_other.m_owner.GetEntity()
            || std::less<Scene*>{}(m_owner.GetScene(), p_other.m_owner.GetScene());
    }

    bool LuaContext::ScriptHandle::operator==(const ScriptHandle& p_other) const
    {
        return m_owner == p_other.m_owner && m_script == p_other.m_script;
    }

    LuaContext::ScriptHandle::operator bool() const
    {
        if (!m_owner || !m_script)
            return false;

        const LuaScriptList* scripts = m_owner.Get<LuaScriptList>();
        return scripts->Contains(m_name);
    }

    LuaContext::LuaContext()
        : m_isValid(false), m_hasStarted(false)
    {
    }

    LuaContext::~LuaContext()
    {
        Reset();
    }

    LuaContext& LuaContext::GetInstance()
    {
        static LuaContext instance;
        return instance;
    }

    void LuaContext::Init()
    {
        if (ASSUME_FALSE(m_state, "Attempted to initialize an already initialized lua context."))
            return;

        m_state = std::make_unique<sol::state>();
        m_state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        m_state->add_package_loader(&LoadModule);
        m_isValid = true;

        BindUserTypes(*m_state);

        for (size_t i = m_scripts.size(); i > 0; --i)
        {
            if (i > m_scripts.size())
                continue;

            if (!RegisterScript(m_scripts[i - 1]))
                return;
        }
    }

    void LuaContext::Reset()
    {
        for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
        {
            if (i > m_scripts.size())
                continue;

            m_isValid = TryCall(m_scripts[i - 1].m_table, ScriptingFunctions::DESTROY) != ELuaCallResult::FAILURE;
            ASSERT(m_isValid);

            m_scripts[i - 1].m_table = sol::nil;
        }

        if (m_state)
            m_state.reset();

        m_isValid = false;
    }

    bool LuaContext::IsValid() const
    {
        return m_isValid;
    }

    bool LuaContext::RegisterScript(ScriptHandle& p_handle)
    {
        if (!CHECK(IsValid(), "Attempted to register script \"%s\" to invalid lua context", p_handle.m_script.GetPath().c_str()))
            return false;

        if (!CHECK(p_handle.m_script, "Attempted to register unloaded script \"%s\"", p_handle.m_script.GetPath().c_str()))
            return (m_isValid = false);

        const std::string_view source = p_handle.m_script->GetSource();

        if (source.empty())
            return (m_isValid = false);

        const auto result = m_state->safe_script(source, &sol::script_pass_on_error);

        if (!result.valid())
        {
            [[maybe_unused]] const sol::error err = result;
            CHECK(false, "Failed to register script \"%s\" - %s", p_handle.m_script.GetPath().c_str(), err.what());
            return (m_isValid = false);
        }

        if (!CHECK(result.return_count() == 1 && result[0].is<sol::table>(),
                "Failed to register script \"%s\" - Invalid return", p_handle.m_script.GetPath().c_str()))
            return (m_isValid = false);

        sol::table resultTable = result[0].as<sol::table>();

        if (p_handle.m_table != sol::nil)
            for (auto& [key, value] : p_handle.m_table)
                resultTable[key] = value;

        resultTable["owner"] = static_cast<const EntityHandle&>(p_handle.m_owner);

        if (TryCall(resultTable, ScriptingFunctions::INIT) == ELuaCallResult::FAILURE)
            return (m_isValid = false);

        if (m_hasStarted && TryCall(resultTable, ScriptingFunctions::START) == ELuaCallResult::FAILURE)
            return (m_isValid = false);

        p_handle.m_table = resultTable;
        return m_isValid;
    }

    LuaContext::ScriptHandle LuaContext::AddScript(
        const std::string& p_script, const EntityHandle& p_owner, const sol::table& p_hint)
    {
        const ResourceRef<LuaScript> scriptRef(GetModulePath(p_script));

        ScriptHandle handle = { GetModuleName(p_script), scriptRef, p_owner, p_hint };

        if (!RegisterScript(handle))
            return {};

        const auto insertIt = std::ranges::find_if_not(m_scripts, [&handle](const ScriptHandle& p_other)
        {
            return handle < p_other;
        });

        m_isValid = CHECK(m_scripts.insert(insertIt, handle) != m_scripts.end(), "Failed to add script \"%s\"", p_script.c_str());

        return m_isValid ? handle : ScriptHandle{};
    }

    LuaContext::ScriptHandle LuaContext::GetScript(const std::string& p_script, const EntityHandle& p_owner) const
    {
        const auto& moduleName = GetModuleName(p_script);
        const auto& modulePath = GetModulePath(p_script);

        const auto it = std::ranges::find_if(m_scripts, [&moduleName, &modulePath, &p_owner](const ScriptHandle& p_other)
        {
            return (p_other.m_name == moduleName || p_other.m_script.GetPath() == modulePath)
                && p_other.m_owner.GetEntity() == p_owner.GetEntity()
                && p_other.m_owner.GetScene() == p_owner.GetScene();
        });

        return it != m_scripts.end() ? *it : ScriptHandle{};
    }

    void LuaContext::RemoveScript(const std::string& p_script, EntityHandle& p_owner)
    {
        const auto& moduleName = GetModuleName(p_script);
        const auto& modulePath = GetModulePath(p_script);

        const auto it = std::ranges::find_if(m_scripts, [&moduleName, &modulePath, &p_owner](const ScriptHandle& p_other)
        {
            return (p_other.m_name == moduleName || p_other.m_script.GetPath() == modulePath)
                && p_other.m_owner.GetEntity() == p_owner.GetEntity()
                && p_other.m_owner.GetScene() == p_owner.GetScene();
        });

        if (it != m_scripts.end())
        {
            m_isValid = TryCall(it->m_table, ScriptingFunctions::DESTROY) != ELuaCallResult::FAILURE;

            it->m_table = sol::nil;
            m_scripts.erase(it);
        }
    }

    void LuaContext::Start()
    {
        if (ASSUME_FALSE(m_hasStarted, "Attempted to recall start function on started lua context."))
            return;

        for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
        {
            if (i <= m_scripts.size())
                m_isValid = TryCall(m_scripts[i - 1].m_table, ScriptingFunctions::START) != ELuaCallResult::FAILURE;
        }

        m_hasStarted = m_isValid;
    }

    void LuaContext::Update(const float p_deltaTime)
    {
        for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
        {
            if (i <= m_scripts.size())
                m_isValid = TryCall(m_scripts[i - 1].m_table, ScriptingFunctions::UPDATE, p_deltaTime) != ELuaCallResult::FAILURE;
        }
    }

    void LuaContext::Stop()
    {
        if (!ASSUME(m_hasStarted, "Attempted to stop a non started lua context"))
            return;

        for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
        {
            if (i <= m_scripts.size())
                m_isValid = TryCall(m_scripts[i - 1].m_table, ScriptingFunctions::STOP) != ELuaCallResult::FAILURE;
        }

        m_hasStarted = false;
    }

    lua_State* LuaContext::GetLuaState() const
    {
        return m_state ? m_state->lua_state() : nullptr;
    }

    const std::string& LuaContext::GetModuleName(std::string p_module)
    {
        static std::string empty;

        if (p_module.empty())
            return empty;

        const auto it = s_moduleNames.find(p_module);

        if (it != s_moduleNames.end())
            return it->second;

        const std::string base = p_module;

        for (const auto& extension : EXTENSIONS)
        {
            if (p_module.ends_with(extension))
                p_module = p_module.substr(0, p_module.size() - strlen(extension));
        }

        ReplaceInPlace(p_module, ".", "/");

        p_module = ResourceManager::GetInstance().GetRelativePath(p_module);

        ReplaceInPlace(p_module, "/", ".");
        ReplaceInPlace(p_module, "\\", ".");

        return (s_moduleNames[base] = p_module);
    }

    const std::string& LuaContext::GetModulePath(std::string p_module)
    {
        static std::string empty;

        if (p_module.empty())
            return empty;

        const auto it = s_modulePaths.find(p_module);

        if (it != s_modulePaths.end())
            return it->second;

        const std::string base = p_module;

        p_module = Replace(GetModuleName(p_module), ".", "/");

        const ResourceManager& resourceManager = ResourceManager::GetInstance();

        for (const auto& extension : EXTENSIONS)
        {
            std::string path(resourceManager.GetFullPath(p_module + extension));

            if (PathExists(path))
                return (s_modulePaths[base] = path);
        }

        return empty;
    }

    int LuaContext::LoadModule(lua_State* p_luaState)
    {
        const std::string& module = GetModulePath(sol::stack::get<std::string>(p_luaState, 1));

        if (module.empty())
            return 1;

        const std::vector<char> script = ResourceManager::GetInstance().ReadFile(module);

        if (!script.empty() && luaL_loadbuffer(p_luaState, script.data(), script.size(), module.c_str()) == LUA_OK)
        {
            sol::stack::push(p_luaState, GetModuleName(module).c_str());
            return 2;
        }

        return 1;
    }

    void LuaContext::BindUserTypes(sol::state& p_luaState)
    {
        Bindings::LuaECSBinder::Bind(p_luaState);
        Bindings::LuaMathBinder::Bind(p_luaState);
    }
}
