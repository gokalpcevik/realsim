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
		CameraController(DirectX::XMFLOAT4 const& InitialCameraPosition = {0.0f,0.0f,-5.0f,1.0f});

		void Update(float dt, ECS::PerspectiveCameraComponent* pCamera);
		void SetEnabled(bool Enabled);
		void SetNearZ(float NearZ) { m_NearZ = NearZ; }
		void SetFarZ(float FarZ) { m_FarZ = FarZ; }
		void SetStrafeSpeed(float Speed) { m_NormalStrafeSpeed = Speed; }
		void SetFastStrafeSpeed(float Speed) { m_FastStrafeSpeed = Speed; }
		void SetFoVAngle(float FoV) { m_FOVHalfAngle = FoV / 2.0f; }
		void SetSensitivity(float Sensitivity) { m_Sensitivity = Sensitivity; }
		static float NormalizePitch(float Pitch);
		static float NormalizeYaw(float Yaw);
	private:
		void HandleStrafing(DirectX::XMVECTOR const& CameraForward,float dt);
		void HandleZoom(float dt);
		void HandleCameraSpeed();
		DirectX::XMMATRIX LookAround(float dt);
	private:
		float m_NearZ = 0.1f;
		float m_FarZ = 100.0f;
		float m_NormalStrafeSpeed = 400.0f;
		float m_FastStrafeSpeed = 700.0f;
		float m_StrafeSpeed = m_NormalStrafeSpeed;
		float m_FOVHalfAngle = 45.0f;
		float m_MaxFOV = m_FOVHalfAngle;
		float m_MinFOV = 22.5f;
		float m_Sensitivity = 16.0f;
		bool m_Enabled = false;

		float m_Yaw = 0.0f;
		float m_Pitch = 0.0f;

		DirectX::XMVECTOR m_CameraPosition{};
		Eigen::Vector2i m_PrevCursorPosition{ 0.0f, 0.0f };
	};

}