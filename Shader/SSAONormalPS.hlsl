#include "Common.hlsli"

//struct PSOut
//{
//    float4 normalView : SV_Target0;
//};

struct PSOut {
    float4 normalView : SV_Target0;
    float4 position : SV_Target1;
    float4 normal : SV_Target2;
};


PSOut main(PixelShaderInput input) {

    PSOut output;
     float4 normalView = mul(float4(input.normalWorld, 1.0), view);
    //output.normalView = float4(1.0, 1.0, 1.0, 1.0);
     output.normalView = normalView;
     output.position = float4(0.0, 0.0, 0.0, 0.0);
     output.normal = float4(0.0, 0.0, 0.0, 0.0);

	return output;
}