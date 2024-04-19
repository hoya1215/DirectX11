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


[numthreads(32, 32, 1)] void main(uint3 groupID
                                  : SV_GroupID, uint3 dtID
                                  : SV_DispatchThreadID) {
    float2 texcoord = dtID.xy;
    texcoord.x /= 1280.0f;
    texcoord.y /= 720.0f;
    texcoord.y = 1.0 - texcoord.y;

    float4 world = TerrainPass[dtID.xy];
    float4 C = CorrectionAxis;
    float2 dist = float2(0.5, 0.5);

    float2 texWorld;
    texWorld.x = -terrainScale + (texcoord.x * terrainScale * 2);
    texWorld.y = terrainScale - (texcoord.y * terrainScale * 2);

    if (world.y > 0 && texWorld.x >= C.x && texWorld.x <= C.y &&
        texWorld.y >= C.z && texWorld.y <= C.w)
        world.y = CorrectionHeight;

    TerrainPass[dtID.xy] = world;
}