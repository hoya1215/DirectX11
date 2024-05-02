#pragma once
#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "AppBase.h"







struct ParticleConstant
{
    Matrix view;
    Matrix viewProj;
    int weather; // 1 = snow,  2 = rain
    Vector3 padding;
};

    struct ParticlePoint {
    Vector3 position;
    Vector3 velocity;
    Vector3 color;
    float life = 0.0f;
    float radius = 1.0f;
    };

class Particle {
  public: 

    void Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context);
    void Update(float dt, bool m_leftButton, float m_cursorNdcX,
                float m_cursorNdcY, ComPtr<ID3D11DeviceContext> &context, ComPtr<ID3D11Device> &device, Matrix viewproj, Matrix view,Vector3 cameraPos);
    void Render(ComPtr<ID3D11DeviceContext> &context);

    void DrawSprites(); 

    int snowCount = 10;
    ParticleConstant m_particleConstantData;

  protected:
    vector<ParticlePoint> m_particlesCPU;



    ComPtr<ID3D11Buffer> m_particlesGPU;
    ComPtr<ID3D11Buffer> m_particlesStagingGPU;
    ComPtr<ID3D11Buffer> m_particleConstantBuffer; 

    // Shaders 
    ComPtr<ID3D11VertexShader> m_vertexShader; 
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11GeometryShader> m_spriteGS;
    ComPtr<ID3D11ComputeShader> m_computeShader;

    ComPtr<ID3D11ShaderResourceView> m_particlesSRV;
    ComPtr<ID3D11UnorderedAccessView> m_particlesUAV;
};

