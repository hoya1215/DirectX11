#pragma once
#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "GeometryGenerator.h"
#include "Model.h"


      
                 
            
class Lake : public Model {    
  public:
    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context, Matrix &Worldmat,
                    const std::string &filename);
    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context, Matrix reflectProj);  
           

    void Render(ComPtr<ID3D11DeviceContext> &context,
                const ComPtr<ID3D11ShaderResourceView> m_reflectSRV, const ComPtr<ID3D11ShaderResourceView> m_terrainDeferredSRV);

    void Render(ComPtr<ID3D11DeviceContext> &context);
       
                    
         LakeConstantData m_constantData;           
       
  protected:  
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Texture2D> m_lakeTex;
    ComPtr<ID3D11ShaderResourceView> m_lakeSRV;

    uint32_t m_indexCount = 0;

    Vector3 pos;

    ComPtr<ID3D11Buffer> m_constantBuffer;

};  


