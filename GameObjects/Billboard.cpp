#include "Billboard.h"


void Billboard::Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
                                 const std::vector<Vector4> &points) {

    // Sampler 만들기
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);

    m_indexCount = uint32_t(points.size());

    m_constantData.width = 2.4f;
    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);


    std::vector<std::string> filenames = {
        "d:/Assets/Textures/TreeBillboards/1.png",
        "d:/Assets/Textures/TreeBillboards/2.png",
        "d:/Assets/Textures/TreeBillboards/3.png",
        "d:/Assets/Textures/TreeBillboards/4.png",
        "d:/Assets/Textures/TreeBillboards/5.png"};

    D3D11Utils::CreateTextureArray(device, context, filenames, m_texArray,
                                   m_texArraySRV);

  
}

void Billboard::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, const Vector4 &point)
{
    // Sampler 만들기
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    vector<Vector4> points;
    points.push_back(point);

    D3D11Utils::CreateVertexBuffer(device, points, m_vertexBuffer);

    m_indexCount = uint32_t(points.size());

    m_constantData.width = 2.4f;
    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);


    D3D11Utils::CreateTexture(device, context,
                              "d:/Assets/Textures/TreeBillboards/2.png", false,
                              m_defaultTex, m_defaultSRV);

    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
}
 
void Billboard::Update(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context)
{
    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
}

void Billboard::Render(ComPtr<ID3D11DeviceContext> &context, ComPtr<ID3D11ShaderResourceView> terrainPassSRV) {
     

    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
    context->PSSetShaderResources(0, 1, m_defaultSRV.GetAddressOf());
    //context->PSSetShaderResources(0, 1, m_texArraySRV.GetAddressOf());

    context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Geometry Shader
    context->GSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->GSSetShaderResources(0, 1, terrainPassSRV.GetAddressOf());


    UINT stride = sizeof(Vector4); // sizeof(Vertex);
    UINT offset = 0;

    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);

    context->Draw(m_indexCount, 0);



}
