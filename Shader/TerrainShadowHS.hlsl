#include "Common.hlsli"

cbuffer TerrainConstantData : register(b0) {
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

struct HSInput {
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
};

struct DSInput {
    float3 pos : POSITION;
    float2 texcoord : TEXCOORD;
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

    center = mul(center, World);

    float dis = length(center - eyeWorld);
    float disMin = 1.0f;
    float disMax = 10.0f;
    float tessLevel = 64.0;

    if (!Tessellation)
        dis = 1;

    if (TessellationLevel == 4)
        tessLevel = 64.0;
    else if (TessellationLevel == 3)
        tessLevel = 32.0;
    else if (TessellationLevel == 2)
        tessLevel = 16.0;
    else if (TessellationLevel == 1)
        tessLevel = 8.0;
    else
        tessLevel = 0.0;

    float tess;

    if (graphicLower)
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis);
    else
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis) + 1.0;

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
                output.texcoord = input[i].texcoord;

                return output;
            }
