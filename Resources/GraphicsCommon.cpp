#include "GraphicsCommon.h"

namespace Graphics {

// Sampler States
ComPtr<ID3D11SamplerState> linearWrapSS;
ComPtr<ID3D11SamplerState> linearClampSS;
ComPtr<ID3D11SamplerState> shadowPointSS;
ComPtr<ID3D11SamplerState> shadowCompareSS;
vector<ID3D11SamplerState *> sampleStates;

// Rasterizer States
ComPtr<ID3D11RasterizerState> solidRS;
ComPtr<ID3D11RasterizerState> solidCCWRS;
ComPtr<ID3D11RasterizerState> wireRS;
ComPtr<ID3D11RasterizerState> wireCCWRS;
ComPtr<ID3D11RasterizerState> postProcessingRS;
ComPtr<ID3D11RasterizerState> billRS;
ComPtr<ID3D11RasterizerState> solidBothRS; // front and back
ComPtr<ID3D11RasterizerState> wireBothRS;

// Depth Stencil States
ComPtr<ID3D11DepthStencilState> drawDSS;       // 일반적으로 그리기
ComPtr<ID3D11DepthStencilState> maskDepth;
ComPtr<ID3D11DepthStencilState> maskDSS;       // 스텐실버퍼에 표시
ComPtr<ID3D11DepthStencilState> drawMasked; // 스텐실 표시된 곳만

// Blend States
ComPtr<ID3D11BlendState> mirrorBS;
ComPtr<ID3D11BlendState> accumulateBS;
ComPtr<ID3D11BlendState> alphaBS;

// Shaders
ComPtr<ID3D11VertexShader> basicVS;
ComPtr<ID3D11VertexShader> skyboxVS;
ComPtr<ID3D11VertexShader> samplingVS;
ComPtr<ID3D11VertexShader> normalVS;
ComPtr<ID3D11VertexShader> depthOnlyVS;
ComPtr<ID3D11VertexShader> BillboardVS;
ComPtr<ID3D11VertexShader> terrainVS;
ComPtr<ID3D11VertexShader> terrainShadowVS;
ComPtr<ID3D11VertexShader> sunVS;
ComPtr<ID3D11VertexShader> snowVS;
ComPtr<ID3D11VertexShader> rainVS;
ComPtr<ID3D11VertexShader> lakeVS;
ComPtr<ID3D11VertexShader> skinnedVS;
ComPtr<ID3D11VertexShader> skinnedShadowVS;
ComPtr<ID3D11VertexShader> uiVS;
ComPtr<ID3D11VertexShader> simpleVS;
ComPtr<ID3D11VertexShader> silhouetteVS;

ComPtr<ID3D11PixelShader> basicPS;
ComPtr<ID3D11PixelShader> basicCharacterPS;
ComPtr<ID3D11PixelShader> skyboxPS;
ComPtr<ID3D11PixelShader> samplingPS;
ComPtr<ID3D11PixelShader> combinePS;
ComPtr<ID3D11PixelShader> bloomDownPS;
ComPtr<ID3D11PixelShader> bloomUpPS;
ComPtr<ID3D11PixelShader> normalPS;
ComPtr<ID3D11PixelShader> depthOnlyPS;
ComPtr<ID3D11PixelShader> postEffectsPS;
ComPtr<ID3D11PixelShader> BillboardPS;
ComPtr<ID3D11PixelShader> terrainPS;
ComPtr<ID3D11PixelShader> terrainPassPS;
ComPtr<ID3D11PixelShader> sunPS;
ComPtr<ID3D11PixelShader> simplePS;
ComPtr<ID3D11PixelShader> snowPS;
ComPtr<ID3D11PixelShader> rainPS;
ComPtr<ID3D11PixelShader> lakePS;
ComPtr<ID3D11PixelShader> uiPS;
ComPtr<ID3D11PixelShader> silhouettePS;
ComPtr<ID3D11PixelShader> ssaoNormalPS;

ComPtr<ID3D11GeometryShader> BillboardGS;
ComPtr<ID3D11GeometryShader> normalGS;
ComPtr<ID3D11GeometryShader> snowSBGS;

ComPtr<ID3D11HullShader> terrainHS;
ComPtr<ID3D11HullShader> terrainShadowHS;
ComPtr<ID3D11HullShader> lakeHS;

ComPtr<ID3D11DomainShader> terrainDS;
ComPtr<ID3D11DomainShader> terrainShadowDS;
ComPtr<ID3D11DomainShader> lakeDS;
ComPtr<ID3D11DomainShader> lakeShadowDS;
ComPtr<ID3D11DomainShader> lakeDepthDS;

// Input Layouts
ComPtr<ID3D11InputLayout> basicIL;
ComPtr<ID3D11InputLayout> samplingIL;
ComPtr<ID3D11InputLayout> skyboxIL;
ComPtr<ID3D11InputLayout> postProcessingIL;
ComPtr<ID3D11InputLayout> BillboardIL;
ComPtr<ID3D11InputLayout> terrainIL;
ComPtr<ID3D11InputLayout> snowIL;
ComPtr<ID3D11InputLayout> rainIL;
ComPtr<ID3D11InputLayout> lakeIL;
ComPtr<ID3D11InputLayout> skinnedIL;

// Graphics Pipeline States
GraphicsPSO defaultSolidPSO;
GraphicsPSO defaultWirePSO;
GraphicsPSO stencilMaskPSO;
GraphicsPSO reflectSolidPSO;
GraphicsPSO reflectWirePSO;
GraphicsPSO skyboxSolidPSO;
GraphicsPSO skyboxWirePSO;
GraphicsPSO reflectSkyboxSolidPSO;
GraphicsPSO reflectSkyboxWirePSO;
GraphicsPSO normalsPSO;
GraphicsPSO depthOnlyPSO;
GraphicsPSO postEffectsPSO;
GraphicsPSO postProcessingPSO;
GraphicsPSO samplingPSO;
GraphicsPSO BillboardPSO;
GraphicsPSO particlePSO;
GraphicsPSO terrainSolidPSO;
GraphicsPSO terrainWirePSO;
GraphicsPSO reflectTerrainSolidPSO;
GraphicsPSO reflectTerrainWirePSO;
GraphicsPSO terrainShadowPSO;
GraphicsPSO terrainPassPSO;
GraphicsPSO sunPSO;
GraphicsPSO BoundingBoxPSO;
GraphicsPSO snowPSO;
GraphicsPSO reflectSnowPSO;
GraphicsPSO rainPSO;
GraphicsPSO reflectRainPSO;
GraphicsPSO lakePSO;
GraphicsPSO lakeShadowPSO;
GraphicsPSO lakeDepthPSO;
GraphicsPSO uiPSO;
GraphicsPSO collisionPSO;
GraphicsPSO silhouettePSO;

GraphicsPSO skinnedSolidPSO;
GraphicsPSO reflectSkinnedSolidPSO;
GraphicsPSO skinnedShadowPSO;
GraphicsPSO skinnedSilhouettePSO;

ComPtr<ID3D11ComputeShader> m_InitPixelCS;

} 

