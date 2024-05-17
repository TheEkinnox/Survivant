#pragma once

namespace SvScripting
{
    struct ScriptingFunctions
    {
        static constexpr const char* INIT            = "OnInit";
        static constexpr const char* START           = "OnStart";
        static constexpr const char* UPDATE          = "OnUpdate";
        static constexpr const char* COLLISION_ENTER = "OnCollisionEnter";
        static constexpr const char* COLLISION_STAY  = "OnCollisionStay";
        static constexpr const char* COLLISION_EXIT  = "OnCollisionExit";
        static constexpr const char* TRIGGER_ENTER   = "OnTriggerEnter";
        static constexpr const char* TRIGGER_EXIT    = "OnTriggerExit";
        static constexpr const char* STOP            = "OnStop";
        static constexpr const char* DESTROY         = "OnDestroy";
    };
}
