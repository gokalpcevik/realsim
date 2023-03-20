#include "realsim/editor/CameraController.h"

namespace RSim::Editor
{
	using namespace RSim::Core;

	namespace DX = DirectX;

	CameraController::CameraController(DirectX::XMFLOAT4 const& InitialCameraPosition)
	{
		m_CameraPosition = DX::XMLoadFloat4(&InitialCameraPosition);
	}

	void CameraController::Update(float dt, ECS::PerspectiveCameraComponent* pCamera)
	{
		if (!m_Enabled) return;
		
		int x, y = 0;
		SDL_GetRelativeMouseState(&x, &y);

		float dx = (float)x * dt * 10000.0f;
		float dy = (float)y * dt * 10000.0f;

		m_Yaw += dx * m_Sensitivity;
		m_Pitch += dy * m_Sensitivity;

		m_Yaw = NormalizeYaw(m_Yaw);
		m_Pitch = NormalizePitch(m_Pitch);
		float Pitch = DX::XMConvertToRadians(m_Pitch);
		float Yaw = DX::XMConvertToRadians(m_Yaw);
		DX::XMMATRIX Rotation = DX::XMMatrixRotationRollPitchYaw(Pitch, Yaw, 0.0f);

		DX::XMVECTOR CameraDirection = DX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		CameraDirection = XMVector4Transform(CameraDirection, Rotation);
		CameraDirection = DX::XMVector4Normalize(CameraDirection);
		HandleCameraSpeed();
		HandleStrafing(CameraDirection,dt);

		DX::XMMATRIX View = DX::XMMatrixLookToLH(m_CameraPosition,CameraDirection, DX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		pCamera->SetViewMatrix(View);
		//DX::XMMATRIX InvView = DX::XMMatrixInverse(nullptr, View);
		//DX::XMFLOAT4X4 InvViewF4;
		//DX::XMStoreFloat4x4(&InvViewF4, InvView);
		//rsim_error("******************************************************");
		//rsim_info("Camera Position:{:0.2f}|{:0.2f}|{:0.2f}", InvViewF4._41, InvViewF4._42, InvViewF4._43);
		//rsim_info("Camera Right:{:0.2f}|{:0.2f}|{:0.2f}", InvViewF4._11, InvViewF4._12, InvViewF4._13);
		//rsim_info("Camera Up:{:0.2f}|{:0.2f}|{:0.2f}", InvViewF4._21, InvViewF4._22, InvViewF4._23);
		//rsim_info("Camera Forward:{:0.2f}|{:0.2f}|{:0.2f}", InvViewF4._31, InvViewF4._32, InvViewF4._33);
	}

	void CameraController::SetEnabled(bool Enabled)
	{
		m_Enabled = Enabled;
		SDL_SetRelativeMouseMode(static_cast<SDL_bool>(Enabled));
	}

	float CameraController::NormalizePitch(float Pitch)
	{
		float Pitch_ = Pitch;
		if (Pitch_ > 89.0f)
			Pitch_ = 89.0f;
		if (Pitch_ < -89.0f)
			Pitch_ = -89.0f;
		return Pitch_;
	}

    float CameraController::NormalizeYaw(float Yaw)
    {
		float Yaw_ = Yaw;
		if(Yaw_ >= 360.0f || Yaw_ <= -360.0f) Yaw_ = 0.0f;
        return Yaw_;
    }
    void CameraController::HandleStrafing(DirectX::XMVECTOR const &CameraForward, float dt)
    {
		if (Input::IsKeyPressed(SDL_SCANCODE_W))
		{
			m_CameraPosition = DX::XMVectorAdd(m_CameraPosition, DX::XMVectorScale(CameraForward, dt * m_StrafeSpeed * 1000.0f));
		}
		else if (Input::IsKeyPressed(SDL_SCANCODE_S))
		{
			m_CameraPosition = DX::XMVectorAdd(m_CameraPosition, DX::XMVectorScale(DX::XMVectorNegate(CameraForward), dt * m_StrafeSpeed * 1000.0f));
		}

		DirectX::XMVECTOR CameraRight = DX::XMVector3Cross(DX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),CameraForward);
		CameraRight = DX::XMVector4Normalize(CameraRight);

		if (Input::IsKeyPressed(SDL_SCANCODE_D))
		{
			m_CameraPosition = DX::XMVectorAdd(m_CameraPosition, DX::XMVectorScale(CameraRight, dt * m_StrafeSpeed * 1000.0f));
		}
		else if (Input::IsKeyPressed(SDL_SCANCODE_A))
		{
			m_CameraPosition = DX::XMVectorAdd(m_CameraPosition, DX::XMVectorScale(DX::XMVectorNegate(CameraRight), dt * m_StrafeSpeed * 1000.0f));
		}
	}



    void CameraController::HandleCameraSpeed()
    {
		if(Input::IsKeyPressed(SDL_SCANCODE_LSHIFT))
		{
			m_StrafeSpeed = m_FastStrafeSpeed;
		}
		else
		{
			m_StrafeSpeed = m_NormalStrafeSpeed;
		}
    }
}
