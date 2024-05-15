//WorldContext.h
#pragma once

#include "SurvivantApp/Core/RenderingContext.h"
#include "SurvivantApp/Inputs/InputManager.h"
#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantCore/Resources/ResourceRef.h"
#include "SurvivantRendering/RHI/IShaderStorageBuffer.h"
#include "SurvivantRendering/RHI/IFrameBuffer.h"
#include "SurvivantRendering/Components/CameraComponent.h"	

#include "Vector/Vector2.h"
#include "Transform.h"

#include <string>
#include <memory>
#include <vector>

namespace SvApp::Core
{
	//foward declaration
	class GameInstance;

	struct WorldContext
	{
		using FrameBufferArray = std::vector<std::unique_ptr<SvRendering::RHI::IFrameBuffer>>;
		using DefaultTextureArray = std::vector<std::shared_ptr<SvRendering::RHI::ITexture>>;

		using WorldCreator = std::function<std::shared_ptr<WorldContext>(const LibMath::Vector2I&)>;
		using SceneRef = SvCore::Resources::ResourceRef<SvCore::ECS::Scene>;

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
		//void Update();
		void Save();
		void BakeLighting();

		SvCore::ECS::EntityHandle	GetFirstCamera();
		void						SetCamera(
			const SvRendering::Components::CameraComponent& p_cam, const LibMath::Transform& p_trans);
		void						SetCamera(const SvCore::ECS::EntityHandle& p_camera);

		void						SetInputs();
		SceneRef&					CurrentScene();
		std::weak_ptr<SceneRef>		CurrentSceneRef();


		//TODO: deal with persistentLevel
		//std::shared_ptr<Scene>				m_persistentLevel = nullptr;

		EWorldType				m_worldType = EWorldType::NONE;
		GameInstance*			m_owningGameInstance = nullptr;

		std::shared_ptr<InputManager::InputBindings>				m_inputs;
		std::shared_ptr<RenderingContext>							m_renderingContext;
		bool														m_isVisalbe;
		std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer>		m_lightsSSBO = nullptr;

		std::shared_ptr<SceneRef>	m_currentSceneRef = std::make_shared<SceneRef>(SceneRef());
	};
}