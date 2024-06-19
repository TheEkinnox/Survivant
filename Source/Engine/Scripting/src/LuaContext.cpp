#include "SurvivantScripting/LuaContext.h"

#include "SurvivantScripting/FunctionNames.h"
#include "SurvivantScripting/Bindings/LuaECSBinder.h"
#include "SurvivantScripting/Bindings/LuaInputBinder.h"
#include "SurvivantScripting/Bindings/LuaMathBinder.h"
#include "SurvivantScripting/Bindings/LuaPhysicsBinder.h"
#include "SurvivantScripting/Bindings/LuaRenderingBinder.h"
#include "SurvivantScripting/Bindings/LuaResourceBinder.h"
#include "SurvivantScripting/Bindings/LuaUtilityBinder.h"
#include "SurvivantScripting/Bindings/LuaAudioBinder.h"

#include <SurvivantCore/Resources/ResourceManager.h>
#include <SurvivantCore/Utility/FileSystem.h>

#include <SurvivantPhysics/PhysicsContext.h>

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Utility;

using namespace SvPhysics;

namespace SvScripting
{
    LuaContext::LuaContext()
        : m_userTypeBinders(&DefaultUserTypeBindings), m_collisionListenerId(0), m_triggerListenerId(0),
        m_isValid(false), m_hasStarted(false)
    {
    }

    LuaContext::~LuaContext()
    {
        if (m_hasStarted)
            Stop();

        Reset();
    }

    void LuaContext::Init()
    {
        if (ASSUME_FALSE(m_state, "Attempted to initialize an already initialized lua context."))
            return;

        m_state = std::make_unique<sol::state>();
        m_state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math);
        m_state->add_package_loader(&LoadModule);
        m_isValid = true;

        if (m_userTypeBinders)
            m_userTypeBinders(*m_state);

