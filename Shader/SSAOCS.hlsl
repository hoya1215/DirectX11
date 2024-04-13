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
	// ���� ������ ã�� ( �ؽ�ó ��ǥ -> ���̰� ���� -> ���� ��ǥ -> �þ߰������� ��ȯ )
    float2 texcoord = dtID.xy;
    texcoord.x /= width;
    texcoord.y /= height;

    float3 posView = TexcoordToView(texcoord).xyz; 

    // ���� ã��
    float3 n = normalMap.SampleLevel(linearWrapSampler, texcoord, 0).xyz;

    float ssaoSum = 0.0;

    // ������ ã��
    for (int i = 0; i < SampleCount; i++)
    {
        float3 offset = reflect(offSets[i].xyz, normalize(ssaoVec));

        float flip = sign(dot(offset, n));

        float3 ssaoPoint = posView + flip * ssaoRadius * offset;

        // ������ �����ؽ�ó ��ǥ 
        float4 ssaoProj = mul(float4(ssaoPoint, 1.0), proj);
        float2 ssaoTex = ssaoProj.xy;
        ssaoTex.xy /= ssaoProj.w;
        ssaoTex.x = ssaoTex.x * 0.5 + 0.5;
        ssaoTex.y = -ssaoTex.y * 0.5 + 0.5;

        // ������������ �ּ� ���̰� -> �þ� ���� ��ȯ
        float3 ssaoView = TexcoordToView(ssaoTex).xyz;

        // ��������
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



