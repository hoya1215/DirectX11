#include "Common.hlsli"

cbuffer TerrainConstantData : register(b0)
{
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

struct HSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 texcoord : TEXCOORD;
    float3 tangent : TANGENT;

};

struct DSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 texcoord : TEXCOORD;
    float3 tangent : TANGENT;

};

struct PatchOutput
{
	float edges[4]	: SV_TessFactor;
	float inside[2]	: SV_InsideTessFactor;
};

float GetTess(float3 center , float tessLevel, float disMin , float disMax) {

    float dis = length(center - eyeWorld);

    float tess;

    if (graphicLower)
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis);
    else
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis) + 1.0;

    return tess;
    
}


PatchOutput
PatchPoint(InputPatch<HSInput, 4> input,
	uint PatchID : SV_PrimitiveID)
{
    float4 center =
        float4((input[0].pos + input[1].pos + input[2].pos + input[3].pos) * 0.25, 1.0);


    center = mul(center, World);

    float interval = terrainScale * 0.2;
    float3 center0 = center - float3(interval, center.y, 0);
    float3 center1 = center - float3(0, center.y, -interval);
    float3 center2 = center - float3(-interval, center.y, 0);
    float3 center3 = center - float3(0, center.y, interval);

    float dis = length(center - eyeWorld);
    float disMin = 1.0f;
    float disMax = 10.0f;
    float tessLevel = 64.0;

    if (!Tessellation) // LOD ON / OFF
        dis = 1;

    if (TessellationLevel == 4) // LOD Level
        tessLevel = 64.0;
    else if (TessellationLevel == 3)
        tessLevel = 32.0;
    else if (TessellationLevel == 2)
        tessLevel = 16.0;
    else if (TessellationLevel == 1)
        tessLevel = 8.0;
    else
        tessLevel = 0.0;

    //float tess = tessLevel * saturate((disMax - dis) / (disMax - disMin)) + 1.0;
    float tess;

    if (graphicLower)
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis);
    else
        tess = tessLevel * smoothstep(disMin, disMax, disMax - dis) + 1.0;

    PatchOutput output;

    // Crack 현상 해결
    output.edges[0] = max(tess, GetTess(center0, tessLevel, disMin, disMax));
    output.edges[1] = max(tess, GetTess(center1, tessLevel, disMin, disMax));
    output.edges[2] = max(tess, GetTess(center2, tessLevel, disMin, disMax));
    output.edges[3] = max(tess, GetTess(center3, tessLevel, disMin, disMax));

    //output.edges[0] = tess;
    //output.edges[1] = tess;
    //output.edges[2] = tess;
    //output.edges[3] = tess;

    output.inside[0] = tess;
    output.inside[1] = tess;


	return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchPoint")] DSInput
            main( 
	InputPatch<HSInput, 4> input, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	DSInput output;

	output.pos = input[i].pos;
    output.normal = input[i].normal;
    output.texcoord = input[i].texcoord;
    output.tangent = input[i].tangent;


	return output;
}
