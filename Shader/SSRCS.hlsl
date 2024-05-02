#include "Common.hlsli"

cbuffer SSConstantData : register(b0)
{
    float MaxCount;
    float RayDist;
    float ZDist;
    float padding;
}

Texture2D RenderTex : register(t0);
Texture2D positionTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D depthTex : register(t3); 

RWTexture2D<float4> SSRmap : register(u0);

static const float width = 1280.0;
static const float height = 720.0;
//static const int MaxCount = 2;

float4 TexcoordToView(float2 texcoord) {
    float4 posProj;

    posProj.xy = texcoord * 2.0 - 1.0;
    posProj.y *= -1;
    posProj.z = depthTex.SampleLevel(linearClampSampler, texcoord, 0).r;
    posProj.w = 1.0;

    float4 posView = mul(posProj, invProj);
    posView.xyz /= posView.w;

    return posView;
}

float3 BinarySearch(float3 pos, float3 dir) {
    float3 start = pos - dir;
    float3 end = pos;
    int count = 0;

    while (start.z < end.z)
    {
        float3 middle = (start + end) * 0.5;
        float4 posProj = mul(float4(middle, 1.0), proj);
        posProj.xyz /= posProj.w;
        float2 tex = posProj.xy;
        tex.x = tex.x * 0.5 + 0.5;
        tex.y = -tex.y * 0.5 + 0.5;

        float4 position = positionTex.SampleLevel(linearWrapSampler, tex, 0);
        position = mul(position, viewProj);
        position.xyz /= position.w;
        float dist = posProj.z - position.z;
        if (dist >= 0)
        {
            end = middle;
        } else
            start = middle;

        count++;
        if (count > 10)
            break;
    }

    float4 p = mul(float4((start + end) / 2, 1.0), proj);
    p.xyz /= p.w;
    float2 t = p.xy;
    p.x = p.x * 0.5 + 0.5;
    p.y = -p.y * 0.5 + 0.5;
    float3 color = RenderTex.SampleLevel(linearWrapSampler, t, 0).rgb;

    return color;
}


float3 RayMarching(float3 start, float3 dir)
{

    float3 color = float3(0.0, 0.0, 0.0);
    float count = 0;
    float3 end = start;

    // DDA 사용 RayMarching
    float4 start2 = float4(start, 1.0);
    start2 = mul(start2, proj);
    start2.xyz /= start2.w;
    start2.x = start2.x * 0.5 + 0.5;
    start2.y = -start2.y * 0.5 + 0.5;
   // start2.x *= width;
    //start2.y *= height;

    float4 end2 = float4(start + dir * MaxCount, 1.0);
    end2 = mul(end2, proj);
    end2.xyz /= end2.w;
    end2.x = end2.x * 0.5 + 0.5;
    end2.y = -end2.y * 0.5 + 0.5;
    //end2.x *= width;
    //end2.y *= height;

    float3 dp = end2.xyz - start2.xyz;
    //if (abs(dp.x) >= abs(dp.y))
    //{  
    //    float maxD = dp.x;
    //    dp /= maxD;
    //}
    //else
    //{
    //    float maxD = dp.y;
    //   // dp = float3(maxD / dp.x, 1.0, dp.z / maxD);
    //    dp /= maxD;
    //}
    float maxD = max(abs(dp.x), abs(dp.y));
    dp *= maxD;
    dp *= RayDist;

    for (int i = 1; i < MaxCount; i++)
    {
        float3 pos = start2.xyz + dp * i;
        //pos.y = round(pos.y + 0.5);
        //pos.x /= width;
        //pos.y /= height;

        float depth = depthTex.SampleLevel(linearWrapSampler, pos.xy, 0).r;
        float4 posW = positionTex.SampleLevel(linearWrapSampler, pos.xy, 0);
        float dist = pos.z - depth;
        if (posW.w > 0.0 && dist > 0.001 && dist < ZDist)
        {
            color = RenderTex.SampleLevel(linearWrapSampler, pos.xy, 0).rgb;
            return color;
        }
    }


    // View 공간 Ray Marching
    //for (int i = 1; i < MaxCount; i++) {
    //    end = start + dir * i;
    //
 
    //    // 시야공간 -> 텍스처좌표로 변환하여 position 맵 샘플링
    //    float4 endProj = mul(float4(end, 1.0), proj);
    //    endProj.xyz /= endProj.w;
    //    //float2 startTex = endProj.xy * 0.5 + 0.5;
    //    float2 startTex = endProj.xy;
    //    startTex.x = startTex.x * 0.5 + 0.5;
    //    startTex.y = -startTex.y * 0.5 + 0.5; 

    //    float4 position = positionTex.SampleLevel(linearWrapSampler, startTex, 0);
    //    position = mul(position, viewProj);
    //    position.xyz /= position.w;

    //    float depth = depthTex.SampleLevel(linearWrapSampler, startTex, 0).r;

    //    float dist = endProj.z - position.z;
    //    if (position.w > 0.0 && dist >= 0.0 ) {
    //        color = RenderTex.SampleLevel(linearWrapSampler, startTex, 0).rgb;
    //        //color = float3(count / 10, count / 10, count / 10);
    //        //color = BinarySearch(end, dir);
    //        return color;
    //    }
    //    count++;

    //        
    //}
    return color;
}

[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
	// 현재 텍스처좌표 -> 시야공간으로 변환 , 법선값 -> 시야공간으로 변환
    float2 texcoord = dtID.xy;
    texcoord.x /= width;
    texcoord.y /= height;

    float4 posW = positionTex.SampleLevel(linearWrapSampler, texcoord, 0);
    float3 View = TexcoordToView(texcoord).xyz;

    //float4 posView = mul(posW, view);
    //float3 View = posView.xyz / posView.w;

    float3 startPos = View.xyz;
    View = normalize(View);
    float3 n = normalTex.SampleLevel(linearWrapSampler, texcoord, 0).xyz;
   //n = mul(float4(n, 1.0), view).xyz;
    //n = float3(0.0, 1.0, 0.0);
    n = normalize(n);
    

    // 반사 벡터 구하기
    float3 reflectVec = reflect(View, n);
    reflectVec = normalize(reflectVec);
    
    // 충돌 검사
    //reflectVec *= 0.1;
    float3 color = float3(0.0, 0.0, 0.0);


    

    if (posW.w > 0.0)
        color = RayMarching(startPos, reflectVec);


    SSRmap[dtID.xy] = float4(color, 0.0);
}