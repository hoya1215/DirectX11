#include "Common.hlsli"

Texture2D g_height : register(t0);
Texture2D TerrainPass : register(t1);
SamplerState g_sampler : register(s0);

cbuffer MeshConstants : register(b0) {
    matrix world;   
    matrix worldIT; 
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

cbuffer TerrainConstantData : register(b1) {
    // matrix view;
    // matrix proj;
    matrix World;
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

struct PatchOutput {
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DSInput {
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
};


[domain("quad")] float4 main(PatchOutput input, float2 uv
                             : SV_DomainLocation,
                               const OutputPatch<DSInput, 4> quad)
    : SV_POSITION {

    float4 posProj = float4(1.0, 1.0, 1.0, 1.0);

    float2 t1 = lerp(quad[0].texcoord, quad[1].texcoord, uv.x);
    float2 t2 = lerp(quad[2].texcoord, quad[3].texcoord, uv.x);
    float2 t = lerp(t1, t2, uv.y);


    float3 p1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 p2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    posProj = float4(p, 1.0);


    posProj = mul(posProj, world);

    float height = TerrainPass.SampleLevel(g_sampler, t, 0).g;

    posProj.y = height;



    posProj = mul(posProj, view);
    posProj = mul(posProj, proj);



    return posProj;
}
