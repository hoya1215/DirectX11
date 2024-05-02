#include "Common.hlsli"

Texture2D forwardTex : register(t0);
Texture2D positionTex : register(t1);
Texture2D normalTex : register(t2);

RWTexture2D<float4> finalTex : register(u0);


[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    float3 posW = positionTex.SampleLevel(linearWrapSampler, dtID.xy, 0).rgb;
    float3 normalW = normalTex.SampleLevel(linearWrapSampler, dtID.xy, 0).rgb;

    float3 sunRadiance =
        BasicLightRadiance(Sun, normalW, posW, 0.1, 0.7, 0.0, SunShadowMap);

    finalTex[dtID.xy] = forwardTex[dtID.xy];


}