void Graphics::InitCommonStates(ComPtr<ID3D11Device> &device) {

    InitShaders(device);
    InitSamplers(device);
    InitRasterizerStates(device);
    InitBlendStates(device);
    InitDepthStencilStates(device);
    InitPipelineStates(device);
}
 
void Graphics::InitSamplers(ComPtr<ID3D11Device> &device) {
     
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, linearWrapSS.GetAddressOf());
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    device->CreateSamplerState(&sampDesc, linearClampSS.GetAddressOf());

    // shadowPointSS
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.BorderColor[0] = 1.0f; // 큰 Z값
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    device->CreateSamplerState(&sampDesc, shadowPointSS.GetAddressOf());


    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sampDesc.BorderColor[0] = 100.0f; // 큰 Z값
    sampDesc.Filter =
        D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    device->CreateSamplerState(&sampDesc, shadowCompareSS.GetAddressOf());


    sampleStates.push_back(linearWrapSS.Get());
    sampleStates.push_back(linearClampSS.Get());
    sampleStates.push_back(shadowPointSS.Get());
    sampleStates.push_back(shadowCompareSS.Get());
}

void Graphics::InitRasterizerStates(ComPtr<ID3D11Device> &device) {

    // Rasterizer States
    D3D11_RASTERIZER_DESC rastDesc;
    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidRS.GetAddressOf()));

    // 거울에 반사되면 삼각형의 Winding이 바뀌기 때문에 CCW로 그려야함
    rastDesc.FrontCounterClockwise = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidCCWRS.GetAddressOf()));

    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, wireCCWRS.GetAddressOf()));
     
    rastDesc.FrontCounterClockwise = false;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, wireRS.GetAddressOf()));

    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = false;
    ThrowIfFailed(device->CreateRasterizerState(
        &rastDesc, postProcessingRS.GetAddressOf()));

    rastDesc.DepthClipEnable = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, billRS.GetAddressOf()));

    ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
    rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE; // 양면
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = true;
    ThrowIfFailed(
        device->CreateRasterizerState(&rastDesc, solidBothRS.GetAddressOf()));

        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME; // 양면, Wire
    ThrowIfFailed(device->CreateRasterizerState(&rastDesc,
                                                    wireBothRS.GetAddressOf()));

}

