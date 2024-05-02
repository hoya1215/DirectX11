#include "Camera.h"


#include <iostream>



using namespace std;
using namespace DirectX;

Matrix Camera::GetViewRow() {


    return Matrix::CreateTranslation(-this->m_position) *
           Matrix::CreateRotationY(-this->m_yaw) *
           Matrix::CreateRotationX(this->m_pitch);
}  

Vector3 Camera::GetEyePos() { return m_position; }

void Camera::UpdateViewDir() {

    m_viewDir = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
                                   Matrix::CreateRotationY(this->m_yaw));
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::UpdateViewDirBasic(Vector3 objDir)
{ m_viewDir = objDir;
    m_rightDir = m_upDir.Cross(m_viewDir);
}

void Camera::UpdateEditKeyboard(const float dt, bool const keyPressed[256], shared_ptr<SkinnedMeshModel> character) {
    if (m_useFirstPersonView) {
        if (keyPressed['W'])
            MoveForward(dt);
        if (keyPressed['S'])
            MoveForward(-dt);
        if (keyPressed['D'])
            MoveRight(dt);
        if (keyPressed['A'])
            MoveRight(-dt);
        if (keyPressed['E'])
            MoveUp(dt);
        if (keyPressed['Q'])
            MoveUp(-dt);
    } else
    {

        Vector3 pos = character->m_worldRow.Translation();
        m_viewDir.Normalize();
        Vector3 posDir = m_viewDir * 2.0f;

        m_position = Vector3(pos.x - posDir.x, pos.y + 0.7f, pos.z - posDir.z);
        //m_position = Vector3(pos.x, pos.y + 0.7f, pos.z - 2.0f);
        //m_yaw = -0.019635f, m_pitch = -0.120477f;
        UpdateViewDir();
    }
}

   void Camera::UpdateBasicKeyboard(const float dt, bool const keyPressed[256],
                        shared_ptr<SkinnedMeshModel> character, shared_ptr<Model> obj)
   {
    if (m_useFirstPersonView) {
        if (keyPressed['W'])
            MoveForward(dt);
        if (keyPressed['S'])
            MoveForward(-dt);
        if (keyPressed['D'])
            MoveRight(dt);
        if (keyPressed['A'])
            MoveRight(-dt);
        if (keyPressed['E'])
            MoveUp(dt);
        if (keyPressed['Q'])
            MoveUp(-dt);
    } else {

        // 회전
        Vector3 objDir =
            obj->m_worldRow.Translation() - character->m_worldRow.Translation();
        Vector3 characterDir = character->m_worldRow.Translation() - m_position;
        objDir.y = 0;

        objDir.Normalize();

        // 회전
        m_yaw = atan2(objDir.x, objDir.z);
        Vector3 pos = character->m_worldRow.Translation();


        // 이동
        objDir *= 2.0f;

        m_position = Vector3(pos.x - objDir.x, 1.2f, pos.z - objDir.z);

        UpdateViewDir();
    }
   }

void Camera::UpdateKeyboard(const float dt, bool const keyPressed[256])
{
    if (m_useFirstPersonView) {
        if (keyPressed['W'])
            MoveForward(dt);
        if (keyPressed['S'])
            MoveForward(-dt);
        if (keyPressed['D'])
            MoveRight(dt);
        if (keyPressed['A'])
            MoveRight(-dt);
        if (keyPressed['E'])
            MoveUp(dt);
        if (keyPressed['Q'])
            MoveUp(-dt);
    }
}

void Camera::UpdateMouse(float mouseNdcX, float mouseNdcY) {
    if (m_useFirstPersonView) {
        // 얼마나 회전할지 계산
        m_yaw = mouseNdcX * DirectX::XM_2PI;     // 좌우 360도
        m_pitch = mouseNdcY * DirectX::XM_PIDIV2; // 위 아래 90도

        UpdateViewDir();
    }
}

void Camera::MoveForward(float dt) {

    m_position += m_viewDir * m_speed * dt;
}

void Camera::MoveUp(float dt) {

    m_position += m_upDir * m_speed * dt;
}

void Camera::MoveRight(float dt) { m_position += m_rightDir * m_speed * dt; }

void Camera::SetAspectRatio(float aspect) { m_aspect = aspect; }
  
Matrix Camera::GetProjRow() {
    return m_usePerspectiveProjection
               ? XMMatrixPerspectiveFovLH(XMConvertToRadians(m_projFovAngleY),
                                          m_aspect, m_nearZ, m_farZ)
               : XMMatrixOrthographicOffCenterLH(-m_aspect, m_aspect, -1.0f,
                                                 1.0f, m_nearZ, m_farZ);
}

 

