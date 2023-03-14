struct TransformationData
{
    matrix WorldViewProjection;
    matrix ModelToWorld;
};

ConstantBuffer<TransformationData> Transformation : register(b0);

struct VertexShaderInput
{
    float3 Position : ATTRIB0;
    float3 Normal : ATTRIB1;
    float3 Color : ATTRIB2;
    float2 UV : ATTRIB3;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float4 Normal : RS_Normal;
};

void main(in VertexShaderInput VSIn,
    out VertexShaderOutput VSOut)
{
    VSOut.Position = mul(float4(VSIn.Position, 1.0f),Transformation.WorldViewProjection);
	VSOut.Normal = normalize(mul(float4(VSIn.Normal, 0.0f), Transformation.ModelToWorld));
}