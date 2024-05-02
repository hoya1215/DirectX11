#include "Common.hlsli"

cbuffer TerrainConstantData : register(b0) {
    // matrix view;
    // matrix proj;
    matrix World;
    matrix TerrainView;
    matrix TerrainProj;
    float4 edges;
    float2 inside;
    float heightControlUp;
    float heightControlDown;
    bool Tessellation;
    int TessellationLevel;
    int Texindex;
    int Useheightmap;
    float3 terrainCenter;
    float terrainScale;
    float4 refractPlane;
    float brushIndex;
    int EditFlag;
    float CorrectionHeight;
    float4 CorrectionAxis;
};

RWTexture2D<float4> TerrainPass : register(u0);

// texcoord.x /= 1280.0f;
// texcoord.y /= 720.0f;

#define PI 3.14159265358979323846

[numthreads(32, 32, 1)] void main(uint3 groupID
                                  : SV_GroupID, uint3 dtID
                                  : SV_DispatchThreadID) {

    float4 world = TerrainPass[dtID.xy];

    int width, height;
    TerrainPass.GetDimensions(width, height);

    float2 texcoord = dtID.xy;

    texcoord.x /= (float)width;
    texcoord.y /= (float)height;
    texcoord.y = 1.0 - texcoord.y;

    float dist = length(mouseTexcoord - texcoord);
    float range = radius / (terrainScale * 2.0);
    float newDist = sin((PI / 2) + (PI / 2) * (dist / range));

    if (dist < range) {

        if (leftclicked == 1) {
            world.y += heightlength * newDist;
        } else if (rightclicked == 1) {
            world.y -= heightlength * newDist;
        }
    }

    // if (leftclicked == 1) {

    //    if (dist < radius / 40.0) {
    //        world.y += (heightlength * pow(dist, 2));
    //    }
    //}
    // else if (rightclicked == 1)
    //{
    //    if (dist < radius / 40.0)
    //    {
    //        world.y -= (heightlength * pow(dist, 2));
    //    }
    //}

    TerrainPass[dtID.xy] = world;







}