#include "Common.hlsli"

cbuffer OffsetVectors : register(b0) {
    float4 offSets[14];
    float ssaoRadius;
    float dist;
    float ssaoStart;
    float ssaoEnd;
};



RWTexture2D<float4> ssaoUAV : register(u0);

static const float2 offset[9] = {float2(-1, -1), float2(0, -1), float2(1, -1),
                                 float2(-1, 0),  float2(0, 0),  float2(1, 0),
                                 float2(-1, 1),  float2(0, 1),  float2(1, 1)};


[numthreads(32, 32, 1)] void main(uint3 dtID
                                  : SV_DispatchThreadID) {

    float2 texcoord = dtID.xy;
    texcoord.x /= 1280.0;
    texcoord.y /= 720.0f;

    float blur = 0.0;

    for (int i = 0; i < 9; i++)
    {
        blur += ssaoUAV[dtID.xy + offset[i]].r;
    }

    blur /= 9;

    ssaoUAV[dtID.xy] = float4(blur, blur, blur, 1.0);

}