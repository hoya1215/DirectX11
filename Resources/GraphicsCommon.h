#pragma once

#include "D3D11Utils.h"
#include "GraphicsPSO.h"


// ����: DirectX_Graphic-Samples �̴Ͽ���
// https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/GraphicsCommon.h

namespace Graphics {

// Samplers
extern ComPtr<ID3D11SamplerState> linearWrapSS;
extern ComPtr<ID3D11SamplerState> linearClampSS;
extern ComPtr<ID3D11SamplerState> shadowPointSS;
extern ComPtr<ID3D11SamplerState> shadowCompareSS;
extern vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
extern ComPtr<ID3D11RasterizerState> solidRS;
extern ComPtr<ID3D11RasterizerState> solidCCWRS; // Counter-ClockWise
extern ComPtr<ID3D11RasterizerState> wireRS;
extern ComPtr<ID3D11RasterizerState> wireCCWRS;
extern ComPtr<ID3D11RasterizerState> postProcessingRS;
extern ComPtr<ID3D11RasterizerState> billRS;
extern ComPtr<ID3D11RasterizerState> solidBothRS; // front and back
extern ComPtr<ID3D11RasterizerState> wireBothRS;

// Depth Stencil States
extern ComPtr<ID3D11DepthStencilState> drawDSS; // �Ϲ������� �׸���
extern ComPtr<ID3D11DepthStencilState> maskDepth; // ���ٽ� + ����
extern ComPtr<ID3D11DepthStencilState> maskDSS; // ���ٽǹ��ۿ� ǥ��, ����x
extern ComPtr<ID3D11DepthStencilState> drawMasked; // ���ٽ� ǥ�õ� ����

// Shaders
extern ComPtr<ID3D11VertexShader> basicVS;
extern ComPtr<ID3D11VertexShader> skyboxVS;
extern ComPtr<ID3D11VertexShader> samplingVS;
extern ComPtr<ID3D11VertexShader> normalVS;
extern ComPtr<ID3D11VertexShader> depthOnlyVS;
extern ComPtr<ID3D11VertexShader> BillboardVS;
extern ComPtr<ID3D11VertexShader> terrainVS;
extern ComPtr<ID3D11VertexShader> terrainShadowVS;
extern ComPtr<ID3D11VertexShader> sunVS;
extern ComPtr<ID3D11VertexShader> snowVS;
extern ComPtr<ID3D11VertexShader> rainVS;
extern ComPtr<ID3D11VertexShader> lakeVS;
extern ComPtr<ID3D11VertexShader> skinnedVS;
extern ComPtr<ID3D11VertexShader> skinnedShadowVS;
extern ComPtr<ID3D11VertexShader> uiVS;
extern ComPtr<ID3D11VertexShader> simpleVS;
extern ComPtr<ID3D11VertexShader> silhouetteVS;
  
extern ComPtr<ID3D11PixelShader> basicPS;
extern ComPtr<ID3D11PixelShader> basicCharacterPS;
extern ComPtr<ID3D11PixelShader> basicDeferredPS;
extern ComPtr<ID3D11PixelShader> skyboxPS;
extern ComPtr<ID3D11PixelShader> samplingPS;
extern ComPtr<ID3D11PixelShader> combinePS;
extern ComPtr<ID3D11PixelShader> bloomDownPS;
extern ComPtr<ID3D11PixelShader> bloomUpPS;
extern ComPtr<ID3D11PixelShader> normalPS;
extern ComPtr<ID3D11PixelShader> depthOnlyPS;
extern ComPtr<ID3D11PixelShader> postEffectsPS;   
extern ComPtr<ID3D11PixelShader> BillboardPS;
extern ComPtr<ID3D11PixelShader> terrainPS;
extern ComPtr<ID3D11PixelShader> terrainPassPS;
extern ComPtr<ID3D11PixelShader> sunPS; 
extern ComPtr<ID3D11PixelShader> simplePS;  
extern ComPtr<ID3D11PixelShader> snowPS;
extern ComPtr<ID3D11PixelShader> rainPS;
extern ComPtr<ID3D11PixelShader> lakePS;
extern ComPtr<ID3D11PixelShader> uiPS;
extern ComPtr<ID3D11PixelShader> silhouettePS;
extern ComPtr<ID3D11PixelShader> ssaoNormalPS;

extern ComPtr<ID3D11GeometryShader> BillboardGS;
extern ComPtr<ID3D11GeometryShader> normalGS;
extern ComPtr<ID3D11GeometryShader> snowSBGS;

extern ComPtr<ID3D11HullShader> terrainHS;
extern ComPtr<ID3D11HullShader> terrainShadowHS;
extern ComPtr<ID3D11HullShader> lakeHS;

extern ComPtr<ID3D11DomainShader> terrainDS;
extern ComPtr<ID3D11DomainShader> terrainShadowDS;
extern ComPtr<ID3D11DomainShader> lakeDS;
extern ComPtr<ID3D11DomainShader> lakeShadowDS;
extern ComPtr<ID3D11DomainShader> lakeDepthDS;

// Input Layouts
extern ComPtr<ID3D11InputLayout> basicIL;
extern ComPtr<ID3D11InputLayout> samplingIL;
extern ComPtr<ID3D11InputLayout> skyboxIL;
extern ComPtr<ID3D11InputLayout> postProcessingIL;
extern ComPtr<ID3D11InputLayout> BillboardIL;
extern ComPtr<ID3D11InputLayout> terrainIL;
extern ComPtr<ID3D11InputLayout> snowIL;
extern ComPtr<ID3D11InputLayout> rainIL;
extern ComPtr<ID3D11InputLayout> lakeIL;
extern ComPtr<ID3D11InputLayout> skinnedIL;

// Blend States
extern ComPtr<ID3D11BlendState> mirrorBS;
extern ComPtr<ID3D11BlendState> accumulateBS;
extern ComPtr<ID3D11BlendState> alphaBS;

// Graphics Pipeline States
extern GraphicsPSO defaultSolidPSO;
extern GraphicsPSO defaultWirePSO;
extern GraphicsPSO defaultDeferredPSO;
extern GraphicsPSO stencilMaskPSO;
extern GraphicsPSO reflectSolidPSO;
extern GraphicsPSO reflectWirePSO;
extern GraphicsPSO skyboxSolidPSO;
extern GraphicsPSO skyboxWirePSO;
extern GraphicsPSO reflectSkyboxSolidPSO;
extern GraphicsPSO reflectSkyboxWirePSO;
extern GraphicsPSO normalsPSO;
extern GraphicsPSO depthOnlyPSO;
extern GraphicsPSO postEffectsPSO;
extern GraphicsPSO postProcessingPSO;
extern GraphicsPSO samplingPSO;
extern GraphicsPSO BillboardPSO;
extern GraphicsPSO particlePSO;
extern GraphicsPSO terrainSolidPSO;
extern GraphicsPSO terrainWirePSO;
extern GraphicsPSO reflectTerrainSolidPSO;
extern GraphicsPSO reflectTerrainWirePSO;
extern GraphicsPSO terrainShadowPSO;
extern GraphicsPSO terrainPassPSO;


extern GraphicsPSO sunPSO;
extern GraphicsPSO BoundingBoxPSO;
extern GraphicsPSO snowPSO;
extern GraphicsPSO reflectSnowPSO;
extern GraphicsPSO rainPSO;
extern GraphicsPSO reflectRainPSO;
extern GraphicsPSO lakePSO;
extern GraphicsPSO lakeShadowPSO;
extern GraphicsPSO lakeDepthPSO;
extern GraphicsPSO uiPSO;
extern GraphicsPSO collisionPSO;
extern GraphicsPSO silhouettePSO;

extern GraphicsPSO skinnedSolidPSO;
extern GraphicsPSO reflectSkinnedSolidPSO;
extern GraphicsPSO skinnedShadowPSO;
extern GraphicsPSO skinnedSilhouettePSO;

extern ComPtr<ID3D11ComputeShader> m_InitPixelCS;
extern ComPtr<ID3D11ComputeShader> m_ssrCS;


void InitCommonStates(ComPtr<ID3D11Device> &device);

// ���������� InitCommonStates()���� ���
void InitSamplers(ComPtr<ID3D11Device> &device);
void InitRasterizerStates(ComPtr<ID3D11Device> &device);
void InitBlendStates(ComPtr<ID3D11Device> &device);
void InitDepthStencilStates(ComPtr<ID3D11Device> &device);
void InitPipelineStates(ComPtr<ID3D11Device> &device);
void InitShaders(ComPtr<ID3D11Device> &device);

} 


