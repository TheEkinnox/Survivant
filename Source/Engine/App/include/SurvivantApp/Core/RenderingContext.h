//RenderingContext.h
#pragma once

#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/EntityHandle.h"

#include "SurvivantRendering/RHI/IShaderStorageBuffer.h"
#include "SurvivantRendering/RHI/IFrameBuffer.h"
#include "SurvivantRendering/Core/Camera.h"	

#include "Vector/Vector2.h"
#include "Transform.h"

#include <string>
#include <memory>
#include <vector>

namespace SvApp::Core
{
	//foward declaration
	class GameInstance;

	class RenderingContext
	{
		using FrameBufferArray = std::vector<std::unique_ptr<SvRendering::RHI::IFrameBuffer>>;
		using DefaultTextureArray = std::vector<std::shared_ptr<SvRendering::RHI::ITexture>>;

		using Scene = SvCore::ECS::Scene;

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

	private:
		class MainCamera
		{
		public:
			using CamInfo = std::pair<SvRendering::Core::Camera*, LibMath::Transform*>;

			MainCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans);
			MainCamera(SvCore::ECS::EntityHandle p_entity);
			~MainCamera() = default;
			
			CamInfo GetCamInfo();
			void	SetCamera(const SvRendering::Core::Camera&	p_cam, const LibMath::Transform& p_trans);
			void	SetEntity(SvCore::ECS::EntityHandle p_entity);

		private:
			union UnionCam
			{
				UnionCam(
					const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans) : 
					m_camInfo({ p_cam , p_trans }) {};
				UnionCam(SvCore::ECS::EntityHandle p_entity) : 
					m_entity(p_entity) {};
				~UnionCam() {};

				SvCore::ECS::EntityHandle m_entity;
				struct
				{
					SvRendering::Core::Camera	m_cam;
					LibMath::Transform			m_trans;
				}m_camInfo;
			}m_union;

			bool m_hasEntity = false;
		};

		void DefaultRender(Scene& p_scene);
		void IdPassRender(Scene& p_scene);

		void AddDefaultRenderPass();
		void AddIdRenderPass();

		LibMath::TVector2<int>		m_viewport = LibMath::Vector2(1, 1);
		MainCamera					m_mainCamera;
		FrameBufferArray			m_frameBuffers;
		std::vector<ERenderType>	m_renderTypes;
		DefaultTextureArray			m_frameTextures;
		bool						m_isDisplayed;
	};
}