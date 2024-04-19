#include "Common.hlsli"

struct LakeEffect
{
    float3 pos;
    float maxRadius;
    float nowRadius;
};

RWStructuredBuffer<LakeEffect> lakeEffect : register(u0);

[numthreads(1000, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{

    lakeEffect[dtID.x].nowRadius += globalTime;

    //if (lakeEffect[dtID.x].nowRadius >= lakeEffect[dtID.x].maxRadius)
    //    lakeEffect[dtID.x].nowRadius = 0.0;

}