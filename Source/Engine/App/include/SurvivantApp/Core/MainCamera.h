//MainCamera.h
#pragma once

#include <SurvivantCore/ECS/EntityHandle.h>

#include <SurvivantRendering/Components/CameraComponent.h>

#include <Transform.h>
#include <Vector/Vector2.h>

namespace SvApp::Core
{
	//forward declaration
	class GameInstance;

	class MainCamera
	{
	public:
		using Cam = SvRendering::Components::CameraComponent;
		using CamInfo = std::pair<Cam*, LibMath::Transform*>;
		using Vec2 = LibMath::Vector2;

		MainCamera(const Cam& p_cam, const LibMath::Transform& p_trans);
		MainCamera(SvCore::ECS::EntityHandle p_entity);
		~MainCamera() = default;

		void	SetCamera(const Cam& p_cam, const LibMath::Transform& p_trans);
		void	SetEntity(SvCore::ECS::EntityHandle p_entity);
		CamInfo GetCamInfo();

		void	UpdateInput();

		Vec2* MoveInput();
		Vec2* RotateInput();

	private:
		static LibMath::Transform MoveTransformInput(
			const LibMath::Transform& p_trans,
			const Vec2& p_move, const Vec2& p_rotation, float p_dt);

		struct MyCam
		{
			SvCore::ECS::EntityHandle m_entity;

			Cam					m_cam;
			LibMath::Transform	m_trans;
		}m_myCam;

		bool m_hasEntity = false;

		Vec2 m_moveInput = Vec2::zero();
		Vec2 m_rotateInput = Vec2::zero();
	};
}