void Graphics::InitBlendStates(ComPtr<ID3D11Device> &device) {

    // "이미 그려져있는 화면"과 어떻게 섞을지를 결정
    // Dest: 이미 그려져 있는 값들을 의미
    // Src: 픽셀 쉐이더가 계산한 값들을 의미 

    D3D11_BLEND_DESC mirrorBlendDesc;
    ZeroMemory(&mirrorBlendDesc, sizeof(mirrorBlendDesc));
    mirrorBlendDesc.AlphaToCoverageEnable = true; // MSAA
    mirrorBlendDesc.IndependentBlendEnable = false;
    // 개별 RenderTarget에 대해서 설정 (최대 8개)
    mirrorBlendDesc.RenderTarget[0].BlendEnable = true;
    mirrorBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
    mirrorBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    mirrorBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    mirrorBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    // 필요하면 RGBA 각각에 대해서도 조절 가능
    mirrorBlendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(
        device->CreateBlendState(&mirrorBlendDesc, mirrorBS.GetAddressOf()));

        D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = true; // MSAA
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR; // INV 아님
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;
    ThrowIfFailed(
        device->CreateBlendState(&blendDesc, accumulateBS.GetAddressOf()));

    


    D3D11_BLEND_DESC alphablendDesc;
    ZeroMemory(&alphablendDesc, sizeof(alphablendDesc));
    alphablendDesc.RenderTarget[0].BlendEnable = true;
    alphablendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    alphablendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    alphablendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    alphablendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    alphablendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    alphablendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    alphablendDesc.RenderTarget[0].RenderTargetWriteMask =
        D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(
        device->CreateBlendState(&alphablendDesc, alphaBS.GetAddressOf()));
}

void Graphics::InitDepthStencilStates(ComPtr<ID3D11Device> &device) {


    // StencilPassOp : 둘 다 pass일 때 할 일
    // StencilDepthFailOp : Stencil pass, Depth fail 일 때 할 일
    // StencilFailOp : 둘 다 fail 일 때 할 일

    // m_drawDSS: 기본 DSS
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = false; // Stencil 불필요
    dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
    dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, drawDSS.GetAddressOf()));

    // Stencil에 1로 표기해주는 DSS , 깊이 o
    dsDesc.DepthEnable = true; // 이미 그려진 물체 유지
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = true;    // Stencil 필수   
    dsDesc.StencilReadMask = 0xFF;  // 모든 비트 다 사용
    dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    //dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, maskDepth.GetAddressOf()));

    // Stencil에 1로 표기해주는 DSS , 깊이 X
    dsDesc.DepthEnable = false; // 이미 그려진 물체 유지
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = true;    // Stencil 필수
    dsDesc.StencilReadMask = 0xFF;  // 모든 비트 다 사용
    dsDesc.StencilWriteMask = 0xFF; // 모든 비트 다 사용
    // 앞면에 대해서 어떻게 작동할지 설정
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, maskDSS.GetAddressOf()));

    // Stencil에 1로 표기된 경우에"만" 그리는 DSS
    // DepthBuffer는 초기화된 상태로 가정
    // D3D11_COMPARISON_EQUAL 이미 1로 표기된 경우에만 그리기
    // OMSetDepthStencilState(..., 1); <- 여기의 1
    dsDesc.DepthEnable = false;   
    dsDesc.StencilEnable = true; // Stencil 사용
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_EQUAL; // <- 주의
    dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

    ThrowIfFailed(
        device->CreateDepthStencilState(&dsDesc, drawMasked.GetAddressOf()));
}

