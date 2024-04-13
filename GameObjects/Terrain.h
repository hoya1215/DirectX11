#pragma once
#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "Model.h"
#include "ConstantBuffers.h"
#include "StructuredBuffer.h"
#include "GraphicsCommon.h"





struct HeightInfo 
{
    Vector3 pos;  
    float index;
};

struct CharacterHeight
{
    float height;
};
         
         
          

  
class Terrain : public Model{       
      public: 
        void Initialize(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,Matrix Worldmat,
                        const std::string &filename);
        void Update(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, Matrix view, 
                    Matrix proj, Vector3 eyeworld); 
              
        void HeightRender(
                          ComPtr<ID3D11DeviceContext> &context);

        void Render(ComPtr<ID3D11DeviceContext> &context);

        void PixelRender(ComPtr<ID3D11DeviceContext> &context);
          
        void
        PassRender(ComPtr<ID3D11DeviceContext> &context);

        void SmoothRender(ComPtr<ID3D11DeviceContext> &context);


        // 기능
        void CreateBuffer(ComPtr<ID3D11Device> &device);

        void CreateComputeShader(ComPtr<ID3D11Device> &device);

        void CreateTexture(ComPtr<ID3D11Device> &device,
                           ComPtr<ID3D11DeviceContext> &context);

        void TextureBufferInitialize(ComPtr<ID3D11Device> &device,
                                    ComPtr<ID3D11DeviceContext> &context);

        void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext> &context);
           
        void heightCorrection(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context); // 높이 보정

        void heightReset(ComPtr<ID3D11DeviceContext> &context);

        void ChangeTexture(ComPtr<ID3D11DeviceContext> &context);

        float GetMaxheight(ComPtr<ID3D11DeviceContext> &context, float minX, float maxX, float minZ, float maxZ);

        float GetheightGPUtoCPU(ComPtr<ID3D11DeviceContext> &context);

        TerrainConstantData m_constantData;
        vector<HeightInfo> m_heightInfo;
        vector<CharacterHeight> m_CharacterHeight;

        float scale = 0.0f;
               
        public:
        ComPtr<ID3D11Buffer> m_vertexBuffer;
        ComPtr<ID3D11Buffer> m_constantBuffer;
        ComPtr<ID3D11Buffer> m_heightInfoBuffer;
        ComPtr<ID3D11Buffer> m_stagingBuffer; 
        ComPtr<ID3D11Buffer> m_CharacterHeightBuffer;
        ComPtr<ID3D11Buffer> m_heightStagingBuffer;
              

        ComPtr<ID3D11SamplerState> m_samplerState;

        uint32_t m_indexCount = 0;    
                
        ComPtr<ID3D11Texture2D> m_heightTex;
        ComPtr<ID3D11Texture2D> m_albedoTex;
        ComPtr<ID3D11Texture2D> m_albedoTex02;
        ComPtr<ID3D11Texture2D> m_sandTex;
        ComPtr<ID3D11Texture2D> m_grassTex;
        ComPtr<ID3D11Texture2D> m_normalTex;   
        ComPtr<ID3D11Texture2D> m_aoTex;
        ComPtr<ID3D11Texture2D> m_roughnessTex;
                 
        ComPtr<ID3D11ShaderResourceView> m_heightSRV;
        ComPtr<ID3D11ShaderResourceView> m_albedoSRV;
        ComPtr<ID3D11ShaderResourceView> m_albedoSRV02;
        ComPtr<ID3D11ShaderResourceView> m_sandSRV;
        ComPtr<ID3D11ShaderResourceView> m_grassSRV;
        ComPtr<ID3D11ShaderResourceView> m_normalSRV;
        ComPtr<ID3D11ShaderResourceView> m_aoSRV; 
        ComPtr<ID3D11ShaderResourceView> m_roughnessSRV;
        ComPtr<ID3D11ShaderResourceView> m_heigthInfoSRV;
        ComPtr<ID3D11UnorderedAccessView> m_heightInfoUAV;
        ComPtr<ID3D11ShaderResourceView> m_CharacterHeightSRV;
        ComPtr<ID3D11UnorderedAccessView> m_CharacterHeightUAV;

         
            ComPtr<ID3D11Texture2D> m_terrainPixelBuffer;
        ComPtr<ID3D11ShaderResourceView> m_terrainPixelSRV;
        ComPtr<ID3D11UnorderedAccessView> m_terrainPixelUAV;

        // Terrain Pass
        ComPtr<ID3D11Texture2D> m_terrainPassBuffer;
        ComPtr<ID3D11RenderTargetView> m_terrainPassRTV;
        ComPtr<ID3D11ShaderResourceView> m_terrainPassSRV;
        ComPtr<ID3D11UnorderedAccessView> m_terrainPassUAV;

        ComPtr<ID3D11ComputeShader> m_terrainCS;
        ComPtr<ID3D11ComputeShader> m_terrainSmoothCS;
        ComPtr<ID3D11ComputeShader> m_terrainPassCS;
        ComPtr<ID3D11ComputeShader> m_changeTextureCS;
        ComPtr<ID3D11ComputeShader> m_terrainCorrectionCS;
        ComPtr<ID3D11ComputeShader> m_getheightCS;

        ComPtr<ID3D11Texture2D> m_stagingTexture;
        ComPtr<ID3D11Texture2D> m_resolveTexture;


        ComPtr<ID3D11ComputeShader> m_terrainPixelCS;

         
};  

