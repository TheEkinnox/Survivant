#if defined(_DEBUG) && defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "SurvivantRuntime/RuntimeApp.h"

int main()
{
#if defined(_DEBUG) && defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    SvRuntime::RuntimeApp app;

    app.Init();
    app.Run();

    return 0;
}
