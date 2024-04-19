#pragma once

#include "Headers.h"





struct Vertex {
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;
    //Vector3 biTangentModel;
    // biTangent는 쉐이더에서 계산
};

struct SkinnedVertex {
    Vector3 position;
    Vector3 normalModel;
    Vector2 texcoord;
    Vector3 tangentModel;

    float blendWeights[8] = {0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f};  // BLENDWEIGHT0 and 1
    uint8_t boneIndices[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // BLENDINDICES0 and 1


};

struct VertexIndex {
    Vector3 position;
    Vector3 normalModel;
    Vector3 texcoord;
    Vector3 tangentModel;
    //float index;
    // Vector3 biTangentModel;
    //  biTangent는 쉐이더에서 계산
};


struct GrassVertex {
    Vector3 posModel;
    Vector3 normalModel;
    Vector2 texcoord;

};



struct GrassInstance {
    Matrix instanceWorld;
    float windStrength;
};

struct SnowVertex
{
    Vector3 posModel;
};


struct SnowInstance {
    Matrix instanceWorld;
    float velocity;
};



