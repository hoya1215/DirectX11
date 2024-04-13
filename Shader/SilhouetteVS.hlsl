#include "Common.hlsli" 



cbuffer MeshConstants : register(b0) {
    matrix world;   
    matrix worldIT; 
    int useHeightMap;
    float heightScale;
    float TexMove;
    int clickedOBB;
};

#ifdef SKINNED

Texture2D<float4> TerrainPass : register(t0);

#endif

PixelShaderInput main(VertexShaderInput input) {

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
    output.normalWorld = mul(normal, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);


    float4 tangentWorld = float4(input.tangentModel, 0.0f);
    tangentWorld = mul(tangentWorld, world);

    float4 pos = float4(input.posModel, 1.0f);
    pos = mul(pos, world);

#ifdef SKINNED

    

#endif


    output.posWorld = pos.xyz; 

    pos.xyz += output.normalWorld * 0.1;

    pos = mul(pos, viewProj);

    output.posProj = pos;
    output.texcoord = input.texcoord;



    output.tangentWorld = tangentWorld.xyz;

    return output;
}