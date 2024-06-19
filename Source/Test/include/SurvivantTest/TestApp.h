#pragma once
#include <SurvivantApp/Core/IApp.h>
#include <SurvivantApp/Windows/Window.h>

#include <SurvivantCore/ECS/Scene.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Core/Renderer.h>
#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>

#include <memory>

#include <Geometry/Frustum.h>

#include <Vector/Vector2.h>

namespace SvRendering::Resources
{
    class Model;
}

namespace SvTest
{
    class TestApp final : public SvApp::Core::IApp
    {
    public:
        TestApp();
        ~TestApp() override;

        void Init() override;
        void Run() override;

    private:
        std::unique_ptr<SvApp::Window>                     m_window;
        SvCore::Resources::ResourceRef<SvCore::ECS::Scene> m_scene;

        SvCore::Utility::Timer m_timer;

        LibMath::Vector2  m_moveInput, m_rotateInput;
        LibMath::Vector2I m_windowSize;

        std::unique_ptr<SvRendering::Core::Renderer>            m_renderer;
        std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer> m_lightsSSBO;

        SvApp::Window::OnFrameBufferSize::ListenerId m_resizeListenerId;

        void SetupInput();
        void MakeScene();
        void DrawScene() const;
    };
}
