#pragma once

#include "ConstantBuffers.h"
#include "D3D11Utils.h"
#include "Mesh.h"
#include "MeshData.h"
#include "StructuredBuffer.h"


struct BoxInfo
{
        int pick;
    int IsCollisionBox = 0;
        Vector2 boxpadding;
};


class Model {
  public:
    Model() {}
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::string &basePath, const std::string &filename);
    Model(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context,
          const std::vector<MeshData> &meshes);

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    const std::string &basePath, const std::string &filename);

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context,
                    const std::vector<MeshData> &meshes);

    virtual void InitMeshBuffers(ComPtr<ID3D11Device> &device,
                                 const MeshData &meshData,
                                 shared_ptr<Mesh> &newMesh);

    void InitializeBoundingBox(ComPtr<ID3D11Device> &device, vector<Vertex> vertices);

    void UpdateConstantBuffers(ComPtr<ID3D11Device> &device,
                               ComPtr<ID3D11DeviceContext> &context);



    void Render(ComPtr<ID3D11DeviceContext> &context);

    virtual void UpdateAnimation(ComPtr<ID3D11DeviceContext> &context,
                                  int clipId, int frame);

    void RenderWireBBox(ComPtr<ID3D11DeviceContext> &context);

    void RenderNormals(ComPtr<ID3D11DeviceContext> &context);

    void UpdateWorldRow(const Matrix &worldRow);
     
public:
    Matrix m_worldRow = Matrix();   
    Matrix m_worldITRow = Matrix(); 
    Vector3 scale = Vector3(1.0f);
    Vector3 extent = Vector3(0.0f);
    Vector3 rotation = Vector3(0.0f);
    Quaternion q =
        Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);

    MeshConstants m_meshConstsCPU;  
    MaterialConstants m_materialConstsCPU;
    BoxInfo m_boxConstData;


    bool m_drawNormals = false;
    bool m_isVisible = true;
    bool m_castShadow = true;

    std::vector<shared_ptr<Mesh>> m_meshes;

    DirectX::BoundingBox m_box;
    DirectX::BoundingSphere m_sphere;

public: 
    ComPtr<ID3D11Buffer> m_meshConstsGPU;
    ComPtr<ID3D11Buffer> m_materialConstsGPU;
    ComPtr<ID3D11Buffer> m_boxConstBuffer;

    shared_ptr<Mesh> m_boxmesh;

    vector<MeshData> mesh;

    bool m_boxDraw = false; // boundingbox 가 그려질때만 edit 가능

};