void Graphics::InitShaders(ComPtr<ID3D11Device> &device) {

    // Shaders, InputLayouts

    vector<D3D11_INPUT_ELEMENT_DESC> basicIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> samplingIED = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> skyboxIE = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    vector<D3D11_INPUT_ELEMENT_DESC> BillboardIE = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };


    

    vector<D3D11_INPUT_ELEMENT_DESC> terrainIES = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
     D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
     D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,
     D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36,
     D3D11_INPUT_PER_VERTEX_DATA, 0} };

     vector<D3D11_INPUT_ELEMENT_DESC> SnowIE = {
     {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
     0,                                 // Slot 1, 0부터 시작
     D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
    {"WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
     D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
    {"WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,
     D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
    {"WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,
     D3D11_INPUT_PER_INSTANCE_DATA, 1}, // 마지막 1은 instance step
    {"COLOR", 0, DXGI_FORMAT_R32_FLOAT, 1, 64,
     D3D11_INPUT_PER_INSTANCE_DATA, 1}
     };

    vector<D3D11_INPUT_ELEMENT_DESC> rainIE = {
         {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D11_INPUT_PER_VERTEX_DATA, 0},
     };

    vector<D3D11_INPUT_ELEMENT_DESC> lakeIE = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0}};

        vector<D3D11_INPUT_ELEMENT_DESC> skinnedIEs = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 76,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BLENDINDICES", 1, DXGI_FORMAT_R8G8B8A8_UINT, 0, 80,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"BasicVS.hlsl", basicIEs, basicVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"NormalVS.hlsl",
                                                 basicIEs, normalVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"SamplingVS.hlsl", samplingIED, samplingVS, samplingIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SkyboxVS.hlsl",
                                                 skyboxIE, skyboxVS, skyboxIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"DepthOnlyVS.hlsl", basicIEs, depthOnlyVS, skyboxIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"BillboardVS.hlsl", BillboardIE, BillboardVS, BillboardIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"TerrainVS.hlsl", terrainIES, terrainVS, terrainIL);

    
    D3D11Utils::CreateVertexShaderAndInputLayout(
            device, L"TerrainShadowVS.hlsl", terrainIES, terrainShadowVS,
            terrainIL);

     


    D3D11Utils::CreateVertexShaderAndInputLayout(
            device, L"SunVS.hlsl", basicIEs, sunVS, basicIL);


    D3D11Utils::CreateVertexShaderAndInputLayout(
                device, L"SnowVS.hlsl", SnowIE, snowVS, snowIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"rainVS.hlsl",
                                                 rainIE, rainVS, rainIL);

    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"LakeVS.hlsl", lakeIE,
                                                 lakeVS, lakeIL);
 
     
     D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"BasicVS.hlsl", skinnedIEs, skinnedVS, skinnedIL,
        vector<D3D_SHADER_MACRO>{{"SKINNED", "1"}, {NULL, NULL}});

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"DepthOnlyVS.hlsl", skinnedIEs, skinnedShadowVS, skinnedIL,
        vector<D3D_SHADER_MACRO>{{"SKINNED", "1"}, {NULL, NULL}});
    
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"UIVS.hlsl", basicIEs,
                                                 uiVS, basicIL);
    D3D11Utils::CreateVertexShaderAndInputLayout(device, L"SimpleVS.hlsl",
                                                 basicIEs, simpleVS, basicIL);

     D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"SilhouetteVS.hlsl", skinnedIEs, silhouetteVS, skinnedIL,
        vector<D3D_SHADER_MACRO>{{"SKINNED", "1"}, {NULL, NULL}});
       
    D3D11Utils::CreatePixelShader(
         device, L"BasicPS.hlsl", basicPS,
         vector<D3D_SHADER_MACRO>{{"OBJECT", "1"}, {NULL, NULL}});
    D3D11Utils::CreatePixelShader(device, L"BasicPS.hlsl", basicCharacterPS);
    D3D11Utils::CreatePixelShader(device, L"NormalPS.hlsl", normalPS);
    D3D11Utils::CreatePixelShader(device, L"SkyboxPS.hlsl", skyboxPS);
    D3D11Utils::CreatePixelShader(device, L"SamplingPS.hlsl", samplingPS);
    D3D11Utils::CreatePixelShader(device, L"CombinePS.hlsl", combinePS);
    D3D11Utils::CreatePixelShader(device, L"BloomDownPS.hlsl", bloomDownPS);
    D3D11Utils::CreatePixelShader(device, L"BloomUpPS.hlsl", bloomUpPS);
    D3D11Utils::CreatePixelShader(device, L"DepthOnlyPS.hlsl", depthOnlyPS);
    D3D11Utils::CreatePixelShader(device, L"PostEffectsPS.hlsl", postEffectsPS);
    D3D11Utils::CreatePixelShader(device, L"BillboardPS.hlsl", BillboardPS);
    D3D11Utils::CreatePixelShader(device, L"TerrainPS.hlsl", terrainPS);
    D3D11Utils::CreatePixelShader(device, L"TerrainPassPS.hlsl", terrainPassPS);

    D3D11Utils::CreatePixelShader(device, L"SunPS.hlsl", sunPS);
    D3D11Utils::CreatePixelShader(device, L"SimplePS.hlsl", simplePS);
    D3D11Utils::CreatePixelShader(device, L"SnowPS.hlsl", snowPS);
    D3D11Utils::CreatePixelShader(device, L"rainPS.hlsl", rainPS);
    D3D11Utils::CreatePixelShader(device, L"LakePS.hlsl", lakePS);
    D3D11Utils::CreatePixelShader(device, L"UIPS.hlsl", uiPS);
    D3D11Utils::CreatePixelShader(device, L"SilhouettePS.hlsl", silhouettePS);
    D3D11Utils::CreatePixelShader(device, L"SSAONormalPS.hlsl", ssaoNormalPS);
     
    D3D11Utils::CreateGeometryShader(device, L"BillboardGS.hlsl", BillboardGS);
    D3D11Utils::CreateGeometryShader(device, L"NormalGS.hlsl", normalGS);
    D3D11Utils::CreateGeometryShader(device, L"rainGS.hlsl", snowSBGS);

    D3D11Utils::CreateHullShader(device, L"TerrainHS.hlsl", terrainHS);
    D3D11Utils::CreateHullShader(device, L"TerrainShadowHS.hlsl", 
                                 terrainShadowHS);
    D3D11Utils::CreateHullShader(device, L"LakeHS.hlsl", lakeHS);
    D3D11Utils::CreateDomainShader(device, L"TerrainDS.hlsl", terrainDS);
    D3D11Utils::CreateDomainShader(device, L"TerrainShadowDS.hlsl",
                                   terrainShadowDS);
    D3D11Utils::CreateDomainShader(device, L"LakeDS.hlsl", lakeDS);
    D3D11Utils::CreateDomainShader(device, L"LakeShadowDS.hlsl", lakeShadowDS);
    D3D11Utils::CreateDomainShader(device, L"LakeDepthDS.hlsl", lakeDepthDS);

    D3D11Utils::CreateComputeShader(device, L"InitPixelCS.hlsl", m_InitPixelCS);
}
 
