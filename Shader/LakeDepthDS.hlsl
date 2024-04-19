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
                               const OutputPatch<DSInput, 4> quad)
    : SV_POSITION {
    float4 posProj;

   

    float3 p1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 p2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    posProj = float4(p, 1.0);

    posProj = mul(posProj, world);


    posProj = mul(posProj, depthProj);


  

    return posProj;
}
