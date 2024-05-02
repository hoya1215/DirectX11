#pragma once
#include "D3D11Utils.h"
#include "Headers.h"

struct OffsetVectors
{
    Vector4 offSets[14];
    float ssaoRadius = 0.5f;
    float dist = 0.01f;
    float ssaoStart = 0.1f;
    float ssaoEnd = 5.0f;
};

class SSAO {

	public:
    void Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context);

    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);

    void CreateSSAOMap(ComPtr<ID3D11DeviceContext> &context,
                       ComPtr<ID3D11ShaderResourceView> &DepthMap);

    void CreateOffsetVectors();

    void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext> &context);

    ComPtr<ID3D11RenderTargetView> &GetRTV() { return m_normalRTV; }
    ComPtr<ID3D11ShaderResourceView> &GetSRV() { return m_normalSRV; }
    ComPtr<ID3D11ShaderResourceView> &GetSSAOMapSRV() { return m_ssaoMapSRV; }

    OffsetVectors m_constantData;

	private:

      ComPtr<ID3D11Buffer> m_constantBuffer;

    // ³ë¸Ö¸Ê
    ComPtr<ID3D11Texture2D> m_normalBuffer;
    ComPtr<ID3D11RenderTargetView> m_normalRTV;
    ComPtr<ID3D11ShaderResourceView> m_normalSRV;

    ComPtr<ID3D11Texture2D> m_ssaoMap;
    ComPtr<ID3D11UnorderedAccessView> m_ssaoMapUAV;
    ComPtr<ID3D11ShaderResourceView> m_ssaoMapSRV;

    ComPtr<ID3D11ComputeShader> m_ssaoCS;
    ComPtr<ID3D11ComputeShader> m_ssaoBlurCS;
};
