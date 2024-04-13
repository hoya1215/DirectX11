#include "Common.hlsli"




struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

struct HSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};


HSInput main(VSInput input) {

    HSInput output;

    output.pos = input.pos;





    // 나중에 변환 필요
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.tangent = input.tangent;

 


	return output;
}