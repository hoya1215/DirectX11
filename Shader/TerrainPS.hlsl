#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D aoTex : register(t2);
Texture2D metallicRoughnessTex : register(t3);
Texture2D albedoTex02 : register(t4);
Texture2D<float4> terrainPixel : register(t5);
Texture2D sandTex : register(t6);
Texture2D grassTex : register(t7);

Texture2D CustomMap : register(t8);

static const float3 Fdielectric = 0.04; 

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

cbuffer TerrainConstantData : register(b1) {
    // matrix view;
    // matrix proj;
    matrix World;
    matrix TerrainView;
    matrix TerrainProj;
    float4 edges;
    float2 inside;
    float heightControlUp;
    float heightControlDown;
    bool Tessellation;
    int TessellationLevel;
    int Texindex;
    int Useheightmap;
    float3 terrainCenter;
    float terrainScale;
    float4 refractPlane;
    bool brush;
    float brushIndex;
    int EditFlag;
    float CorrectionHeight;
    float4 CorrectionAxis;
};

cbuffer TurningPointConstantData : register(b2)
{
    float3 turningPos;
    float turningRadius;
    float turningAngle;
    float3 centerVector;
    float3 startPos;
    int TPpadding;
    float3 endPos;
    int AreaOn;
};



float3 SchlickFresnel(float3 F0, float NdotH) {
    return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
    // return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

struct PixelShaderOutput {
    float4 pixelColor : SV_Target0;
    float4 pixelPos : SV_Target1;
};

float3 GetNormal(PixelShaderInput input) {
    float3 normalWorld = normalize(input.normalWorld);

    if (useNormalMap) // NormalWorld를 교체
    {
        float3 normal =
            normalTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias)
                .rgb;
        normal = 2.0 * normal - 1.0; // 범위 조절 [-1.0, 1.0]

        // OpenGL 용 노멀맵일 경우에는 y 방향을 뒤집어줍니다.
        normal.y = invertNormalMapY ? -normal.y : normal.y;

        float3 N = normalWorld;
        float3 T =
            normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
        float3 B = cross(N, T);

        // matrix는 float4x4, 여기서는 벡터 변환용이라서 3x3 사용
        float3x3 TBN = float3x3(T, B, N);
        normalWorld = normalize(mul(normal, TBN));
    }

    return normalWorld;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic) {
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
    float3 kd = lerp(1.0 - F, 0.0, metallic);
    float3 irradiance =
        irradianceIBLTex.SampleLevel(linearWrapSampler, normalWorld, 0).rgb;

    return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness) {
    float2 specularBRDF =
        brdfTex
            .SampleLevel(linearClampSampler,
                         float2(dot(normalWorld, pixelToEye), 1.0 - roughness),
                         0.0f)
            .rg;
    float3 specularIrradiance =
        specularIBLTex
            .SampleLevel(linearWrapSampler, reflect(-pixelToEye, normalWorld),
                         2 + roughness * 5.0f)
            .rgb;
    const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
    float3 F0 = lerp(Fdielectric, albedo, metallic);

    return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye,
                            float ao, float metallic, float roughness) {
    float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
    float3 specularIBL =
        SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);

    return (diffuseIBL + specularIBL) * ao;
}


float NdfGGX(float NdotH, float roughness) {
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

    return alphaSq / (3.141592 * denom * denom);
}


float SchlickG1(float NdotV, float k) {
    return NdotV / (NdotV * (1.0 - k) + k);
}


