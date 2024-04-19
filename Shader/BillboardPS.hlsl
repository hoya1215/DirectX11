#include "Common.hlsli"


//Texture2DArray g_texArray : register(t0);
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct PSInput {
    float4 pos : SV_POSITION;
    //float4 posWorld : POSITION;
    float2 texcoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

struct PSOutput
{
    float4 color : SV_Target0;
};


PSOutput main(PSInput input)
{
    //float3 uvw = float3(input.texcoord, float(input.primID % 5));

    //float4 pixelColor = g_texArray.Sample(g_sampler, uvw);

    float4 pixelColor = g_texture.Sample(g_sampler, input.texcoord);

        clip((pixelColor.a < 0.9f) ||
                 (pixelColor.r + pixelColor.g + pixelColor.b) > 2.4
             ? -1
             : 1);

            PSOutput output;
        output.color = pixelColor;

        output.color *= strengthIBL;

    return output;
}