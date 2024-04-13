#pragma once
#include "D3D11Utils.h"
#include "Headers.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "MeshData.h"


    struct CollisionBoxConstantData
    {
    Matrix world;
    };

	class CollisionBox
	{
		public:
    CollisionBox(){}
        CollisionBox(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context, Vector3 center, Vector3 extent) {
            m_box.Center = center;
            m_box.Extents = extent;

            Initialize(device, context);
        }

        void Initialize(ComPtr<ID3D11Device> &device,
                        ComPtr<ID3D11DeviceContext> &context)
        {
            // box mesh
            meshdata =
                GeometryGenerator::MakeWireBBox(m_box.Center, m_box.Extents);

            mesh = make_shared<Mesh>();

            D3D11Utils::CreateVertexBuffer(device, meshdata.vertices,
                                           mesh->vertexBuffer);

            mesh->vertexCount = UINT(meshdata.vertices.size());
            mesh->indexCount = UINT(meshdata.indices.size());
            mesh->stride = UINT(sizeof(Vertex));
            D3D11Utils::CreateIndexBuffer(device, meshdata.indices,
                                          mesh->indexBuffer);

            D3D11Utils::CreateConstBuffer(device, m_constantData,
                                          m_constantBuffer);

            m_constantData.world = Matrix::CreateTranslation(m_box.Center);
            m_constantData.world = m_constantData.world.Transpose();

            D3D11Utils::UpdateBuffer(device, context, m_constantData,
                                     m_constantBuffer);
        }

        void Render(ComPtr<ID3D11DeviceContext>& context)
        {

            context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

            context->IASetVertexBuffers(0, 1,
                                        mesh->vertexBuffer.GetAddressOf(),
                                        &mesh->stride, &mesh->offset);
            context->IASetIndexBuffer(mesh->indexBuffer.Get(),
                                      DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(mesh->indexCount, 0, 0);
        }

  public:
        CollisionBoxConstantData m_constantData;
    ComPtr<ID3D11Buffer> m_constantBuffer;
        MeshData meshdata;

        DirectX::BoundingBox m_box;
        shared_ptr<Mesh> mesh;

    };


