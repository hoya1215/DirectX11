#include "UI.h"

void UI::Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, const Vector3 pos,
                    const float scale, const std::string &filename, bool sphere) {
    MeshData meshdata;

    if (sphere)
    {
         meshdata = GeometryGenerator::MakeCircle(scale);

    }
    else
    {
         meshdata = GeometryGenerator::MakeSquare(scale);

    }
    m_ui = make_shared<Model>(device, context, vector{meshdata});
    m_ui->UpdateWorldRow(Matrix::CreateTranslation(pos));

    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);

    if (!filename.empty())
        D3D11Utils::CreateTexture(device, context, filename, false, m_texture,
                              m_texSRV);

    m_ui->UpdateConstantBuffers(device, context);

}

void UI::Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, const Vector3 pos,
                    const float scale, bool sphere) {
    MeshData meshdata;

    if (sphere) {
        meshdata = GeometryGenerator::MakeCircle(scale);

    } else {
        meshdata = GeometryGenerator::MakeSquare(scale);
    }
    m_ui = make_shared<Model>(device, context, vector{meshdata});
    m_ui->UpdateWorldRow(Matrix::CreateTranslation(pos));

    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);


    m_ui->UpdateConstantBuffers(device, context);
}

void UI::Update(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context)
{
    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);

}      

void UI::Render(ComPtr<ID3D11DeviceContext>& context)
{
    context->PSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
    context->PSSetShaderResources(5, 1, m_texSRV.GetAddressOf());
    m_ui->Render(context); }

void UI::Render(ComPtr<ID3D11DeviceContext> &context , ComPtr<ID3D11ShaderResourceView> terrainMapSRV) {
    context->PSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
    context->PSSetShaderResources(5, 1, terrainMapSRV.GetAddressOf());
    m_ui->Render(context);
}
