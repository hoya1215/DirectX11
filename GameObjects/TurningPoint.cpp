#include "TurningPoint.h"

void TurningPoint::Initialize(ComPtr<ID3D11Device>& device,
	ComPtr<ID3D11DeviceContext>& context)
{
    m_nextPoint = nullptr;
    m_prevPoint = nullptr;
    m_currentPoint = nullptr;

	D3D11Utils::CreateConstBuffer(device, m_constantData, m_constantBuffer);
}

void TurningPoint::Update(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) {
    if (m_pickedPoint) {
        m_constantData.turningPos = m_pickedPoint->pos;
        m_constantData.turningAngle = m_pickedPoint->angle;
        m_constantData.turningRadius = m_pickedPoint->radius;
        m_constantData.centerVector = m_pickedPoint->centerVector;
        m_constantData.startPos = m_pickedPoint->startPos;
        m_constantData.endPos = m_pickedPoint->endPos;
        m_constantData.AreaOn = 1;


    } else
        m_constantData.AreaOn = 0;

        D3D11Utils::UpdateBuffer(device, context, m_constantData, m_constantBuffer);
       
    for (auto& i : m_points)  
    {
        Vector3 left =Vector3(-i->radius, 0.0f, 0.0f);


        left = Vector3::Transform(left, Matrix::CreateRotationY(i->startAngle));


        Vector3 up;
        
        if (i->DirFlag == 0)
            up =Vector3::Transform(left, Matrix::CreateRotationY(i->angle));
        else if (i->DirFlag == 1)
            up = Vector3::Transform(left, Matrix::CreateRotationY(-i->angle));

        i->endPos = up;


        i->centerVector = (left + up) * 0.5f;
        i->centerVector.Normalize();
        if (i->angle >= XM_PI)
            i->centerVector = -i->centerVector;
        i->startPos = left;
        i->startPos.Normalize();

        i->UpdateConstantBuffers(device, context);
        i->UpdateRange(device, context);
    } 

}

void TurningPoint::Render(ComPtr<ID3D11Device> &device,
                          ComPtr<ID3D11DeviceContext> &context) {

    for (auto &i : m_points)
        i->Render(context);

}

void TurningPoint::RenderRange(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context)
{
    for (auto& i : m_points)
    {
        context->GSSetConstantBuffers(0, 1, i->m_meshConstsGPU.GetAddressOf());
        context->GSSetConstantBuffers(1, 1, i->m_constantBuffer.GetAddressOf());

        UINT stride = sizeof(Vector4); // sizeof(Vertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, i->m_vertexBuffer.GetAddressOf(),
                                    &stride, &offset);

        context->Draw(i->m_indexCount, 0);
    }
}


bool TurningPoint::InArea(const Vector3 characterPos) {
	
    // 모든 점과 거리비교해서 최소인점 찾기
    float dist = 100.0f;
    shared_ptr<Point> p; // 영역 검사할 점

    for (auto& i : m_points)
    {
        Vector3 Vec = (characterPos - i->pos);
        Vec.y = 0.0f;

        float length = Vec.Length();

        if (length < dist)
        {
            dist = length;
            p = i;
        }
    }

    // 영역 검사
    Vector3 CVec = Vector3(characterPos - p->pos);
    CVec.y = 0.0f;
    CVec.Normalize();
    float cosTheta = CVec.Dot(p->centerVector);
    float theta = acos(cosTheta);

    if (dist <= p->radius && theta <= p->angle * 0.5f) // 영역안에 있음
    {
        m_currentPoint = p;
        return true;
    }
    else
    {
        m_currentPoint = nullptr;
        return false;
    }
}

void TurningPoint::AddPoint(ComPtr<ID3D11Device> &device,
                            ComPtr<ID3D11DeviceContext> &context,
                            Vector3 mousePos) {
    auto meshdata = GeometryGenerator::MakeSphere(0.2f, 10, 10);
    Vector3 center = mousePos;
    center.y = 0.0f;
    shared_ptr<Point> point = make_shared<Point>(device, context,vector{meshdata});
    point->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 0.0f, 0.0f);
    point->UpdateWorldRow(Matrix::CreateTranslation(center));
    point->pos = center;
    point->radius = 2.0f;
    point->angle = XM_PI * 0.5f;

	Vector3 left = Vector3(-2.0f, 0.0f, 0.0f);


	Vector3 up =
        Vector3::Transform(left, Matrix::CreateRotationY(point->angle));

	point->centerVector = (left + up) * 0.5f;
    point->centerVector.Normalize();



	if (!m_nextPoint)
        m_nextPoint = point;

    vector<Vector4> cPos;
    Vector4 cPoint = Vector4(center.x, center.y, center.z, 1.0);
    cPos.push_back(cPoint);
     
    D3D11Utils::CreateVertexBuffer(device, cPos, point->m_vertexBuffer);

    point->m_indexCount = uint32_t(cPos.size());
    //point->m_indexCount = uint32_t(sizeof(Vector4));
    //point->m_vertexCoount = uint32_t(Vector4.size());
     
    D3D11Utils::CreateConstBuffer(device, point->m_constantData,
                                  point->m_constantBuffer);


    	m_points.push_back(point);

}

void TurningPoint::Turning(Camera &Editcamera, const Vector3 characterPos) {
    // point 와 캐릭터 벡터의 수직
    // 회전
    if (m_currentPoint)
    {
        Vector3 CVec = characterPos - m_currentPoint->pos;
        CVec.y = 0.0f;

        // 설정한 방향에 따라서 카메라 view 방향도 고려
        Vector3 cameraDir;

        if (m_currentPoint->DirFlag == 0)
            cameraDir = Vector3(CVec.z, CVec.y, -CVec.x);
        else if (m_currentPoint->DirFlag == 1)
            cameraDir = Vector3(-CVec.z, CVec.y, CVec.x);
        
        cameraDir.Normalize();

        // 회전
        Editcamera.m_yaw = atan2(cameraDir.x, cameraDir.z);


        // 이동
        cameraDir *= 2.0f;

        Vector3 position = Vector3(characterPos.x - cameraDir.x, characterPos.y + 0.7f,
                                   characterPos.z - cameraDir.z);

        Editcamera.SetEyePos(position);

        Editcamera.UpdateViewDir();
    }

}
