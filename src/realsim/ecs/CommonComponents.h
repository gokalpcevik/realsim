#pragma once

#include <DirectXMath.h>
#include <utility>
#include <string>
#include <string_view>
#include <Eigen/Eigen>

#include "BasicMath.hpp"
#include "AdvancedMath.hpp"

namespace RSim::ECS
{
    namespace Dl = Diligent;

    struct TransformComponent
    {
        TransformComponent() = default;

        TransformComponent(Dl::float3 translation, Dl::float3 rotation, Dl::float3 scale)
            : Translation(translation), Rotation(rotation),
            Scale(scale)
        {
        }

        [[nodiscard]] Dl::float4x4 GetTransform() const noexcept
        {
            return
                Dl::float4x4::Scale(Scale) *
                Dl::float4x4::RotationX(DirectX::XMConvertToRadians(Rotation.x)) *
                Dl::float4x4::RotationY(DirectX::XMConvertToRadians(Rotation.y)) *
                Dl::float4x4::RotationZ(DirectX::XMConvertToRadians(Rotation.z)) *
                Dl::float4x4::Translation(Translation);
        }

        Dl::float3 Translation{0.0f, 0.0f, 0.0f};
        Dl::float3 Rotation;
        Dl::float3 Scale{1.0f,1.0f,1.0f};
    };

    struct BoxComponent
    {
        Dl::float4 Color{0.5f,0.5f,0.5f,1.0f};
        float Rotation{ 0.0f };
    };

    struct NameComponent
    {
        NameComponent() = default;
        NameComponent(std::string name) : Name(std::move(name)) {}

        operator std::string_view() const { return std::string_view{Name}; }

        std::string Name = "Unnamed Entity";
    };
}
