//RuntimeApp.h
#pragma once

#include "SurvivantRuntime/RuntimeEngine.h"

#include <SurvivantApp/Core/IApp.h>
#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Windows/Window.h>
#include <SurvivantRendering/RHI/IRenderAPI.h>

namespace SvRuntime
{
    class RuntimeApp : public SvApp::Core::IApp
    {
    public:
        RuntimeApp();
        ~RuntimeApp() override;

        // Inherited via IApp
        void Init() override;
        void Run() override;

    private:
        std::unique_ptr<SvApp::Window> m_window;
        std::unique_ptr<RuntimeEngine> m_runtimeEngine;
        bool                           m_gameIsPaused;
    };
}
