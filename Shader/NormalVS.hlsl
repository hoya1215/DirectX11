#include "Common.hlsli"

struct VSInput {
    float4 pos : POSITION;
};

struct NormalGeometryShaderInput
{
    float4 posModel : SV_POSITION;
    //float3 normalWorld : NORMAL;
};

NormalGeometryShaderInput main(VSInput input)
{
    NormalGeometryShaderInput output;

    output.posModel = input.pos;
    //output.normalWorld = input.normalModel;

    return output;
}
