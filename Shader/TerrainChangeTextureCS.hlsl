#include "Common.hlsli"

cbuffer TerrainConstantData : register(b0) {
    // matrix view;
    // matrix proj;
    matrix World;
    matrix TerrainView;
    matrix TerrainProj;
    float4 edges;
    float2 inside;
    float heightControlUp;
    float heightControlDown;
    bool Tessellation;
    int TessellationLevel;
    int Texindex;
    int Useheightmap;
    float3 terrainCenter;
    float terrainScale;
    float4 refractPlane;
    bool brush;
    float brushIndex;
    int EditFlag;
    float CorrectionHeight;
    float4 CorrectionAxis;
};

RWTexture2D<float4> TerrainPixel : register(u0);

[numthreads(32, 32, 1)] void main(uint3 dtID
                                  : SV_DispatchThreadID) {
    uint2 texcoord = uint2(dtID.xy);
    float4 weight = TerrainPixel[texcoord];

    // 기존 텍스처 밀어버리기

    if (weight.r == 1.0)
        weight.r = 0.0;

    if (weight.g == 1.0)
        weight.g = 0.0;

    if (weight.b == 1.0)
        weight.b = 0.0;

    if (weight.a == 1.0)
        weight.a = 0.0;

    switch (brushIndex)
    { case 0.0:
        weight.a = 1.0;
        break;
    case 1.0:
        weight.r = 1.0;
        break;
    case 2.0:
        weight.g = 1.0;
        break;
    case 3.0:
        weight.b = 1.0;
        break;
    }

    TerrainPixel[texcoord] = weight;

}