#pragma once
#include "D3D11Utils.h"
#include "Model.h"
#include <array>


enum PLANE_TYPE
{
		FRONT,
		BACK,
		UP,  
		DOWN,
		LEFT,
		RIGHT,

		END

};

class Frustum {

public:

    void Update(const Matrix invViewProj);

	bool FrustumCulling(shared_ptr<Model> obj);

protected:

    array<Vector4, END> m_plane;

};

