struct ModelProjection
{
    matrix MVP;
};

ConstantBuffer<ModelProjection> ModelViewProjectionCB : register(b0);

struct VertexShaderInput
{
    float3 Position : POSITION;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
};

void main(in VertexShaderInput VSIn,
    out VertexShaderOutput VSOut)
{
    VSOut.Position = mul(ModelViewProjectionCB.MVP,float4(VSIn.Position,1.0f));
}