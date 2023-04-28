#pragma once
#include <DirectXMath.h>
#include "BasicMath.hpp"
#include "AdvancedMath.hpp"

namespace RSim
{
    // Dirty hack till I implement a decent math extension library for diligent math
    Diligent::float4x4 FromDXMath(DirectX::XMMATRIX const& mat);
}

namespace RSim::ECS
{
	struct PerspectiveCameraComponent
	{
		PerspectiveCameraComponent() = default;
		PerspectiveCameraComponent(bool primary) : IsPrimaryCamera(primary) { }

		[[nodiscard]] Diligent::float4x4 GetProjectionMatrix(float aspectRatio) const
		{
            return Diligent::float4x4::Projection(DirectX::XMConvertToRadians(FOVHalfAngle),aspectRatio,NearZ,FarZ,false);
		}

		[[nodiscard]] Diligent::float4x4 GetViewMatrix() const
		{
			return m_ViewMatrix;
		}

        [[nodiscard]] Diligent::float4x4 GetInvViewMatrix() const
        {
            return m_InvViewMatrix;
        }

		[[nodiscard]] Diligent::float4 GetCameraPosition() const
		{
			return {};
		}

		[[nodiscard]] Diligent::float4 GetCameraDirection() const
		{
			return {};
		}

		void SetViewMatrix(Diligent::float4x4 const& viewMatrix)
		{
			m_ViewMatrix = viewMatrix;
			m_InvViewMatrix = viewMatrix.Inverse();
		}

		void SetCameraParameters(float FoVHalfAngle, float nearZ, float farZ)
		{
			this->FOVHalfAngle = FoVHalfAngle;
			this->NearZ = nearZ;
			this->FarZ = farZ;
		}

		bool IsPrimaryCamera = false;
		float FOVHalfAngle = 45.0f;
		float NearZ = 0.1f;
		float FarZ = 1000.0f;
	private:
        Diligent::float4x4 m_ViewMatrix;
        Diligent::float4x4 m_InvViewMatrix;
	};
}