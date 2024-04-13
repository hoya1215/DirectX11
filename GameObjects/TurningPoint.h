#pragma once
#include "Headers.h"
#include "D3D11Utils.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "Camera.h"

    struct TurningPointConstantData {
    Vector3 turningPos;
    float turningRadius;
    float turningAngle;
    Vector3 centerVector;
    Vector3 startPos;
    int TPpadding;
    Vector3 endPos;
    int AreaOn; // 그려줄지 안그려줄지
};

	class Point : public Model
    {
  public:
      Point(ComPtr<ID3D11Device>& device,
          ComPtr<ID3D11DeviceContext>& context,
          const vector<MeshData>& meshes)
      {
        Model::Initialize(device, context, meshes);
        }

      void UpdateRange(ComPtr<ID3D11Device>& device,
          ComPtr<ID3D11DeviceContext>& context)
      {
        m_constantData.centerVector = centerVector;
        m_constantData.endPos = endPos;
        m_constantData.startPos = startPos;
        m_constantData.turningPos = pos;
        m_constantData.turningRadius = radius;

        m_constantData.startPos.Normalize();
        m_constantData.endPos.Normalize();

        D3D11Utils::UpdateBuffer(device, context, m_constantData,
                                 m_constantBuffer);
      }
  public:
    Vector3 pos;
    float radius;
    float angle;
    float startAngle = 0.0f;
    Vector3 centerVector;
    Vector3 startPos;
    Vector3 endPos;
    int DirFlag = 0;  // 0 : left    1 : right

    ComPtr<ID3D11Buffer> m_vertexBuffer;
    uint32_t m_indexCount;

    TurningPointConstantData m_constantData;
    ComPtr<ID3D11Buffer> m_constantBuffer;
    };  



class TurningPoint {



  public:
        TurningPoint(){}; 

    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context);

    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);
    void Render(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);

    void RenderRange(ComPtr<ID3D11Device> &device,
                     ComPtr<ID3D11DeviceContext> &context);

    bool InArea(const Vector3 characterPos); // 캐릭터가 들어와있는지 검사 next , prev 두점만 

    void AddPoint(ComPtr<ID3D11Device> &device, ComPtr<ID3D11DeviceContext> &context, Vector3 mousePos); // 점 추가

    void Turning(Camera &Editcamera,
                 const Vector3 characterPos); // 캐릭터가 전환점을 기준으로 회전

    TurningPointConstantData m_constantData;
    ComPtr<ID3D11Buffer> m_constantBuffer;


    public:
    vector<shared_ptr<Point>> m_points;
      shared_ptr<Point> m_nextPoint;
    shared_ptr<Point> m_prevPoint;
      shared_ptr<Point> m_currentPoint;
    shared_ptr<Point> m_pickedPoint;
      bool b_drawRange = false;


};
