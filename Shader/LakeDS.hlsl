#include "Common.hlsli"


cbuffer MeshConstants : register(b0) {
    matrix world;   // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
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

struct PSInput {
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 reflectSamplingPos : TEXCOORD1;
    float3 tangent : TANGENT;
};



[domain("quad")] PSInput main(PatchOutput input, float2 uv
                                       : SV_DomainLocation,
                                         const OutputPatch<DSInput, 4> quad) {
    PSInput output;

    float3 n1 = lerp(quad[0].normal, quad[1].normal, uv.x);
    float3 n2 = lerp(quad[2].normal, quad[3].normal, uv.x);
    float3 n = lerp(n1, n2, uv.y);

    output.normal = n;

    output.normal = float4(output.normal, 0.0);
    output.normal = mul(output.normal, worldIT).xyz;
    output.normal = normalize(output.normal);

    float2 t1 = lerp(quad[0].texcoord.xy, quad[1].texcoord.xy, uv.x);
    float2 t2 = lerp(quad[2].texcoord.xy, quad[3].texcoord.xy, uv.x);
    float2 t = lerp(t1, t2, uv.y);

    output.texcoord = t;


    float3 p1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 p2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    output.posWorld = float4(p, 1.0);

    output.posWorld = mul(output.posWorld, world).xyz;


    // 물결
    output.posWorld.y += sin(output.posWorld.x *globalTime * 0.25) * waveStrength;

        matrix WRVP = mul(world, reflectProj);
    output.reflectSamplingPos = mul(float4(p, 1.0), WRVP);

    output.posProj = mul(float4(output.posWorld, 1.0), view);
    output.posProj = mul(output.posProj, proj);

    float3 tan1 = lerp(quad[0].tangent, quad[1].tangent, uv.x);
    float3 tan2 = lerp(quad[2].tangent, quad[3].tangent, uv.x);
    float3 tan = lerp(tan1, tan2, uv.y);

    output.tangent = float4(tan, 0.0);
    output.tangent = mul(output.tangent, world).xyz;

    return output;
}
