#include "Common.hlsli"

cbuffer MeshConstants : register(b0)
{
    matrix world;
    matrix worldIT;
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

cbuffer TurningPointConstantData : register(b1) {
    float3 turningPos;
    float turningRadius;
    float turningAngle;
    float3 centerVector;
    float3 startPos;
    int TPpadding;
    float3 endPos;
    int AreaOn;
};

struct NormalGeometryShaderInput
{
    float4 posModel : SV_POSITION;
    //float3 normalModel : NORMAL;
};

struct NormalPixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

static const float lineScale = 0.02;

[maxvertexcount(4)] void
main(point NormalGeometryShaderInput input[1],
     inout LineStream<NormalPixelShaderInput> outputStream) {
    NormalPixelShaderInput output;
    float4 start = float4(startPos, 0.0);
    float4 end = float4(endPos, 0.0);
    
    float4 posWorld = input[0].posModel;
    posWorld.y += 0.1;


    output.pos = mul(posWorld, viewProj);
    output.color = float3(1.0, 1.0, 1.0);
    outputStream.Append(output);


    output.pos = mul(posWorld + start * turningRadius, viewProj);
    output.color = float3(1.0, 0.0, 1.0);
    outputStream.Append(output);



    outputStream.RestartStrip();

    output.pos = mul(posWorld, viewProj);
    output.color = float3(1.0, 1.0, 1.0);
    outputStream.Append(output);

    output.pos = mul(posWorld + end * turningRadius, viewProj);
    output.color = float3(0.0, 1.0, 1.0);
    outputStream.Append(output);

}
