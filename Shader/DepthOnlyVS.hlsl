#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

cbuffer MeshConstants : register(b0)
{
    matrix world; // Model(또는 Object) 좌표계 -> World로 변환
    matrix worldIT; // World의 InverseTranspose
    int useHeightMap;
    float heightScale;
    float2 dummy;
};

float4 main(VertexShaderInput input) : SV_POSITION
{

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


    for (int i = 0; i < 8; ++i) {

        posModel +=
            weights[i] *
            mul(float4(input.posModel, 1.0f), boneTransforms[indices[i]]).xyz;
    }

    input.posModel = posModel;

#endif

    float4 pos = mul(float4(input.posModel, 1.0f), world);
    return mul(pos, viewProj);
}
