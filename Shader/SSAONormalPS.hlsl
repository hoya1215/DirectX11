#include "Common.hlsli"

struct PSOut
{
    float4 normalView : SV_Target0;
};


PSOut main(PixelShaderInput input) {

    PSOut output;
     float4 normalView = mul(float4(input.normalWorld, 1.0), view);
    //output.normalView = float4(1.0, 1.0, 1.0, 1.0);
     output.normalView = normalView;

	return output;
}