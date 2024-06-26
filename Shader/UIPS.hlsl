#include "Common.hlsli"

cbuffer UIConstantData : register(b1)
{
    int push;
    int index; // 0 : map   1 : key  2 : board  3 : g_buffer
    float2 UIpadding;
};

Texture2D g_texture : register(t5);

struct PSOutput {
    float4 pixelColor : SV_Target0;
    float4 position : SV_Target1;
    float4 normal : SV_Target2;
    float4 depth : SV_Target3;
};


PSOutput main(PixelShaderInput input)
{
    PSOutput output;

    float2 sampleTex;
        
    if (index == 0) {

        sampleTex = float2(input.texcoord.y, 1 - input.texcoord.x);
    }
    else if (index == 1 || index == 3)
    {
        sampleTex = input.texcoord;
    }

    
    float4 color = g_texture.Sample(linearWrapSampler, sampleTex);

    if (index == 3)
        color.w = 1;

    if (index == 0) // �̴ϸ�
    {
        float2 characterTexcoord;
        characterTexcoord.x = abs(-20.0 - characterWorldPos.x) / 40.0;
        characterTexcoord.y = (20.0 - characterWorldPos.z) / 40.0;

        if (length(characterTexcoord - input.texcoord) < 0.02) {
            color = float4(1.0, 0.0, 0.0, 1.0);
        }
    }

    if (push == 1) // Ű���
    {
        if (color.r + color.g + color.b >= 2.8)
            color = float4(0.4, 0.7, 0.9, 1.0);
    }

    if (index == 2) // Ű��� ����
    {
        color = float4(1.0, 1.0, 1.0, 0.1);
    }

    output.pixelColor = color;

     output.position = float4(0.0, 0.0, 0.0, 0.0);
    output.normal = float4(0.0, 0.0, 0.0, 0.0);
     output.depth = float4(1.0, 1.0, 1.0, 0.0);

	return output;
}