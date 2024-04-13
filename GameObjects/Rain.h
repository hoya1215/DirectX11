#pragma once

#include "GeometryGenerator.h"
#include "StructuredBuffer.h"




using namespace std;
     
// structuredbuffer
struct RainStructuredBuffer {
        Vector3 pos;
        float velocity;
        float radius;
};


     
class Rain
{
  public:
    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context);
    void Update(ComPtr<ID3D11DeviceContext> &context);

    void RenderCS(ComPtr<ID3D11DeviceContext> &context);
    void Render(ComPtr<ID3D11DeviceContext> &context);

         void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext> &context) {
        ID3D11ShaderResourceView *nullSRV[6] = {
            0,
        };
        context->CSSetShaderResources(0, 6, nullSRV);
        ID3D11UnorderedAccessView *nullUAV[6] = {
            0,
        };
        context->CSSetUnorderedAccessViews(0, 6, nullUAV, NULL);
    }

      
    public:
    float rainCount = 20000.f;  

  protected:
    vector<RainStructuredBuffer> m_rain; 

    // Structured Buffer
    ComPtr<ID3D11Buffer> m_rainBuffer;
    ComPtr<ID3D11Buffer> m_stagingBuffer; 

    // Shaders
    ComPtr<ID3D11ComputeShader> m_computeShader;

    ComPtr<ID3D11ShaderResourceView> m_rainSRV;
    ComPtr<ID3D11UnorderedAccessView> m_rainUAV;
};


