#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

// 참고자료
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

// 메쉬 재질 텍스춰들 t0 부터 시작
Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D aoTex : register(t2);
Texture2D metallicRoughnessTex : register(t3);
Texture2D emissiveTex : register(t4);

static const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0

cbuffer MaterialConstants : register(b0) {
    float ambientC;
    float diffuseC;
    float specularC;
    int materialpadding;

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

 
struct PixelShaderOutput {
    float4 pixelColor : SV_Target0;
    float4 position : SV_Target1;
    float4 normal : SV_Target2;
};


PixelShaderOutput main(PixelShaderInput input) {
    
    PixelShaderOutput output;

    output.pixelColor = float4(0.0, 1.0, 0.0, 1.0);

    // Rim 
    float rim = (1.0f - dot(input.normalWorld, normalize(eyeWorld - input.posWorld)));

    float rimC = 1.0 - thickness;

    if (rim <= rimC)
        output.pixelColor.w = 0.0;

    return output;
}