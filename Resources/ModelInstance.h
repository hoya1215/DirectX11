#pragma once

#include <memory>

#include "Model.h"



class ModelInstance {
  public:
  public:
    std::shared_ptr<const Model> m_model;
    // UploadBuffer m_MeshConstantsCPU;
    // ByteAddressBuffer m_MeshConstantsGPU;
    // UniformTransform locator (��ġ ã���� ���°�, bounding box ��)
};