float SchlickGGX(float NdotI, float NdotO, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

float random(float3 seed, int i) {
    float4 seed4 = float4(seed, i);
    float dot_product = dot(seed4, float4(12.9898, 78.233, 45.164, 94.673));
    return frac(sin(dot_product) * 43758.5453);
}


float N2V(float ndcDepth, matrix invProj) {
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}

#define NEAR_PLANE 0.1

#define LIGHT_FRUSTUM_WIDTH 0.34641



float PCF_Filter(float2 uv, float zReceiverNdc, float filterRadiusUV,
                 Texture2D shadowMap) {
    float sum = 0.0f;
    for (int i = 0; i < 128; ++i) {
        float2 offset = diskSamples128[i] * filterRadiusUV;
        sum += shadowMap.SampleCmpLevelZero(shadowCompareSampler, uv + offset,
                                            zReceiverNdc);
    }
    return sum / 128;
}



void FindBlocker(out float avgBlockerDepthView, out float numBlockers,
                 float2 uv, float zReceiverView, Texture2D shadowMap,
                 matrix invProj, float lightRadiusWorld) {
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;

    float searchRadius =
        lightRadiusUV * (zReceiverView - NEAR_PLANE) / zReceiverView;

    float blockerSum = 0;
    numBlockers = 0;
    for (int i = 0; i < 128; ++i) {
        float shadowMapDepth =
            shadowMap
                .SampleLevel(shadowPointSampler,
                             float2(uv + diskSamples128[i] * searchRadius), 0)
                .r;

        shadowMapDepth = N2V(shadowMapDepth, invProj);

        if (shadowMapDepth < zReceiverView) {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }
    avgBlockerDepthView = blockerSum / numBlockers;
}

float PCSS(float2 uv, float zReceiverNdc, Texture2D shadowMap, matrix invProj,
           float lightRadiusWorld) {
    float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;

    float zReceiverView = N2V(zReceiverNdc, invProj);


    float avgBlockerDepthView = 0;
    float numBlockers = 0;

    FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap,
                invProj, lightRadiusWorld);

    if (numBlockers < 1) {

        return 1.0f;
    } else {

        float penumbraRatio =
            (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView;
        float filterRadiusUV =
            penumbraRatio * lightRadiusUV * NEAR_PLANE / zReceiverView;


        return PCF_Filter(uv, zReceiverNdc, filterRadiusUV, shadowMap);
    }
}

float3 LightRadiance(Light light, float3 representativePoint, float3 posWorld,
                     float3 normalWorld, Texture2D shadowMap) {
    // Directional light
    float3 lightVec =
        light.type & LIGHT_DIRECTIONAL
            ? -light.direction
            : representativePoint - posWorld; //: light.position - posWorld;

    float lightDist = length(lightVec);
    lightVec /= lightDist;

    // Spot light
    float spotFator =
        light.type & LIGHT_SPOT
            ? pow(max(-dot(lightVec, light.direction), 0.0f), light.spotPower)
            : 1.0f;

    // Distance attenuation
    float att = saturate((light.fallOffEnd - lightDist) /
                         (light.fallOffEnd - light.fallOffStart));

    // Shadow map
    float shadowFactor = 1.0;

    if (light.type & LIGHT_SHADOW) {
        const float nearZ = 0.01; 


        float4 lightScreen = mul(float4(posWorld, 1.0), light.viewProj);
        lightScreen.xyz /= lightScreen.w;


        float2 lightTexcoord = float2(lightScreen.x, -lightScreen.y);
        lightTexcoord += 1.0;
        lightTexcoord *= 0.5;



        uint width, height, numMips;
        shadowMap.GetDimensions(0, width, height, numMips);


        shadowFactor = PCSS(lightTexcoord, lightScreen.z - 0.01, shadowMap,
                            light.invProj, light.radius);
    }

    float3 radiance = light.radiance * spotFator * att * shadowFactor;

    return radiance;
}

PixelShaderOutput main(PixelShaderInput input) {
    float3 pixelToEye = normalize(eyeWorld - input.posWorld);
    float3 normalWorld = GetNormal(input);



    float3 albedo;

    if (useAlbedoMap) {
        if (Texindex == 0) {
            albedo =albedoTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).rgb *albedoFactor;
        } else if (Texindex == 1) {
            albedo =albedoTex02.SampleLevel(linearWrapSampler, input.texcoord, lodBias).rgb *albedoFactor;
        }
    }

    float ao =useAOMap ? aoTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).r : 1.0;

    float metallic = useMetallicMap ? metallicRoughnessTex .SampleLevel(linearWrapSampler, input.texcoord, lodBias).b * metallicFactor : metallicFactor;

    float roughness = useRoughnessMap ? metallicRoughnessTex.SampleLevel(linearWrapSampler, input.texcoord, lodBias).g * roughnessFactor : roughnessFactor;

    float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness) * strengthIBL;


    float3 directLighting = float3(0, 0, 0);


    PixelShaderOutput output;
    output.pixelColor = float4(ambientLighting + directLighting, 1.0);

    if (TexOnly == 1) {
        if (Texindex == 0)
            output.pixelColor = albedoTex.Sample(linearWrapSampler, input.texcoord);

    } else if (TexOnly == 2) {
        output.pixelColor = EditRTV.Sample(linearWrapSampler, input.texcoord);
    }

    // 기존 텍스처 splatting X
    // float texIndex = terrainPixel.Sample(linearWrapSampler,
    // input.texcoord).a;

    // if (texIndex == 0.0)
    //{
    //     output.pixelColor = albedoTex.Sample(linearWrapSampler,
    //     input.texcoord);
    // } else if (texIndex == 1.0)
    //{
    //     output.pixelColor =
    //         sandTex.Sample(linearWrapSampler, input.texcoord);

    //    //float4 Color = albedoTex02.Sample(linearWrapSampler,
    //    input.texcoord);

    //    //output.pixelColor = lerp(output.pixelColor, Color, 0.5);
    //} else if (texIndex == 2.0)
    //{
    //    output.pixelColor = grassTex.Sample(linearWrapSampler,
    //    input.texcoord);
    //} else if (texIndex == 3.0)
    //{
    //    output.pixelColor =
    //        albedoTex02.Sample(linearWrapSampler, input.texcoord);
    //}

    // 텍스처 splatting
    float4 Texweight = terrainPixel.Sample(linearWrapSampler, input.texcoord);

    float4 color1 =
        albedoTex.Sample(linearWrapSampler, input.texcoord) * Texweight.a;
    float4 color2 =
        sandTex.Sample(linearWrapSampler, input.texcoord) * Texweight.r;
    float4 color3 =
        grassTex.Sample(linearWrapSampler, input.texcoord) * Texweight.g;
    float4 color4 =
        albedoTex02.Sample(linearWrapSampler, input.texcoord) * Texweight.b;

    output.pixelColor = color1 + color2 + color3 + color4;

    if (Texindex == 1) // 눈
        output.pixelColor =
            albedoTex02.Sample(linearWrapSampler, input.texcoord);

    float3 sunRadiance =
        BasicLightRadiance(Sun, input.normalWorld, input.posWorld, ambientC,
                           diffuseC, specularC, SunShadowMap);

    // if (sunRadiance.x + sunRadiance.y + sunRadiance.z == 0)
    //     sunRadiance = float3(0.1, 0.1, 0.1);


        output.pixelColor = output.pixelColor * float4(sunRadiance, 1.0);

    // output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
    // output.pixelColor *= 0.1;

    output.pixelColor *= (strengthIBL + 0.5);

    float4 axis = CorrectionAxis;

    if (EditFlag == 2 && axis.x != 100.0)
    {
        if (input.posWorld.x >= axis.x && input.posWorld.x <= axis.y &&
            input.posWorld.z >= axis.z && input.posWorld.z <= axis.w)
            output.pixelColor = float4(0.0, 1.0, 0.0, 1.0);
    } 
    else if (EditFlag == 0 || EditFlag == 1 || EditFlag == 3)
    {
        if (picked == 1) // terrain
        {
            input.posWorld.y = 0.0;

            float dist = length(mousePos - input.posWorld);

            if (dist < radius) {

                if (brush) {

                    switch (brushIndex) {
                    case 0.0:
                        output.pixelColor = lerp(
                            output.pixelColor,
                            albedoTex.Sample(linearWrapSampler, input.texcoord),
                            0.5);
                        break;
                    case 1.0:
                        output.pixelColor = lerp(
                            output.pixelColor,
                            sandTex.Sample(linearWrapSampler, input.texcoord),
                            0.5);
                        break;
                    case 2.0:
                        output.pixelColor = lerp(
                            output.pixelColor,
                            grassTex.Sample(linearWrapSampler, input.texcoord),
                            0.5);
                        break;
                    case 3.0:
                        output.pixelColor =
                            lerp(output.pixelColor,
                                 albedoTex02.Sample(linearWrapSampler,
                                                    input.texcoord),
                                 0.5);
                        break;
                    }

                } else {

                    output.pixelColor = float4(0.0, 1.0, 0.0, 1.0);
                }
            } else {
                output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
            }
        }
    }

    if (EditFlag == 3 && AreaOn == 1) // 터닝포인트 부채꼴 영역 표시
    {
        float3 Tpos = input.posWorld;
        Tpos.y = 0;
        float3 terrainToturn = Tpos - turningPos;

        float lengthR = length(terrainToturn);

        terrainToturn = normalize(terrainToturn);

        float cosT = dot(terrainToturn, centerVector);
        float acosT = acos(cosT);

       float start = dot(startPos, terrainToturn);
       float startT = acos(start);

        if (lengthR <= turningRadius && acosT <= turningAngle * 0.5) {

        if (startT <= 3.14 / 18.0) {
                output.pixelColor = float4(1.0, 0.0, 0.0, 1.0);
            }
        else
            output.pixelColor = float4(0.0, 1.0, 0.0, 1.0);
        }




    }


    return output;
}