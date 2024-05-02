#pragma once
#include "Headers.h"





inline void ThrowIfFailed(HRESULT hr) {
    if (FAILED(hr)) {
        // 디버깅할 때 여기에 breakpoint 설정
        throw std::exception();
    }
}

class D3D11Utils {

  public:

    static void CreateVertexShaderAndInputLayout(
        ComPtr<ID3D11Device> &device, const wstring &filename,
        const vector<D3D11_INPUT_ELEMENT_DESC> &inputElements,
        ComPtr<ID3D11VertexShader> &m_vertexShader,
        ComPtr<ID3D11InputLayout> &m_inputLayout,
        const vector<D3D_SHADER_MACRO> shaderMacros = {});

    static void CreateHullShader(ComPtr<ID3D11Device> &device,
                                 const wstring &filename,
                                 ComPtr<ID3D11HullShader> &m_hullShader);

    static void CreateDomainShader(ComPtr<ID3D11Device> &device,
                                   const wstring &filename,
                                   ComPtr<ID3D11DomainShader> &m_domainShader);

    static void CreateGeometryShader(ComPtr<ID3D11Device> &device, const wstring &filename,
                         ComPtr<ID3D11GeometryShader> &m_geometryShader);

    static void CreateComputeShader(ComPtr<ID3D11Device> &device,
                                    const wstring &filename,
                                    ComPtr<ID3D11ComputeShader> &computeShader);

    static void CreatePixelShader(ComPtr<ID3D11Device> &device,
                                  const wstring &filename,
                      ComPtr<ID3D11PixelShader> &m_pixelShader,
                      const vector<D3D_SHADER_MACRO> shaderMacros = {});

    static void CreateIndexBuffer(ComPtr<ID3D11Device> &device,
                                  const vector<uint32_t> &indices,
                                  ComPtr<ID3D11Buffer> &indexBuffer);

