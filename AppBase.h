#pragma once

#include <directxtk/SimpleMath.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <iostream>
#include <vector>

#include "Camera.h"  
#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"


using DirectX::BoundingSphere;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Vector3;
using Microsoft::WRL::ComPtr;
using std::shared_ptr;
using std::vector;
using std::wstring; 

    struct MouseInfo // PS로 넘겨줄 const data
{
    Vector3 mousePos;
    float radius = 5.0f;
    Vector3 WorldNearPos;
    float heightlength = 0.3f;
    Vector3 WorldFarPos;
    int leftclicked = 0;
    int rightclicked = 0;
    int picked = 0; // terrain 만
    Vector2 mouseTexcoord;
};

    static_assert((sizeof(MouseInfo) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

class AppBase {
  public:
    AppBase();
    virtual ~AppBase();

    int Run();
    float GetAspectRatio() const;

    virtual bool Initialize();
    virtual void UpdateGUI() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
    virtual void OnMouseMove(int mouseX, int mouseY);
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void InitCubemaps(wstring basePath, wstring envFilename,
                      wstring specularFilename, wstring irradianceFilename,
                      wstring brdfFilename, wstring trmapFilename);
    void UpdateGlobalConstants(const Vector3 &eyeWorld, const Matrix &viewRow,
                               const Matrix &projRow, const Matrix &refl, const Matrix &OrthoView, const Matrix &OrthoProj);
    void UpdateBasicConstants(const Vector3 &eyeWorld, const Matrix &viewRow,
                              const Matrix &projRow);
    void SetGlobalConsts(ComPtr<ID3D11Buffer> &globalConstsGPU);

    void CreateDepthBuffers();
    void SetPipelineState(const GraphicsPSO &pso);
    bool UpdateMouseControl(const BoundingSphere &bs, Quaternion &q,
                            Vector3 &dragTranslation, Vector3 &pickPoint);



  protected: 
    bool InitMainWindow();
    bool InitDirect3D();
    bool InitGUI();
    void CreateBuffers();
    void SetMainViewport();
    void SetShadowViewport();


  public:

    int m_screenWidth; 
    int m_screenHeight;
    HWND m_mainWindow;
    bool m_useMSAA = true;
    UINT m_numQualityLevels = 0;
    bool m_drawAsWire = false;
    bool m_drawOBB = false;

    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_backBufferRTV;

    // 삼각형 레스터화 -> float(MSAA) -> resolved(No MSAA)
    // -> 후처리(블룸, 톤매핑) -> backBuffer(최종 SwapChain Present)
    ComPtr<ID3D11Texture2D> m_floatBuffer;
    ComPtr<ID3D11Texture2D> m_resolvedBuffer[4];
    ComPtr<ID3D11Texture2D> m_postEffectsBuffer;
    ComPtr<ID3D11RenderTargetView> m_floatRTV;
    ComPtr<ID3D11RenderTargetView> m_resolvedRTV[4];
    ComPtr<ID3D11RenderTargetView> m_postEffectsRTV;
    ComPtr<ID3D11ShaderResourceView> m_resolvedSRV[4];
    ComPtr<ID3D11ShaderResourceView> m_postEffectsSRV;

    // Edit Scene
    ComPtr<ID3D11Texture2D> m_EditBuffer;
    ComPtr<ID3D11RenderTargetView> m_EditRTV;
    ComPtr<ID3D11ShaderResourceView> m_EditSRV;

    ComPtr<ID3D11Texture2D> m_EditresolvedBuffer; // RTT로 사용
    ComPtr<ID3D11RenderTargetView> m_EditresolvedRTV;
    ComPtr<ID3D11ShaderResourceView> m_EditresolvedSRV;

    ComPtr<ID3D11Texture2D> m_BasicBuffer;
    ComPtr<ID3D11RenderTargetView> m_BasicRTV;

    // Deferred 관련
    ComPtr<ID3D11Texture2D> m_positionBuffer;
    ComPtr<ID3D11RenderTargetView> m_positionRTV;
    ComPtr<ID3D11ShaderResourceView> m_positionSRV;

    ComPtr<ID3D11Texture2D> m_normalBuffer;
    ComPtr<ID3D11RenderTargetView> m_normalRTV;
    ComPtr<ID3D11ShaderResourceView> m_normalSRV;

    ComPtr<ID3D11Texture2D> m_colorBuffer;
    ComPtr<ID3D11RenderTargetView> m_colorRTV;
    ComPtr<ID3D11ShaderResourceView> m_colorSRV;

    ComPtr<ID3D11Texture2D> m_depthBuffer;
    ComPtr<ID3D11RenderTargetView> m_depthRTV;
    ComPtr<ID3D11ShaderResourceView> m_depthSRV;


    // Depth buffer 관련
    ComPtr<ID3D11Texture2D> m_depthOnlyBuffer; // No MSAA
    ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;

    ComPtr<ID3D11Texture2D> m_depthOnlyBuffer2;
    ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV2;
    ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV2;

    ComPtr<ID3D11Texture2D> m_terrainDepthOnlyBuffer;
    ComPtr<ID3D11DepthStencilView> m_terrainDepthOnlyDSV;
    ComPtr<ID3D11ShaderResourceView> m_terrainDepthOnlySRV;

    ComPtr<ID3D11DepthStencilState> m_defaultDepthState;
    ComPtr<ID3D11DepthStencilState> m_uiDepthState;
    ComPtr<ID3D11DepthStencilState> m_noDepthState;

    // Shadow maps
    int m_shadowWidth = 1280;
    int m_shadowHeight = 1280;
    ComPtr<ID3D11Texture2D> m_shadowBuffers[MAX_LIGHTS]; // No MSAA
    ComPtr<ID3D11DepthStencilView> m_shadowDSVs[MAX_LIGHTS];
    ComPtr<ID3D11ShaderResourceView> m_shadowSRVs[MAX_LIGHTS];

    // 햇빛 Shadow map
    ComPtr<ID3D11Texture2D> m_SunShadowBuffer;
    ComPtr<ID3D11DepthStencilView> m_SunShadowDSV;
    ComPtr<ID3D11ShaderResourceView> m_SunShadowSRV;


    D3D11_VIEWPORT m_screenViewport;

    // 반사 굴절맵
    ComPtr<ID3D11Texture2D> m_reflectBuffer;
    ComPtr<ID3D11RenderTargetView> m_reflectRTV;
    ComPtr<ID3D11ShaderResourceView> m_reflectSRV;

    // Terrain 미니맵

    ComPtr<ID3D11Texture2D> m_terrainMapBuffer;
    ComPtr<ID3D11RenderTargetView> m_terrainMapRTV;
    ComPtr<ID3D11ShaderResourceView> m_terrainMapSRV;



    bool EditScene = false;

    // skybox
    shared_ptr<Model> m_skybox;

    Camera mainCamera;
    Camera m_Editcamera;
    Camera m_Basiccamera;
    Camera m_uiCamera;
    Camera m_terrainCamera;
    float EditFarZ = 100.0f;
    float currentFarZ = 20.0f;

    bool m_keyPressed[256] = {
        false,
    };

    bool m_leftButton = false;
    bool m_rightButton = false;
    bool m_dragStartFlag = false;   
    float directFlag = 0.0f;

    // 마우스 커서 위치 저장 (Picking에 사용)
    float m_cursorNdcX = 0.0f;
    float m_cursorNdcY = 0.0f;

    // 렌더링 -> PostEffects -> PostProcess
    PostEffectsConstants m_postEffectsConstsCPU;
    ComPtr<ID3D11Buffer> m_postEffectsConstsGPU;

    MouseInfo m_mouseInfo;

    PostProcess m_postProcess;


    GlobalConstants m_globalConstsCPU;
    GlobalConstants m_basicConstsCPU;
    GlobalConstants m_reflectGlobalConstsCPU;
    GlobalConstants m_shadowGlobalConstsCPU[MAX_LIGHTS];
    GlobalConstants m_SunShadowGlobalConstCPU;
    GlobalConstants m_terrainMapConstCPU;

    ComPtr<ID3D11Buffer> m_globalConstsGPU;
    ComPtr<ID3D11Buffer> m_basicConstsGPU;
    ComPtr<ID3D11Buffer> m_reflectGlobalConstsGPU;
    ComPtr<ID3D11Buffer> m_shadowGlobalConstsGPU[MAX_LIGHTS];
    ComPtr<ID3D11Buffer> m_mouseConstBuffer; // CS로 넘겨줄 constBuffer
    ComPtr<ID3D11Buffer> m_SunShadowGlobalConstGPU;
    ComPtr<ID3D11Buffer> m_terrainMapConstGPU;

    // 언제든지 추가 constbuffer
    SurplusConstants m_surplusConstsCPU;
    ComPtr<ID3D11Buffer> m_surplusConstsGPU;

    // 공통으로 사용하는 텍스춰들
    // skybox
    ComPtr<ID3D11ShaderResourceView> m_envSRV;
    ComPtr<ID3D11ShaderResourceView> m_irradianceSRV;
    ComPtr<ID3D11ShaderResourceView> m_specularSRV;
    ComPtr<ID3D11ShaderResourceView> m_brdfSRV;
    ComPtr<ID3D11ShaderResourceView> m_TRmapSRV;

    bool m_lightRotate = false;
    bool particlepoint = false;
    bool terrainEdit = false;
    bool brush = false;
    bool objEdit = false;
    bool m_edit = false;

    // 기후로 인한 sky 변화 상태 업데이트
    bool updateSkyState = false;
};
