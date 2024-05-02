#pragma once
#include "Headers.h"
#include "Model.h"
#include "GeometryGenerator.h"

struct UIConstantData
{
    int push = 0;
    int index = 0; // 0 : key   1 : keyboard
    Vector2 UIpadding;
};


class UI {
  public:
    UI(){};
    UI(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
       const Vector3 pos, const float scale , bool sphere = false) {
        Initialize(device, context, pos, scale, sphere);
    }
    UI(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
       const Vector3 pos, const float scale, const std::string &filename , bool sphere = false) {
        Initialize(device, context, pos, scale, filename, sphere);
    }

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, const Vector3 pos,
                    const float scale, const std::string &filename , bool sphere);
    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, const Vector3 pos,
                    const float scale , bool sphere);

    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context, ComPtr<ID3D11ShaderResourceView> terrainMapSRV);

    public:
    UIConstantData m_constantData;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_texSRV;

    public:
    shared_ptr<Model> m_ui;



};
