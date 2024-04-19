#pragma once

#include "Headers.h"

#define MAX_LIGHTS 1
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10     
 

__declspec(align(256)) struct MeshConstants {
    Matrix world;
    Matrix worldIT;
    int useHeightMap = 0;
    float heightScale = 0.0f;
    float TexMove = 0;
    int clickedOBB = 0;
};


__declspec(align(256)) struct MaterialConstants {

    float ambientC = 0.1f;
    float diffuseC = 0.7f;
    float specularC = 0.0f;
    int materialpadding;

    Vector3 albedoFactor = Vector3(1.0f);
    float roughnessFactor = 1.0f;
    float metallicFactor = 1.0f;
    Vector3 emissionFactor = Vector3(0.0f);

      
    int useAlbedoMap = 0;
    int useNormalMap = 0;
    int useAOMap = 0; 
    int invertNormalMapY = 0;
    int useMetallicMap = 0;
    int useRoughnessMap = 0;
    int useEmissiveMap = 0;
    int pick = 0;

    Vector3 rimColor = {0.f, 1.f, 0.f};
    float rimPower = 9.f;
    float rimStrength = 2.f;
    int Ocean = 0.f;
    int TexOnly = 0.f;
    int padding;

};

struct Light {
    Vector3 radiance = Vector3(5.0f); // strength
    float fallOffStart = 0.0f;
    Vector3 direction = Vector3(0.0f, 0.0f, 1.0f);
    float fallOffEnd = 20.0f;
    Vector3 position = Vector3(0.0f, 0.0f, -2.0f);
    float spotPower = 6.0f;

    // Light type bitmasking
    // ex) LIGHT_SPOT | LIGHT_SHADOW
    uint32_t type = LIGHT_OFF;
    float radius = 0.0f; // 반지름
    
    float haloRadius = 0.0f;
    float haloStrength = 0.0f;

    Matrix viewProj;
    Matrix invProj;
};
  
// register(b1) 사용
__declspec(align(256)) struct GlobalConstants {
    Matrix view;
    Matrix proj;
    Matrix invProj;       
    Matrix viewProj;
    Matrix invViewProj; 
    Matrix OrthoView;
    Matrix OrthoProj; // UI

    Vector3 characterWorldPos;
    bool graphicLower = false;

    Vector3 eyeWorld;
    float strengthIBL = 0.5f;

    int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 0.0f;    // 다른 물체들 LodBias
    float globalTime = 0.0f;

    float thickness = 1.0f;
    int ssao = 0;
    Vector2 globalpadding;

    Light lights[MAX_LIGHTS];
    Light Sun;       
};  
   
// register(b3) 사용, PostEffectsPS.hlsl
__declspec(align(256)) struct PostEffectsConstants {
    int mode = 1; // 1: Rendered image, 2: DepthOnly
    float depthScale = 1.0f;
    float fogStrength = 0.0f;
    int onlySSAO = 0;
};   

// register (b20) 사용 여분const
__declspec(align(256)) struct SurplusConstants {
    
    float cutAwayRadius = 0.0f;
    bool EditScene = false;
    bool SnowOn = false;
    int surplusPadding;
    Matrix TerrainInvViewProj;
    Matrix TerrainViewProj;


};
 

	struct TerrainConstantData {

    Matrix World; 
    Matrix TerrainView;
    Matrix TerrainProj;
    Vector4 edges = Vector4(1.0f);
    Vector2 inside = Vector2(1.0f);
    float heightControlUp = 0.0f;
    float heightControlDown = 0.0f;
    bool Tessellation = true;
    int TessellationLevel = 4;
    int Texindex = 0;
    int Useheightmap = 0;
    Vector3 terrainCenter; 
    float terrainScale;
    Vector4 refractPlane;
    bool brush = false;
    float brushIndex;
    int EditFlag;
    float CorrectionHeight;
    Vector4 CorrectionAxis; // minX , maxX , minZ, maxZ

    };  
     
static_assert((sizeof(TerrainConstantData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

struct LakeConstantData
{
    Matrix reflectProj;
    Matrix depthProj;
    Vector4 edges = Vector4(1.0f);
    Vector2 inside = Vector2(1.0f);
    float lakeAlpha = 0.5f;
    float waveStrength = 0.0f;
    bool snowOn = false;
    Vector3 Lakepadding;
};

struct TerrainPixelInfo 
{
    Vector3 pos;
};

