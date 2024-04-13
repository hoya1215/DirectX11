#include "Common.hlsli"

cbuffer CollisionBoxConstantData
{ matrix world; };




PixelShaderInput main(VertexShaderInput input) {

	PixelShaderInput output;

	//float4 pos = mul(float4(input.posModel, 1.0), world);
    float4 pos = float4(input.posModel, 1.0);

	output.posWorld = pos.xyz;
    output.posProj = mul(pos, viewProj);

	// »ç¿ë x
	output.normalWorld = input.normalModel; 
	output.texcoord = input.texcoord;
    output.tangentWorld = input.tangentModel;

	return output;
}