#pragma once

#include "SkinnedMeshModel.h"
#include "Headers.h"



class Camera {
  public:

    Camera() { UpdateViewDir(); }

    Matrix GetViewRow();
    Matrix GetProjRow();
    Vector3 GetEyePos();
    Vector3 GetViewDir() { return m_viewDir; }
    void SetEyePos(Vector3 pos) { m_position = pos; }
    void SetViewDir(Vector3 dir) { m_viewDir = dir; }
    void SetFar(float f) { m_farZ = f; }
    void SetAngle(float a) { m_projFovAngleY = a; }

    void UpdateViewDir();
    void UpdateViewDirBasic(Vector3 objDir);
    void UpdateEditKeyboard(const float dt, bool const keyPressed[256], shared_ptr<SkinnedMeshModel> character);
    void UpdateBasicKeyboard(const float dt, bool const keyPressed[256],
                            shared_ptr<SkinnedMeshModel> character, shared_ptr<Model> obj);
    void UpdateKeyboard(const float dt, bool const keyPressed[256]);
    void UpdateMouse(float mouseNdcX, float mouseNdcY);
    void MoveForward(float dt);
    void MoveRight(float dt);
    void MoveUp(float dt);
    void SetAspectRatio(float aspect);

    float m_yaw = -0.019635f, m_pitch = -0.120477f;

    bool m_useFirstPersonView = false;
    float m_nearZ = 0.01f;
    float m_farZ = 400.0f;
    float m_projFovAngleY = 90.0f;

    float m_aspect = 16.0f / 9.0f; 

  private:

    Vector3 m_position = Vector3(0.275514f, 0.461257f, 0.0855238f);
    Vector3 m_viewDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 m_upDir = Vector3(0.0f, 1.0f, 0.0f); 
    Vector3 m_rightDir = Vector3(1.0f, 0.0f, 0.0f);


    float m_speed = 6.0f; // 움직이는 속도 



    bool m_usePerspectiveProjection = true;

};

