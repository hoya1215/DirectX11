#include "Common.hlsli" // ���̴������� include ��� ����

Texture2D g_heightTexture : register(t0);

struct HeightInfo {
    float3 pos;
    int index;
};

StructuredBuffer<HeightInfo> outputHeight : register(t1);

cbuffer MeshConstants : register(b0) {
    matrix world;   // Model(�Ǵ� Object) ��ǥ�� -> World�� ��ȯ
    matrix worldIT; // World�� InverseTranspose
    int useHeightMap;
    float heightScale;
    float TexMove;
    int dummy;
};

cbuffer TerrainConstantData : register(b1) {
    // matrix view;
    // matrix proj;
    matrix World;
    float4 edges;
    float2 inside;
    float heightControlUp;
    float heightControlDown;
    bool Tessellation;
    int TessellationLevel;
    int Texindex;
    int Useheightmap;
    float3 terrainCenter;
    float4 refractPlane;
};

struct VSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

struct HSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

HSInput main(VSInput input) {
    HSInput output;
    output.pos = input.pos;

    output.pos = outputHeight[input.texcoord.z].pos;
    

    float3 posWorld = mul(float4(output.pos, 1.0), world).xyz;
    float3 n = refractPlane.xyz;
    float clipValue = dot(n, posWorld) + refractPlane.w;

    //if (clipValue > 0.0) {
    //    clip(-1);
    //}

    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.tangent = input.tangent;

    return output;
}