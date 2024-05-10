#pragma once

#ifdef _DEBUG
    #if defined(_WIN32)
        #include "SurvivantCore/Utility/LeanWindows.h"
        #include <intrin.h>

        inline bool HasDebugger()
        {
            return IsDebuggerPresent();
        }

        #define DEBUG_BREAK_IMPL() (__nop(), __debugbreak())
    #else
        inline bool HasDebugger()
        {
            return true;
        }

        #define DEBUG_BREAK_IMPL() *((int*)3) = 13
    #endif // _WIN32

    #define DEBUG_BREAK() ((void)(HasDebugger() && (DEBUG_BREAK_IMPL(), 1)))
#else
    inline bool HasDebugger()
    {
        return false;
    }

    #define DEBUG_BREAK()
#endif // _DEBUG
