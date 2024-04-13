#include "Rain.h"
#include "GraphicsCommon.h"


#include <numeric>
#include <random>
#include <cmath>


void Rain::Initialize(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) {



            m_rain.resize(rainCount);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dp(-10.f, 10.f);
    uniform_real_distribution<float> v(0.05f, 0.1f);
    uniform_real_distribution<float> r(0.005f, 0.01f);
    
    for (auto &p : m_rain) 
    {
        p.pos = Vector3(dp(gen), 10.0f, dp(gen));
        p.velocity = v(gen);
        p.radius = 0.02f;
    }



    D3D11Utils::CreateStructuredBuffer(device, UINT(m_rain.size()), sizeof(RainStructuredBuffer),
        m_rain.data(),
                                       m_rainBuffer, 
                                       m_rainSRV, m_rainUAV);

    D3D11Utils::CreateStagingBuffer(device, UINT(m_rain.size()), sizeof(m_rain),
                                    NULL, m_stagingBuffer);

    D3D11Utils::CreateComputeShader(device, L"RainCS.hlsl", m_computeShader);




}

void Rain::Update(ComPtr<ID3D11DeviceContext> &context) {


}

void Rain::RenderCS(ComPtr<ID3D11DeviceContext>& context)
{
    context->CSSetShader(m_computeShader.Get(), 0, 0);
    context->CSSetUnorderedAccessViews(0, 1, m_rainUAV.GetAddressOf(), NULL);
    context->Dispatch(UINT(ceil(m_rain.size() / 1024.f)), 1, 1);
    this->ComputeShaderBarrier(context);
} 

void Rain::Render(ComPtr<ID3D11DeviceContext> &context) {






    context->VSSetShaderResources(0, 1, m_rainSRV.GetAddressOf());

    context->Draw(UINT(m_rain.size()), 0);
}


