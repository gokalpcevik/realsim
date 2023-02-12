#pragma once
#include <DirectXMath.h>

namespace RSim::ECS
{
	struct PerspectiveCameraComponent
	{
		PerspectiveCameraComponent() = default;
		PerspectiveCameraComponent(bool primary) : IsPrimaryCamera(primary) { }

		[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const
		{
			return DirectX::XMMatrixPerspectiveFovLH(FOVHalfAngle, aspectRatio,NearZ,FarZ);
		}

		[[nodiscard]] DirectX::XMMATRIX GetViewMatrix() const
		{
			return m_ViewMatrix;
		}

		[[nodiscard]] DirectX::XMVECTOR GetCameraPosition() const
		{
			return m_InvViewMatrix.r[3];
		}

		[[nodiscard]] DirectX::XMVECTOR GetCameraDirection() const
		{
			return m_InvViewMatrix.r[2];
		}

		void SetViewMatrix(DirectX::XMMATRIX const& viewMatrix)
		{
			m_ViewMatrix = viewMatrix;
			m_InvViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);
		}

		void SetCameraParameters(float FoVHalfAngle, float nearZ, float farZ)
		{
			this->FOVHalfAngle = FoVHalfAngle;
			this->NearZ = nearZ;
			this->FarZ = farZ;
		}

		bool IsPrimaryCamera = false;
		float FOVHalfAngle = 30.0f;
		float NearZ = 0.1f;
		float FarZ = 1000.0f;
	private:
		DirectX::XMMATRIX m_ViewMatrix = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX m_InvViewMatrix = DirectX::XMMatrixIdentity();
	};
}