
#include "Model.h"
#include "GeometryGenerator.h"
#include <filesystem>


Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::string &basePath, const std::string &filename) {
    this->Initialize(device, context, basePath, filename);
}

Model::Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
             const std::vector<MeshData> &meshes) {
    this->mesh = meshes;
    this->Initialize(device, context, meshes);
}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::string &basePath,
                       const std::string &filename) {

    auto meshes = GeometryGenerator::ReadFromFile(basePath, filename);

    Initialize(device, context, meshes);
}

void Model::InitMeshBuffers(ComPtr<ID3D11Device> &device,
                            const MeshData &meshData,
                            shared_ptr<Mesh> &newMesh) {

    D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
                                   newMesh->vertexBuffer);
    newMesh->indexCount = UINT(meshData.indices.size());
    newMesh->vertexCount = UINT(meshData.vertices.size());
    newMesh->stride = UINT(sizeof(Vertex));
    D3D11Utils::CreateIndexBuffer(device, meshData.indices,
                                  newMesh->indexBuffer);
}

void Model::Initialize(ComPtr<ID3D11Device> &device,
                       ComPtr<ID3D11DeviceContext> &context,
                       const std::vector<MeshData> &meshes) {

    // ConstantBuffer 만들기
    m_meshConstsCPU.world = Matrix();

    D3D11Utils::CreateConstBuffer(device, m_meshConstsCPU, m_meshConstsGPU);
    D3D11Utils::CreateConstBuffer(device, m_materialConstsCPU,
                                  m_materialConstsGPU);
    D3D11Utils::CreateConstBuffer(device, m_boxConstData, m_boxConstBuffer);

    for (const auto &meshData : meshes) {
    auto newMesh = std::make_shared<Mesh>();

    InitMeshBuffers(device, meshData, newMesh);

    if (!meshData.albedoTextureFilename.empty()) {
        if (filesystem::exists(meshData.albedoTextureFilename)) {
            if (!meshData.opacityTextureFilename.empty()) {
                D3D11Utils::CreateTexture(
                    device, context, meshData.albedoTextureFilename,
                    meshData.opacityTextureFilename, false,
                    newMesh->albedoTexture, newMesh->albedoSRV);
            } else {
                D3D11Utils::CreateTexture(
                    device, context, meshData.albedoTextureFilename, true,
                    newMesh->albedoTexture, newMesh->albedoSRV);
            }

            m_materialConstsCPU.useAlbedoMap = true;
        } else {
            cout << meshData.albedoTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    if (!meshData.emissiveTextureFilename.empty()) {
        if (filesystem::exists(meshData.emissiveTextureFilename)) {
            D3D11Utils::CreateTexture(
                device, context, meshData.emissiveTextureFilename, true,
                newMesh->emissiveTexture, newMesh->emissiveSRV);
            m_materialConstsCPU.useEmissiveMap = true;
        } else {
            cout << meshData.emissiveTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    if (!meshData.normalTextureFilename.empty()) {
        if (filesystem::exists(meshData.normalTextureFilename)) {
            D3D11Utils::CreateTexture(
                device, context, meshData.normalTextureFilename, false,
                newMesh->normalTexture, newMesh->normalSRV);
            m_materialConstsCPU.useNormalMap = true;
        } else {
            cout << meshData.normalTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    if (!meshData.heightTextureFilename.empty()) {
        if (filesystem::exists(meshData.heightTextureFilename)) {
            D3D11Utils::CreateTexture(
                device, context, meshData.heightTextureFilename, false,
                newMesh->heightTexture, newMesh->heightSRV);
            m_meshConstsCPU.useHeightMap = true;
        } else {
            cout << meshData.heightTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    if (!meshData.aoTextureFilename.empty()) {
        if (filesystem::exists(meshData.aoTextureFilename)) {
            D3D11Utils::CreateTexture(device, context,
                                      meshData.aoTextureFilename, false,
                                      newMesh->aoTexture, newMesh->aoSRV);
            m_materialConstsCPU.useAOMap = true;
        } else {
            cout << meshData.aoTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    // GLTF 방식으로 Metallic과 Roughness를 한 텍스춰에 넣음
    // Green : Roughness, Blue : Metallic(Metalness)
    if (!meshData.metallicTextureFilename.empty() ||
        !meshData.roughnessTextureFilename.empty()) {

        if (filesystem::exists(meshData.metallicTextureFilename) &&
            filesystem::exists(meshData.roughnessTextureFilename)) {

            D3D11Utils::CreateMetallicRoughnessTexture(
                device, context, meshData.metallicTextureFilename,
                meshData.roughnessTextureFilename,
                newMesh->metallicRoughnessTexture,
                newMesh->metallicRoughnessSRV);
        } else {
            cout << meshData.metallicTextureFilename << " or "
                 << meshData.roughnessTextureFilename
                 << " does not exists. Skip texture reading." << endl;
        }
    }

    if (!meshData.metallicTextureFilename.empty()) {
        m_materialConstsCPU.useMetallicMap = true;
    }

    if (!meshData.roughnessTextureFilename.empty()) {
        m_materialConstsCPU.useRoughnessMap = true;
    }

    newMesh->vertexConstBuffer = m_meshConstsGPU;
        newMesh->pixelConstBuffer = m_materialConstsGPU;

    this->m_meshes.push_back(newMesh);
    }

    this->InitializeBoundingBox(device, meshes[0].vertices);

}

void Model::InitializeBoundingBox(ComPtr<ID3D11Device> &device, vector<Vertex> vertices)
{
    Vector3 minPos = vertices[0].position;
    Vector3 maxPos = vertices[0].position;

    for (int i = 0; i < vertices.size(); i++)
    {
        minPos = Vector3::Min(minPos, vertices[i].position);
        maxPos = Vector3::Max(maxPos, vertices[i].position);
    }



    m_box.Center = (minPos + maxPos) * 0.5f;
    m_sphere.Center = m_box.Center;
    m_sphere.Radius = (maxPos - minPos).Length() * 0.5f;
    m_box.Extents =
        Vector3(maxPos.x - minPos.x, maxPos.y - minPos.y, maxPos.z - minPos.z) * 0.5f;

    extent = m_box.Extents;

    auto meshdata =
        GeometryGenerator::MakeWireBBox(m_box.Center, m_box.Extents);

    m_boxmesh = std::make_shared<Mesh>();
    D3D11Utils::CreateVertexBuffer(device, meshdata.vertices, m_boxmesh->vertexBuffer);

    m_boxmesh->vertexCount = UINT(meshdata.vertices.size());
    m_boxmesh->indexCount = UINT(meshdata.indices.size());
    m_boxmesh->stride = UINT(sizeof(Vertex));
    D3D11Utils::CreateIndexBuffer(device, meshdata.indices,
                                  m_boxmesh->indexBuffer);

}

void Model::UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                                  ComPtr<ID3D11DeviceContext> &context) {
    if (m_isVisible) {
        D3D11Utils::UpdateBuffer(device, context, m_meshConstsCPU,
                                 m_meshConstsGPU);

        D3D11Utils::UpdateBuffer(device, context, m_materialConstsCPU,
                                 m_materialConstsGPU);
        D3D11Utils::UpdateBuffer(device, context, m_boxConstData,
                                 m_boxConstBuffer);
         
        Vector3 center = m_worldRow.Translation();

        UpdateWorldRow(Matrix::CreateScale(scale) * Matrix::CreateRotationX(rotation.x) *
            Matrix::CreateRotationY(rotation.y) *
            Matrix::CreateRotationZ(rotation.z) *
                       Matrix::CreateTranslation(center));

        m_box.Center = center;
        //m_box.Extents =
        //    Vector3(extent.x * m_worldRow._11, extent.y * m_worldRow._22,
        //            extent.z * m_worldRow._33);
        m_box.Extents =
            Vector3(extent.x * scale.x, extent.y * scale.y, extent.z * scale.z);

        m_sphere.Center = m_box.Center;
        Vector3 minVertex = m_box.Center - m_box.Extents;
        Vector3 maxVertex = m_box.Center + m_box.Extents;
        m_sphere.Radius = (maxVertex - minVertex).Length() * 0.5f;


        //m_box.Extents.x = m_box.Extents.x * this->m_worldRow._11;
        //m_box.Extents.y = m_box.Extents.y * this->m_worldRow._22;
        //m_box.Extents.z = m_box.Extents.z * this->m_worldRow._33;

    }
}



void Model::Render(ComPtr<ID3D11DeviceContext> &context) {
    if (m_isVisible) {
        for (const auto &mesh : m_meshes) {
            context->VSSetConstantBuffers(
                0, 1, mesh->vertexConstBuffer.GetAddressOf());
            context->PSSetConstantBuffers(
                0, 1, mesh->pixelConstBuffer.GetAddressOf()); 

            //context->PSSetConstantBuffers(
            //   1, 1, mesh->vertexConstBuffer.GetAddressOf());
              
            //context->VSSetShaderResources(0, 1, mesh->heightSRV.GetAddressOf());

            // 물체 렌더링할 때 여러가지 텍스춰 사용 (t0 부터시작)
            vector<ID3D11ShaderResourceView *> resViews = {
                mesh->albedoSRV.Get(), mesh->normalSRV.Get(), mesh->aoSRV.Get(),
                mesh->metallicRoughnessSRV.Get(), mesh->emissiveSRV.Get()};
            context->PSSetShaderResources(0, UINT(resViews.size()),
                                          resViews.data());

            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                                        &mesh->stride, &mesh->offset);

            context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                                      DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(mesh->indexCount, 0, 0);
        }
    }
}

void Model::RenderWireBBox(ComPtr<ID3D11DeviceContext>& context)
{
    ID3D11Buffer *constBuffer[2] = {m_meshConstsGPU.Get(),
                                    m_materialConstsGPU.Get()};
    context->VSSetConstantBuffers(0, 2, constBuffer);
    context->PSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf()); 
    context->PSSetConstantBuffers(1, 1, m_boxConstBuffer.GetAddressOf());
    context->IASetVertexBuffers(0, 1, m_boxmesh->vertexBuffer.GetAddressOf(),
                                &m_boxmesh->stride, &m_boxmesh->offset);
    context->IASetIndexBuffer(m_boxmesh->indexBuffer.Get(),
                              DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexed(m_boxmesh->indexCount, 0, 0);
}

void Model::RenderNormals(ComPtr<ID3D11DeviceContext> &context) {
    for (const auto &mesh : m_meshes) {
        context->GSSetConstantBuffers(0, 1, m_meshConstsGPU.GetAddressOf());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
                                    &mesh->stride, &mesh->offset);
        context->Draw(mesh->vertexCount, 0);
    }
}

void Model::UpdateWorldRow(const Matrix &worldRow) {
    this->m_worldRow = worldRow;
    this->m_worldITRow = worldRow;
    m_worldITRow.Translation(Vector3(0.0f));
    m_worldITRow = m_worldITRow.Invert().Transpose();

    m_meshConstsCPU.world = worldRow.Transpose();
    m_meshConstsCPU.worldIT = m_worldITRow.Transpose();
}

void Model::UpdateAnimation(ComPtr<ID3D11DeviceContext> &context, int clipId,
                            int frame) {
    // class SkinnedMeshModel에서 override
    cout << "Model::UpdateAnimation(ComPtr<ID3D11DeviceContext> &context, "
            "int clipId, int frame) was not implemented."
         << endl;
    exit(-1);
}

