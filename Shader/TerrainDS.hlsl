#include "Common.hlsli"

Texture2D g_height : register(t0);
Texture2D TerrainPass : register(t1);
SamplerState g_sampler : register(s0);

cbuffer MeshConstants : register(b0) {
    matrix world;   
    matrix worldIT; 
    int useHeightMap;
    float heightScale;
    float2 dummy;
};



cbuffer TerrainConstantData : register(b1)
{

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



struct PatchOutput {
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct DSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 texcoord : TEXCOORD;
    float3 tangent : TANGENT;

};

static const float2 offset[8] = {float2(-1, -1), float2(0, -1), float2(1, -1),
                                 float2(-1, 0),  float2(1, 0),  float2(-1, 1),
                                 float2(0, 1),   float2(1, 1)};


[domain("quad")] PixelShaderInput main(PatchOutput input,
	float2 uv : SV_DomainLocation,
	const OutputPatch<DSInput, 4> quad)
{
    PixelShaderInput output;

        float3 n1 = lerp(quad[0].normal, quad[1].normal, uv.x);
    float3 n2 = lerp(quad[2].normal, quad[3].normal, uv.x);
    float3 n = lerp(n1, n2, uv.y);

    output.normalWorld = n;

    output.normalWorld = float4(output.normalWorld, 0.0);
    //output.normalWorld = mul(output.normalWorld, world).xyz;
    //output.normalWorld = mul(output.normalWorld, view).xyz;
    output.normalWorld = mul(output.normalWorld, worldIT).xyz;
    output.normalWorld = normalize(output.normalWorld);

    float2 t1 = lerp(quad[0].texcoord.xy, quad[1].texcoord.xy, uv.x);
    float2 t2 = lerp(quad[2].texcoord.xy, quad[3].texcoord.xy, uv.x);
    float2 t = lerp(t1, t2, uv.y);

    output.texcoord = t;


    float3 p1 = lerp(quad[0].pos, quad[1].pos, uv.x);
    float3 p2 = lerp(quad[2].pos, quad[3].pos, uv.x);
    float3 p = lerp(p1, p2, uv.y);

    output.posWorld = float4(p, 1.0); 

    //output.pos.z = g_height.SampleLevel(g_sampler, t, 0).x * 2.0;




        output.posWorld = mul(output.posWorld, world);

            // Pass 텍스처 이용
        output.posWorld.y +=
            TerrainPass.SampleLevel(g_sampler, output.texcoord, 0).y;

        //int maxheight = 0;

        //for (int i = 0; i < 8; i++)
        //{
        //    float2 tex = offset[i];
        //    tex.x /= 1280.0;
        //    tex.y /= 720.0;
        //    int offsetheight =
        //        TerrainPass.SampleLevel(g_sampler, output.texcoord + tex, 0).y;

        //    maxheight = max(maxheight, offsetheight);
        //}

        //float3 Vec = 

    if (output.posWorld.y > 0.5)
        output.posWorld.y += heightControlUp;

    if (output.posWorld.y < -0.5)
        output.posWorld.y -= heightControlDown;

    if (Useheightmap == 1) // 높이 맵 텍스처로 높이값 불러올때
    {
        output.posWorld.y = g_height.SampleLevel(g_sampler, output.texcoord, 0).r;
    }




    output.posProj = mul(float4(output.posWorld, 1.0), view);
    output.posProj = mul(output.posProj, proj);





    float3 tan1 = lerp(quad[0].tangent, quad[1].tangent, uv.x);
    float3 tan2 = lerp(quad[2].tangent, quad[3].tangent, uv.x);
    float3 tan = lerp(tan1, tan2, uv.y);

    output.tangentWorld = float4(tan, 0.0);
    output.tangentWorld = mul(output.tangentWorld, world).xyz;
    

  

	return output;
}
