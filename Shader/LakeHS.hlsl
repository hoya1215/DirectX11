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

Texture2D<float4> TerrainPass : register(t0);

struct HSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

struct DSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

struct PatchOutput {
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

PatchOutput PatchPoint(InputPatch<HSInput, 4> input, uint PatchID
                       : SV_PrimitiveID) {
    float4 center = float4(
        (input[0].pos + input[1].pos + input[2].pos + input[3].pos) * 0.25,
        1.0);

       float height1 =
        TerrainPass.SampleLevel(linearWrapSampler, input[0].texcoord, 0).y;
    float height2 =
        TerrainPass.SampleLevel(linearWrapSampler, input[1].texcoord, 0).y;
       float height3 =
           TerrainPass.SampleLevel(linearWrapSampler, input[2].texcoord, 0).y;
    float height4 =
        TerrainPass.SampleLevel(linearWrapSampler, input[3].texcoord, 0).y;


    center = mul(center, world);

    float dis = length(center - eyeWorld);
    float disMin = 1.0f;
    float disMax = 10.0f;
    float tessLevel = 1.0;


    float tess;


    if (graphicLower)
        tess = tessLevel * saturate((disMax - dis) / (disMax - disMin));
    else
        tess = 1.0f;


    if (center.y <= height1 && center.y <= height2 && center.y <= height3 && center.y <= height4)
        tess = 0.0;

    PatchOutput output;
    output.edges[0] = tess;
    output.edges[1] = tess;
    output.edges[2] = tess;
    output.edges[3] = tess;

    output.inside[0] = tess;
    output.inside[1] = tess;

    return output;
}

[domain(
    "quad")][partitioning("integer")][outputtopology("triangle_cw")]
            [outputcontrolpoints(4)][patchconstantfunc("PatchPoint")] DSInput
            main(InputPatch<HSInput, 4> input, uint i
                 : SV_OutputControlPointID, uint PatchID
                 : SV_PrimitiveID) {
                DSInput output;

                output.pos = input[i].pos;
                output.normal = input[i].normal;
                output.texcoord = input[i].texcoord;
                output.tangent = input[i].tangent;

                return output;
            }
