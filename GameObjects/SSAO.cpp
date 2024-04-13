#include "SSAO.h"
#include <numeric>
#include <random>
#include <cmath>


void SSAO::Initialize(ComPtr<ID3D11Device> &device,
                      ComPtr<ID3D11DeviceContext> &context) {
    D3D11_TEXTURE2D_DESC desc;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Width = 1280;
    desc.Height = 720;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    // desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
    desc.Usage = D3D11_USAGE_DEFAULT; // 스테이징 텍스춰로부터 복사 가능
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = 0;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;

    // 노멀맵
    ThrowIfFailed(
        device->CreateTexture2D(&desc, NULL, m_normalBuffer.GetAddressOf()));

    ThrowIfFailed(device->CreateRenderTargetView(m_normalBuffer.Get(), NULL,
                                                   m_normalRTV.GetAddressOf()));
    ThrowIfFailed(device->CreateShaderResourceView(
        m_normalBuffer.Get(), NULL, m_normalSRV.GetAddressOf()));

    D3D11Utils::CreateTexture2D(device, context, m_ssaoMap, m_ssaoMapSRV,
                                m_ssaoMapUAV);

    D3D11Utils::CreateComputeShader(device, L"SSAOCS.hlsl", m_ssaoCS);

    D3D11Utils::CreateComputeShader(device, L"SSAOBlurCS.hlsl", m_ssaoBlurCS);

    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);

    this->CreateOffsetVectors(); 

    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
}

void SSAO::Update(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context)
{
    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
}

void SSAO::CreateSSAOMap(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11ShaderResourceView> &DepthMap)
{
    vector<ID3D11ShaderResourceView *> SRVs = {DepthMap.Get(),
                                              m_normalSRV.Get()};
    context->CSSetShader(m_ssaoCS.Get(), 0, 0);
    //context->CSSetShaderResources(0 , UINT(SRVs.size()), SRVs.data());
    context->CSSetShaderResources(0, 1, DepthMap.GetAddressOf());
    context->CSSetShaderResources(1, 1, m_normalSRV.GetAddressOf());
    context->CSSetUnorderedAccessViews(0, 1, m_ssaoMapUAV.GetAddressOf(), NULL);
    context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(720.0f / 32.0f)),
                      1);
    this->ComputeShaderBarrier(context);

    // Blur
    context->CSSetShader(m_ssaoBlurCS.Get(), 0, 0);
    // context->CSSetShaderResources(0 , UINT(SRVs.size()), SRVs.data());
    context->CSSetUnorderedAccessViews(0, 1, m_ssaoMapUAV.GetAddressOf(), NULL);
    context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(720.0f / 32.0f)),
                      1);
    this->ComputeShaderBarrier(context);
}

void SSAO::CreateOffsetVectors() {
    m_constantData.offSets[0] = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
    m_constantData.offSets[1] = Vector4(-1.0f, -1.0f, -1.0f, 0.0f);

    m_constantData.offSets[2] = Vector4(-1.0f, 1.0f, 1.0f, 0.0f);
    m_constantData.offSets[3] = Vector4(1.0f, -1.0f, -1.0f, 0.0f);

    m_constantData.offSets[4] = Vector4(1.0f, 1.0f, -1.0f, 0.0f);
    m_constantData.offSets[5] = Vector4(-1.0f, -1.0f, 1.0f, 0.0f);
     
    m_constantData.offSets[6] = Vector4(-1.0f, 1.0f, -1.0f, 0.0f);
    m_constantData.offSets[7] = Vector4(1.0f, -1.0f, 1.0f, 0.0f);

    m_constantData.offSets[8] = Vector4(-1.0f, 0.0f, 0.0f, 0.0f);
    m_constantData.offSets[9] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);

    m_constantData.offSets[10] = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
    m_constantData.offSets[11] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    m_constantData.offSets[12] = Vector4(0.0f, 0.0f, -1.0f, 0.0f);
    m_constantData.offSets[13] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> length(0.25f, 1.f);

    for (int i = 0; i < 14; i++)
    {
        m_constantData.offSets[i].Normalize();

        m_constantData.offSets[i] *= length(gen);


    }



    
}

    void SSAO::ComputeShaderBarrier(ComPtr<ID3D11DeviceContext> &context) {
    // 참고: BreadcrumbsDirectX-Graphics-Samples (DX12)
    // void CommandContext::InsertUAVBarrier(GpuResource & Resource, bool
    // FlushImmediate)

    // 예제들에서 최대 사용하는 SRV, UAV 갯수가 6개
    ID3D11ShaderResourceView *nullSRV[6] = {
        0,
    };
    context->CSSetShaderResources(0, 6, nullSRV);
    ID3D11UnorderedAccessView *nullUAV[6] = {
        0,
    };
    context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
}