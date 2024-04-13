#include "Common.hlsli"

cbuffer BillboardConstData : register(b0)
{
    //Matrix view;
    //Matrix proj;
    float width;
    float3 BillboardPadding;
};

Texture2D<float4> TerrainPass : register(t0);

struct GSInput
{
    float4 pos : SV_POSITION;
};

struct PSInput {
	float4 pos : SV_POSITION;
    //float4 posWorld : POSITION;
    float2 texcoord : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(point GSInput input[1], uint primID
                              : SV_PrimitiveID,
                                inout TriangleStream<PSInput> outputStream) { 
    float hw = width * 0.5;
    float4 up = float4(0.0, 1.0, 0.0, 0.0);
    float4 front = float4(eyeWorld, 1.0) - input[0].pos;
    front.w = 0.0;
    float4 right = float4(cross(up.xyz, normalize(front.xyz)), 0.0);

    // terrain 높이 가져와서 더해주기
    float2 heightTex;
    heightTex.x = abs(-20.0 - input[0].pos.x) / 40.0;

    float tex = 20.0 * (720.0 / 1280.0);
    float cpos = input[0].pos.z * (720.0 / 1280.0);
    heightTex.y = (tex - cpos) / (tex * 2.0);
    heightTex.y = 1 - heightTex.y;

    float height = TerrainPass.SampleLevel(linearWrapSampler, heightTex, 0).y;
    
    PSInput output;


    output.pos = input[0].pos - hw * right - hw * up;
    output.pos.y += height;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texcoord = float2(1.0, 1.0);
    output.primID = primID;

    outputStream.Append(output);

    output.pos = input[0].pos - hw * right + hw * up;
    output.pos.y += height;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texcoord = float2(1.0, 0.0);
    output.primID = primID;

    outputStream.Append(output);

    output.pos = input[0].pos + hw * right - hw * up;
    output.pos.y += height;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texcoord = float2(0.0, 1.0);
    output.primID = primID;

    outputStream.Append(output);

    output.pos = input[0].pos + hw * right + hw * up;
    output.pos.y += height;
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, proj);
    output.texcoord = float2(0.0, 0.0);
    output.primID = primID;

    outputStream.Append(output);


}