#include "Common.hlsli"

cbuffer MeshConstants : register(b0) {
    matrix world;   
    matrix worldIT; // World¿« InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

cbuffer LakeConstantData : register(b1) {
    matrix reflectProj;
    matrix depthProj;
    float4 edges;
    float2 inside;
    float lakeAlpha;
    float waveStrength;
    bool snowOn;
    float3 Lakepadding;
};

struct PatchOutput {
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};


[domain("quad")] float4 main(PatchOutput input, float2 uv
                              : SV_DomainLocation,
                                const OutputPatch<DSInput, 4> quad) : SV_POSITION {
  
    float4 posWorld;
    float4 posProj;

    float3 p1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 p2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    posWorld = float4(p, 1.0);

    posWorld = mul(posWorld, world);


    // π∞∞·
    posWorld.y +=
        sin(posWorld.x * globalTime * 0.25) * waveStrength;



    posProj = mul(float4(posWorld.xyz, 1.0), view);
    posProj = mul(posProj, proj);





    return posProj;
}