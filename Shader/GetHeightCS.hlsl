#include "Common.hlsli"

struct CharacterHeight
{
    float height;
};

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

Texture2D<float4> TerrainPass : register(t0);
RWStructuredBuffer<CharacterHeight> m_CharacterHeight : register(u0);

[numthreads(1, 1, 1)] void main(uint3 groupID
                                  : SV_GroupID, uint3 dtID
                                  : SV_DispatchThreadID) {
    

    float2 CTex;
    CTex.x = abs(-20.0 - characterWorldPos.x) / 40.0;

    float tex = 20.0 * (720.0 / 1280.0);
    float cpos = characterWorldPos.z * (720.0 / 1280.0);
    CTex.y = (tex - cpos) / (tex * 2.0);
    CTex.y = 1 - CTex.y;

    m_CharacterHeight[0].height = TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;
}