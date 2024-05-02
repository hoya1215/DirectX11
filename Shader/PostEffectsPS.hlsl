#include "Common.hlsli" 

// t20에서부터 시작

Texture2D RenderTex : register(t20);
Texture2D positionTex : register(t21);
Texture2D normalTex : register(t22);
Texture2D depthTex : register(t23); 
Texture2D ssrTex : register(t24);
//Texture2D ssaoMap : register(t22); // SSAO

cbuffer PostEffectsConstants : register(b3)
{
    int mode; // 1: Rendered image, 2: DepthOnly
    float depthScale;
    float fogStrength;
    int onlySSAO;
    bool ssr;
    float3 PECpadding;
};

struct SamplingPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 TexcoordToView(float2 texcoord)
{
    float4 posProj;

    posProj.xy = texcoord * 2.0 - 1.0;
    posProj.y *= -1;
    posProj.z = depthTex.Sample(linearClampSampler, texcoord).r;
    posProj.w = 1.0;
    
    float4 posView = mul(posProj, invProj);
    posView.xyz /= posView.w;
    
    return posView;
}

int RaySphereIntersection(in float3 start, in float3 dir, in float3 center, in float radius,
                            out float t1, out float t2)
{
    float3 p = start - center;
    float pdotv = dot(p, dir);
    float p2 = dot(p, p);
    float r2 = radius * radius;
    float m = pdotv * pdotv - (p2 - r2);
    
    if (m < 0.0)
    {
        t1 = 0;
        t2 = 0;
        return 0;
    }
    else
    {
        m = sqrt(m);
        t1 = -pdotv - m;
        t2 = -pdotv + m;
        return 1;
    }
}


float HaloEmission(float3 posView, float radius)
{
    // Halo
    float3 rayStart = float3(0, 0, 0); // View space
    float3 dir = normalize(posView - rayStart);

    float3 center = mul(float4(lights[0].position, 1.0), view).xyz; // View 공간으로 변환

    float t1 = 0.0;
    float t2 = 0.0;
    if (RaySphereIntersection(rayStart, dir, center, radius, t1, t2) && t1 < posView.z)
    {
        t2 = min(posView.z, t2);
            
        float p2 = dot(rayStart - center, rayStart - center);
        float pdotv = dot(rayStart - center, dir);
        float r2 = radius * radius;
        float invr2 = 1.0 / r2;
        float haloEmission = (1 - p2 * invr2) * (t2 - t1)
                          - pdotv * invr2 * (t2 * t2 - t1 * t1)
                          - 1.0 / (3.0 * r2) * (t2 * t2 * t2 - t1 * t1 * t1);
            
        haloEmission /= (4 * radius / 3.0);

        return haloEmission;
    }
    else
    {
        return 0.0;
    }
}

#define aC 0.1
#define dC 0.7
#define sC 0.0



float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    if (mode == 1)
    {
        // float3 color = clamp(RenderTex.Sample(linearClampSampler,
        // input.texcoord).rgb, 0, 1);

        // deferred ( position 의 w 값으로 deferred 인지 forward 인지 구분 )

        float3 color = RenderTex.Sample(linearWrapSampler, input.texcoord).rgb;
        float4 position = positionTex.Sample(linearWrapSampler, input.texcoord);
        float4 normal = normalTex.Sample(linearWrapSampler, input.texcoord);

        if (ssr) {
            float3 ssr = ssrTex.Sample(linearWrapSampler, input.texcoord).rgb;
            if (ssr.r + ssr.g + ssr.b > 0.0)
                color = color + ssr;
        }
        
        if (position.w  == 1.0)
        {
            float3 sunRadiance = BasicLightRadiance(Sun, normal.xyz, position.xyz, aC, dC ,sC, SunShadowMap);
            color = color * sunRadiance;
        }

        //color = depthTex.Sample(linearWrapSampler, input.texcoord).rgb;

        
        
        float4 posView = TexcoordToView(input.texcoord);

        // Halo
        float3 haloColor = float3(0.96, 0.94, 0.82);
        float radius = lights[0].haloRadius;
        color += HaloEmission(posView.xyz, radius) * haloColor * lights[0].haloStrength;

        // Fog
        float dist = length(posView.xyz); // 눈의 위치가 원점인 좌표계
        float3 fogColor = float3(1, 1, 1);
        float fogMin = 1.0;
        float fogMax = 10.0;
        float distFog = saturate((dist - fogMin) / (fogMax - fogMin));
        float fogFactor = exp(-distFog * fogStrength);

        color = lerp(fogColor, color, fogFactor);

            if (onlySSAO == 1)
                color = ssaoMap.Sample(linearWrapSampler, input.texcoord).rgb;



        return float4(color, 1.0);
    }
    else // if (mode == 2)
    {
        float z = TexcoordToView(input.texcoord).z * depthScale;
        return float4(z, z, z, 1);
    }

    
}
