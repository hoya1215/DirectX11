#pragma once
#include "D3D11Utils.h"
#include <directxtk/SimpleMath.h>
#include <string>
#include <vector>



// billboard ÀÇ constdata


struct BillboardConstData
{
    float width;
    Vector3 BillboardPadding;
};

    static_assert((sizeof(BillboardConstData) % 16) == 0,
              "Constant Buffer size must be 16-byte aligned");

class Billboard {

    public:
    Billboard() {}


        Billboard(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context, const Vector4 &point)
        {
          Initialize(device, context, point);
    }

        void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                        const std::vector<Vector4> &points);

        void Initialize(ComPtr<ID3D11Device> &device,
                        ComPtr<ID3D11DeviceContext> &context,
                        const Vector4 &point);

        void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);

        void Render(ComPtr<ID3D11DeviceContext> &context,
                    ComPtr<ID3D11ShaderResourceView> terrainPassSRV);


        BillboardConstData m_constantData;
        ComPtr<ID3D11Buffer> m_constantBuffer;

    protected:

        ComPtr<ID3D11Buffer> m_vertexBuffer;
        ComPtr<ID3D11SamplerState> m_samplerState;
        ComPtr<ID3D11VertexShader> m_vertexShader;
        ComPtr<ID3D11GeometryShader> m_geometryShader;
        ComPtr<ID3D11PixelShader> m_pixelShader;
        ComPtr<ID3D11InputLayout> m_inputLayout;

        uint32_t m_indexCount = 0;

        ComPtr<ID3D11Texture2D> m_texArray;
        ComPtr<ID3D11ShaderResourceView> m_texArraySRV;

        ComPtr<ID3D11Texture2D> m_defaultTex;
        ComPtr<ID3D11ShaderResourceView> m_defaultSRV;
 
};
