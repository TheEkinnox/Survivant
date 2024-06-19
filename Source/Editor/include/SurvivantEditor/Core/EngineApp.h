//EngineApp.h
#pragma once

#include "SurvivantEditor/Core/EditorEngine.h"
#include "SurvivantEditor/Core/EditorWindow.h"

#include <SurvivantApp/Core/GameInstance.h>
#include <SurvivantApp/Core/IApp.h>

namespace SvEditor::Core
{
    class EngineApp : public SvApp::Core::IApp
    {
    public:
        using GameInstance = SvApp::Core::GameInstance;

        EngineApp();
        ~EngineApp() override;

        // Inherited via IApp
        void Init() override;
        void Run() override;

    private:
        void DrawLogo() const;

        void TogglePlayPIE();
        void TogglePausePIE();
        void PressFramePIE();
        void UpdatePIE();

        std::unique_ptr<EditorWindow> m_window;
        std::unique_ptr<EditorEngine> m_editorEngine;
        std::weak_ptr<GameInstance>   m_gameInstance;
    };
}
