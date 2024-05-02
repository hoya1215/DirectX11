#pragma once

#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "Mesh.h"





class ImageFilter {
  public:
    ImageFilter(){};

    ImageFilter(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context,
                ComPtr<ID3D11PixelShader> &pixelShader, int width, int height);

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    ComPtr<ID3D11PixelShader> &pixelShader, int width,
                    int height);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context) const;

    void SetShaderResources(
        const std::vector<ComPtr<ID3D11ShaderResourceView>> &resources);

    void SetRenderTargets(
        const std::vector<ComPtr<ID3D11RenderTargetView>> &targets);

  public:  
    struct ImageFilterConstData {
        float dx;
        float dy;
        float threshold;
        bool dof = false; // dof On / Off
        float option1; // exposure in CombinePS.hlsl
        float option2; // gamma in CombinePS.hlsl
        float option3;
        float dofLevel = 0.0f;
    };

    ImageFilterConstData m_constData = {};

  protected:
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11Buffer> m_constBuffer;
    D3D11_VIEWPORT m_viewport = {};


    std::vector<ID3D11ShaderResourceView *> m_shaderResources;
    std::vector<ID3D11RenderTargetView *> m_renderTargets;
};
