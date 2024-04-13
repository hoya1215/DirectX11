#include "Particle.h"
#include "GraphicsCommon.h"
#include "Camera.h"
 
#include <numeric>
#include <random>
#include <cmath>


void Particle::Initialize(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) {
    m_particlesCPU.resize(256);



        vector<Vector3> snow = {  
        {1.0, 1.0, 1.0}
    };

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dp(-1.0f, 1.0f);
    uniform_int_distribution<size_t> dc(0, snow.size() - 1);
    for (auto &p : m_particlesCPU) {
        p.position = Vector3(dp(gen), dp(gen), dp(gen));
        p.color = snow[dc(gen)];
        p.velocity = Vector3(0.0, -1.0, 0.0) * 0.1f;   
        p.radius = (dp(gen) + 1.3f) * 0.02f;
        p.life = -1.0f;
    }
     
    D3D11Utils::CreateConstBuffer(device, m_particleConstantData,
                                  m_particleConstantBuffer); 

    D3D11Utils::CreateStructuredBuffer(
        device, UINT(m_particlesCPU.size()), sizeof(Particle),
        m_particlesCPU.data(), m_particlesGPU, m_particlesSRV, m_particlesUAV);

    D3D11Utils::CreateStagingBuffer(device, UINT(m_particlesCPU.size()),
                                    sizeof(Particle), m_particlesCPU.data(),
                                    m_particlesStagingGPU);
     


    const vector<D3D11_INPUT_ELEMENT_DESC> inputElements = { 
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0}}; // Dummy
    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, L"ParticleVS.hlsl", inputElements, m_vertexShader,
        m_inputLayout);
    D3D11Utils::CreatePixelShader(device, L"ParticlePS.hlsl",
                                  m_pixelShader);
    D3D11Utils::CreateGeometryShader(device, L"ParticleGS.hlsl",
                                     m_spriteGS);

    D3D11Utils::CreateComputeShader(device, L"ParticleCS.hlsl",
                                    m_computeShader);
}

void Particle::Update(float dt, bool m_leftButton, float m_cursorNdcX, float m_cursorNdcY, ComPtr<ID3D11DeviceContext> &context, ComPtr<ID3D11Device> &device,Matrix viewproj, Matrix view,Vector3 cameraPos)
{
    m_particleConstantData.viewProj = viewproj;
    m_particleConstantData.view = view.Transpose();
    D3D11Utils::UpdateBuffer(device, context, m_particleConstantData,
                             m_particleConstantBuffer); 


    dt *= 0.5f; 

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
    uniform_real_distribution<float> randomSpeed(1.5f, 2.0f);
    uniform_real_distribution<float> randomSpeed2(-10.f, 10.f);
    uniform_real_distribution<float> randomLife(0.0f, 1.0f);
    uniform_real_distribution<float> randomZ(-1.f, 2.f);
    uniform_real_distribution<float> randomX(-2.f, 2.f); 



    int newCount = snowCount; // 한 프레임에 새로 만들어질 수 있는 파티클 개수
    //if (m_leftButton) {
        for (auto &p : m_particlesCPU) { 
            // TODO: 
            if (p.life < 0.0f && newCount > 0) {
                const float theta = randomTheta(gen);
                //p.position = Vector3(m_cursorNdcX, m_cursorNdcY, randomZ(gen));
                p.position = Vector3(randomX(gen), 1.f, randomZ(gen));
               //p.velocity = 
               //    Vector3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen);
                p.velocity = Vector3(0.0, -1.0, 0.0) * randomSpeed(gen);
                p.life = randomLife(gen) * 1.5f; // 수명 추가
                newCount--;
            }
        }   
   // }   
         
   // //// 항상 추가하는 Source
   // //newCount = 10;
   // //for (auto &p : m_particlesCPU) {

   // //    // 비활성화되어 있는 입자를 찾으면 활성화하는 방식
   // //    if (p.life < 0.0f && newCount > 0) {

   // //        const float theta = randomTheta(gen);
   // //        p.position =
   // //            Vector3(cos(theta), -sin(theta), 0.0) * randomLife(gen) * 0.1f +
   // //            Vector3(0.0f, -0.3f, 0.0f);
   // //        p.velocity = Vector3(-1.0f, 0.0f, 0.0) * randomSpeed(gen);
   // //        p.life = randomLife(gen) * 1.5f; // 수명 추가
   // //        newCount--;
   // //    }
   // //}

    const Vector3 gravity = Vector3(0.0f, -9.8f, 0.0f);
    const float cor = 0.5f; // Coefficient Of Restitution
    const float groundHeight = -0.8f;

    for (auto &p : m_particlesCPU) {

        if (p.life < 0.0f) // 수명이 다했다면 무시
            continue;

        p.velocity = p.velocity + gravity * dt * 0.1f;
        p.position += p.velocity * dt;
        p.life -= dt;

        //if (p.position.y < groundHeight && p.velocity.y < 0.0f) {
        //    // TODO: ...

        //    p.velocity.y *= -randomLife(gen);
        //    p.position.y = groundHeight;
        //}

        //// TODO: ...

        //if (p.position.x < -1.0f && p.velocity.x < 0.0f) {
        //    p.velocity.x *= -randomLife(gen);
        //    p.position.x = -1.0f;
        //}

        //if (p.position.x > 1.0f && p.velocity.x > 0.0f) {
        //    p.life = -1.0;
        //}
    }

    //// CS 연습
    //context->CSSetUnorderedAccessViews(0, 1, m_particlesUAV.GetAddressOf(),
    //                                   NULL);
    //context->CSSetShader(m_computeShader.Get(), 0, 0); 
    //context->Dispatch(UINT(ceil(m_particlesCPU.size() / 512.0f)), 1, 1);

    D3D11Utils::CopyToStagingBuffer(
        context, m_particlesStagingGPU,
        UINT(sizeof(Particle) * m_particlesCPU.size()), m_particlesCPU.data());
    context->CopyResource(m_particlesGPU.Get(), m_particlesStagingGPU.Get());
}

void Particle::Render(ComPtr<ID3D11DeviceContext> &context)
{
    //AppBase::SetMainViewport(); 
     

    context->VSSetConstantBuffers(0, 1,
                                  m_particleConstantBuffer.GetAddressOf());
    context->VSSetShader(m_vertexShader.Get(), 0, 0);
    context->GSSetShader(m_spriteGS.Get(), 0, 0);
    context->PSSetShader(m_pixelShader.Get(), 0, 0);
    context->PSSetConstantBuffers(0, 1, m_particleConstantBuffer.GetAddressOf());
    //m_context->CSSetShader(NULL, 0, 0);   
    context->GSSetConstantBuffers(0, 1,
                                   m_particleConstantBuffer.GetAddressOf());
    // 색을 모두 더하면서 그리는 accumulateBS 사용
    const float blendColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    context->OMSetBlendState(Graphics::accumulateBS.Get(), blendColor,
                               0xffffffff); 

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    context->VSSetShaderResources(0, 1, m_particlesSRV.GetAddressOf());
    context->Draw(UINT(m_particlesCPU.size()), 0);
}

