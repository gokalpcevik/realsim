#pragma once
#include <DirectXMath.h>
#include <Eigen/Eigen>
#include <iostream>

#include "realsim/core/Input.h"
#include "realsim/core/Logger.h"

#include "realsim/ecs/Entity.h"
#include "realsim/ecs/CameraComponent.h"

namespace RSim::Editor
{
	class CameraController
	{
	public:
		CameraController(DirectX::XMFLOAT4 const& InitialCameraPosition = {-5.0f, 10.0f, -10.0f, 1.0f});

		void Update(float dt, ECS::PerspectiveCameraComponent* pCamera);
		void SetEnabled(bool Enabled);
		void SetStrafeSpeed(float Speed) { m_NormalStrafeSpeed = Speed; }
		void SetFastStrafeSpeed(float Speed) { m_FastStrafeSpeed = Speed; }
		void SetSensitivity(float Sensitivity) { m_Sensitivity = Sensitivity; }
		static float NormalizePitch(float Pitch);
		static float NormalizeYaw(float Yaw);
	private:
		void HandleStrafing(DirectX::XMVECTOR const& CameraForward,float dt);
		void HandleCameraSpeed();
	private:
		float m_NormalStrafeSpeed = 1200.0f;
		float m_FastStrafeSpeed = 1800.0f;
		float m_StrafeSpeed = m_NormalStrafeSpeed;
		float m_Sensitivity = 30.0f;
		bool m_Enabled = false;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;

		DirectX::XMVECTOR m_CameraPosition{};
		Eigen::Vector2i m_PrevCursorPosition{ 0.0f, 0.0f };
	};

}