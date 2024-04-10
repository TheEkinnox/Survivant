#include "SurvivantScripting/LuaContext.h"

#include "SurvivantScripting/FunctionNames.h"
#include "SurvivantScripting/LuaScriptComponent.h"

#include <SurvivantCore/Resources/ResourceManager.h>

#include <ranges>

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

namespace SvScripting
{
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
        if (!ASSUME(!m_state, "Attempted to initialize an already initialized lua context."))
            return;

        m_state = std::make_unique<sol::state>();
        m_state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
        m_state->add_package_loader(&LoadModule);
        m_isValid = true;

        for (auto entity : m_scripts | std::views::values)
        {
            LuaScriptComponent* script = entity.Get<LuaScriptComponent>();

            if (script && !RegisterScript(entity, *script))
                return;
        }
    }

    void LuaContext::Reset()
    {
        for (auto entity : m_scripts | std::views::values)
        {
            if (LuaScriptComponent* script = entity.Get<LuaScriptComponent>())
                script->m_table = sol::nil;
        }

        if (m_state)
            m_state.reset();

        m_isValid = false;
    }

    bool LuaContext::IsValid() const
    {
        return m_isValid;
    }

    bool LuaContext::RegisterScript(const EntityHandle& p_entity, LuaScriptComponent& p_script)
    {
        [[maybe_unused]] const std::string path = p_script.m_script.GetPath();

        if (!CHECK(IsValid(), "Attempted to register script %s to invalid lua context", path.c_str()))
            return false;

        const LuaScript* luaScript = p_script.m_script.Get();

        if (!CHECK(luaScript, "Attempted to register unloaded script %s", path.c_str()))
            return (m_isValid = false);

        const std::string_view source = luaScript->GetSource();

        if (source.empty())
            return (m_isValid = false);

        const auto result = m_state->safe_script(source, &sol::script_pass_on_error);

        if (!result.valid())
        {
            [[maybe_unused]] const sol::error err = result;
            CHECK(false, "Failed to register script %s - %s", path.c_str(), err.what());
            return (m_isValid = false);
        }

        if (!CHECK(result.return_count() == 1 && result[0].is<sol::table>(), "Failed to register script %s - Invalid return",
                path.c_str()))
            return (m_isValid = false);

        p_script.m_table          = result[0];
        p_script.m_table["owner"] = static_cast<Entity::Id>(p_entity.GetEntity());

        TryCall(p_script, ScriptingFunctions::INIT);

        if (m_hasStarted)
            TryCall(p_script, ScriptingFunctions::START);

        return (m_isValid = true);
    }

    bool LuaContext::AddScript(const EntityHandle& p_entity, LuaScriptComponent& p_script)
    {
        if (!RegisterScript(p_entity, p_script))
            return false;

        [[maybe_unused]] const std::string path = p_script.m_script.GetPath();

        const LuaScript* luaScript = p_script.m_script.Get();

        const LuaScript::OrderT executionOrder = luaScript->GetExecutionOrder();

        const auto insertIt = std::ranges::find_if_not(m_scripts, [&p_entity, executionOrder](const ScriptHandle& p_other)
        {
            return p_other.first < executionOrder
                || p_other.second.GetEntity() < p_entity.GetEntity()
                || reinterpret_cast<size_t>(p_other.second.GetScene()) < reinterpret_cast<size_t>(p_entity.GetScene());
        });

        if (!CHECK(m_scripts.emplace(insertIt, executionOrder, p_entity) != m_scripts.end(),
                "Failed to add script %s", path.c_str()))
            return (m_isValid = false);

        return (m_isValid = true);
    }

    void LuaContext::RemoveScript(EntityHandle& p_entity)
    {
        const auto it = std::ranges::find_if(m_scripts, [&p_entity](const ScriptHandle& p_other)
        {
            return p_other.second.GetEntity() == p_entity.GetEntity() && p_other.second.GetScene() == p_entity.GetScene();
        });

        if (it != m_scripts.end())
        {
            if (LuaScriptComponent* scriptComponent = p_entity.Get<LuaScriptComponent>())
                TryCall(*scriptComponent, ScriptingFunctions::DESTROY);

            m_scripts.erase(it);
        }
    }

    void LuaContext::Start()
    {
        if (!ASSUME(!m_hasStarted, "Attempted to recall start function on started lua context."))
            return;

        for (auto entity : m_scripts | std::views::values)
        {
            LuaScriptComponent& script = *entity.Get<LuaScriptComponent>();
            TryCall(script, ScriptingFunctions::START);
        }

        m_hasStarted = true;
    }

    void LuaContext::Update(const float p_deltaTime)
    {
        for (auto entity : m_scripts | std::views::values)
        {
            LuaScriptComponent& script = *entity.Get<LuaScriptComponent>();
            TryCall(script, ScriptingFunctions::UPDATE, p_deltaTime);
        }
    }

    void LuaContext::Stop()
    {
        if (!ASSUME(m_hasStarted, "Attempted to stop a non started lua context"))
            return;

        for (auto entity : m_scripts | std::views::values)
        {
            if (LuaScriptComponent* script = entity.Get<LuaScriptComponent>())
                TryCall(*script, ScriptingFunctions::STOP);
        }

        m_hasStarted = false;
    }

    int LuaContext::LoadModule(lua_State* p_state)
    {
        static constexpr const char* formats[] = { "%s.lua", "%s.lc", "scripts/%s", "scripts/%s.lua", "scripts/%s.lc" };

        const std::string module       = sol::stack::get<std::string>(p_state, 1);
        const size_t      moduleLength = module.length();

        if (moduleLength == 0)
            return 1;

        for (const auto& format : formats)
        {
            std::string path(FormatString(format, module.c_str()));

            ResourceManager&  resourceManager = ResourceManager::GetInstance();
            std::vector<char> script          = resourceManager.ReadFile(path);

            if (script.empty())
            {
                path   = FormatString(format, Replace(module, ".", "/").c_str());
                script = resourceManager.ReadFile(path);
            }

            if (!script.empty() && luaL_loadbuffer(p_state, script.data(), script.size(), path.c_str()) == LUA_OK)
            {
                sol::stack::push(p_state, module.c_str());
                return 2;
            }
        }

        return 1;
    }
}
