#include "SurvivantRuntime/RuntimeApp.h"

int main()
{
    SvRuntime::RuntimeApp app;

    app.Init();
    app.Run();

	return 0;
}