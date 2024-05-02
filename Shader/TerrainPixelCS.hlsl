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
    bool brush;
    float brushIndex;
    int EditFlag;
    float CorrectionHeight;
    float4 CorrectionAxis;
};

RWTexture2D<float4> TerrainPixel : register(u0);

[numthreads(32, 32, 1)] void main(uint3 groupID : SV_GroupID, uint3 dtID
                                  : SV_DispatchThreadID) {

            float2 texcoord = dtID.xy;
    texcoord.x /= 1280.0f;
    texcoord.y /= 720.0f;
    texcoord.y = 1.0 - texcoord.y;

    float2 center = float2(0.5, 0.5);

    float dist = length(mouseTexcoord - texcoord);
    float4 color = TerrainPixel[dtID.xy];

    if (leftclicked == 1) {


        if (dist < radius / (terrainScale * 2.0)) {
            // 기존
            //TerrainPixel[dtID.xy] = float4(0.0, 0.0, 0.0, brushIndex);


            // 변경
            if (brushIndex == 1)
            {
                color.x += 0.01;
            }
            else if (brushIndex == 2)
            {
                color.y += 0.01;
            }
            else if (brushIndex == 3)
            {
                color.z += 0.01;
            }
            else
            {
                color.a += 0.01;
            }
        }
    }
    else if (rightclicked == 1) {

        if (dist < radius / (terrainScale * 2.0)) {
            // 기존
            // TerrainPixel[dtID.xy] = float4(0.0, 0.0, 0.0, brushIndex);

            // 변경
            if (brushIndex == 1) {
                color.x -= 0.01;
            } else if (brushIndex == 2) {
                color.y -= 0.01;
            } else if (brushIndex == 3) {
                color.z -= 0.01;
            } else {
                color.a -= 0.01;
            }
        }
    }

    TerrainPixel[dtID.xy] = color;

}