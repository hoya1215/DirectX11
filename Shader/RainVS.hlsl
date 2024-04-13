
struct GSInput
{
    float4 pos : SV_POSITION;
    float radius : PSIZE;
};

struct RainStructuredBuffer {
    float3 pos;
    float velocity;
    float radius;
};

StructuredBuffer<RainStructuredBuffer> particles : register(t0);

// VSInput이 없이 vertexID만 사용
GSInput main(uint vertexID : SV_VertexID) {


    RainStructuredBuffer s = particles[vertexID];

    GSInput output;

    output.pos = float4(s.pos, 1.0);
    output.radius = s.radius;

    return output;
}