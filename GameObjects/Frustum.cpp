#include "Frustum.h"

  
void Frustum::Update(const Matrix invViewProj)
{ 

        vector<Vector3> worldPos = {

        XMVector3TransformCoord(Vector3(-1.f, 1.f, 0.f), invViewProj),
        XMVector3TransformCoord(Vector3(1.f, 1.f, 0.f), invViewProj),
        XMVector3TransformCoord(Vector3(1.f, -1.f, 0.f), invViewProj),
        XMVector3TransformCoord(Vector3(-1.f, -1.f, 0.f), invViewProj),
        XMVector3TransformCoord(Vector3(-1.f, 1.f, 1.f), invViewProj),
        XMVector3TransformCoord(Vector3(1.f, 1.f, 1.f), invViewProj),
        XMVector3TransformCoord(Vector3(1.f, -1.f, 1.f), invViewProj),
        XMVector3TransformCoord(Vector3(-1.f, -1.f, 1.f), invViewProj)};

        m_plane[PLANE_TYPE::FRONT] =
            XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
        m_plane[PLANE_TYPE::BACK] =
          XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
        m_plane[UP] =
           XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
        m_plane[DOWN] =
            XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
        m_plane[LEFT] =
            XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
        m_plane[RIGHT] =
            XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW

}
      
bool Frustum::FrustumCulling(shared_ptr<Model> obj)
{
   for (auto& i : m_plane)
   {
        Vector3 normal = Vector3(i.x, i.y, i.z);

        if (normal.Dot(obj->m_worldRow.Translation())+ i.w > obj->m_sphere.Radius)
            return false;
    }     

    return true;
}
