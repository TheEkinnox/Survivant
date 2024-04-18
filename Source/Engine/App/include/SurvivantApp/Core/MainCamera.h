//RenderingContext.h
#pragma once

#include "SurvivantCore/ECS/EntityHandle.h"
#include "SurvivantRendering/Core/Camera.h"	

#include "Transform.h"
#include "Vector/Vector2.h"

namespace SvApp::Core
{
	//foward declaration
	class GameInstance;

	class MainCamera
	{
	public:
		using CamInfo = std::pair<SvRendering::Core::Camera*, LibMath::Transform*>;
		using Vec2 = LibMath::Vector2;

		MainCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans);
		MainCamera(SvCore::ECS::EntityHandle p_entity);
		~MainCamera() = default;

		void	SetCamera(const SvRendering::Core::Camera& p_cam, const LibMath::Transform& p_trans);
		void	SetEntity(SvCore::ECS::EntityHandle p_entity);
		CamInfo GetCamInfo();
		void	UpdateInput();

		Vec2* MoveInput();
		Vec2* RotateInput();

	private:
		LibMath::Transform MoveTransformInput(
			const LibMath::Transform& p_trans, 
			const Vec2& p_move, const Vec2& p_rotation, const float p_dt);

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

		Vec2 m_moveInput = Vec2::zero();
		Vec2 m_rotateInput = Vec2::zero();
	};
}