#include "Common.hlsli"

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
    float2 texcoord : TEXCOORD;
};

SkyboxPixelShaderInput main(VertexShaderInput input)
{

    SkyboxPixelShaderInput output;
    output.posModel = input.posModel;
    output.posProj = mul(float4(input.posModel, 1.0), view); // È¸Àü¸¸
    output.posProj = mul(output.posProj, proj);

    output.posProj.z = output.posProj.w * 0.9999;

    output.texcoord = input.texcoord;

    return output;
}
