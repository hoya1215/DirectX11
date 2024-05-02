#include "Common.hlsli"

cbuffer OffsetVectors : register(b0){
    float4 offSets[14];
    float ssaoRadius;
    float dist;
    float ssaoStart;
    float ssaoEnd;
};

Texture2D depthMap : register(t0);
Texture2D normalMap : register(t1);

RWTexture2D<float4> ssaoUAV : register(u0);

static const float3 ssaoVec = float3(0.0, 0.0, -1.0);
static const int SampleCount = 14;
static const float width = 1280.0;
static const float height = 720.0;

float CalculateOcclusion(float distZ) {
    float occlusion = 0.0;
    if (distZ > dist) {
        float fadelength = ssaoEnd - ssaoStart;

        occlusion = saturate((ssaoEnd - distZ) / fadelength);
    }
    return occlusion;
}

float4 TexcoordToView(float2 texcoord) {
    float4 posProj;

    posProj.xy = texcoord * 2.0 - 1.0;
    posProj.y *= -1; 
    posProj.z = depthMap.SampleLevel(linearClampSampler, texcoord, 0).r;
    posProj.w = 1.0;

    float4 posView = mul(posProj, invProj);
    posView.xyz /= posView.w;

    return posView;
}

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
	// 현재 기준점 찾기 ( 텍스처 좌표 -> 깊이값 추출 -> 투영 좌표 -> 시야공간으로 변환 )
    float2 texcoord = dtID.xy;
    texcoord.x /= width;
    texcoord.y /= height;

    float3 posView = TexcoordToView(texcoord).xyz; 

    // 법선 찾기
    float3 n = normalMap.SampleLevel(linearWrapSampler, texcoord, 0).xyz;

    float ssaoSum = 0.0;

    // 차폐점 찾기
    for (int i = 0; i < SampleCount; i++)
    {
        float3 offset = reflect(offSets[i].xyz, normalize(ssaoVec));

        float flip = sign(dot(offset, n));

        float3 ssaoPoint = posView + flip * ssaoRadius * offset;

        // 차폐점 투영텍스처 좌표 
        float4 ssaoProj = mul(float4(ssaoPoint, 1.0), proj);
        float2 ssaoTex = ssaoProj.xy;
        ssaoTex.xy /= ssaoProj.w;
        ssaoTex.x = ssaoTex.x * 0.5 + 0.5;
        ssaoTex.y = -ssaoTex.y * 0.5 + 0.5;

        // 차폐점에서의 최소 깊이값 -> 시야 공간 변환
        float3 ssaoView = TexcoordToView(ssaoTex).xyz;

        // 차폐판정
        float distZ = posView.z - ssaoView.z;
        float dp = max(dot(n, normalize(ssaoView - posView)), 0.0);
        float occlusion = dp * CalculateOcclusion(distZ);

        ssaoSum += occlusion;
    }

    ssaoSum /= SampleCount;
    float access = 1.0 - ssaoSum;
    access = saturate(pow(access, 2.0f));

    float4 ssao = float4(access, access, access, 1.0);

    ssaoUAV[dtID.xy] = ssao;
}



