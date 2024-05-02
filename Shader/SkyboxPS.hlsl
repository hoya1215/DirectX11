#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

Texture2D albedoTex : register(t0);

cbuffer MaterialConstants : register(b0) {
    float3 albedoFactor; // baseColor
    float roughnessFactor;
    float metallicFactor;
    float3 emissionFactor;

    int useAlbedoMap;
    int useNormalMap;
    int useAOMap; // Ambient Occlusion
    int invertNormalMapY;
    int useMetallicMap;
    int useRoughnessMap;
    int useEmissiveMap;
    int pick;

    float3 rimColor;
    float rimPower;
    float rimStrength;
    int Ocean;
    int TexOnly;
    int padding;
};

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
    float2 texcoord : TEXCOORD;
};

struct PixelShaderOutput {
    float4 pixelColor : SV_Target0;
    float4 position : SV_Target1;
    float4 normal : SV_Target2;
    float4 depth : SV_Target3;
};



PixelShaderOutput main(SkyboxPixelShaderInput input)
{
    PixelShaderOutput output;
    
    if (textureToDraw == 0)
        output.pixelColor = envIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 1)
        output.pixelColor = specularIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else if (textureToDraw == 2)
        output.pixelColor = irradianceIBLTex.SampleLevel(linearWrapSampler, input.posModel.xyz, envLodBias);
    else
        output.pixelColor = float4(135/255, 206/255, 235/255, 1);


        output.pixelColor *= strengthIBL;

       output.position = float4(input.posModel, 0.0);
       output.normal = float4(0.0, 0.0, 0.0, 0.0);
       float depth = input.posProj.z;
       depth /= input.posProj.w;
       output.depth = float4(1 - depth, 1 - depth, 1 - depth, 1.0);
       output.depth = float4(1.0, 1.0, 1.0, 1.0);

    
    return output;
}
