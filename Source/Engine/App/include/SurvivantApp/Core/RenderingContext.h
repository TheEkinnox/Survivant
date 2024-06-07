//RenderingContext.h
#pragma once

#include "SurvivantApp/Core/MainCamera.h"

#include <SurvivantCore/ECS/Entity.h>
#include <SurvivantCore/ECS/Scene.h>

#include <SurvivantRendering/Core/Renderer.h>

#include <memory>
#include <vector>

namespace SvApp::Core
{
    //forward declaration
    class GameInstance;
    struct WorldContext;

    class RenderingContext
    {
    public:
        friend WorldContext;

        using FrameBufferArray = std::vector<std::unique_ptr<SvRendering::RHI::IFrameBuffer>>;
        using TexturePtr = std::shared_ptr<SvRendering::RHI::ITexture>;
        using DefaultTextureArray = std::vector<TexturePtr>;
        using CameraInfo = MainCamera::CamInfo;

        using Scene = SvCore::ECS::Scene;
        using Vec2 = LibMath::Vector2;

        enum class ERenderType
        {
            DEFAULT,
            GAME,
            SCENE,
            ID
        };

        enum class ETextureType
        {
            COLOR,
            DEPTH,
            ID
        };

        RenderingContext(const MainCamera::Cam& p_cam, const LibMath::Transform& p_trans);
        RenderingContext(SvCore::ECS::EntityHandle p_entity);
        ~RenderingContext() = default;

        void                Render(Scene* p_scene);
        void*               GetTextureId(ETextureType p_renderType);
        SvCore::ECS::Entity GetEntityIdValue(const Vec2& p_uv, Scene* p_scene);

        /// <summary>
        /// Adds corresponding framebuffer, render type and attached texture(s)
        /// </summary>
        /// <param name="p_type">Type of render pass</param>
        void AddRenderPass(ERenderType p_type);

        CameraInfo GetCameraInfo();

        Vec2& CameraMoveInput();
        Vec2& CameraRotateInput();
        void  UpdateCameraInput();
        void  Resize(const LibMath::Vector2I& p_size);
        float GetAspect() const;
        void  ResetCameraAspect();

        const SvRendering::Core::Renderer& GetRenderer() const;

        static inline SvCore::ECS::EntityHandle s_editorSelectedEntity = {};

    private:
        static constexpr const char*         EDITOR_SCENE_SHADER_PATH = "assets/shaders/EditorScene.glsl";
        static inline const LibMath::Vector4 SELECTION_TINT           = LibMath::Vector4(0.3f, 0.3f, 0.3f, 1);

        void GameRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo) const;
        void SceneRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo);
        void IdRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo);

        void       AddColorRenderPass();
        void       AddIdRenderPass();
        TexturePtr CreateTexture(const ETextureType& p_type) const;

        static std::shared_ptr<SvRendering::RHI::IShader> CreateEditorSceneShader();

        static int                 EntityToTextureValue(SvCore::ECS::Entity p_entity);
        static SvCore::ECS::Entity TextureValueToEntity(int p_value, Scene* p_scene);

        static void OnIdDraw(const SvRendering::Core::Renderer::DrawInfo& p_drawInfo);
        static void OnBeforeSceneDraw(const SvRendering::Core::Renderer::DrawInfo& p_drawInfo);
        static void OnAfterSceneDraw(const SvRendering::Core::Renderer::DrawInfo& p_drawInfo);

        SvRendering::Core::Renderer m_renderer;

        LibMath::Vector2I        m_viewport;
        MainCamera               m_mainCamera;
        FrameBufferArray         m_frameBuffers;
        std::vector<ERenderType> m_renderTypes;

        DefaultTextureArray       m_frameTextures;
        std::vector<ETextureType> m_textureTypeBuffer;
    };
}