        LinkPhysicsEvents();
    }

    void LuaContext::Clear()
    {
        for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
        {
            if (i > m_scripts.size())
                continue;

            m_isValid = TryCall(m_scripts[i - 1].m_table, ScriptingFunctions::DESTROY) != ELuaCallResult::FAILURE;
            ASSERT(m_isValid);

            m_scripts[i - 1].m_table = sol::nil;
        }

        m_scripts.clear();
    }

    void LuaContext::Reset()
    {
        UnlinkPhysicsEvents();

        Clear();

        if (m_state)
            m_state.reset();

        m_hasStarted = false;
        m_isValid    = false;
    }

    void LuaContext::Reload()
    {
        Reset();
        Init();
    }

    bool LuaContext::IsValid() const
    {
        return m_isValid;
    }

    bool LuaContext::RegisterScript(LuaScriptHandle& p_handle)
    {
        if (!CHECK(IsValid(), "Attempted to register script \"%s\" to invalid lua context", p_handle.m_script.GetPath().c_str()))
            return false;

        if (!CHECK(p_handle.m_script, "Attempted to register unloaded script \"%s\"", p_handle.m_script.GetPath().c_str()))
            return false;

        const std::string_view source = p_handle.m_script->GetSource();

        if (source.empty())
            return false;

        const auto result = m_state->safe_script(source, &sol::script_pass_on_error, p_handle.m_script.GetPath());

        if (!result.valid())
        {
            [[maybe_unused]] const sol::error err = result;
            CHECK(false, "Failed to register script \"%s\" - %s", p_handle.m_script.GetPath().c_str(), err.what());
            return false;
        }

        if (!CHECK(result.return_count() == 1 && result[0].is<sol::table>(),
                "Failed to register script \"%s\" - Invalid return", p_handle.m_script.GetPath().c_str()))
            return false;

        sol::table resultTable = result[0].as<sol::table>();

        if (p_handle.m_table != sol::nil)
        {
            for (auto& [key, value] : p_handle.m_table)
            {
                if (resultTable[key].valid())
                    resultTable[key] = value;
            }
        }

        resultTable["owner"] = static_cast<const EntityHandle&>(p_handle.m_owner);

        if (TryCall(resultTable, ScriptingFunctions::INIT) == ELuaCallResult::FAILURE)
            return (m_isValid = false);

        if (m_hasStarted && TryCall(resultTable, ScriptingFunctions::START) == ELuaCallResult::FAILURE)
            return (m_isValid = false);

        p_handle.m_table = resultTable;
        return m_isValid;
    }

    LuaScriptHandle LuaContext::AddScript(const std::string& p_script, const EntityHandle& p_owner, const sol::table& p_hint)
    {
        const ResourceRef<LuaScript> scriptRef(GetModulePath(p_script));

        LuaScriptHandle handle = { GetModuleName(p_script), scriptRef, p_owner, p_hint };

        // Check for handle validity since scripts might be removed on init
        if (!RegisterScript(handle) || !handle.m_table.valid())
            return {};

        const auto insertIt = std::ranges::find_if_not(m_scripts, [&handle](const LuaScriptHandle& p_other)
        {
            return handle < p_other;
        });

        m_isValid = CHECK(m_scripts.insert(insertIt, handle) != m_scripts.end(), "Failed to add script \"%s\"", p_script.c_str());

        return m_isValid ? handle : LuaScriptHandle{};
    }

    LuaScriptHandle LuaContext::GetScript(const std::string& p_script, const EntityHandle& p_owner) const
    {
        const auto& moduleName = GetModuleName(p_script);
        const auto& modulePath = GetModulePath(p_script);

        const auto it = std::ranges::find_if(m_scripts, [&moduleName, &modulePath, &p_owner](const LuaScriptHandle& p_other)
        {
            return (p_other.m_name == moduleName || p_other.m_script.GetPath() == modulePath)
                && p_other.m_owner.GetEntity() == p_owner.GetEntity()
                && p_other.m_owner.GetScene() == p_owner.GetScene();
        });

        return it != m_scripts.end() ? *it : LuaScriptHandle{};
    }

    void LuaContext::RemoveScript(const std::string& p_script, EntityHandle& p_owner)
    {
        const auto& moduleName = GetModuleName(p_script);
        const auto& modulePath = GetModulePath(p_script);

        const auto it = std::ranges::find_if(m_scripts, [&moduleName, &modulePath, &p_owner](const LuaScriptHandle& p_other)
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

        p_module = ResourceManager::GetInstance().GetRelativePath(GetModulePath(p_module, true));

        for (const auto& extension : EXTENSIONS)
        {
            if (p_module.ends_with(extension))
            {
                p_module = p_module.substr(0, p_module.size() - strlen(extension));
                break;
            }
        }

        ReplaceInPlace(p_module, "/", ".");
        ReplaceInPlace(p_module, "\\", ".");

        return (s_moduleNames[base] = p_module);
    }

    std::string LuaContext::GetModulePath(std::string p_module, const bool p_fromGetName)
    {
        static std::string empty;

        if (p_module.empty())
            return empty;

        const auto it = s_modulePaths.find(p_module);

        const ResourceManager& resourceManager = ResourceManager::GetInstance();

        if (it != s_modulePaths.end() && PathExists(it->second))
            return resourceManager.GetFullPath(it->second);

        const std::string base = p_module;

        p_module = resourceManager.GetFullPath(p_module);

        if (PathExists(p_module))
        {
            s_modulePaths[base] = resourceManager.GetRelativePath(p_module);
            return p_module;
        }

        p_module = Replace(p_fromGetName ? p_module : GetModuleName(p_module), ".", "/");

        for (const auto& extension : EXTENSIONS)
        {
            std::string path(resourceManager.GetFullPath(p_module + extension));

            if (PathExists(path))
            {
                s_modulePaths[base] = resourceManager.GetRelativePath(path);
                return path;
            }
        }

        return empty;
    }

    void LuaContext::SetUserTypeBinders(const Binder p_binder)
    {
        m_userTypeBinders = p_binder;
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

    void LuaContext::DefaultUserTypeBindings(sol::state& p_luaState)
    {
        Bindings::LuaECSBinder::Bind(p_luaState);
        Bindings::LuaMathBinder::Bind(p_luaState);
        Bindings::LuaInputBinder::Bind(p_luaState);
        Bindings::LuaUtilityBinder::Bind(p_luaState);
        Bindings::LuaRenderingBinder::Bind(p_luaState);
        Bindings::LuaResourceBinder::Bind(p_luaState);
        Bindings::LuaPhysicsBinder::Bind(p_luaState);
        Bindings::LuaAudioBinder::Bind(p_luaState);
    }

    void LuaContext::LinkPhysicsEvents()
    {
        PhysicsContext& context = PhysicsContext::GetInstance();

        m_collisionListenerId = context.m_onCollision.AddListener(
            [this](const EPhysicsEvent p_event, const CollisionInfo& p_info)
            {
                if (!m_isValid)
                    return;

                ASSERT(p_info.m_colliders[0]);
                ASSERT(p_info.m_colliders[1]);

                for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
                {
                    if (i > m_scripts.size())
                        continue;

                    LuaScriptHandle& handle = m_scripts[i - 1];

                    if (!handle ||
                        (handle.m_owner != p_info.m_colliders[0].m_owner && handle.m_owner != p_info.m_colliders[1].m_owner))
                        continue;

                    switch (p_event)
                    {
                    case EPhysicsEvent::ENTER:
                        m_isValid = TryCall(handle.m_table, ScriptingFunctions::COLLISION_ENTER, p_info) != ELuaCallResult::FAILURE;
                        break;
                    case EPhysicsEvent::STAY:
                        m_isValid = TryCall(handle.m_table, ScriptingFunctions::COLLISION_STAY, p_info) != ELuaCallResult::FAILURE;
                        break;
                    case EPhysicsEvent::EXIT:
                        m_isValid = TryCall(handle.m_table, ScriptingFunctions::COLLISION_EXIT, p_info) != ELuaCallResult::FAILURE;
                        break;
                    case EPhysicsEvent::NONE:
                    default:
                        ASSERT(false, "Invalid collision event type");
                        return;
                    }
                }
            });

        m_triggerListenerId = context.m_onTrigger.AddListener(
            [this](const EPhysicsEvent p_event, const TriggerInfo& p_info)
            {
                if (!m_isValid)
                    return;

                ASSERT(p_info.m_triggerCollider);
                ASSERT(p_info.m_otherCollider);

                for (size_t i = m_scripts.size(); i > 0 && m_isValid; --i)
                {
                    if (i > m_scripts.size())
                        continue;

                    LuaScriptHandle& handle = m_scripts[i - 1];

                    if (!handle || handle.m_owner != p_info.m_triggerCollider.m_owner)
                        continue;

                    switch (p_event)
                    {
                    case EPhysicsEvent::ENTER:
                        m_isValid = TryCall(handle.m_table, ScriptingFunctions::TRIGGER_ENTER, p_info) != ELuaCallResult::FAILURE;
                        break;
                    case EPhysicsEvent::EXIT:
                        m_isValid = TryCall(handle.m_table, ScriptingFunctions::TRIGGER_EXIT, p_info) != ELuaCallResult::FAILURE;
                        break;
                    case EPhysicsEvent::NONE:
                    case EPhysicsEvent::STAY:
                    default:
                        ASSERT(false, "Invalid trigger event type");
                        return;
                    }
                }
            });
    }

    void LuaContext::UnlinkPhysicsEvents()
    {
        PhysicsContext& context = PhysicsContext::GetInstance();

        context.m_onCollision.RemoveListener(m_collisionListenerId);
        m_collisionListenerId = 0;

        context.m_onTrigger.RemoveListener(m_triggerListenerId);
        m_triggerListenerId = 0;
    }
}
