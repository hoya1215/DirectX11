#include "Common.hlsli"

Texture2D g_texture0 : register(t0);
Texture2D depthTex : register(t2);
SamplerState g_sampler : register(s10);

float weights[5] = {0.05, 0.2, 0.5, 0.2, 0.05};


cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float4 options;
};

struct SamplingPixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float4 main(SamplingPixelShaderInput input) : SV_TARGET
{
    float x = input.texcoord.x;
    float y = input.texcoord.y;
    
    float3 a = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y + 2 * dy)).rgb;
    float3 b = g_texture0.Sample(g_sampler, float2(x, y + 2 * dy)).rgb;
    float3 c = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y + 2 * dy)).rgb;

    float3 d = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y)).rgb;
    float3 e = g_texture0.Sample(g_sampler, float2(x, y)).rgb;
    float3 f = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y)).rgb;

    float3 g = g_texture0.Sample(g_sampler, float2(x - 2 * dx, y - 2 * dy)).rgb;
    float3 h = g_texture0.Sample(g_sampler, float2(x, y - 2 * dy)).rgb;
    float3 i = g_texture0.Sample(g_sampler, float2(x + 2 * dx, y - 2 * dy)).rgb;

    float3 j = g_texture0.Sample(g_sampler, float2(x - dx, y + dy)).rgb;
    float3 k = g_texture0.Sample(g_sampler, float2(x + dx, y + dy)).rgb;
    float3 l = g_texture0.Sample(g_sampler, float2(x - dx, y - dy)).rgb;
    float3 m = g_texture0.Sample(g_sampler, float2(x + dx, y - dy)).rgb;

    float3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;

    color = float3(0, 0, 0);

    float depth = depthTex.Sample(linearWrapSampler, input.texcoord).r;
    
    color = (a + b + c + d + e + f + g + h + i) / 9.0 ;

    
    
  
    return float4(color, 1.0);
    //return g_texture0.Sample(g_sampler, input.texcoord);
}