#include "Common.hlsli"

cbuffer UIConstantData : register(b1)
{
    int push;
    int index; // 0 : map   1 : key
    float2 UIpadding;
};

Texture2D g_texture : register(t5);

struct PSOutput
{
    float4 pixelColor : SV_Target0;
};


PSOutput main(PixelShaderInput input)
{
    PSOutput output;

    float2 sampleTex;
        
    if (index == 0) {

        sampleTex = float2(input.texcoord.y, 1 - input.texcoord.x);
    }
    else if (index == 1)
    {
        sampleTex = input.texcoord;
    }

    
    float4 color = g_texture.Sample(linearWrapSampler, sampleTex);

    if (index == 0) // ¹Ì´Ï¸Ê
    {
        float2 characterTexcoord;
        characterTexcoord.x = abs(-20.0 - characterWorldPos.x) / 40.0;
        characterTexcoord.y = (20.0 - characterWorldPos.z) / 40.0;

        if (length(characterTexcoord - input.texcoord) < 0.02) {
            color = float4(1.0, 0.0, 0.0, 1.0);
        }
    }

    if (push == 1) // Å°ºä¾î
    {
        if (color.r + color.g + color.b >= 2.8)
            color = float4(0.4, 0.7, 0.9, 1.0);
    }

    if (index == 2) // Å°ºä¾î º¸µå
    {
        color = float4(1.0, 1.0, 1.0, 0.1);
    }

    output.pixelColor = color;

	return output;
}