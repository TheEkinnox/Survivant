//Engine.h
#pragma once

//#include "SurvivantEditor/App/GameInstance.h"

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

namespace SvEditor::App
{
	//foward declaration
	class GameInstance;

	struct WorldContext
	{
		using FrameBufferArray = std::vector<std::unique_ptr<SvRendering::RHI::IFrameBuffer>>;
		using DefaultTextureArray = std::vector<std::shared_ptr<SvRendering::RHI::ITexture>>;

		using WorldCreator = std::function<std::shared_ptr<WorldContext>(const LibMath::Vector2I&)>;

		enum class EWorldType
		{
			NONE,
			PIE,
			EDITOR,
			GAME
		};

		enum class ERenderType
		{
			DEFAULT,
			ID
		};

		void BeginPlay();
		void Update();
		void Render();

		intptr_t GetDefaultTextureId();

		void SetSceneCamera(const SvCore::ECS::EntityHandle& p_entity);
		void SetSceneCamera();

		void InitCamera();

		/// <summary>
		/// Adds coresponding framebuffer, render type and attached texture(s)
		/// </summary>
		/// <param name="p_renderType">Type of render pass</param>
		void AddRenderPass(ERenderType p_renderType);

		//TODO: deal with persistentLevel
		//std::shared_ptr<Scene>				m_persistentLevel = nullptr;


		EWorldType				m_worldType = EWorldType::NONE;
		GameInstance*			m_owningGameInstance = nullptr;
		LibMath::TVector2<int>	m_viewport = LibMath::Vector2(800, 600);

		std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer>		m_lightsSSBO = nullptr;
		std::shared_ptr<SvCore::ECS::Scene>							m_currentScene = nullptr;

		SvCore::Events::Event<>		m_onGainFocus;
		SvCore::Events::Event<>		m_onLoseFocus;

	private:
		void DefaultRender();
		void IdPassRender();

		void AddDefaultRenderPass();
		void AddIdRenderPass();

		SvCore::ECS::EntityHandle	m_cameraHandle;
		SvRendering::Core::Camera	m_cameraCam;
		LibMath::Transform			m_cameraTrans;

		FrameBufferArray			m_frameBuffers;
		std::vector<ERenderType>	m_renderTypes;
		DefaultTextureArray			m_frameTextures;
	};
}