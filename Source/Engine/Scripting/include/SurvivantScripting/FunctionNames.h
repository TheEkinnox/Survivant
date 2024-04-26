#pragma once

namespace SvScripting
{
    struct ScriptingFunctions
    {
        static constexpr const char* INIT    = "OnInit";
        static constexpr const char* START   = "OnStart";
        static constexpr const char* UPDATE  = "OnUpdate";
        static constexpr const char* STOP    = "OnStop";
        static constexpr const char* DESTROY = "OnDestroy";
    };
}
