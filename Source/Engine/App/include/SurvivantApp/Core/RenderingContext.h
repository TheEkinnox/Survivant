//RenderingContext.h
#pragma once

#include "SurvivantApp/Core/MainCamera.h"

#include <SurvivantCore/ECS/Entity.h>
#include <SurvivantCore/ECS/Scene.h>

#include <SurvivantRendering/Core/Renderer.h>

#include <Transform.h>
#include <Vector/Vector2.h>

#include <memory>
#include <vector>

namespace SvApp::Core
{
	//foward declaration
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

		void Render(Scene* p_scene);
		void* GetTextureId(ETextureType p_renderType);
		SvCore::ECS::Entity GetEntityIdValue(const Vec2& p_uv, Scene* p_scene);

		/// <summary>
		/// Adds corresponding framebuffer, render type and attached texture(s)
		/// </summary>
		/// <param name="p_renderType">Type of render pass</param>
		void AddRenderPass(ERenderType p_renderType);

		CameraInfo		GetCameraInfo();

		Vec2&		CameraMoveInput();
		Vec2&		CameraRotateInput();
		void		UpdateCameraInput();
		void		Resize(const LibMath::TVector2<int>& p_size);
		float		GetAspect() const;
		void		ResetCameraAspect();
		void		SetCanMove(bool p_canMove);

		const SvRendering::Core::Renderer& GetRenderer() const;

		static inline SvCore::ECS::EntityHandle s_editorSelectedEntity = {};

	private:
		void GameRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo) const;
		void SceneRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo);
		void IdRender(SvRendering::Core::Renderer::RenderInfo& p_renderInfo);

		void		AddColorRenderPass();
		void		AddIdRenderPass();
		void		AddDefaultRenderPass();
		TexturePtr	CreateTexture(const ETextureType& p_type);

		SvRendering::Core::Renderer m_renderer;

		LibMath::TVector2<int>		m_viewport = LibMath::Vector2(800, 600);
		MainCamera					m_mainCamera;
		FrameBufferArray			m_frameBuffers;
		std::vector<ERenderType>	m_renderTypes;

		DefaultTextureArray			m_frameTextures;
		std::vector<ETextureType>	m_textureTypeBuffer;
	};
}