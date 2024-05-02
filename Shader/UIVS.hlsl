#include "Common.hlsli" // 쉐이더에서도 include 사용 가능


cbuffer MeshConstants : register(b0) {
    matrix world;  
    matrix worldIT; 
    int useHeightMap;
    float heightScale;
    float TexMove;
    int clickedOBB;
};



PixelShaderInput main(VertexShaderInput input) {


    PixelShaderInput output;




    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);

    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);



    output.posWorld = pos.xyz;


    pos = mul(pos, OrthoView);
    pos = mul(pos, OrthoProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;

    // if (TexMove !=0)
    //     output.texcoord += TexMove * 0.1;

    output.tangentWorld = tangentWorld.xyz;

    return output;
}