    template <typename T_INSTANCE>
    static void CreateInstanceBuffer(ComPtr<ID3D11Device> &device,
                                     const vector<T_INSTANCE> &instances,
                                     ComPtr<ID3D11Buffer> &instanceBuffer) {


        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // <- 애니메이션에 사용
        bufferDesc.ByteWidth = UINT(sizeof(T_INSTANCE) * instances.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // <- CPU에서 복사
        bufferDesc.StructureByteStride = sizeof(T_INSTANCE);

        D3D11_SUBRESOURCE_DATA vertexBufferData = {
            0}; 
        vertexBufferData.pSysMem = instances.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        ThrowIfFailed(device->CreateBuffer(&bufferDesc, &vertexBufferData,
                                           instanceBuffer.GetAddressOf()));
    }

    template <typename T_VERTEX>
    static void CreateVertexBuffer(ComPtr<ID3D11Device> &device,
                                   const vector<T_VERTEX> &vertices,
                                   ComPtr<ID3D11Buffer> &vertexBuffer) {



        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; 
        bufferDesc.StructureByteStride = sizeof(T_VERTEX);

        D3D11_SUBRESOURCE_DATA vertexBufferData = {
            0}; 
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        ThrowIfFailed(device->CreateBuffer(&bufferDesc, &vertexBufferData,
                                           vertexBuffer.GetAddressOf()));
    }

    template <typename T_CONSTANT>
    static void CreateConstBuffer(ComPtr<ID3D11Device> &device,
                                  const T_CONSTANT &constantBufferData,
                                  ComPtr<ID3D11Buffer> &constantBuffer) {

        static_assert((sizeof(T_CONSTANT) % 16) == 0,
                      "Constant Buffer size must be 16-byte aligned");

        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(constantBufferData);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        ThrowIfFailed(device->CreateBuffer(&desc, &initData,
                                           constantBuffer.GetAddressOf()));
    }

    template <typename T_DATA>
    static void UpdateBuffer(ComPtr<ID3D11Device> &device,
                             ComPtr<ID3D11DeviceContext> &context,
                             const T_DATA &bufferData,
                             ComPtr<ID3D11Buffer> &buffer) {

        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized."
                      << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        context->Unmap(buffer.Get(), NULL);
    }

    static void
    CreateTexture(ComPtr<ID3D11Device> &device,
                  ComPtr<ID3D11DeviceContext> &context,
                  const std::string filename, const bool usSRGB,
                  ComPtr<ID3D11Texture2D> &texture,
                  ComPtr<ID3D11ShaderResourceView> &textureResourceView);

    static void CreateTexture(
        ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
        const std::string albedoFilename, const std::string opacityFilename,
        const bool usSRGB, ComPtr<ID3D11Texture2D> &texture,
        ComPtr<ID3D11ShaderResourceView> &textureResourceView);

    static void CreateTextureUAV(ComPtr<ID3D11Device> &device, 
                          ComPtr<ID3D11Texture2D> &texture,
                          ComPtr<ID3D11UnorderedAccessView> &uav);

    static void CreateBufferUAV(ComPtr<ID3D11Device> &device,
                                 ComPtr<ID3D11Buffer> &buffer,
                                 ComPtr<ID3D11UnorderedAccessView> &uav);

    static void CreateMetallicRoughnessTexture(
        ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
        const std::string metallicFiilename,
        const std::string roughnessFilename, ComPtr<ID3D11Texture2D> &texture,
        ComPtr<ID3D11ShaderResourceView> &srv);

    static void CreateTextureArray(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<std::string> filenames,
                       ComPtr<ID3D11Texture2D> &texture,
                       ComPtr<ID3D11ShaderResourceView> &textureResourceView);

    static void CreateDDSTexture(ComPtr<ID3D11Device> &device,
                                 const wchar_t *filename, const bool isCubeMap,
                                 ComPtr<ID3D11ShaderResourceView> &texResView);

    // 텍스춰를 이미지 파일로 저장
    static void WriteToFile(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            ComPtr<ID3D11Texture2D> &textureToWrite,
                            const std::string filename);


    // structured 
    static void CreateStagingBuffer(ComPtr<ID3D11Device> &device,
                                    const UINT numElements,
                                    const UINT sizeElement,
                                    const void *initData,
                                    ComPtr<ID3D11Buffer> &buffer);

    static void CopyFromStagingBuffer(ComPtr<ID3D11DeviceContext> &context,
                                      ComPtr<ID3D11Buffer> &buffer, UINT size,
                                      void *dest);

    static void CopyToStagingBuffer(ComPtr<ID3D11DeviceContext> &context,
                                    ComPtr<ID3D11Buffer> &buffer, UINT size,
                                    void *src) {
        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
        memcpy(ms.pData, src, size);
        context->Unmap(buffer.Get(), NULL);
    }

    static void CreateStructuredBuffer(ComPtr<ID3D11Device> &device,
                                       const UINT numElements,
                                       const UINT sizeElement,
                                       const void *initData,
                                       ComPtr<ID3D11Buffer> &buffer,
                                       ComPtr<ID3D11ShaderResourceView> &srv,
                                       ComPtr<ID3D11UnorderedAccessView> &uav);


    void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context)
    {
        ID3D11ShaderResourceView *nullSRV[6] = {
            0,
        };
        context->CSSetShaderResources(0, 6, nullSRV);
        ID3D11UnorderedAccessView *nullUAV[6] = {
            0,
        };
        context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
    }

    static void CreateTexture2D(ComPtr<ID3D11Device> &device,
                                ComPtr<ID3D11DeviceContext> &context,
                                ComPtr<ID3D11Texture2D> &buffer,
                                ComPtr<ID3D11ShaderResourceView> &srv,
                                ComPtr<ID3D11UnorderedAccessView> &uav);

    static void CreateStagingTexture2(ComPtr<ID3D11Device> &device,
                                     ComPtr<ID3D11DeviceContext> &context,
                                     ComPtr<ID3D11Texture2D> &buffer);

        static void CreateResolveTexture2(ComPtr<ID3D11Device> &device,
                                      ComPtr<ID3D11DeviceContext> &context,
                                      ComPtr<ID3D11Texture2D> &buffer);
};

