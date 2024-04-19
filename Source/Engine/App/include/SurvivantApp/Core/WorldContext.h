//WorldContext.h
#pragma once

#include "SurvivantCore/ECS/Scene.h"
#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantApp/Core/RenderingContext.h"
#include "SurvivantApp/Inputs/InputManager.h"

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

	struct WorldContext
	{
		using FrameBufferArray = std::vector<std::unique_ptr<SvRendering::RHI::IFrameBuffer>>;
		using DefaultTextureArray = std::vector<std::shared_ptr<SvRendering::RHI::ITexture>>;

		using WorldCreator = std::function<std::shared_ptr<WorldContext>(const LibMath::Vector2I&)>;
		using ScenePtr = std::shared_ptr<SvCore::ECS::Scene>;

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
		void LoadCurrentScene();

		void						SetOwningCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans);
		void						SetSceneCamera(const SvCore::ECS::EntityHandle& p_entity);
		SvCore::ECS::EntityHandle	GetDefaultSceneCamera();
		void						SetInputs();
		ScenePtr&					CurrentScene();
		std::weak_ptr<ScenePtr>		CurrentSceneRef();


		//TODO: deal with persistentLevel
		//std::shared_ptr<Scene>				m_persistentLevel = nullptr;

		EWorldType				m_worldType = EWorldType::NONE;
		GameInstance*			m_owningGameInstance = nullptr;
		LibMath::TVector2<int>	m_viewport = LibMath::Vector2(800, 600);

		std::unique_ptr<SvRendering::RHI::IShaderStorageBuffer>		m_lightsSSBO = nullptr;
		std::shared_ptr<InputManager::InputBindings>				m_inputs;
		std::shared_ptr<RenderingContext>							m_renderingContext;
		bool														m_isVisalbe;

	private:
		std::shared_ptr<ScenePtr>	m_currentSceneRef = std::make_shared<ScenePtr>(nullptr);
	};
}