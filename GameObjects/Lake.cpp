#include "Lake.h"

void Lake::Initialize(ComPtr<ID3D11Device> &device,
                         ComPtr<ID3D11DeviceContext> &context, Matrix &Worldmat,
                         const std::string &filename) {


    int numStack = 10;
    int numSlice = 10;

    auto meshdata = GeometryGenerator::MakeSquareGrid(numStack, numSlice, 20.f);
    // auto meshdata = GeometryGenerator::MakeSquare(1.0f);
    vector<VertexIndex> p;
    float Index = 0;

    for (int i = 0; i < numStack; i++) // 테셀레이션 위해 패치 재정렬
    {
        for (int j = 0; j < numSlice; j++) {
            VertexIndex v1;

            // 텍스처 같은 거 여러번 붙일때
            // meshdata.vertices[(numSlice+1)*i+j].texcoord = Vector2(0.0f,
            // 0.0f); meshdata.vertices[(numSlice + 1) * i + j + 1].texcoord =
            //    Vector2(1.0f, 0.0f);
            // meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1].texcoord
            // =
            //    Vector2(0.0f, 1.0f);
            // meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2].texcoord
            // =
            //    Vector2(1.0f, 1.0f);

            v1.position = meshdata.vertices[(numSlice + 1) * i + j].position;
            v1.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j].normalModel;
            v1.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j].tangentModel;
            Vector2 texcoord =
                meshdata.vertices[(numSlice + 1) * i + j].texcoord;
            v1.texcoord = Vector3(texcoord.x, texcoord.y, Index);
            // v.index = Index;

            p.push_back(v1);



            VertexIndex v2;

            v2.position =
                meshdata.vertices[(numSlice + 1) * i + j + 1].position;
            v2.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j + 1].normalModel;
            v2.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j + 1].tangentModel;
            texcoord = meshdata.vertices[(numSlice + 1) * i + j + 1].texcoord;
            v2.texcoord = Vector3(texcoord.x, texcoord.y, Index);
            // v.index = Index;

            p.push_back(v2);




            VertexIndex v3;

            v3.position =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1]
                    .position;
            v3.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1]
                    .normalModel;
            v3.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1]
                    .tangentModel;
            texcoord = meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1]
                           .texcoord;
            v3.texcoord = Vector3(texcoord.x, texcoord.y, Index);
            // v.index = Index;

            p.push_back(v3);


            VertexIndex v4;

            v4.position =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]
                    .position;
            v4.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]
                    .normalModel;
            v4.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]
                    .tangentModel;
            texcoord = meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]
                           .texcoord;
            v4.texcoord = Vector3(texcoord.x, texcoord.y, Index);

            // v.index = Index;

            p.push_back(v4);


            // p.push_back(meshdata.vertices[(numSlice +1) *i +j]);
            // p.push_back(meshdata.vertices[(numSlice + 1) * i + j+1]);
            // p.push_back(meshdata.vertices[(numSlice + 1) * i + j +
            // numSlice+1]); p.push_back(
            //     meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]);
        }
    }



    m_meshConstsCPU.world = Worldmat.Transpose();
    m_meshConstsCPU.worldIT = Worldmat;

    m_meshConstsCPU.worldIT.Translation(Vector3(0.0f));
    m_meshConstsCPU.worldIT = m_meshConstsCPU.worldIT.Invert().Transpose();

    m_meshConstsCPU.worldIT = m_meshConstsCPU.worldIT.Transpose();

    D3D11Utils::CreateVertexBuffer(device, p, m_vertexBuffer);

    m_indexCount = uint32_t(p.size());

    D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);
    D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU,
                                  m_materialConstsGPU);
    D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);

    D3D11Utils::CreateTexture(device, context, filename, false, m_lakeTex, m_lakeSRV);


}

void Lake::Update(ComPtr<ID3D11Device> &device,
                  ComPtr<ID3D11DeviceContext> &context, Matrix reflectProj) {

        m_constantData.reflectProj = reflectProj;

    D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU,
                             m_materialConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU,
                             m_meshConstsGPU);
    D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);

    pos = m_worldRow.Translation();
      
}

void Lake::Render(ComPtr<ID3D11DeviceContext> &context,
                  const ComPtr<ID3D11ShaderResourceView> m_reflectSRV , const ComPtr<ID3D11ShaderResourceView> m_terrainPassSRV)
{  
                        
                         
    UINT stride = sizeof(VertexIndex); // sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);   
    context->DSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
    context->DSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
    context->HSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());

    context->HSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
    context->HSSetShaderResources(0, 1, m_terrainPassSRV.GetAddressOf());

    vector<ID3D11ShaderResourceView *> resViews = {
        m_reflectSRV.Get(), m_lakeSRV.Get(), m_terrainPassSRV.Get()};
    context->PSSetShaderResources(0, UINT(resViews.size()), resViews.data());
    context->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_materialConstsGPU.GetAddressOf());
          


    context->Draw(m_indexCount, 0);

    context->HSSetShader(nullptr, 0, 0);
    context->DSSetShader(nullptr, 0, 0);
}

void Lake::Render(ComPtr<ID3D11DeviceContext>& context)
{
    UINT stride = sizeof(VertexIndex); // sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                &offset);
    context->DSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
    context->DSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
    context->HSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());

    context->HSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());

  

    context->Draw(m_indexCount, 0);

    context->HSSetShader(nullptr, 0, 0);
    context->DSSetShader(nullptr, 0, 0);
}



