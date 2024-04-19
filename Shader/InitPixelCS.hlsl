RWTexture2D<float4> g_texture : register(u0);


[numthreads(32, 32, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    uint2 texcoord = uint2(dtID.xy);

    g_texture[texcoord] = float4(0.0, 0.0, 0.0, 1.0);

}