#include "Common.hlsli" // 쉐이더에서도 include 사용 가능


Texture2D g_reflect : register(t0);
//Texture2D g_lakeDepth : register(t1);
Texture2D g_lakeTex : register(t1);
Texture2D<float4> TerrainPass : register(t2);

cbuffer LakeConstantData : register(b0) {
    matrix reflectProj;
    matrix depthProj;
    float4 edges;
    float2 inside;
    float lakeAlpha;
    float waveStrength;
    bool snowOn;
    float3 Lakepadding;
};

cbuffer MaterialConstants : register(b1) {

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

struct PSInput
{
    float4 posProj : SV_POSITION;
    float3 posWorld : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 reflectSamplingPos : TEXCOORD1;
    float3 tangent : TANGENT;
};
 

struct PSOutput {
    float4 pixelColor : SV_Target0;
};

#define terrainScale 20.0


PSOutput main(PSInput input) {

    PSOutput output;

    float2 refractPos;

    float2 reflectPos;

    reflectPos.x =
        input.reflectSamplingPos.x / input.reflectSamplingPos.w * 0.5 + 0.5;
    reflectPos.y =
        -input.reflectSamplingPos.y / input.reflectSamplingPos.w * 0.5 + 0.5;

    refractPos.x = input.posProj.x / input.posProj.w * 0.5 + 0.5;
    refractPos.y = -input.posProj.x / input.posProj.w * 0.5 + 0.5;

    float4 reflectColor = g_reflect.Sample(linearWrapSampler, reflectPos);

    output.pixelColor = reflectColor;
    output.pixelColor.w = lakeAlpha;



    if (TexOnly == 1) {
        //output.pixelColor = g_lakeTex.Sample(linearWrapSampler, input.texcoord);
        float4 color = g_lakeTex.Sample(linearWrapSampler, input.texcoord);
        output.pixelColor = lerp(output.pixelColor, color, 0.5);
    }

    if (snowOn)
    {
        output.pixelColor =
            lerp(output.pixelColor, float4(1.0, 1.0, 1.0, 1.0), 0.2);
    }

float3 sunRadiance = BasicLightRadiance(Sun, input.normal, input.posWorld, ambientC,
                           diffuseC, specularC, SunShadowMap);

    output.pixelColor = output.pixelColor * float4(sunRadiance, 1.0);



    // lake 픽셀 깊이 검사

     float2 CTex;
    CTex.x = abs(-terrainScale - input.posWorld.x) / (terrainScale * 2);

     float tex = terrainScale * (720.0 / 1280.0);
    float cpos = input.posWorld.z * (720.0 / 1280.0);
     CTex.y = (tex - cpos) / (tex * 2.0);
     CTex.y = 1 - CTex.y;
    //float height =
    //    TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;

    //if (characterWorldPos.y - extent < height)
     float height = TerrainPass.SampleLevel(linearWrapSampler, CTex, 0).y;


    //if (input.posWorld.y < height)
    //    output.pixelColor.w = 0.0;

    if (input.posWorld.y < height)
        clip(-1);

    return output;
}