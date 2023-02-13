#pragma once

#include <DirectXMath.h>
#include <utility>
#include <string>
#include <string_view>
#include <Eigen/Eigen>

namespace RSim::ECS
{
    struct TransformComponent
    {
        TransformComponent() = default;

        TransformComponent(DirectX::XMFLOAT3 translation, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale)
            : Translation(std::move(translation)), Rotation(rotation),
            Scale(std::move(scale))
        {
        }

        [[nodiscard]] DirectX::XMMATRIX GetTransform() const noexcept
        {
            return
        		DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z) * 
                DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&Rotation)) *
                DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
        }

        DirectX::XMFLOAT3 Translation{0.0f, 0.0f, 0.0f};
        DirectX::XMFLOAT4 Rotation{ 0.0f,0.0f,0.0f,1.0f };
        DirectX::XMFLOAT3 Scale{1.0f,1.0f,1.0f};
    };

    struct BoxComponent
    {
        DirectX::XMFLOAT2 ScreenPosition{0.0f,0.0f};
        DirectX::XMFLOAT4 Color{0.5f,0.5f,0.5f,1.0f};
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
