#include "realsim/ecs/CameraComponent.h"

Diligent::float4x4 RSim::FromDXMath(const DirectX::XMMATRIX &mat) {
    Diligent::float4x4 f4x4;
    DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)&f4x4,mat);
    return f4x4;
}
