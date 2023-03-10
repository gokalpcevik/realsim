static const float PI = 3.14159265f;
static const float Epsilon = 0.00001;
static const float3 Fdielectric = 0.04;

float Alpha_Burley(float roughness)
{
    return roughness * roughness;
}

float D_BlinnPhong(float NdotH, float alphaR)
{
    float alphaR2 = max(alphaR * alphaR, 0.1f);
    return pow(NdotH, 2 / alphaR2) / (PI * alphaR2);
}

float D_Beckmann(float alphaR, float NdotH)
{
    float alphaR2 = max(alphaR * alphaR, 0.1f);
    float nDotH2 = max(NdotH * NdotH, 0.0001f);
    float nDotH4 = nDotH2 * nDotH2;
    return exp((nDotH2 - 1) / (alphaR2 * nDotH2)) / (PI * alphaR2 * nDotH4);
}

float D_GGX(float alphaR, float NdotH)
{
    float alphaR2 = alphaR * alphaR;
    float d = max(NdotH * NdotH * (alphaR2 - 1) + 1, 0.0001f);
    return alphaR2 / (PI * d * d);
}


float3 Fresnel_None(float3 f0)
{
    return f0;
}

// A slightly modified version of Schlick's
// Following the notes of Karis from Epic, https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float3 Fresnel_SchlickModified(float3 f0, float VdotH)
{
    return f0 + (float3(1.0f, 1.0f, 1.0f) - f0) * pow(2.0f, (-5.55473f * VdotH - 6.98316f) * VdotH);
}

float3 Fresnel_Schlick(float3 f0, float NdotV)
{
    return f0 + (1.0 - f0) * pow(1.0 - NdotV, 5.0);
}

float G_Implicit(float NdotL, float NdotV)
{
    return NdotL * NdotV;
}

float G_Neumann(float NdotL, float NdotV)
{
    return NdotL * NdotV / max(NdotL, NdotV);
}

float G_CookTorrance(float NdotH, float NdotV, float NdotL, float VdotH)
{
    return min(min(1.0f, 2.0f * NdotH * NdotV / VdotH), 2.0f * NdotH * NdotL / VdotH);
}

float G_Kelemen(float NdotL, float NdotV, float VdotH)
{
    return NdotL * NdotV / (VdotH * VdotH);
}

float G_GGX(float NdotV, float alpha)
{
    float alphaSqr = pow(alpha, 2.0f);
    float NdotV_Sqr = pow(NdotV, 2.0f);
    return 2.0f * NdotV / (NdotV + sqrt(alphaSqr + (1.0f - alphaSqr) * NdotV_Sqr));
}

float K_Disney(float roughness)
{
    return pow(roughness + 1.0f, 2) / 8.0f;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(float3 N, float3 V, float3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}
