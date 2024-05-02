#include "Common.hlsli"

struct GSInput {
    float4 pos : SV_POSITION;
    float radius : PSIZE;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 center : POSITION0;
    float3 posWorld : POSITION1;
    float radius : PSIZE;
    uint primID : SV_PrimitiveID;
};

[maxvertexcount(24)] 
void main(point GSInput input[1], uint primID
     : SV_PrimitiveID, inout TriangleStream<PSInput> outputStream) {

    float r = input[0].radius;



    PSInput output;
    output.radius = r;
    output.center = input[0].pos.xyz;
    output.posWorld = output.center; // dummy
    output.primID = primID;

    // rain
    output.pos = float4(input[0].pos.x - input[0].radius * 0.05,
                        input[0].pos.y - input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

        output.pos = float4(input[0].pos.x - input[0].radius * 0.05,
                        input[0].pos.y + input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

        output.pos = float4(input[0].pos.x + input[0].radius * 0.05,
                        input[0].pos.y + input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

    outputStream.RestartStrip();

        output.pos = float4(input[0].pos.x - input[0].radius * 0.05,
                        input[0].pos.y - input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

        output.pos = float4(input[0].pos.x + input[0].radius * 0.05,
                        input[0].pos.y + input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

        output.pos = float4(input[0].pos.x + input[0].radius * 0.05,
                        input[0].pos.y - input[0].radius, input[0].pos.z, 1.0);
    output.pos = mul(output.pos, viewProj);
    outputStream.Append(output);

    outputStream.RestartStrip();


    // Snow
    //float4 vertices[8] = {float4(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z - input[0].radius, 1.0), 
    //                      float4(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z + input[0].radius, 1.0),
    //                      float4(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z - input[0].radius, 1.0),
    //                      float4(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z + input[0].radius, 1.0),
    //                      float4(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z - input[0].radius, 1.0),
    //                      float4(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z + input[0].radius, 1.0),
    //                      float4(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z - input[0].radius, 1.0),
    //                      float4(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z + input[0].radius, 1.0)};

    //    float3 vertices2[8] = {float3(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z - input[0].radius),
    //                      float3(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z + input[0].radius),
    //                      float3(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z - input[0].radius),
    //                      float3(input[0].pos.x - input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z + input[0].radius),
    //                      float3(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z - input[0].radius),
    //                      float3(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y - input[0].radius,
    //                             input[0].pos.z + input[0].radius),
    //                      float3(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z - input[0].radius),
    //                      float3(input[0].pos.x + input[0].radius,
    //                             input[0].pos.y + input[0].radius,
    //                             input[0].pos.z + input[0].radius)};


    //for (int i = 0; i < 8; i++)
    //{
    //    vertices[i] = mul(vertices[i], viewProj);
    //}

    // // 앞면
    //output.pos = vertices[0];
    //output.posWorld = vertices2[0];
    //outputStream.Append(output);
    //output.pos = vertices[2];
    //output.posWorld = vertices2[2];
    //outputStream.Append(output);
    //output.pos = vertices[6];
    //output.posWorld = vertices2[6];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[0];
    //output.posWorld = vertices2[0];
    //outputStream.Append(output);
    //output.pos = vertices[6];
    //output.posWorld = vertices2[6];
    //outputStream.Append(output);
    //output.pos = vertices[4];
    //output.posWorld = vertices2[4];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //// 뒷면
    //output.pos = vertices[5];
    //output.posWorld = vertices2[5];
    //outputStream.Append(output);
    //output.pos = vertices[7];
    //output.posWorld = vertices2[7];
    //outputStream.Append(output);
    //output.pos = vertices[3];
    //output.posWorld = vertices2[3];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[5];
    //output.posWorld = vertices2[5];
    //outputStream.Append(output);
    //output.pos = vertices[3];
    //output.posWorld = vertices2[3];
    //outputStream.Append(output);
    //output.pos = vertices[1];
    //output.posWorld = vertices2[1];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //// 왼쪽면
    //output.pos = vertices[1];
    //output.posWorld = vertices2[1];
    //outputStream.Append(output);
    //output.pos = vertices[3];
    //output.posWorld = vertices2[3];
    //outputStream.Append(output);
    //output.pos = vertices[2];
    //output.posWorld = vertices2[2];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[1];
    //output.posWorld = vertices2[1];
    //outputStream.Append(output);
    //output.pos = vertices[2];
    //output.posWorld = vertices2[2];
    //outputStream.Append(output);
    //output.pos = vertices[0];
    //output.posWorld = vertices2[0];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //// 오른쪽면
    //output.pos = vertices[4];
    //output.posWorld = vertices2[4];
    //outputStream.Append(output);
    //output.pos = vertices[6];
    //output.posWorld = vertices2[6];
    //outputStream.Append(output);
    //output.pos = vertices[7];
    //output.posWorld = vertices2[7];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[4];
    //output.posWorld = vertices2[4];
    //outputStream.Append(output);
    //output.pos = vertices[7];
    //output.posWorld = vertices2[7];
    //outputStream.Append(output);
    //output.pos = vertices[5];
    //output.posWorld = vertices2[5];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //// 위쪽면
    //output.pos = vertices[2];
    //output.posWorld = vertices2[2];
    //outputStream.Append(output);
    //output.pos = vertices[3];
    //output.posWorld = vertices2[3];
    //outputStream.Append(output);
    //output.pos = vertices[7];
    //output.posWorld = vertices2[7];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[2];
    //output.posWorld = vertices2[2];
    //outputStream.Append(output);
    //output.pos = vertices[7];
    //output.posWorld = vertices2[7];
    //outputStream.Append(output);
    //output.pos = vertices[6];
    //output.posWorld = vertices2[6];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //// 아래쪽면
    //output.pos = vertices[1];
    //output.posWorld = vertices2[1];
    //outputStream.Append(output);
    //output.pos = vertices[0];
    //output.posWorld = vertices2[0];
    //outputStream.Append(output);
    //output.pos = vertices[4];
    //output.posWorld = vertices2[4];
    //outputStream.Append(output);

    //outputStream.RestartStrip();

    //output.pos = vertices[1];
    //output.posWorld = vertices2[1];
    //outputStream.Append(output);
    //output.pos = vertices[4];
    //output.posWorld = vertices2[4];
    //outputStream.Append(output);
    //output.pos = vertices[5];
    //output.posWorld = vertices2[5];
    //outputStream.Append(output);

    //outputStream.RestartStrip();




      
}