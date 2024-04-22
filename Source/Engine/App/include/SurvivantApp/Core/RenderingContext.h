//RenderingContext.h
#pragma once

#include "SurvivantApp/Core/MainCamera.h"
#include "SurvivantCore/ECS/Entity.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantRendering/RHI/IShaderStorageBuffer.h"
#include "SurvivantRendering/RHI/IFrameBuffer.h"

#include "Vector/Vector2.h"
#include "Transform.h"

#include <string>
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

		void Render(Scene& p_scene);
		intptr_t GetTextureId(ETextureType p_renderType);
		SvCore::ECS::Entity GetEntityIdValue(const Vec2& p_uv);

		/// <summary>
		/// Adds coresponding framebuffer, render type and attached texture(s)
		/// </summary>
		/// <param name="p_renderType">Type of render pass</param>
		void AddRenderPass(ERenderType p_renderType);

		CameraInfo		GetCameraInfo();
		

		Vec2&		CameraMoveInput();
		Vec2&		CameraRotateInput();
		void		UpdateCameraInput();
		void		Resize(const Vec2& p_size);

		static inline SvCore::ECS::Entity s_editorSelectedEntity = SvCore::ECS::NULL_ENTITY;

	private:
		void GameRender(Scene& p_scene);
		void SceneRender(Scene& p_scene);
		void IdRender(Scene& p_scene);

		void AddDefaultRenderPass();
		void AddIdRenderPass();

		TexturePtr CreateTexture(const ETextureType& p_type);

		LibMath::TVector2<int>		m_viewport = LibMath::Vector2(800, 600);
		MainCamera					m_mainCamera;
		FrameBufferArray			m_frameBuffers;
		std::vector<ERenderType>	m_renderTypes;

		DefaultTextureArray			m_frameTextures;
		std::vector<ETextureType>	m_textureTypeBuffer;
	};
}