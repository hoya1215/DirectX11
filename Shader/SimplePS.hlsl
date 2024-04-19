#include "Common.hlsli"

cbuffer MeshConstants : register(b0) {
    matrix world;  
    matrix worldIT; 
    int useHeightMap;
    float heightScale;
    float TexMove;
    int dummy;
};

cbuffer BoxInfo : register(b1) { 
    int pick;
    bool IsCollisionBox;
    float2 boxpadding;
}

struct PixelShaderOutput {
    float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;



    if (pick == 1) {
        output.pixelColor = float4(0.0, 1.0, 0.0, 1.0);
    }
    else
    {
        if (IsCollisionBox == 1)
            output.pixelColor = float4(1.0, 0.0, 0.0, 1.0);
        else
            output.pixelColor = float4(1.0, 1.0, 1.0, 1.0);
    }




	return output;
}