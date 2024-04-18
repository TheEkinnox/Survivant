//RenderingContext.h
#pragma once

#include "SurvivantApp/Core/MainCamera.h"
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
		using DefaultTextureArray = std::vector<std::shared_ptr<SvRendering::RHI::ITexture>>;
		using CamInfo = std::pair<SvRendering::Core::Camera*, LibMath::Transform*>;

		using Scene = SvCore::ECS::Scene;
		using Vec2 = LibMath::Vector2;

		enum class ERenderType
		{
			DEFAULT,
			ID
		};

		RenderingContext(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans);
		RenderingContext(SvCore::ECS::EntityHandle p_entity);
		~RenderingContext() = default;

		void Render(Scene& p_scene);
		intptr_t GetDefaultTextureId();

		/// <summary>
		/// Adds coresponding framebuffer, render type and attached texture(s)
		/// </summary>
		/// <param name="p_renderType">Type of render pass</param>
		void AddRenderPass(ERenderType p_renderType);

		void SetIsDisplayed(bool p_isDisplayed);
		CamInfo GetCameraInfo();

		Vec2&		CameraMoveInput();
		Vec2&		CameraRotateInput();
		void		UpdateCameraInput();

	private:
		void DefaultRender(Scene& p_scene);
		void IdPassRender(Scene& p_scene);

		void AddDefaultRenderPass();
		void AddIdRenderPass();

		LibMath::TVector2<int>		m_viewport = LibMath::Vector2(800, 600);
		MainCamera					m_mainCamera;
		FrameBufferArray			m_frameBuffers;
		std::vector<ERenderType>	m_renderTypes;
		DefaultTextureArray			m_frameTextures;
	};
}