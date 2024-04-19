#include "Common.hlsli"

struct VSInput
{
    float3 posModel : POSITION;
    matrix insWorld : WORLD;
    float velocity : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
};


PSInput main(VSInput input)
{
    PSInput output;

    float v = (globalTime * input.velocity)% 11.0f;
    input.posModel.y -= v;

    output.pos = mul(float4(input.posModel, 1.0), input.insWorld);
    output.pos = mul(output.pos, viewProj);


	return output;
}