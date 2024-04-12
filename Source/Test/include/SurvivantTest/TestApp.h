#pragma once
#include <SurvivantApp/Core/IApp.h>
#include <SurvivantApp/Windows/Window.h>

#include <SurvivantCore/ECS/Scene.h>
#include <SurvivantCore/Utility/Timer.h>

#include <SurvivantRendering/Geometry/Frustum.h>
#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/RHI/IShaderStorageBuffer.h>

#include <memory>

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
        void Init() override;
        void Run() override;

    private:
        std::unique_ptr<SvApp::Window> m_window;
        SvCore::Utility::Timer         m_timer;
        SvCore::ECS::Scene             m_scene;
        LibMath::Vector2               m_moveInput, m_rotateInput;

        std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer> m_lightsSSBO;

        void SetupInput();
        void MakeScene();
        void UpdateLightSSBO() const;
        void UpdateTemporaries();
        void UpdateInput();
        void UpdateRotators();
        void DrawScene();

        static void BindCamUBO(const LibMath::Matrix4& p_viewProj, const LibMath::Vector3& p_viewPos);
        static void BindModelUBO(const LibMath::Matrix4& p_modelMat);

        static void DrawModel(const SvRendering::Resources::Model&    p_model,
                              const SvRendering::Geometry::Frustum&   p_viewFrustum,
                              const LibMath::Matrix4&                 p_transform,
                              const SvRendering::Resources::Material& p_material);
    };
}