void Graphics::InitPipelineStates(ComPtr<ID3D11Device> &device) {

    // defaultSolidPSO;
    defaultSolidPSO.m_vertexShader = basicVS;
    defaultSolidPSO.m_inputLayout = basicIL;
    defaultSolidPSO.m_pixelShader = basicPS;
    defaultSolidPSO.m_rasterizerState = solidRS;
    defaultSolidPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    defaultSolidPSO.m_blendState = alphaBS;

      

    // Skinned mesh solid
    skinnedSolidPSO = defaultSolidPSO;
    skinnedSolidPSO.m_vertexShader = skinnedVS;
    skinnedSolidPSO.m_inputLayout = skinnedIL;
    skinnedSolidPSO.m_pixelShader = basicCharacterPS;

    // 실루엣  
    skinnedSilhouettePSO = skinnedSolidPSO;
    skinnedSilhouettePSO.m_pixelShader = silhouettePS;
    skinnedSilhouettePSO.m_blendState = alphaBS;

    silhouettePSO = skinnedSilhouettePSO;
    silhouettePSO.m_vertexShader = silhouetteVS;
 

    // defaultWirePSO
    defaultWirePSO = defaultSolidPSO;
    defaultWirePSO.m_rasterizerState = wireRS;

    // stencilMarkPSO;
    stencilMaskPSO = defaultSolidPSO;
    stencilMaskPSO.m_depthStencilState = maskDSS;
    stencilMaskPSO.m_stencilRef = 1;
    stencilMaskPSO.m_vertexShader = depthOnlyVS;
    stencilMaskPSO.m_pixelShader = depthOnlyPS;

    // reflectSolidPSO: 반사되면 Winding 반대
    reflectSolidPSO = defaultSolidPSO;
    //reflectSolidPSO.m_depthStencilState = drawMaskedDSS;
    reflectSolidPSO.m_rasterizerState = solidCCWRS; // 반시계
    //reflectSolidPSO.m_stencilRef = 1;

    reflectSkinnedSolidPSO = reflectSolidPSO;
    reflectSkinnedSolidPSO.m_vertexShader = skinnedVS;
    reflectSkinnedSolidPSO.m_inputLayout = skinnedIL;
    reflectSkinnedSolidPSO.m_pixelShader = basicCharacterPS;

    // reflectWirePSO: 반사되면 Winding 반대
    reflectWirePSO = reflectSolidPSO;
    reflectWirePSO.m_rasterizerState = wireCCWRS; // 반시계
    //reflectWirePSO.m_stencilRef = 1;



    // skyboxSolidPSO
    skyboxSolidPSO = defaultSolidPSO;
    skyboxSolidPSO.m_vertexShader = skyboxVS;
    skyboxSolidPSO.m_pixelShader = skyboxPS;
    skyboxSolidPSO.m_inputLayout = skyboxIL;

    // skyboxWirePSO
    skyboxWirePSO = skyboxSolidPSO;
    skyboxWirePSO.m_rasterizerState = wireRS;

    // reflectSkyboxSolidPSO
    reflectSkyboxSolidPSO = skyboxSolidPSO;
    //reflectSkyboxSolidPSO.m_depthStencilState = drawMaskedDSS;
    reflectSkyboxSolidPSO.m_rasterizerState = solidCCWRS; // 반시계
    //reflectSkyboxSolidPSO.m_stencilRef = 1;

    // reflectSkyboxWirePSO
    reflectSkyboxWirePSO = reflectSkyboxSolidPSO;
    reflectSkyboxWirePSO.m_rasterizerState = wireCCWRS;
    //reflectSkyboxWirePSO.m_stencilRef = 1;

    // normalsPSO
    normalsPSO = defaultSolidPSO;
    normalsPSO.m_vertexShader = normalVS;     
    normalsPSO.m_geometryShader = normalGS;
    normalsPSO.m_pixelShader = normalPS;
    normalsPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    normalsPSO.m_inputLayout = BillboardIL;

    // depthOnlyPSO
    depthOnlyPSO = defaultSolidPSO;
    depthOnlyPSO.m_vertexShader = depthOnlyVS;
    depthOnlyPSO.m_pixelShader = depthOnlyPS;

    skinnedShadowPSO = depthOnlyPSO;
    skinnedShadowPSO.m_vertexShader = skinnedShadowVS;
    skinnedShadowPSO.m_inputLayout = skinnedIL;

    // postEffectsPSO
    postEffectsPSO.m_vertexShader = samplingVS;
    postEffectsPSO.m_pixelShader = postEffectsPS;
    postEffectsPSO.m_inputLayout = samplingIL;
    postEffectsPSO.m_rasterizerState = postProcessingRS;

    // postProcessingPSO
    postProcessingPSO.m_vertexShader = samplingVS;
    postProcessingPSO.m_pixelShader = depthOnlyPS; // dummy
    postProcessingPSO.m_inputLayout = samplingIL;
    postProcessingPSO.m_rasterizerState = postProcessingRS;

    // 단순 텍스처 샘플링
    samplingPSO.m_vertexShader = samplingVS;
    samplingPSO.m_pixelShader = samplingPS;
    samplingPSO.m_inputLayout = samplingIL;
    samplingPSO.m_rasterizerState = solidRS;
    samplingPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // BillboardPSO
    BillboardPSO.m_vertexShader = BillboardVS;
    BillboardPSO.m_pixelShader = BillboardPS;
    BillboardPSO.m_geometryShader = BillboardGS;
    BillboardPSO.m_inputLayout = BillboardIL;
    BillboardPSO.m_rasterizerState = billRS;
    BillboardPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    //particlePSO.m_rasterizerState = billRS;

    // TerrainSolidPSO
    terrainSolidPSO = defaultSolidPSO;
    terrainSolidPSO.m_vertexShader = terrainVS;
    terrainSolidPSO.m_pixelShader = terrainPS;
    terrainSolidPSO.m_hullShader = terrainHS;
    terrainSolidPSO.m_domainShader = terrainDS; 
    terrainSolidPSO.m_inputLayout = terrainIL;       
    terrainSolidPSO.m_primitiveTopology =
        D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    terrainSolidPSO.m_rasterizerState = solidRS;
    terrainSolidPSO.m_blendState = alphaBS; 

       // TerrainWirePSO
    terrainWirePSO.m_vertexShader = terrainVS;   
    terrainWirePSO.m_pixelShader = terrainPS;
    terrainWirePSO.m_hullShader = terrainHS; 
    terrainWirePSO.m_domainShader = terrainDS;
    terrainWirePSO.m_inputLayout = terrainIL;
    terrainWirePSO.m_primitiveTopology =
        D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    terrainWirePSO.m_rasterizerState = wireRS;

    // Terrain 반사
    reflectTerrainSolidPSO = terrainSolidPSO;
    reflectTerrainSolidPSO.m_rasterizerState = solidCCWRS;

    reflectTerrainWirePSO = terrainWirePSO;
    reflectTerrainWirePSO.m_rasterizerState = wireCCWRS;



    // Terrain Shadow
    terrainShadowPSO = terrainSolidPSO;
    terrainShadowPSO.m_vertexShader = terrainShadowVS;
    terrainShadowPSO.m_hullShader = terrainShadowHS;
    terrainShadowPSO.m_domainShader = terrainShadowDS;
    terrainShadowPSO.m_pixelShader = depthOnlyPS;

    // Terrain Pass
    terrainPassPSO = terrainSolidPSO;
    terrainPassPSO.m_pixelShader = terrainPassPS;


    // sunPSO
    sunPSO = defaultSolidPSO;
    sunPSO.m_vertexShader = sunVS;
    sunPSO.m_pixelShader = sunPS;

    // BoundingBoxPSO
    BoundingBoxPSO = defaultWirePSO;
    BoundingBoxPSO.m_pixelShader = simplePS;
    BoundingBoxPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

    // SnowPSO
    snowPSO = defaultSolidPSO;
    snowPSO.m_rasterizerState = solidBothRS;
    snowPSO.m_vertexShader = snowVS;
    snowPSO.m_pixelShader = snowPS;
    snowPSO.m_inputLayout = snowIL;

    // Snow 반사
    reflectSnowPSO = snowPSO;
    reflectSnowPSO.m_rasterizerState = solidCCWRS;

    // RainPSO
    rainPSO.m_rasterizerState = solidBothRS;
    rainPSO.m_vertexShader = rainVS; 
    rainPSO.m_pixelShader = rainPS;
    rainPSO.m_geometryShader = snowSBGS;
    rainPSO.m_inputLayout = rainIL; 
    rainPSO.m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    // Rain 반사 
    reflectRainPSO = rainPSO;
    reflectRainPSO.m_rasterizerState = solidCCWRS;




    // Lake
    lakePSO = defaultSolidPSO;
    lakePSO.m_vertexShader = lakeVS;          
    lakePSO.m_hullShader = lakeHS;
    lakePSO.m_domainShader = lakeDS;
    lakePSO.m_pixelShader = lakePS;
    lakePSO.m_inputLayout = lakeIL;
    lakePSO.m_primitiveTopology =
        D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    //lakePSO.m_rasterizerState = solidRS;
    lakePSO.m_rasterizerState = solidBothRS;
    lakePSO.m_blendState = alphaBS;  

    // Lake Shadow
    lakeShadowPSO = lakePSO;
    lakeShadowPSO.m_domainShader = lakeShadowDS;
    lakeShadowPSO.m_pixelShader = depthOnlyPS;

    // Lake Depth
    lakeDepthPSO = lakePSO;
    lakeDepthPSO.m_domainShader = lakeDepthDS;
    lakeDepthPSO.m_pixelShader = depthOnlyPS;

    // UI
    uiPSO = defaultSolidPSO;
    uiPSO.m_vertexShader = uiVS;
    uiPSO.m_pixelShader = uiPS;
    uiPSO.m_blendState = alphaBS;


    // Collision Box
    collisionPSO = BoundingBoxPSO;    
    collisionPSO.m_vertexShader = simpleVS;
} 


