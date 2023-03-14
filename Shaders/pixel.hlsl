static const float3 L = float3(0.700069f, -0.504497f, 0.504497f);
static const float I = 0.74f;
static const float4 AOf4 = float4(1.0f, 1.0f, 1.0f, 1.0f);
static const float AO = 0.04f;

struct PSConstantData
{
    float4 Color;
};

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float4 Normal : RS_Normal;
};

struct PixelShaderOutput
{
    float4 Color : SV_Target;
};

ConstantBuffer <PSConstantData> PSConstants : register(b1);

void main(in PixelShaderInput PSIn,out PixelShaderOutput PSOut)
{
    float NdotL = max(dot(-L, PSIn.Normal.xyz), 0.0f);
    PSOut.Color = saturate(PSConstants.Color * NdotL * I + AOf4 * AO);
    //float gamma = 2.2f;
    //PSOut.Color.xyz = pow(PSOut.Color.xyz, 1.0f / gamma);
}