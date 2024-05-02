#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

// Vertex Shader에서도 텍스춰 사용
//Texture2D g_heightTexture : register(t0);

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float TexMove;
    int clickedOBB;
};

#ifdef SKINNED

Texture2D<float4> TerrainPass : register(t0);

#endif

PixelShaderInput main(VertexShaderInput input)
{
    // 뷰 좌표계는 NDC이기 때문에 월드 좌표를 이용해서 조명 계산
    
    PixelShaderInput output;

#ifdef SKINNED


    float weights[8];
    weights[0] = input.boneWeights0.x;
    weights[1] = input.boneWeights0.y;
    weights[2] = input.boneWeights0.z;
    weights[3] = input.boneWeights0.w;
    weights[4] = input.boneWeights1.x;
    weights[5] = input.boneWeights1.y;
    weights[6] = input.boneWeights1.z;
    weights[7] = input.boneWeights1.w;

    uint indices[8]; 
    indices[0] = input.boneIndices0.x;
    indices[1] = input.boneIndices0.y;
    indices[2] = input.boneIndices0.z;
    indices[3] = input.boneIndices0.w;
    indices[4] = input.boneIndices1.x;
    indices[5] = input.boneIndices1.y;
    indices[6] = input.boneIndices1.z;
    indices[7] = input.boneIndices1.w;

    float3 posModel = float3(0.0f, 0.0f, 0.0f);
    float3 normalModel = float3(0.0f, 0.0f, 0.0f);
    float3 tangentModel = float3(0.0f, 0.0f, 0.0f);


    for (int i = 0; i < 8; ++i) {
        posModel +=
            weights[i] *
            mul(float4(input.posModel, 1.0f), boneTransforms[indices[i]]).xyz;
        normalModel += weights[i] * mul(input.normalModel,
                                        (float3x3)boneTransforms[indices[i]]);
        tangentModel += weights[i] * mul(input.tangentModel,
                                         (float3x3)boneTransforms[indices[i]]);
    }

    input.posModel = posModel;
    input.normalModel = normalModel;
    input.tangentModel = tangentModel;

#endif
    

    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, world).xyz;
    //output.normalWorld = mul(normal, view).xyz;
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);
    

    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);
    
#ifdef SKINNED

    //float2 CTex;
    //CTex.x= abs(-20.0 - characterWorldPos.x) / 40.0;

    //float tex = 20.0 * (720.0 / 1280.0);
    //float cpos = characterWorldPos.z *(720.0 / 1280.0);
    //CTex.y = (tex - cpos) / (tex * 2.0);
    //CTex.y = 1 - CTex.y;
    //float height = TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;

    //if (characterWorldPos.y - extent < height)
    //    pos.y += TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;


    

#endif


    output.posWorld = pos.xyz; // 월드 위치 따로 저장

    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;


    output.tangentWorld = tangentWorld.xyz;
    
    return output;
}
