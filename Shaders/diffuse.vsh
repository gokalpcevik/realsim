struct Transformation
{
    matrix MVP;
    matrix Model;
};

struct VertexShaderInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
    float3 Normal : NORMAL;
    float4 Position : SV_Position;
};

ConstantBuffer <Transformation> TransformationCB : register(b0,space0);

void main(in VertexShaderInput VSIn,
    out VertexShaderOutput VSOut)
{
    VSOut.Position = mul(TransformationCB.MVP, float4(VSIn.Position, 1.0f));
    VSOut.Normal = mul(TransformationCB.Model,float4(VSIn.Normal,0.0f)).xyz;
}