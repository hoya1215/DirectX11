#pragma once
#include "Model.h"
#include "D3D11Utils.h"
#include "GeometryGenerator.h"


class Snow {    
     
  public:
    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context);

    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context); 

    void Render(ComPtr<ID3D11DeviceContext> &context);
     
    vector<SnowInstance> m_instanceData;
    ComPtr<ID3D11Buffer> m_instanceBuffer;

    float m_snowCount = 3000; 

 private:
      ComPtr<ID3D11Buffer> m_vertexBuffer;
      ComPtr<ID3D11Buffer> m_indexBuffer;

      UINT m_indexCount = 0;
      UINT m_vertexCount = 0;
      UINT m_offset = 0;
      UINT m_instanceCount = 0;

};

