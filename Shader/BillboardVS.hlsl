

cbuffer BillboardConstData : register(b0)
{
    matrix view;
    matrix proj;
    float width;
    float3 eyeWorld;
};


struct VSInput
{
    float4 pos : POSITION;
};

struct GSInput
{
    float4 pos : SV_POSITION;
};

GSInput main(VSInput input)
{
    GSInput output;
    output.pos = input.pos;

	return output;
}