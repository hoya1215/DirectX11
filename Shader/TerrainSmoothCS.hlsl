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

RWTexture2D<float4> TerrainPass : register(u0);

[numthreads(32, 32, 1)] void main(uint3 dtID
                                  : SV_DispatchThreadID) {
    float2 offset[9] = {float2(-1, -1), float2(0, -1), float2(1, -1),
                        float2(-1, 0),  float2(0, 0),  float2(1, 0),
                        float2(-1, 1),  float2(0, 1),  float2(1, 1)};


    
    float height = 0.0;

    float2 texcoord = dtID.xy;
    texcoord.x /= 1280.0f;
    texcoord.y /= 760.0f;
    texcoord.y = 1.0 - texcoord.y;

    float2 center = float2(0.5, 0.5);

    float4 smooth = TerrainPass[texcoord];

    if (length(mouseTexcoord - texcoord) < 0.01) {


        for (int i = 0; i < 9; i++) {
            float4 pos = TerrainPass[texcoord + offset[i]];

            height += pos.y;
        }

        height /= 9.0;

        smooth.y = height;
    }

    TerrainPass[texcoord] = smooth;

}