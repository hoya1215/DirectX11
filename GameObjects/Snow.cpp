#include "Snow.h"

 
	void Snow::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	{
    auto meshdata = GeometryGenerator::MakeSphere(0.01f, 40, 40);

	vector<SnowVertex> snowVertices(meshdata.vertices.size());

	for (int i = 0; i < snowVertices.size(); i++)
	{ 
        snowVertices[i].posModel = meshdata.vertices[i].position;
	}
	 
	D3D11Utils::CreateVertexBuffer(device, snowVertices, m_vertexBuffer);

	assert(m_instanceData.size() > 0);

	m_instanceCount = UINT(m_instanceData.size());
    D3D11Utils::CreateInstanceBuffer(device, m_instanceData, m_instanceBuffer);

	m_indexCount = UINT(meshdata.indices.size());
    m_vertexCount = UINT(snowVertices.size());
    D3D11Utils::CreateIndexBuffer(device, meshdata.indices, m_indexBuffer);


    }  

	void Snow::Update(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	{
	}

	void Snow::Render(ComPtr<ID3D11DeviceContext>& context)
	{
    ID3D11Buffer *const vertexBuffers[2] = {m_vertexBuffer.Get(),
                                            m_instanceBuffer.Get()};
    const UINT strides[2] = {sizeof(GrassVertex), sizeof(GrassInstance)};
    const UINT offsets[2] = {0, 0};
    context->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
    context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->DrawIndexedInstanced(m_indexCount, m_instanceCount, 0, 0, 0);
	}


