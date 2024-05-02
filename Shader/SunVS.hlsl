#include "Common.hlsli"


PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    output.posProj = mul(float4(input.posModel, 1.0), viewProj);
    output.texcoord = input.texcoord;


	return output;
}