#define EPSILON 0.0001f

static const float3 L = float3(0.700069f, -0.504497f, 0.504497f);
static const float I = 0.56f;

struct PSConstantData
{
    float4 Color;
    float Shininess;
};

struct CameraData
{
    float4 Position;
    float4 Forward;
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
ConstantBuffer <CameraData> Camera : register(b2);

void main(in PixelShaderInput PSIn, out PixelShaderOutput PSOut)
{
    // Geometry term
    float NdotL = max(dot(-L, PSIn.Normal.xyz), EPSILON);

    // Halfway vector
    float3 H = normalize((-L + -Camera.Forward.xyz));

    // Term for Blinn-Phong Reflectance model
    float HdotN = max(dot(H, PSIn.Normal.xyz), EPSILON);


    float4 SurfaceColor = PSConstants.Color;

    // BRDF
    float4 f_r = SurfaceColor + SurfaceColor * pow(HdotN, PSConstants.Shininess);
}