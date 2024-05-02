#include "Common.hlsli"

struct PSInput {
    float4 pos : SV_POSITION;
    float3 center : POSITION0;
    float3 posWorld : POSITION1;
    float radius : PSIZE;
    uint primID : SV_PrimitiveID;
};

float4 main(PSInput input) : SV_TARGET {
    float4 color = float4(0.5, 0.6, 0.7, 1.0);


    //float dist = length(input.posWorld - input.center);


    // snow ±¸
    //if (dist <= input.radius + 0.002) {
    //    color = float4(1.0, 1.0, 1.0, 1.0);
    //} else {
    //    color = float4(1.0, 1.0, 1.0, 0.0);
    //}
      

    return color;
}