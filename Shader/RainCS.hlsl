struct RainStructuredBuffer {
    float3 pos;
    float velocity;
    float radius;
};


RWStructuredBuffer<RainStructuredBuffer> output : register(u0);

[numthreads(1024, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
    //SnowSB s = output[dtID.x];


    if (output[dtID.x].pos.y < -1.0) {

        output[dtID.x].pos.y = 10.0;
    }

    output[dtID.x].pos.y -= output[dtID.x].velocity;

    //output[dtID.x] = s;



}