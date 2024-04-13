#include "Common.hlsli"

struct HeightInfo {
    float3 pos;
    int index;
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

RWStructuredBuffer<HeightInfo> outputHeight : register(u0);


[numthreads(400, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
   


    HeightInfo h;
      h.pos = outputHeight[dtID.x].pos;
    h.pos = mul(float4(h.pos, 1.0), World).xyz;

    //float2 texcoord;
    //texcoord.x = abs(-terrainScale - h.pos.x) / (terrainScale * 2.0);
    //texcoord.y = (terrainScale - h.pos.z) / (terrainScale * 2.0);

    //outputHeight[dtID.x].pos.z = -TerrainPass.SampleLevel(linearWrapSampler, texcoord, 0).y;

     float2 CTex;
    CTex.x = abs(-terrainScale - h.pos.x) / (terrainScale * 2.0);

     float tex = 20.0 * (720.0 / 1280.0);
     float cpos = h.pos.z *(720.0 / 1280.0);
     CTex.y = (tex - cpos) / (tex * 2.0);
     CTex.y = 1 - CTex.y;
    float height =
        TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;

    outputHeight[dtID.x].pos.z = -height;


}