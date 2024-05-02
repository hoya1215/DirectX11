#include "Common.hlsli"

struct PSInput {
    float4 pos : SV_POSITION;
    float3 center : POSITION;
    float radius : PSIZE;
    uint primID : SV_PrimitiveID;
};


float4 main(PSInput input) : SV_TARGET {

    float4 color;
    float3 posWorld = mul(input.pos, invViewProj).xyz;

    float dist = length(input.center - posWorld);

    if (dist <= radius)
        color = float4(1.0, 1.0, 1.0, 1.0);
    else
        clip(-1);

    return color;
}