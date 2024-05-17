#pragma once
#include "SurvivantRendering/Core/Color.h"
#include "SurvivantRendering/Core/Layer.h"
#include "SurvivantRendering/Resources/Material.h"
#include "SurvivantRendering/Resources/Model.h"
#include "SurvivantRendering/RHI/IShaderStorageBuffer.h"
#include "SurvivantRendering/RHI/IUniformBuffer.h"

#include <SurvivantCore/ECS/SceneView.h>

#include <Transform.h>

#include <Geometry/Frustum.h>

namespace SvRendering::Components
{
    class CameraComponent;
    struct ModelComponent;
}

namespace SvRendering::RHI
{
    class IFrameBuffer;
}

namespace SvRendering::Core
{
    class Renderer final
    {
    public:
        static constexpr int CAMERA_UBO_INDEX = 0;
        static constexpr int MODEL_UBO_INDEX  = 1;
        static constexpr int LIGHT_SSBO_INDEX = 0;

        struct RenderInfo;

        struct DrawInfo
        {
            SvCore::Resources::ResourceRef<Resources::Model>    m_model;
            SvCore::Resources::ResourceRef<Resources::Material> m_material;

            LibMath::Matrix4    m_modelMat;
            LayerMask           m_layerMask;
            SvCore::ECS::Entity m_entity;
            RenderInfo*         m_renderPass;
        };

        struct RenderInfo
        {
            struct Camera
            {
                Components::CameraComponent* m_camera    = nullptr;
                const LibMath::Transform*    m_transform = nullptr;

                operator bool() const
                {
                    return m_camera;
                }
            };

            Camera m_camera;
            float  m_aspect;

            SvCore::ECS::Scene* m_scene  = nullptr;
            RHI::IFrameBuffer*  m_target = nullptr;

            RHI::IShader*  m_shaderOverride                           = nullptr;
            const Color*   m_clearColorOverride                       = nullptr;
            const uint8_t* m_clearFlagsOverride                       = nullptr;
            void (*        m_onBeforeDraw)(const DrawInfo& p_payload) = nullptr;
            void (*        m_onAfterDraw)(const DrawInfo& p_payload)  = nullptr;

            // DO NOT CHANGE - FOR INTERNAL USE ONLY
            SvCore::Resources::ResourceRef<Resources::Material>                                 m_material;
            const LibMath::Frustum*                                                             m_frustum;
            SvCore::ECS::SceneView<const Components::ModelComponent, const LibMath::Transform>* m_drawables;
            LayerMask                                                                           m_cullingMask;
        };

        /**
         * \brief Creates a default renderer
         */
        Renderer();

        /**
         * \brief Disable renderer copy
         */
        Renderer(const Renderer& p_other) = delete;

        /**
         * \brief Creates a move copy of the given renderer
         * \param p_other The renderer to move
         */
        Renderer(Renderer&& p_other) noexcept = default;

        /**
         * \brief Destroys the renderer
         */
        ~Renderer() = default;

        /**
         * \brief Disable renderer copy
         */
        Renderer& operator=(const Renderer& p_other) = delete;

        /**
         * \brief Moves the given renderer into this one
         * \param p_other The renderer to move
         * \return A reference to the modified renderer
         */
        Renderer& operator=(Renderer&& p_other) noexcept = default;

        /**
         * \brief Creates a shader storage buffer object and initializes it with the given scene's lighting information
         * \param p_scene The scene for which the light ssbo should be created
         * \return An ssbo initialized with the given scene's light information
         */
        static std::unique_ptr<RHI::IShaderStorageBuffer> SetupLightSSBO(const SvCore::ECS::Scene* p_scene);

        /**
         * \brief Updates the given shader storage buffer object with the given scene's lighting information
         * \param p_scene The target scene
         * \param p_lightsSSBO The target light ssbo
         */
        static void UpdateLightSSBO(const SvCore::ECS::Scene* p_scene, RHI::IShaderStorageBuffer& p_lightsSSBO);

        /**
         * \brief Renders the given render pass on the screen
         * \param p_renderPass The current render pass' information
         */
        void Render(RenderInfo& p_renderPass) const;

    private:
        struct CameraUBO
        {
            LibMath::Matrix4 m_viewProjection;
            LibMath::Vector3 m_viewPos;
        };

        struct ModelUBO
        {
            LibMath::Matrix4x4 m_modelMat;
            LibMath::Matrix4x4 m_normalMat;
        };

        std::unique_ptr<RHI::IUniformBuffer> m_cameraBuffer;
        std::unique_ptr<RHI::IUniformBuffer> m_modelBuffer;

        /**
         * \brief Draws the render pass' models from the current camera
         * \param p_renderPass The current render pass' information
         */
        void UpdateCamera(RenderInfo& p_renderPass) const;

        /**
         * \brief Renders the given drawables
         * \param p_renderPass The current render pass' information
         */
        void DrawModels(RenderInfo& p_renderPass) const;
    };
}
