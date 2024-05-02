#include "Terrain.h"
#include "MeshData.h"
#include "AppBase.h"
#include "SceneManager.h"



	void Terrain::Initialize(ComPtr<ID3D11Device> &device,ComPtr<ID3D11DeviceContext> &context, Matrix Worldmat,
                         const std::string &filename)
	{ 

    // Sampler 만들기 
    D3D11_SAMPLER_DESC sampDesc;   
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0; 
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, m_samplerState.GetAddressOf());

    int numStack = 10; 
    int numSlice = 10;        

    auto meshdata = GeometryGenerator::MakeSquareGrid(numStack, numSlice, 20.f);  
    //auto meshdata = GeometryGenerator::MakeSquare(1.0f);
    vector<VertexIndex> p;  
    float Index = 0;

    for (int i = 0; i < numStack; i++) // 테셀레이션 위해 패치 재정렬
    {
        for (int j = 0; j < numSlice; j++)
        {
            VertexIndex v1;
            HeightInfo h;


            // 텍스처 같은 거 여러번 붙일때
            //meshdata.vertices[(numSlice+1)*i+j].texcoord = Vector2(0.0f, 0.0f);
            //meshdata.vertices[(numSlice + 1) * i + j + 1].texcoord =
            //    Vector2(1.0f, 0.0f);
            //meshdata.vertices[(numSlice + 1) * i + j + numSlice + 1].texcoord =
            //    Vector2(0.0f, 1.0f);
            //meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2].texcoord =
            //    Vector2(1.0f, 1.0f);
             
            v1.position = meshdata.vertices[(numSlice + 1) * i + j].position;
            v1.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j].normalModel;
            v1.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j].tangentModel;
            Vector2 texcoord = 
                meshdata.vertices[(numSlice + 1) * i + j].texcoord;
            v1.texcoord = Vector3(texcoord.x, texcoord.y, Index);
            //v.index = Index;

            p.push_back(v1);
            h.pos = v1.position;
            h.index = Index; 
            m_heightInfo.push_back(h);

            Index++; 

                        VertexIndex v2;

            v2.position = meshdata.vertices[(numSlice + 1) * i + j+1].position;
            v2.normalModel =
                meshdata.vertices[(numSlice + 1) * i + j+1].normalModel;
            v2.tangentModel =
                meshdata.vertices[(numSlice + 1) * i + j+1].tangentModel;
            texcoord = meshdata.vertices[(numSlice + 1) * i + j + 1].texcoord;
            v2.texcoord =Vector3(texcoord.x , texcoord.y, Index);
            //v.index = Index;

            p.push_back(v2);
            h.pos = v2.position;
            h.index = Index;
            m_heightInfo.push_back(h);

            Index++;

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
            //v.index = Index;

            p.push_back(v3);
            h.pos = v3.position;
            h.index = Index;
            m_heightInfo.push_back(h);

            Index++;



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
            h.pos = v4.position;
            h.index = Index;  
            m_heightInfo.push_back(h);

            Index++;

            //p.push_back(meshdata.vertices[(numSlice +1) *i +j]);
            //p.push_back(meshdata.vertices[(numSlice + 1) * i + j+1]);
            //p.push_back(meshdata.vertices[(numSlice + 1) * i + j + numSlice+1]);
            //p.push_back(
            //    meshdata.vertices[(numSlice + 1) * i + j + numSlice + 2]);
        }
    }
    CharacterHeight C;
    C.height = 0.0f;
    m_CharacterHeight.push_back(C);

    m_constantData.World = Worldmat.Transpose(); 
    m_constantData.Useheightmap = 1;

    m_meshConstsCPU.world = Worldmat.Transpose();
    m_meshConstsCPU.worldIT = Worldmat;

        m_meshConstsCPU.worldIT.Translation(Vector3(0.0f));
    m_meshConstsCPU.worldIT = m_meshConstsCPU.worldIT.Invert().Transpose();

    m_meshConstsCPU.worldIT = m_meshConstsCPU.worldIT.Transpose();

    m_constantData.CorrectionAxis = Vector4(100.0f, 100.0f, 100.0f, 100.0f);
    m_constantData.CorrectionHeight = 0.0f;



    D3D11Utils::CreateVertexBuffer(device, p, m_vertexBuffer);

     m_indexCount = uint32_t(p.size());

     CreateBuffer(device);

     CreateComputeShader(device);

     CreateTexture(device, context);
     

     // 텍스춰 초기화
     // D3D11Utils::CreateTextureArray(device, filenames, m_texArray,
     //                               m_texArraySRV);

     // SRV 생성
     D3D11Utils::CreateTexture(device, context, filename, false, m_heightTex,
                               m_heightSRV);
             
     this->InitializeBoundingBox(device, meshdata.vertices);

     TextureBufferInitialize(device, context);

	}

    void Terrain::Update(ComPtr<ID3D11Device> &device, 
                         ComPtr<ID3D11DeviceContext> &context, Matrix view,
                         Matrix proj, Vector3 eyeworld) 
    {
    // m_constantData.eyeWorld = eyeworld;  
     //m_constantData.view = view.Transpose();
     //m_constantData.proj = proj.Transpose();


     D3D11Utils::UpdateBuffer(device,context, m_constantData, m_constantBuffer);
     D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU,
                              m_materialConstsGPU);
     D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU,
                              m_meshConstsGPU);

     context->CopyResource(m_stagingBuffer.Get(), m_heightInfoBuffer.Get());

     //context->CopyResource(m_stagingTexture.Get(), m_terrainPassBuffer.Get());


    }

     void Terrain::HeightRender(ComPtr<ID3D11DeviceContext> &context)
     {  
     // Compute Shader에서 높이 위치 변경
     if (m_constantData.Useheightmap == 0) {
        context->CSSetUnorderedAccessViews(0, 1, m_heightInfoUAV.GetAddressOf(),
                                           NULL);
        context->CSSetShaderResources(0, 1, m_terrainPassSRV.GetAddressOf());
        context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
        context->CSSetShader(m_terrainCS.Get(), 0, 0);
        context->Dispatch(UINT(ceil(m_heightInfo.size() / 400.0f)), 1, 1);
        this->ComputeShaderBarrier(context);     
     }  
     }       

     void Terrain::PixelRender(ComPtr<ID3D11DeviceContext>& context)
     {
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPixelUAV.GetAddressOf(),
                                        NULL);
     context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
     context->CSSetShader(m_terrainPixelCS.Get(), 0, 0);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)), 1);
     this->ComputeShaderBarrier(context);  
     } 

     void Terrain::PassRender( 
         ComPtr<ID3D11DeviceContext> &context)
     {
         context->CSSetShader(m_terrainPassCS.Get(), 0, 0);
     context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
         context->CSSetUnorderedAccessViews(
             0, 1, m_terrainPassUAV.GetAddressOf(), NULL);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)),
                       1);
         this->ComputeShaderBarrier(context);
     }
       
     void Terrain::SmoothRender(ComPtr<ID3D11DeviceContext>& context)
     {
         context->CSSetShader(m_terrainSmoothCS.Get(), 0, 0);
         context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
         context->CSSetUnorderedAccessViews(
             0, 1, m_terrainPassUAV.GetAddressOf(), NULL);
         context->Dispatch(UINT(ceil(1280.0f / 32.0f)),
                           UINT(ceil(760.0f / 32.0f)), 1);
         this->ComputeShaderBarrier(context);
     }


    void Terrain::Render(ComPtr<ID3D11DeviceContext> &context)
    { 
         
             ID3D11ShaderResourceView *resViews[2] = {m_heightSRV.Get(),
                                                  m_terrainPassSRV.Get()};
                
     UINT stride = sizeof(VertexIndex); // sizeof(Vertex); 
     UINT offset = 0;
     context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride,
                                 &offset);   
     context->VSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
     context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
     context->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());
     context->VSSetShaderResources(0, 2, resViews);  
     context->PSSetConstantBuffers(0, 1, m_materialConstsGPU.GetAddressOf());
     context->PSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
     context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());     
     vector<ID3D11ShaderResourceView *>resViews1 = {m_albedoSRV.Get(),
                                              m_normalSRV.Get(), m_aoSRV.Get(),           
                                              m_roughnessSRV.Get(), m_albedoSRV02.Get(), m_terrainPixelSRV.Get(), m_sandSRV.Get(), m_grassSRV.Get()}; 
      context->PSSetShaderResources(0, UINT(resViews1.size()), resViews1.data()); 
      //픽셀 srv 세팅 나중에
                   

     context->HSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());   
       

     context->DSSetSamplers(0, 1, m_samplerState.GetAddressOf());
     context->DSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
     context->DSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());


     context->DSSetShaderResources(0, 2, resViews);       
            

     context->Draw(m_indexCount, 0);
       
     context->HSSetShader(nullptr, 0, 0);
     context->DSSetShader(nullptr, 0, 0);
    }

    void Terrain::CreateBuffer(ComPtr<ID3D11Device>& device)
    {
     D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);
     D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU,
                                   m_materialConstsGPU);
     D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);

     D3D11Utils::CreateStructuredBuffer(device, UINT(m_heightInfo.size()),
                                        sizeof(HeightInfo), m_heightInfo.data(),
                                        m_heightInfoBuffer, m_heigthInfoSRV,
                                        m_heightInfoUAV);

     D3D11Utils::CreateStagingBuffer(device, UINT(m_heightInfo.size()),
                                     sizeof(HeightInfo), m_heightInfo.data(),
                                     m_stagingBuffer); 

     D3D11Utils::CreateStructuredBuffer(
         device, UINT(m_CharacterHeight.size()), sizeof(CharacterHeight),
         m_CharacterHeight.data(), m_CharacterHeightBuffer,
         m_CharacterHeightSRV, m_CharacterHeightUAV);
     D3D11Utils::CreateStagingBuffer(
         device, UINT(m_CharacterHeight.size()), sizeof(CharacterHeight),
         m_CharacterHeight.data(), m_heightStagingBuffer);
    }

    void Terrain::CreateComputeShader(ComPtr<ID3D11Device>& device)
    {
     D3D11Utils::CreateComputeShader(device, L"TerrainCS.hlsl", m_terrainCS);
     D3D11Utils::CreateComputeShader(device, L"TerrainSmoothCS.hlsl",
                                     m_terrainSmoothCS);

     D3D11Utils::CreateComputeShader(device, L"TerrainPixelCS.hlsl",
                                     m_terrainPixelCS);
     D3D11Utils::CreateComputeShader(device, L"TerrainPassCS.hlsl",
                                     m_terrainPassCS);
     D3D11Utils::CreateComputeShader(device, L"TerrainChangeTextureCS.hlsl",
                                     m_changeTextureCS);
     D3D11Utils::CreateComputeShader(device, L"TerrainCorrectionCS.hlsl",
                                     m_terrainCorrectionCS);
     D3D11Utils::CreateComputeShader(device, L"GetHeightCS.hlsl",
                                     m_getheightCS);
    }
      
    void Terrain::CreateTexture(ComPtr<ID3D11Device>& device,
        ComPtr<ID3D11DeviceContext>& context)
    {


     D3D11Utils::CreateTexture(device, context, "d:/image/terrain/Albedo.jpg",
                               true, m_albedoTex, m_albedoSRV);

     D3D11Utils::CreateTexture(device, context,
                               "d:/image/terrain/TerrainSnow.jpg", true,
                               m_albedoTex02, m_albedoSRV02);

     D3D11Utils::CreateTexture(device, context, "d:/image/terrain/Normal.jpg",
                               true, m_normalTex, m_normalSRV);

     D3D11Utils::CreateTexture(device, context, "d:/image/terrain/Ao.jpg", true,
                               m_aoTex, m_aoSRV);

     D3D11Utils::CreateTexture(device, context,
                               "d:/image/terrain/Roughness.jpg", true,
                               m_roughnessTex, m_roughnessSRV);

     D3D11Utils::CreateTexture(device, context, "d:/image/terrain/Sand.png",
                               true, m_sandTex, m_sandSRV);
     D3D11Utils::CreateTexture(device, context, "d:/image/terrain/Grass.jpeg",
                               false, m_grassTex, m_grassSRV);

     m_materialConstsCPU.useAlbedoMap = 1;
     m_materialConstsCPU.useAOMap = 1;
     m_materialConstsCPU.useNormalMap = 1;
     m_materialConstsCPU.useRoughnessMap = 1;
     m_materialConstsCPU.TexOnly = 1;
     m_meshConstsCPU.useHeightMap = 0;
     m_meshConstsCPU.heightScale = 1.0f;
     m_constantData.Useheightmap = 0;  

    }

    void Terrain::TextureBufferInitialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
    {
     // Terrain Pixel Texture
     D3D11Utils::CreateTexture2D(device, context, m_terrainPixelBuffer,
                                 m_terrainPixelSRV, m_terrainPixelUAV);

     context->CSSetShader(Graphics::m_InitPixelCS.Get(), 0, 0);
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPixelUAV.GetAddressOf(),
                                        NULL);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)),
                       1);

     this->ComputeShaderBarrier(context);

     // Terrain Pass Texture
     D3D11Utils::CreateTexture2D(device, context, m_terrainPassBuffer,
                                 m_terrainPassSRV, m_terrainPassUAV);
     context->CSSetShader(Graphics::m_InitPixelCS.Get(), 0, 0);
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPassUAV.GetAddressOf(),
                                        NULL);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)),
                       1);

     D3D11Utils::CreateStagingTexture2(device, context, m_stagingTexture);
     D3D11Utils::CreateResolveTexture2(device, context, m_resolveTexture);
    }

    void Terrain::ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context)
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

    void Terrain::heightCorrection(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context)
    {
        
        // 최대 최소 높이 찾기
    float heightMax1 = 0;
     float heightMin1 = 100; 

     float heightMax2 = -100;
     float heightMin2 = 100;

     Vector4 axis = m_constantData.CorrectionAxis;

             // UAV 매핑
             HeightInfo *  
         pData;
     D3D11_MAPPED_SUBRESOURCE mappedResource;
     context->Map(m_stagingBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);
     pData = (HeightInfo *)mappedResource.pData;

    // 높이 최대 최소 구하기

     for (int i = 0; i < m_heightInfo.size(); i++)
     {


        if (pData[i].pos.z < 0 && pData[i].pos.x >= axis.x &&
            pData[i].pos.x <= axis.y && -pData[i].pos.y >= axis.z && -pData[i].pos.y <= axis.w) {
            heightMax1 = max(heightMax1, abs(pData[i].pos.z));
            heightMin1 = min(heightMin1, abs(pData[i].pos.z));
        } 

     }



       
       
     // 매핑 해제
     context->Unmap(m_stagingBuffer.Get(), 0);



     this->m_constantData.CorrectionHeight = heightMax1 * 0.5f;

     cout << heightMax1 << endl;

     D3D11Utils::UpdateBuffer(device, context, m_constantData,
                              m_constantBuffer);


     
        // 보정할 높이 계산 후 바꿔주기
     context->CSSetShader(m_terrainCorrectionCS.Get(), 0, 0);
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPassUAV.GetAddressOf(),
                                        NULL);
     context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(720.0f / 32.0f)),
                       1);

     this->ComputeShaderBarrier(context);


    }

    void Terrain::heightReset(ComPtr<ID3D11DeviceContext>& context)
    {

     // 변경 전
     
     //HeightInfo *pData;
     //D3D11_MAPPED_SUBRESOURCE mappedResource;
     //context->Map(m_stagingBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0,
     //             &mappedResource);
     //pData = (HeightInfo *)mappedResource.pData;


     //// 높이값 초기상태로 초기화
     //for (int i = 0; i < m_heightInfo.size(); i++)
     //{
     //   pData[i].pos.z = 0.0f;
     //}
     // 
     //context->Unmap(m_stagingBuffer.Get(), 0);

     //context->CopyResource(m_heightInfoBuffer.Get(), m_stagingBuffer.Get());


    // 변경 후

    context->CSSetShader(Graphics::m_InitPixelCS.Get(), 0, 0);
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPassUAV.GetAddressOf(),
                                        NULL);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)),
                       1);
     this->ComputeShaderBarrier(context);

    }

    void Terrain::ChangeTexture(ComPtr<ID3D11DeviceContext>& context)
    {
     context->CSSetShader(m_changeTextureCS.Get(), 0, 0);
     context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
     context->CSSetUnorderedAccessViews(0, 1, m_terrainPixelUAV.GetAddressOf(), NULL);
     context->Dispatch(UINT(ceil(1280.0f / 32.0f)), UINT(ceil(760.0f / 32.0f)),
                       1);
     this->ComputeShaderBarrier(context);  
    }

    float Terrain::GetMaxheight(ComPtr<ID3D11DeviceContext> &context,
                                float minX, float maxX, float minZ,
                                float maxZ)
    {
     // 최대 최소 높이 찾기
     float heightMax = 0.f;

     // UAV 매핑
     HeightInfo *pData;
     D3D11_MAPPED_SUBRESOURCE mappedResource;
     context->Map(m_stagingBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0,
                  &mappedResource);
     pData = (HeightInfo *)mappedResource.pData;

     // 높이가 최대인 점 찾기  

     for (int i = 0; i < m_heightInfo.size(); i++) {


        if (-pData[i].pos.z > heightMax && pData[i].pos.x >= minX &&
            pData[i].pos.x <= maxX && pData[i].pos.y >= minZ && pData[i].pos.y <= maxZ)
                heightMax = -pData[i].pos.z;
     }
 
     // 매핑 해제
     context->Unmap(m_stagingBuffer.Get(), 0);

     return heightMax;
    }

    float Terrain::GetheightGPUtoCPU(ComPtr<ID3D11DeviceContext>& context)
    {
     context->CSSetShader(m_getheightCS.Get(), 0, 0);
     context->CSSetShaderResources(0, 1, m_terrainPassSRV.GetAddressOf());
     context->CSSetUnorderedAccessViews(0, 1,
                                        m_CharacterHeightUAV.GetAddressOf(), NULL);

     context->Dispatch(1, 1, 1);  
     this->ComputeShaderBarrier(context);

     context->CopyResource(m_heightStagingBuffer.Get(),
                           m_CharacterHeightBuffer.Get());

     float height = -1.0f; // height 값 제대로 나오는지 디버그 확인용으로 0 이 아닌 -1 값으로 지정

      CharacterHeight *pData;
      D3D11_MAPPED_SUBRESOURCE mappedResource;
      context->Map(m_heightStagingBuffer.Get(), 0, D3D11_MAP_READ_WRITE, 0,
                   &mappedResource);
      pData = (CharacterHeight *)mappedResource.pData;

      height = pData->height;
     
      context->Unmap(m_heightStagingBuffer.Get(), 0);

      return height;
    }

