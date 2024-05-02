#include "KeyViewer.h"

void KeyViewer::Initialize(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context)
{
    shared_ptr<UI> board =
        make_shared<UI>(device, context, Vector3(500.0f, 50.0f, 150.0f), 100.0f, false);
    board->m_ui->UpdateWorldRow(
        Matrix::CreateScale(Vector3(1.0f, 1.3f, 1.0f)) *
        Matrix::CreateTranslation(Vector3(500.0f, 30.0f, 150.0f)));
    board->m_ui->UpdateConstantBuffers(device, context);
    board->m_constantData.index = 2;

    m_key[KEY_BOARD] = board;


    shared_ptr<UI> up = make_shared<UI>(device, context, Vector3(500.0f, 100.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/up.png" , false);
    up->m_constantData.index = 1;
 
    m_key[KEY_UP] = up;

    shared_ptr<UI> down = make_shared<UI>(device, context, Vector3(500.0f, 50.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/down.png", false);
    down->m_constantData.index = 1;
    m_key[KEY_DOWN] = down;

    shared_ptr<UI> right = make_shared<UI>(device, context, Vector3(550.0f, 50.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/right.png", false);
    right->m_constantData.index = 1;
    m_key[KEY_RIGHT] = right;

    shared_ptr<UI> left = make_shared<UI>(device, context, Vector3(450.0f, 50.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/left.png", false);
    left->m_constantData.index = 1;
    m_key[KEY_LEFT] = left;

    shared_ptr<UI> jump = make_shared<UI>(device, context, Vector3(500.0f, -50.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/x.png", false);
    jump->m_constantData.index = 1;
    m_key[KEY_X] = jump;

    shared_ptr<UI> dash = make_shared<UI>(device, context, Vector3(500.0f, 0.0f, 100.0f), 20.0f,
                        "d:/image/keyimage/z.png", false);
    dash->m_constantData.index = 1;
    m_key[KEY_Z] = dash;


}

void KeyViewer::Update(ComPtr<ID3D11Device>& device,
    ComPtr<ID3D11DeviceContext>& context)
{
    for (auto& i : m_key)
    {
        i->Update(device, context);
    }
}

void KeyViewer::Render(ComPtr<ID3D11DeviceContext> &context)
{
    for (auto& i : m_key)
    {
        i->Render(context);
    }}
