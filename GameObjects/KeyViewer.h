#pragma once
#include "Headers.h"
#include "Model.h"
#include "GeometryGenerator.h"
#include "UI.h"
#include "D3D11Utils.h"
#include <array>


enum KEY_TYPE
{
    KEY_BOARD,
    KEY_UP,
    KEY_DOWN,
    KEY_RIGHT,
    KEY_LEFT,
    KEY_X,
    KEY_Z,

    KEY_END
};

class KeyViewer {
  public:
    KeyViewer(){};
    void Initialize(ComPtr<ID3D11Device> &device,
                    ComPtr<ID3D11DeviceContext> &context);

    void Update(ComPtr<ID3D11Device> &device,
                ComPtr<ID3D11DeviceContext> &context);

    void Render(ComPtr<ID3D11DeviceContext> &context);

    public:

    array<shared_ptr<UI>, KEY_END> m_key;



};
