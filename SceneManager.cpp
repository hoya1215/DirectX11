#include "SceneManager.h"

#include <DirectXCollision.h> // 구와 광선 충돌 계산에 사용
#include <directxtk/DDSTextureLoader.h>
#include <directxtk/SimpleMath.h>
#include <tuple>
#include <vector>
#include <random>
#include <cmath>
#include <glm/glm.hpp>

#include "GeometryGenerator.h"
#include "GraphicsCommon.h"
 
   

SceneManager::SceneManager() : AppBase() {}

bool SceneManager::Initialize() {

    if (!AppBase::Initialize())
        return false;
       
    mainCamera = m_Basiccamera;
      
#pragma region Skybox
    AppBase::InitCubemaps(L"d:/image/", L"hdr-SkyEnvHDR.dds",
                          L"hdr-SkySpecularHDR.dds", L"hdr-SkyDiffuseHDR.dds",
                          L"hdr-SkyBrdf.dds", L"TRmapEnvHDR.dds");

     
             
    // 환경 박스 초기화     
    { 
 /*       MeshData skyboxMesh = GeometryGenerator::MakeBox(40.0f);*/
        MeshData skyboxMesh = GeometryGenerator::MakeSphere(100.0f, 40, 40);
        skyboxMesh.albedoTextureFilename = "d:/image/skystar.jpeg";
        std::reverse(skyboxMesh.indices.begin(), skyboxMesh.indices.end());
        m_skybox = make_shared<Model>(m_device, m_context, vector{skyboxMesh});
        m_skybox->m_materialConstsCPU.TexOnly = 1;  

        m_skybox->UpdateConstantBuffers(m_device, m_context);    
    }

#pragma endregion



// Edit Scene
{
#pragma region Billboard
    // billboard
    // 포인트로 빌보드 만들기
    //vector<Vector4> points;
    //Vector4 p = {-2.0f, 0.0f, 10.0f, 1.0f};
    //for (int i = 0; i < 5; i++) {
    //    points.push_back(p);
    //    p.x += 3.f;
    //}

    //billboard.Initialize(m_device, m_context, points);

    Vector4 point = Vector4(0.0f, 1.2f, 10.0f, 1.0f);

    billboard.Initialize(m_device, m_context, point);

#pragma endregion


#pragma region Snow
{
    std::mt19937 gen(0); 
    std::uniform_real_distribution<float> scale(1.0f, 2.0f);
    std::uniform_real_distribution<float> velocity(0.5f, 2.f);
    std::uniform_real_distribution<float> position(-20.0f, 20.0f);

    vector<SnowInstance> &snowInstances = m_snow.m_instanceData;

    for (int i = 0; i < m_snow.m_snowCount; i++) {
        
        SnowInstance snow;

        Vector3 startPos = Vector3(position(gen), 10.f, position(gen));

        snow.instanceWorld = Matrix::CreateScale(Vector3(scale(gen))) *Matrix::CreateTranslation(startPos);
        snow.velocity = velocity(gen);

        snowInstances.push_back(snow);
    }

    // 쉐이더로 보내기 위해 transpose
    for (auto &i : snowInstances) {
        i.instanceWorld = i.instanceWorld.Transpose();
    }

    m_snow.Initialize(m_device, m_context);
}
#pragma endregion

#pragma region SnowParticle
{ m_rain.Initialize(m_device, m_context); }
#pragma endregion


#pragma region PostEffect
    // 후처리용 화면 사각형
    {  
        MeshData meshData = GeometryGenerator::MakeSquare(); 
        m_screenSquare = 
            make_shared<Model>(m_device, m_context, vector{meshData});
    }
#pragma endregion

#pragma region Terrain
    // 터레인
    {

        Vector3 center = Vector3(0.0f, 0.0f, 0.0f);
        m_terrain.scale = 20.0f;
        Matrix Worldmat = Matrix ::CreateRotationX(3.14f * 0.5f) *
                          Matrix::CreateTranslation(center);

        m_terrain.m_constantData.terrainCenter = center; 
        m_terrain.m_constantData.terrainScale = m_terrain.scale;
        m_terrain.Initialize(m_device, m_context, Worldmat,  
                             "d:/image/terrain/Heightmap.png");  
              
        m_terrainBox.Center = center;
        m_terrainBox.Extents = Vector3(m_terrain.scale, 0, m_terrain.scale);
          
          
        // Lake
        Vector3 center2 = Vector3(0.0f, -2.0f, 0.0f);
        Matrix Worldmat2 = Matrix::CreateRotationX(3.14f * 0.5f) *
                           Matrix::CreateTranslation(center2);

        m_lake.Initialize(m_device, m_context, Worldmat2, 
                          "d:/image/terrain/lake3.jpg");  
        m_lake.UpdateWorldRow(Worldmat2);

        m_lakePlane = SimpleMath::Plane(center2, Vector3(0.0f, 1.0f, 0.0f));
        

        m_terrain.m_constantData.refractPlane = m_lakePlane;


        // 맵 밖으로 못나가게 하는 box
        shared_ptr<CollisionBox> defaultBoxLeft = make_shared<CollisionBox>(
            m_device, m_context, Vector3(-m_terrain.scale, 0.0f, 0.0f), Vector3(0.0f, m_terrain.scale, m_terrain.scale));

        m_collisionBox.push_back(defaultBoxLeft);

        shared_ptr<CollisionBox> defaultBoxRight = make_shared<CollisionBox>(
            m_device, m_context, Vector3(m_terrain.scale, 0.0f, 0.0f),
            Vector3(0.0f, m_terrain.scale, m_terrain.scale));

        m_collisionBox.push_back(defaultBoxRight);

        shared_ptr<CollisionBox> defaultBoxUp = make_shared<CollisionBox>(
            m_device, m_context, Vector3(0.0f, 0.0f, m_terrain.scale),
            Vector3(m_terrain.scale, m_terrain.scale, 0.0f));

        m_collisionBox.push_back(defaultBoxUp);

        shared_ptr<CollisionBox> defaultBoxDown = make_shared<CollisionBox>(
            m_device, m_context, Vector3(0.0f, 0.0f, -m_terrain.scale),
            Vector3(m_terrain.scale, m_terrain.scale, 0.0f));

        m_collisionBox.push_back(defaultBoxDown);



        Vector3 eyePos = Vector3(-0.00001f, 30.f, 0.0f);
        Vector3 viewDir = Vector3(-eyePos.x, -eyePos.y, -eyePos.z);
        viewDir.Normalize();

        float tanTheta = m_terrain.scale / eyePos.y;
        float theta = atan(tanTheta);
        theta = XMConvertToDegrees(theta) * 2;
          
        Matrix TerrainView = XMMatrixLookAtLH(eyePos, eyePos + viewDir, Vector3(0.0f, 1.0f, 0.0f));
        Matrix TerrainProj =
            XMMatrixPerspectiveFovLH(XMConvertToRadians(theta), 1.0f, 0.1f, 100.0f);

         //m_terrain.m_constantData.TerrainView = TerrainView.Transpose();
         //m_terrain.m_constantData.TerrainProj = TerrainProj.Transpose();
             
        m_terrainMapConstCPU.eyeWorld = eyePos; 
        m_terrainMapConstCPU.view = TerrainView.Transpose();
        m_terrainMapConstCPU.proj = TerrainProj.Transpose();
        m_terrainMapConstCPU.viewProj =
            (TerrainView * TerrainProj).Transpose();
        m_terrainMapConstCPU.invProj = TerrainProj.Invert().Transpose();

        D3D11Utils::UpdateBuffer(m_device, m_context, m_terrainMapConstCPU,
                                 m_terrainMapConstGPU);

        m_surplusConstsCPU.TerrainInvViewProj =
            (TerrainView.Transpose() * TerrainProj.Transpose()).Invert();

                m_surplusConstsCPU.TerrainViewProj = m_terrainMapConstCPU.viewProj;

        D3D11Utils::UpdateBuffer(m_device, m_context, m_surplusConstsCPU,
                                 m_surplusConstsGPU);



        auto meshdata = GeometryGenerator::MakeSquare(1.0f);
        Vector3 center3 = Vector3(0.0f);
        m_terrainQuad = make_shared<Model>(m_device, m_context ,vector{meshdata});


        Matrix Worldmat3 = Matrix::CreateTranslation(center3);

        m_terrainQuad->UpdateWorldRow(Worldmat3);
        m_terrainQuad->m_isVisible = false;

    }
#pragma endregion    


#pragma region Character
    // Main Object
    {

        string path2 = "d:/FBX/boy/";
        string path = "d:/FBX/girl/";
        vector<string> clipNames = {
            "CatwalkIdle.fbx", "Running.fbx",
            "Jumping.fbx", "CatwalkWalkStop.fbx",
            "BreakdanceFreezeVar2.fbx"};
        vector<string> clipNames2 = {"ChildBoy.fbx"};

        AnimationData aniData;

        auto [meshes, _] =
            GeometryGenerator::ReadAnimationFromFile(path, "character.fbx");


        for (auto &name : clipNames) {
        auto [_, ani] = GeometryGenerator::ReadAnimationFromFile(path, name);

        if (aniData.clips.empty()) {
            aniData = ani;
        } else {
            aniData.clips.push_back(ani.clips.front());
        }
        }
             
        Vector3 center(0.0f, 1.5f, -30.0f);  
        m_character =
            make_shared<SkinnedMeshModel>(m_device, m_context, meshes, aniData);
        m_character->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
        m_character->m_materialConstsCPU.roughnessFactor = 0.8f;
        m_character->m_materialConstsCPU.metallicFactor = 0.0f;
        m_character->UpdateWorldRow(Matrix::CreateScale(1.0f) *
                                    Matrix::CreateTranslation(center));
        m_character->m_materialConstsCPU.TexOnly = 1;

        m_character->m_boxDraw = true;

          
        // 투명 연습
         auto meshdata = GeometryGenerator::MakeSquare(2.0f);
         Vector3 center2 = Vector3(m_Editcamera.GetEyePos().x,
         m_Editcamera.GetEyePos().y,
                          m_Editcamera.GetEyePos().z + 3.0f);
         m_test = make_shared<Model>(m_device, m_context, vector{meshdata});

         Matrix Worldmat3 = Matrix::CreateScale(Vector3(2.0f, 1.0f, 1.0f)) *
         Matrix::CreateTranslation(center2); m_test->scale =
         Vector3(2.0f, 1.0f, 1.0f); m_test->UpdateWorldRow(Worldmat3);
         m_test->m_isVisible = true;
         m_test->m_boxDraw = true;
         m_test->m_materialConstsCPU.albedoFactor = Vector3(2.0f, 2.0f, 2.0f);
        // m_EditList.push_back(m_test);
       
    }
#pragma endregion

#pragma region 물체들

// Tree
{
    string path = "d:/3DTexture/Tree1/";
    auto meshdata = GeometryGenerator::ReadFromFile(path, "tree1.fbx");
    meshdata[0].albedoTextureFilename = "d:/3Dtexture/Tree1/tree1.png";
    m_tree01 = make_shared<Model>(m_device, m_context, meshdata);
    Vector3 center(0.0f, 0.5f, 0.0f);
    m_tree01->UpdateWorldRow(Matrix::CreateScale(Vector3(1.0f)) * Matrix::CreateTranslation(center));
    m_tree01->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 1.f, 1.f);
    m_tree01->m_materialConstsCPU.TexOnly = 1;
    m_tree01->m_boxDraw = true;

    //m_EditList.push_back(m_tree01);

    string path2 = "d:/3DTexture/Tree3/";

        auto meshdata2 = GeometryGenerator::ReadFromFile(path2, "Tree03.fbx");
    meshdata2[0].aoTextureFilename = "d:/3Dtexture/Tree3/ao.jpeg";
        meshdata2[0].albedoTextureFilename = "d:/3Dtexture/Tree3/leaf01.png";
    m_tree02 = make_shared<Model>(m_device, m_context, vector{meshdata2});
    m_tree02->UpdateWorldRow(Matrix::CreateTranslation(center));
   
    m_tree02->m_materialConstsCPU.albedoFactor = Vector3(0.0f, 1.f, 0.2f);
    m_tree02->m_boxDraw = true;
    m_tree02->m_materialConstsCPU.TexOnly = 1;
}

// Grass ( texture )
{
    string path = "d:/3DTexture/grass/";
    auto meshdata = GeometryGenerator::ReadFromFile(path, "Grass.fbx");
    meshdata[0].albedoTextureFilename = "d:/3Dtexture/grass/GrassColor.png";
    Vector3 center = Vector3(0.0f, 0.5f, 0.0f);

    m_grass01 = make_shared<Model>(m_device, m_context, meshdata);;
    m_grass01->m_boxDraw = true;
    m_grass01->UpdateWorldRow(Matrix::CreateTranslation(center));
}

// Rock 
{
    string path = "d:/3DTexture/Rock/";
    Vector3 center = Vector3(0.0f, 0.5f, 0.0f);

    auto meshdata = GeometryGenerator::ReadFromFile(path, "rock.fbx", false);
    meshdata[0].albedoTextureFilename = "d:/3DTexture/Rock/rockuv.png";
    m_rock01 =
        make_shared<Model>(m_device, m_context, vector{meshdata});
    m_rock01->UpdateWorldRow(
                            Matrix::CreateTranslation(center));

    m_rock01->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 1.0f, 1.0f);
    m_rock01->m_boxDraw = true;
    m_rock01->m_materialConstsCPU.TexOnly = 1;


}

// House
{

    string path2 = "d:/3DTexture/House1/";
    Vector3 center = Vector3(0.0f, 0.5f, 0.0f);

    auto meshdata2 = GeometryGenerator::ReadFromFile(path2, "house3.fbx");
    meshdata2[0].albedoTextureFilename = "d:/3DTexture/House1/house3.png";
    m_house01 =
        make_shared<Model>(m_device, m_context, vector{meshdata2});
    m_house01->UpdateWorldRow(Matrix::CreateTranslation(center));

    m_house01->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 1.0f, 1.0f);
    m_house01->m_boxDraw = true;
    m_house01->m_materialConstsCPU.TexOnly = 1;

}

// Fence
{
     string path = "d:/3DTexture/Fence/";
     Vector3 center = Vector3(0.0f, 0.5f, 0.0f);

     auto meshdata = GeometryGenerator::ReadFromFile(path, "fence.fbx", false);
     meshdata[0].albedoTextureFilename = "d:/3DTexture/Fence/fenceAlbedo.png";
        m_fence =
         make_shared<Model>(m_device, m_context,
     vector{meshdata});
     m_fence->UpdateWorldRow(Matrix::CreateRotationX(XM_PI * 0.5f) *
                             Matrix::CreateTranslation(center));

     m_fence->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 1.0f, 1.0f);
     m_fence->m_boxDraw = true;
     m_fence->rotation.x = XM_PI * 0.5f;

     //m_EditList.push_back(fence);
}


#pragma endregion 
// 스텐실 연습
{
     //auto meshdata = GeometryGenerator::MakeSquare(1.0f);
     //Vector3 center = Vector3(m_Editcamera.GetEyePos().x, m_Editcamera.GetEyePos().y,
     //                 m_Editcamera.GetEyePos().z + 3.0f);
     //m_test = make_shared<Model>(m_device, m_context, vector{meshdata});

     //Matrix Worldmat3 = Matrix::CreateScale(Vector3(2.0f, 1.0f, 1.0f)) * Matrix::CreateTranslation(center);
     //m_test->scale = Vector3(2.0f, 1.0f, 1.0f);
     //m_test->UpdateWorldRow(Worldmat3);
     //m_test->m_isVisible = true;
     //m_test->m_boxDraw = true;
     //m_test->m_materialConstsCPU.albedoFactor = Vector3(2.0f, 2.0f, 2.0f);

     //auto meshdata1 = GeometryGenerator::MakeBox(1.0f);
     //Vector3 center1 =
     //    Vector3(m_Editcamera.GetEyePos().x - 2.0f, m_Editcamera.GetEyePos().y+ 1.0f,
     //            m_Editcamera.GetEyePos().z + 3.0f);
     //m_test1 = make_shared<Model>(m_device, m_context, vector{meshdata1});

     //Matrix Worldmat1 = Matrix::CreateTranslation(center1);

     //m_test1->UpdateWorldRow(Worldmat1);
     //m_test1->m_isVisible = true;
     //m_test1->m_boxDraw = true;
     //m_test1->m_materialConstsCPU.albedoFactor = Vector3(1.0f, 0.0f, 0.0f);

     //auto meshdata2 = GeometryGenerator::MakeSphere(1.0f, 10, 10);
     //Vector3 center2 =
     //    Vector3(m_Editcamera.GetEyePos().x + 2.0f, m_Editcamera.GetEyePos().y + 1.0f,
     //            m_Editcamera.GetEyePos().z + 3.0f);
     //m_test2 = make_shared<Model>(m_device, m_context, vector{meshdata2});

     //Matrix Worldmat2 = Matrix::CreateTranslation(center2);

     //m_test2->UpdateWorldRow(Worldmat2);
     //m_test2->m_isVisible = true;
     //m_test2->m_boxDraw = true;
     //m_test2->m_materialConstsCPU.albedoFactor = Vector3(0.0f, 1.0f, 0.0f);
}

// 터닝 포인트 
{ m_turn = make_shared<TurningPoint>();
     m_turn->Initialize(m_device, m_context);
}

// SSAO
{ m_ssao.Initialize(m_device, m_context); }
} 
// Edit Snene End

    
// Basic Scene

    m_Basiccamera.SetEyePos(Vector3(0.0f, 10.0f, -60.f));
    { 
        // back ground
{
    auto meshdata = GeometryGenerator::MakeSquare(30.f);
    Vector3 center = Vector3(0.0f, 0.0f, 2.0f);
    meshdata.albedoTextureFilename = "d:/image/door.jpg";
    shared_ptr<Model> ground =
        make_shared<Model>(m_device, m_context, vector{meshdata});
    ground->UpdateWorldRow(Matrix::CreateScale(2.0f, 1.0f, 1.0f) *
                           Matrix::CreateTranslation(center));
    ground->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
    ground->UpdateConstantBuffers(m_device, m_context);
    ground->m_boxDraw = true;
    ground->m_materialConstsCPU.TexOnly = 1;
    ground->scale.x = 2.0f;

    m_BasicList.push_back(ground);  
}     
  
// ground
{
    auto meshdata = GeometryGenerator::MakeSquare(30.f);
    Vector3 center = Vector3(0.0f, 0.0f, 1.0f);
    meshdata.albedoTextureFilename = "d:/image/door.jpg";
    shared_ptr<Model> ground =
        make_shared<Model>(m_device, m_context, vector{meshdata});
    ground->UpdateWorldRow(Matrix::CreateScale(2.0f, 2.0f, 1.0f) *Matrix::CreateRotationX(3.14 * 0.5f) *
                           Matrix::CreateTranslation(center));
    ground->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
    ground->UpdateConstantBuffers(m_device, m_context);
    ground->m_boxDraw = true;
    ground->m_materialConstsCPU.TexOnly = 1;
    ground->scale.x = 2.0f;
    ground->scale.y = 2.0f;
    ground->rotation.x = XM_PI * 0.5f;

    m_BasicList.push_back(ground);
}

// Sphere 

{ auto meshdata = GeometryGenerator::MakeSphere(2.0f, 20, 20);
    Vector3 center = Vector3(0.0f, 5.0f, -10.0f);

     cutAway = make_shared<Model>(m_device, m_context, vector{meshdata});
    cutAway->UpdateWorldRow(Matrix::CreateScale(1.0f, 1.0f, 1.0f) *
                           Matrix::CreateTranslation(center));
     cutAway->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
    cutAway->UpdateConstantBuffers(m_device, m_context);
     cutAway->m_boxDraw = true;
    cutAway->m_materialConstsCPU.TexOnly = 2; // 장면전환

    //m_BasicList.push_back(sphere);

}


}

#pragma region 조명
    // 조명 설정
    {
        m_globalConstsCPU.Sun.radiance = Vector3(1.0f);
m_globalConstsCPU.Sun.position = Vector3(0.0f, 30.0f, 0.0f);
        SunStartPos = m_globalConstsCPU.Sun.position;
        m_globalConstsCPU.Sun.type =
            LIGHT_DIRECTIONAL | LIGHT_SHADOW; // Point with shadow

    } 

    // 조명 위치 표시 햇빛 
    { 
        for (int i = 0; i < MAX_LIGHTS; i++) {
            MeshData sphere = GeometryGenerator::MakeSphere(10.0f, 20, 20);
            m_lightSphere[i] =
                make_shared<Model>(m_device, m_context, vector{sphere});
            m_lightSphere[i]->UpdateWorldRow(Matrix::CreateTranslation(
                m_globalConstsCPU.Sun.position));
            m_lightSphere[i]->m_materialConstsCPU.albedoFactor = Vector3(1.0f);
            m_lightSphere[i]->m_materialConstsCPU.emissionFactor =
                Vector3(1.0f, 1.0f, 0.0f);
            m_lightSphere[i]->m_castShadow = 
                false; // 조명 표시 물체들은 그림자 X     

            if (m_globalConstsCPU.Sun.type == 0)
                m_lightSphere[i]->m_isVisible = false;
              
        }
    }


#pragma endregion

    // UI  
    {

        Vector3 center = Vector3(500.0f, 300.0f, 100.f);


        m_map = make_shared<UI>(m_device, m_context, center, 50.0f);

        m_key = make_shared<UI>(m_device, m_context,
                                Vector3(500.0f, 170.0f, 80.0f), 30.0f, "d:/image/keyimage/key.png" ,true);
        m_key->m_constantData.index = 1;

        // 키뷰어
        m_keyViewer.Initialize(m_device, m_context);
    }
      
    // 카메라
    m_Editcamera.m_farZ = currentFarZ;

    return true;    
}
   
void SceneManager::UpdateLights(float dt) {

    // 햇빛 이동
    float sunThetaRadian = XMConvertToRadians(sunTheta);

    m_globalConstsCPU.Sun.position = 
        Vector3(SunStartPos.x + cos(XM_PIDIV2 - sunThetaRadian) * SunStartPos.y,
                sin(XM_PIDIV2 - sunThetaRadian) * SunStartPos.y, SunStartPos.z);
    m_globalConstsCPU.Sun.direction =
        Vector3(-m_globalConstsCPU.Sun.position.x,
                -m_globalConstsCPU.Sun.position.y,
                -m_globalConstsCPU.Sun.position.z);
    m_globalConstsCPU.Sun.direction.Normalize();
      

    // 해 위치에 따른 sky 색 
    m_globalConstsCPU.strengthIBL = 0.5f - 0.5f * abs(sunTheta) / 90.0f;

    // 그림자 맵 햇빛 행렬
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
       
    Matrix sunView =
        XMMatrixLookAtLH(m_globalConstsCPU.Sun.position,
                         m_globalConstsCPU.Sun.position +m_globalConstsCPU.Sun.direction, up);
    
    Matrix sunProj =
        XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.0f, 0.1f, 50.0f);
                
    m_SunShadowGlobalConstCPU.eyeWorld = m_globalConstsCPU.Sun.position;
    m_SunShadowGlobalConstCPU.view = sunView.Transpose();
    m_SunShadowGlobalConstCPU.proj = sunProj.Transpose();
    m_SunShadowGlobalConstCPU.viewProj = (sunView * sunProj).Transpose();
    m_SunShadowGlobalConstCPU.invProj = sunProj.Invert().Transpose();

    D3D11Utils::UpdateBuffer(m_device, m_context, m_SunShadowGlobalConstCPU,
                             m_SunShadowGlobalConstGPU);

      
    m_globalConstsCPU.Sun.viewProj = m_SunShadowGlobalConstCPU.viewProj;
    m_globalConstsCPU.Sun.invProj = m_SunShadowGlobalConstCPU.invProj;

}

void SceneManager::MousePickingUpdate(const Matrix View, const Matrix Proj) {
    // picking
    static bool checkPick = true;

    Vector3 near_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);

    Vector3 far_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

    Matrix inverseProjView = (View * Proj).Invert();
    static float prevRatio = 0.0f;
    static Vector3 prevVector = Vector3(0.0f);
    Vector3 pickPoint(0.0f);
    // 적용할 회전과 이동 초기화
    Quaternion q =
        Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
    Vector3 MoveDistance;
    Vector3 ScaleDistance = Vector3(0.0f);
    static Vector3 scale = Vector3(0.0f);

    // 사이즈 줄일때 사용
    static Vector3 prevCenter = Vector3(0.0f);
    static Vector3 newCenter = Vector3(0.0f);

    Vector3 cursorWorldnear = Vector3::Transform(near_pos, inverseProjView);
    Vector3 cursorWorldfar = Vector3::Transform(far_pos, inverseProjView);
    Vector3 ray = cursorWorldfar - cursorWorldnear;
    ray.Normalize();


    SimpleMath::Ray pick = SimpleMath::Ray(cursorWorldnear, ray);

    // 가장 가까운 피킹 물체 선별
    float dis = -1.f;
    float minDis = 1000.f;


    
    if (checkPick) {
        for (auto &i : m_EditList) {

            if (i->m_boxDraw == false)
                continue;

            i->m_boxConstData.pick = 0;

            if (pick.Intersects(i->m_box, dis)) {

                if (dis < minDis) {
                    minDis = dis;
                    pickedModel = i;
                    pickPoint = pick.position + minDis * pick.direction;
                    pickedModel->m_boxConstData.pick = 1;
                }
            }
        }
    }
    
    if (test)
    {


            if (pick.Intersects(m_test->m_box, dis)) {

                if (dis < minDis) {
                    minDis = dis;
                    pickedModel = m_test;
                    pickPoint = pick.position + minDis * pick.direction;
                    pickedModel->m_boxConstData.pick = 1;
                }
            }
        
    }


    // 클릭 이벤트
    if (minDis != 1000.f || pickedModel !=nullptr) {


        if (m_rightButton && m_dragStartFlag) {
            if (m_clicked) {
                m_clicked = false;
                pickedModel = nullptr;
                checkPick = true;

            } else if (m_clicked == false) {
                m_clicked = true;
                newMove = true;
                prevRatio =
                    minDis / (cursorWorldfar - cursorWorldnear).Length();
                prevPos = pick.position + minDis * pick.direction;
                checkPick = false;

            }

            m_dragStartFlag = false;
        }  
        //else if (m_leftButton ) // 회전
        //{


        //    if (m_dragStartFlag)
        //    {
        //        m_dragStartFlag = false;

        //        prevVector = pickPoint - pickedModel->m_box.Center;
        //        prevVector.Normalize();

        //    }
        //    else if (m_dragStartFlag == false)
        //    {
        //        Vector3 currentVector = pickPoint - pickedModel->m_box.Center;
        //        currentVector.Normalize();
        //        float theta = acos(prevVector.Dot(currentVector));
        //        if (theta > 3.141592f / 180.0f * 3.0f) {
        //            Vector3 axis = prevVector.Cross(currentVector);
        //            axis.Normalize();
        //            q = SimpleMath::Quaternion::CreateFromAxisAngle(axis,
        //                                                            theta);
        //            pickedModel->q *= q;

        //            prevVector = currentVector;
        //        }
        //    }

        //} 
        // else if (m_scale == true && m_leftButton) // scale
        //{
        //    if (m_dragStartFlag)
        //    {
        //        m_dragStartFlag = false;
        //        prevRatio =
        //            minDis / (cursorWorldfar - cursorWorldnear).Length();
        //        //pickPoint = pick.position + minDis * pick.direction;
        //        prevPos = pickPoint;
        //        //prevCenter = XMVector3TransformCoord(prevPos, View.Transpose());
        //    }
        //    else if (m_dragStartFlag == false)
        //    {
        //        newPos = cursorWorldnear +
        //                 prevRatio * (cursorWorldfar - cursorWorldnear);
        //       // newCenter =
        //           // XMVector3TransformCoord(newPos, View.Transpose());
        //        if ((newPos - prevPos).Length() > 1e-3)
        //        {
        //            ScaleDistance = Vector3((newPos - prevPos).Length());

        //            if (m_keyPressed['J'])
        //                ScaleDistance = -ScaleDistance;

        //            prevPos = newPos;

        //                        pickedModel->scale =
        //                pickedModel->scale + ScaleDistance * 2.0f;
        //            pickedModel->m_worldRow._11 = pickedModel->scale.x;
        //            pickedModel->m_worldRow._22 = pickedModel->scale.y;
        //            pickedModel->m_worldRow._33 = pickedModel->scale.z;

        //            pickedModel->m_box.Extents = Vector3(extent);
        //        }
        //    }
        //}
    }

    // 이동 이벤트
    if (m_clicked) {
        

        newPos =
            cursorWorldnear + prevRatio * (cursorWorldfar - cursorWorldnear);

        if ((newPos - prevPos).Length() > 1e-3) {
            MoveDistance = newPos - prevPos;
            prevPos = newPos;
        }


    }

    // 최종 이동
    if (pickedModel) {
        Vector3 translation = pickedModel->m_worldRow.Translation();
        pickedModel->m_worldRow.Translation(Vector3(0.0f));

 /*       if (m_scale) {
            pickedModel->scale = pickedModel->scale + ScaleDistance * 2.0f;
            pickedModel->m_worldRow._11 = pickedModel->scale.x;
            pickedModel->m_worldRow._22 = pickedModel->scale.y;
            pickedModel->m_worldRow._33 = pickedModel->scale.z;
            float extent = pickedModel->extent * pickedModel->scale.x;
            pickedModel->m_box.Extents = Vector3(extent);
        }*/


        pickedModel->UpdateWorldRow( 
           pickedModel->m_worldRow *
            Matrix::CreateFromQuaternion(q) *
            Matrix::CreateTranslation(translation + MoveDistance));




    }  


    if (m_createNewObj && pickedModel != nullptr) // 새로운 물체 생성
    {
        // 현재 피킹 obj 의 정보를 가진 새로운 model 객체 생성하고 리스트에 넣기 terrainedit 시점에서만 사용 가능


            shared_ptr<Model> newObj =
                make_shared<Model>(m_device, m_context, pickedModel->mesh);


            Vector3 center;
            if (terrainEdit)
            {
            center = Vector3(cursorWorldnear.x,
                                     pickedModel->m_worldRow.Translation().y,
                                     cursorWorldnear.z);
            }
            else
            {
            center = Vector3(m_Editcamera.GetEyePos().x, m_Editcamera.GetEyePos().y, m_Editcamera.GetEyePos().z+ 3.0f);
            }

            newObj->rotation.x = pickedModel->rotation.x;

            Matrix world = pickedModel->m_worldRow;
            world.Translation() = center;
            //world.Translation() = Vector3(0.0f);
            Vector3 axis = Vector3(1.0f, 0.0f, 0.0f);
            newObj->UpdateWorldRow(Matrix::CreateRotationX(pickedModel->rotation.x) *Matrix::CreateTranslation(center));
            //newObj->UpdateWorldRow(Matrix::CreateTranslation(center));
            //newObj->m_worldRow._11 = 1.0f;
            //newObj->m_worldRow._22 = 1.0f;
            //newObj->m_worldRow._33 = 1.0f;
            //newObj->UpdateWorldRow(world);
            newObj->m_materialConstsCPU.albedoFactor =
                pickedModel->m_materialConstsCPU.albedoFactor;
            newObj->m_materialConstsCPU.roughnessFactor =
                pickedModel->m_materialConstsCPU.roughnessFactor;
            newObj->m_materialConstsCPU.metallicFactor =
                pickedModel->m_materialConstsCPU.metallicFactor;
            newObj->m_materialConstsCPU.emissionFactor =
                pickedModel->m_materialConstsCPU.emissionFactor;
            newObj->UpdateConstantBuffers(m_device, m_context);
            newObj->m_boxDraw = true;

            m_EditList.push_back(newObj);

            //pickedModel = newObj;
            //m_clicked = true;
            prevPos = center;

           

    } 

    if (m_deletePickedObj && pickedModel != nullptr) {

            // 오브젝트 리스트 순회하면서 pickedModel이 참조하고 있는 오브젝트
            // 찾으면 erase
            for (auto it = m_EditList.begin(); it != m_EditList.end(); it++) {
            if (*it == pickedModel) {
                m_EditList.erase(it);
                pickedModel = nullptr;
                break;
            }
            }

          
    } 

    if (m_deleteCollisionBox && pickedModel != nullptr)
    {
            for (auto it = m_collisionObj.begin(); it != m_collisionObj.end(); it++) {
            if (*it == pickedModel) {
                m_collisionObj.erase(it);
                pickedModel->m_boxConstData.IsCollisionBox = 0;
                pickedModel = nullptr;
                break;
            }
            }
    }
           

     m_createNewObj = false;
    m_deletePickedObj = false;
     //m_deleteCollisionBox = false;
}
 
void SceneManager::MousePickingUpdateBasic(const Matrix View,
                                           const Matrix Proj) {  
    Vector3 near_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);

    Vector3 far_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

    Matrix inverseProjView = (View * Proj).Invert();
    static float prevRatio = 0.0f;
    static Vector3 prevVector = Vector3(0.0f);
    Vector3 pickPoint(0.0f);
    // 적용할 회전과 이동 초기화
    Quaternion q =
        Quaternion::CreateFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.0f);
    Vector3 MoveDistance;
    Vector3 ScaleDistance = Vector3(0.0f);
    static Vector3 scale = Vector3(0.0f);

    // 사이즈 줄일때 사용
    static Vector3 prevCenter = Vector3(0.0f);
    static Vector3 newCenter = Vector3(0.0f);

    Vector3 cursorWorldnear = Vector3::Transform(near_pos, inverseProjView);
    Vector3 cursorWorldfar = Vector3::Transform(far_pos, inverseProjView);
    Vector3 ray = cursorWorldfar - cursorWorldnear;
    ray.Normalize();

    SimpleMath::Ray pick = SimpleMath::Ray(cursorWorldnear, ray);

    // 가장 가까운 피킹 물체 선별
    float dis = -1.f;
    float minDis = 1000.f;

    if (pick.Intersects(cutAway->m_box, dis))
    {
            cutAway->m_materialConstsCPU.pick = 1;
    }
    else
    {
            cutAway->m_materialConstsCPU.pick = 0;
    }

    // 클릭으로 이동은 update 맨처음에

}


void SceneManager::Update(float dt) {

                D3D11Utils::UpdateBuffer(m_device, m_context, m_surplusConstsCPU,
                             m_surplusConstsGPU);
    
    if (cutAway->m_materialConstsCPU.pick == 1 && m_leftButton && !EditScene)
            StartcutAway = true;

    if (StartcutAway) {
            m_surplusConstsCPU.cutAwayRadius += dt;

            if (m_surplusConstsCPU.cutAwayRadius >= 1.4f)
            {
            StartcutAway = false;
            m_surplusConstsCPU.cutAwayRadius = 0.0f;
            EditScene = true;
            m_surplusConstsCPU.EditScene = true;
            Vector3 characterPos = m_character->m_worldRow.Translation();
            characterPos.z = 0.0f;
            Matrix characterWorld = Matrix::CreateTranslation(characterPos);
            m_character->UpdateWorldRow(characterWorld);


            }  

       
    }
    
    if (!EditScene) {
            mainCamera = m_Basiccamera;
            UpdateEditScene(dt);
            UpdateBasicScene(dt);
    }
    else
    {
            mainCamera = m_Editcamera;
            UpdateEditScene(dt);
    }

    m_key->Update(m_device, m_context);
    m_keyViewer.Update(m_device, m_context);
  
}

void SceneManager::Render() {

    AppBase::SetMainViewport();

    if (!EditScene)
    {
            RenderEditScene();

            RenderBasicScene();

            RenderPostEffect();
    }
    else
    {
            RenderEditScene();
            RenderPostEffect();
    }



}  

void SceneManager::UpdateGUI() {
        ImGui::PushItemWidth(100);

        ImGui::SetNextItemOpen(true, ImGuiCond_Once); 
            ImGui::Text("Total Obj : %d", m_EditList.size());
        ImGui::Text("Now Obj : %d", nowObjCount);
          
        ImGui::SliderFloat("Sun", &sunTheta, -90.0f, 90.0f);
          

        //ImGui::Checkbox("StencilTest", &test);
        ImGui::Checkbox("Silhouette", &silhouette);

        if (silhouette)
            ImGui::SliderFloat("Outline", &m_globalConstsCPU.thickness, 0.0f,
                           1.0f);
        
        ImGui::Checkbox("SSAO", &b_ssao);
           
        if (b_ssao) {
            m_globalConstsCPU.ssao = 1;
            ImGui::SliderFloat("Radius", &m_ssao.m_constantData.ssaoRadius,
                               0.0f, 2.0f);
            ImGui::SliderFloat("Dist", &m_ssao.m_constantData.dist, 0.0f, 0.1f);
            ImGui::SliderFloat("FadeStart", &m_ssao.m_constantData.ssaoStart,
                               0.0f, 1.0f);
            ImGui::SliderFloat("FadeEnd", &m_ssao.m_constantData.ssaoEnd, 1.0f,
                               10.0f);
        } else
            m_globalConstsCPU.ssao = 0;

        ImGui::Checkbox("Only SSAO", &b_onlySSAO);        
         
        if (b_onlySSAO)
        {
            m_postEffectsConstsCPU.onlySSAO = 1;
            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);
        }
        else
        {
            m_postEffectsConstsCPU.onlySSAO = 0;
            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_postEffectsConstsCPU, 
                                     m_postEffectsConstsGPU);
        }


        


    if (ImGui::TreeNode("Edit")) {
        ImGui::Checkbox("TerrainEdit", &terrainEdit);

        if (terrainEdit) {

            m_Editcamera.m_farZ = EditFarZ;


            if (ImGui::TreeNode("Height"))
            {
                ImGui::SliderFloat("radius", &m_mouseInfo.radius, 1.0f, 6.0f);

                ImGui::SliderFloat("length", &m_mouseInfo.heightlength, 0.5f,
                                   10.0f);

                /*        if (ImGui::Button("Correction", ImVec2(80, 30)))
                        {
                            m_terrain.heightCorrection(m_context);
                        }
                        ImGui::SameLine();  */
                if (ImGui::Button("Reset", ImVec2(80, 30))) {
                    m_terrain.heightReset(m_context);
                }

                ImGui::SliderFloat("HeightControlUp",
                                   &m_terrain.m_constantData.heightControlUp,
                                   0.0f, 5.0f);
                // ImGui::SameLine();
                ImGui::SliderFloat("HeightControlDown",
                                   &m_terrain.m_constantData.heightControlDown,
                                   0.0f, 5.0f);
                ImGui::TreePop();

                ImGui::Spacing();

                ImGui::Indent();

                if(ImGui::TreeNode("CollisionBox"))
                {
                    if (ImGui::Button("AutoCollisionBox", ImVec2(80, 30))) {
                        AutoTerrainCollisionBox = true;
                    }

                    ImGui::Checkbox("DirectCollisionBox",
                                    &DirectTerrainCollisionBox);
                    if (DirectTerrainCollisionBox) {
                        ImGui::SliderFloat("QuadX", &quadX, 1.0f, 5.0f);
                        ImGui::SliderFloat("QuadZ", &quadZ, 1.0f, 5.0f);
                        if (ImGui::Button("DeleteCollisionBox",
                                          ImVec2(80, 30))) {
                            cout << m_collisionBox.size() << endl;

                            m_collisionBox.pop_back();
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::Unindent();
            }

            ImGui::Spacing();

            if (ImGui::TreeNode("Brush"))
            {
                ImGui::Checkbox("Brush", &brush);

                if (brush) {
                    if (ImGui::Button("Change", ImVec2(80, 30))) {
                        m_terrain.ChangeTexture(m_context);
                    }

                    if (ImGui::ImageButton(m_terrain.m_albedoSRV.Get(),
                                           ImVec2(30, 30))) {
                        m_terrain.m_constantData.brushIndex = 0.0f;
                    }
                    ImGui::SameLine(); 
                    if (ImGui::ImageButton(m_terrain.m_sandSRV.Get(),
                                           ImVec2(30, 30))) {
                        m_terrain.m_constantData.brushIndex = 1.0f;
                    } 
                    ImGui::SameLine();
                    if (ImGui::ImageButton(m_terrain.m_grassSRV.Get(),
                                           ImVec2(30, 30))) {
                        m_terrain.m_constantData.brushIndex = 2.0f;
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton(m_terrain.m_albedoSRV02.Get(),
                                           ImVec2(30, 30))) {
                        m_terrain.m_constantData.brushIndex = 3.0f;
                    }
                }

                        ImGui::TreePop();
            }

            ImGui::Spacing();

            if (ImGui::TreeNode("Billboard"))
            {
                ImGui::Checkbox("Billboard", &b_billboard);

                if (b_billboard) {
                    m_terrain.m_constantData.EditFlag = 1;
                    m_mouseInfo.radius = 1.0f;

                    if (ImGui::Button("Billboard Delete", ImVec2(160, 30))) {
                        m_BillboardList.pop_back();
                    }
                } else {
                    m_terrain.m_constantData.EditFlag = 0;
                    //m_mouseInfo.radius = 5.0f;
                }

                        ImGui::TreePop();
            }

            ImGui::Spacing();


            if (ImGui::TreeNode("TurningPoint"))
            {
                 ImGui::Checkbox("TurningPoint", &b_turn);

                 if (b_turn)
                 {
                    m_terrain.m_constantData.EditFlag = 3;
                    m_mouseInfo.radius = 1.0f;

                    ImGui::Checkbox("DrawRange", &m_turn->b_drawRange);

                    ImGui::Checkbox("CreateTurningPoint", &b_createTurn);

                    if (ImGui::Button("DeleteTurningPoint", ImVec2(80, 30))) {
                        m_turn->m_points.pop_back();
                        m_turn->m_pickedPoint = nullptr;
                        //m_turn->m_constantData.AreaOn = 0;
                    }
                     
                    if (m_turn->m_pickedPoint)
                    {
                        ImGui::SliderFloat("Radius",
                                           &m_turn->m_pickedPoint->radius, 2.0f,
                                           5.0f);
                        ImGui::SliderFloat("Angle",
                                           &m_turn->m_pickedPoint->angle, 0.0f,
                                           XM_2PI);
                        ImGui::SliderFloat("StartAngle",
                                           &m_turn->m_pickedPoint->startAngle,
                                           0.0f, XM_2PI);
                        if (ImGui::Button("Reverse", ImVec2(50, 30)))
                        {
                            m_turn->m_pickedPoint->DirFlag =
                                (m_turn->m_pickedPoint->DirFlag + 1) % 2;
                        }
                    }
                 }
                 else
                 {
                    m_terrain.m_constantData.EditFlag = 0;
                 }



                 ImGui::TreePop();
            }






        }
        else
        {
            m_Editcamera.m_farZ = currentFarZ;
        }

        ImGui::Spacing();
        ImGui::Spacing();

            if (ImGui::TreeNode("Lake")) {
            ImGui::Checkbox("Real Lake", &realLake);

            if (realLake) {
                        ImGui::SliderFloat("Lake Alpha",
                                           &m_lake.m_constantData.lakeAlpha,
                                           0.0f, 1.0f);
                        ImGui::SliderFloat("Wave Strength",
                                           &m_lake.m_constantData.waveStrength,
                                           0.0f, 0.3f);

                        m_lake.m_materialConstsCPU.TexOnly = 0;
            } else {
                        m_lake.m_materialConstsCPU.TexOnly = 1;
            }

            ImGui::TreePop();
        }

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::Checkbox("ObjEdit", &objEdit);

        ImGui::Spacing();

        ImGui::Checkbox("OBB", &m_drawOBB);

        if (ImGui::Button("Create", ImVec2(50, 50)))
        {
            m_createNewObj = true;
        }
        ImGui::SameLine(); 
        if (ImGui::Button("Delete", ImVec2(50, 50)))
        {
            m_deletePickedObj = true;
        }

        if (ImGui::Button("CollisionBoxCreate", ImVec2(100, 30)))
        {
            if (pickedModel != nullptr)
            {
                        m_collisionObj.push_back(pickedModel);
                        pickedModel->m_boxConstData.IsCollisionBox = 1;
            }


        }

        if (ImGui::Button("CollisionBoxDelete", ImVec2(100, 30))) {
            
            m_deleteCollisionBox = true;
        }

        if (ImGui::Button("Tree1", ImVec2(50, 50)))
        {
            m_createNewObj = true;
            pickedModel = m_tree01; 
        }
        ImGui::SameLine();

        if (ImGui::Button("Tree2", ImVec2(50, 50)))
        {
            m_createNewObj = true;
            pickedModel = m_tree02;
        }
        ImGui::SameLine();
        if (ImGui::Button("Grass", ImVec2(50, 50)))
        {
            m_createNewObj = true; 
            pickedModel = m_grass01;
        }

        if (ImGui::Button("Fence", ImVec2(50, 50)))
        {
            m_createNewObj = true;
            pickedModel = m_fence;
        }

        ImGui::SameLine();
        if (ImGui::Button("Rock", ImVec2(50, 50)))
        {
            m_createNewObj = true;
            pickedModel = m_rock01;
        }

        ImGui::SameLine();
        if (ImGui::Button("House", ImVec2(50, 50)))
        {
            m_createNewObj = true;
            pickedModel = m_house01;
        }

        if (pickedModel) {
            //ImGui::SetNextItemWidth(50);
            //ImGui::SliderFloat("Scale_X", &pickedModel->m_worldRow._11, 1.0f,
            //                   3.0f);
            //ImGui::SameLine();
            //ImGui::SetNextItemWidth(50);
            //ImGui::SliderFloat("Scale_Y", &pickedModel->m_worldRow._22, 1.0f,
            //                   3.0f);
            //ImGui::SameLine();
            //ImGui::SetNextItemWidth(50);
            //ImGui::SliderFloat("Scale_Z", &pickedModel->m_worldRow._33, 1.0f,
            //                   3.0f);
            ImGui::SetNextItemWidth(50);
            if (ImGui::Button("Rotation_X", ImVec2(50, 50)))
            {
                        pickedModel->rotation.x += XM_PI * 0.5f;
            }
            ImGui::SameLine();

            ImGui::SetNextItemWidth(50);
            if (ImGui::Button("Rotation_Y", ImVec2(50, 50)))
            {
                        pickedModel->rotation.y += XM_PI * 0.5f;
            }
            ImGui::SameLine();

            if (ImGui::Button("Rotation_Z", ImVec2(50, 50)))
            {
                        pickedModel-> rotation.z += XM_PI * 0.5f;
            }


            ImGui::SetNextItemWidth(50);
            ImGui::SliderFloat("Scale_X", &pickedModel->scale.x, 1.0f,
                               3.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::SliderFloat("Scale_Y", &pickedModel->scale.y, 1.0f,
                               3.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::SliderFloat("Scale_Z", &pickedModel->scale.z, 1.0f,
                               3.0f);
        }

        ImGui::TreePop();
    }  

    ImGui::Spacing();

    ImGui::SetNextItemOpen(false, ImGuiCond_Once); 
    if (ImGui::TreeNode("Optimization"))
    {
        ImGui::Spacing();
        ImGui::Spacing();

        if (ImGui::TreeNode("Graphics"))
        {
            ImGui::Spacing();

            if (ImGui::RadioButton("Upper", (int*)&m_graphicState,
                                   (int)GraphicState::UPPER))
            {
                currentFarZ = 30.0f;
                m_Editcamera.m_farZ = currentFarZ;
                m_globalConstsCPU.graphicLower = false;
            }


            if (ImGui::RadioButton("Middle", (int*)&m_graphicState,
                (int)GraphicState::MIDDLE))
            {
                currentFarZ = 20.0f;
                m_Editcamera.m_farZ = currentFarZ;
                m_globalConstsCPU.graphicLower = false;
            }
            if (ImGui::RadioButton("Lower", (int*)&m_graphicState,
                (int)GraphicState::LOWER))
            {
                currentFarZ = 10.0f;
                m_Editcamera.m_farZ = currentFarZ;
                m_globalConstsCPU.graphicLower = true;
            }

            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNode("Tessellation"))
        {
            ImGui::Checkbox("Tessellation",
                            &m_terrain.m_constantData.Tessellation);

            ImGui::Spacing();

            ImGui::SliderInt("TessLevel",
                             &m_terrain.m_constantData.TessellationLevel, 0, 4);



            ImGui::TreePop();
        }



        ImGui::TreePop();
    }

     ImGui::Spacing();  

    ImGui::SetNextItemOpen(false, ImGuiCond_Once);
    if (ImGui::TreeNode("General")) {

        ImGui::Spacing();

        ImGui::Checkbox("Use FPV", &m_Editcamera.m_useFirstPersonView);

        ImGui::Spacing();

        ImGui::Checkbox("Wireframe", &m_drawAsWire);

        ImGui::Spacing();

        if (ImGui::Checkbox("MSAA ON", &m_useMSAA)) {
            CreateBuffers();
        }
        ImGui::TreePop();
    }

    ImGui::Spacing();

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Weather"))
    {
        if (ImGui::Checkbox("Snow", &m_snowOn))
        {
            m_rainOn = false;
            updateSkyState = true;
            m_postEffectsConstsCPU.fogStrength = 0.1f;
            m_globalConstsCPU.strengthIBL = 0.15f;
            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Rain", &m_rainOn))
        {
            m_snowOn = false;
            updateSkyState = true;
            m_postEffectsConstsCPU.fogStrength = 0.1f;
            m_globalConstsCPU.strengthIBL = 0.15f;
            D3D11Utils::UpdateBuffer(m_device, m_context,
                                     m_postEffectsConstsCPU,
                                     m_postEffectsConstsGPU);
        }

         
        ImGui::TreePop(); 
    }

    ImGui::Spacing();

    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Skybox")) {
        ImGui::SliderFloat("Strength", &m_globalConstsCPU.strengthIBL, 0.0f,
                           5.0f);
        ImGui::RadioButton("Env", &m_globalConstsCPU.textureToDraw, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Specular", &m_globalConstsCPU.textureToDraw, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Irradiance", &m_globalConstsCPU.textureToDraw, 2);
        ImGui::SliderFloat("EnvLodBias", &m_globalConstsCPU.envLodBias, 0.0f,
                           10.0f);
        ImGui::TreePop();     
    }

    //ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    //if (ImGui::TreeNode("Post Effects")) {
    //    int flag = 0;
    //    flag += ImGui::RadioButton("Render", &m_postEffectsConstsCPU.mode, 1);
    //    ImGui::SameLine();
    //    flag += ImGui::RadioButton("Depth", &m_postEffectsConstsCPU.mode, 2);
    //    flag += ImGui::SliderFloat(
    //        "DepthScale", &m_postEffectsConstsCPU.depthScale, 0.0, 1.0);
    //    flag += ImGui::SliderFloat("Fog", &m_postEffectsConstsCPU.fogStrength,
    //                               0.0, 10.0);

    //    if (flag)
    //        D3D11Utils::UpdateBuffer(m_device, m_context,
    //                                 m_postEffectsConstsCPU,
    //                                 m_postEffectsConstsGPU);

    //    ImGui::TreePop();
    //}

    //if (ImGui::TreeNode("Post Processing")) {
    //    int flag = 0;
    //    flag += ImGui::SliderFloat(
    //        "Bloom Strength",
    //        &m_postProcess.m_combineFilter.m_constData.strength, 0.0f, 1.0f);
    //    flag += ImGui::SliderFloat(
    //        "Exposure", &m_postProcess.m_combineFilter.m_constData.option1,
    //        0.0f, 10.0f);
    //    flag += ImGui::SliderFloat(
    //        "Gamma", &m_postProcess.m_combineFilter.m_constData.option2, 0.1f,
    //        5.0f);
    //    // 편의상 사용자 입력이 인식되면 바로 GPU 버퍼를 업데이트
    //    if (flag) {
    //        m_postProcess.m_combineFilter.UpdateConstantBuffers(m_device,
    //                                                            m_context);
    //    }
    //    ImGui::TreePop();
    //}

 
}

void SceneManager::UpdateEditScene(float dt) {

    if (EditScene) { 
        // 카메라의 이동
        m_Editcamera.UpdateEditKeyboard(dt, m_keyPressed, m_character);
    }
    else
    {
        m_Editcamera.UpdateKeyboard(dt, m_keyPressed);
    }

    if (!m_snowOn && !m_rainOn && updateSkyState)
    {
        m_postEffectsConstsCPU.fogStrength = 0.0f;
        m_globalConstsCPU.strengthIBL = 0.5f;
        D3D11Utils::UpdateBuffer(m_device, m_context, m_postEffectsConstsCPU,
                                 m_postEffectsConstsGPU);
        updateSkyState = false;
    }
     

    if (terrainEdit && m_Editcamera.m_useFirstPersonView == false) {

        m_Editcamera.SetEyePos(Vector3(0.0f, 20.0f, 0.0f));

        const float pi = 3.14159265358979323846f;
        m_Editcamera.m_pitch = -pi / 2.0f;
        m_Editcamera.m_yaw = 0.0f;
    }



    // 반사 행렬 추가
    const Vector3 eyeWorld = m_Editcamera.GetEyePos();
    const Matrix reflectRow = Matrix::CreateReflection(m_lakePlane);
    const Matrix viewRow = m_Editcamera.GetViewRow();
    const Matrix projRow = m_Editcamera.GetProjRow();
    const Matrix OrthoView = m_uiCamera.GetViewRow();
    const Matrix OrthoProj =
        XMMatrixOrthographicLH(m_screenWidth, m_screenHeight,
                               m_uiCamera.m_nearZ, m_uiCamera.m_farZ);
    Matrix viewproj = (viewRow * projRow); 
    Matrix invViewProj = viewproj.Invert();
    const Matrix reflectProj = (reflectRow * viewRow * projRow).Transpose();

    m_frustum.Update(invViewProj);

    UpdateLights(dt);

    if (terrainEdit && !objEdit) {
        
        static float minX = 100.f;
        static float maxX = -100.f;
        static float minZ = 100.f;
        static float maxZ = -100.f;

        static bool CorrectionFlag = false;



        static bool canCreateCollisionBox = false;

        Vector3 MouseWorldPos;

        Vector3 near_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 0.0f);

        Vector3 far_pos = Vector3(m_cursorNdcX, m_cursorNdcY, 1.0f);

        Matrix inverseProjView = (viewRow * projRow).Invert();

        Vector3 cursorWorldnear = Vector3::Transform(near_pos, inverseProjView);
        Vector3 cursorWorldfar = Vector3::Transform(far_pos, inverseProjView);
        m_mouseInfo.WorldNearPos = cursorWorldnear;
        m_mouseInfo.WorldFarPos = cursorWorldfar;

        Vector3 ray = cursorWorldfar - cursorWorldnear;
        ray.Normalize();

        SimpleMath::Ray pick = SimpleMath::Ray(cursorWorldnear, ray);
        float dis = 0.0f;

                
            if (pick.Intersects(m_terrainBox, dis)) {
                // m_cursorSphere->m_isVisible = true;
                MouseWorldPos = cursorWorldnear + dis * ray;

                m_mouseInfo.picked = 1;
                m_mouseInfo.mousePos = MouseWorldPos;

                m_mouseInfo.mouseTexcoord.x =
                    abs(-m_terrain.scale - m_mouseInfo.mousePos.x) /
                    m_terrain.scale / 2;
                m_mouseInfo.mouseTexcoord.y =
                    (m_terrain.scale - m_mouseInfo.mousePos.z) /
                    m_terrain.scale / 2;

                // cout << m_mouseInfo.mouseTexcoord.x << " and"
                //      << m_mouseInfo.mouseTexcoord.y << endl;

                if (m_leftButton) {
                    m_mouseInfo.leftclicked = 1;
                    m_mouseInfo.rightclicked = 0;
                } else if (m_rightButton) {
                    m_mouseInfo.rightclicked = 1;
                    m_mouseInfo.leftclicked = 0;
                } else {
                    m_mouseInfo.rightclicked = 0;
                    m_mouseInfo.leftclicked = 0;
                }

            } else {
                m_mouseInfo.picked = 0;
            }
        
        if(m_terrain.m_constantData.EditFlag == 3 && !b_createTurn) // turn 선택하기
        {
            for (auto& i : m_turn->m_points)
            {
                    i->m_materialConstsCPU.albedoFactor =
                        Vector3(1.0f, 0.0f, 0.0f);

                if (pick.Intersects(i->m_box, dis)) {
                    // m_cursorSphere->m_isVisible = true;
                    MouseWorldPos = cursorWorldnear + dis * ray;

                    //m_mouseInfo.picked = 1;
                    i->m_materialConstsCPU.albedoFactor =
                        Vector3(0.0f, 1.0f, 0.0f);
                    m_mouseInfo.mousePos = MouseWorldPos;

                    m_mouseInfo.mouseTexcoord.x =
                        abs(-m_terrain.scale - m_mouseInfo.mousePos.x) /
                        m_terrain.scale / 2;
                    m_mouseInfo.mouseTexcoord.y =
                        (m_terrain.scale - m_mouseInfo.mousePos.z) /
                        m_terrain.scale / 2;

                    // cout << m_mouseInfo.mouseTexcoord.x << " and"
                    //      << m_mouseInfo.mouseTexcoord.y << endl;

                    if (m_leftButton) {
                        m_mouseInfo.leftclicked = 1;
                        m_mouseInfo.rightclicked = 0;
                        m_turn->m_pickedPoint = i;
                    } else if (m_rightButton) {
                        m_mouseInfo.rightclicked = 1;
                        m_mouseInfo.leftclicked = 0;
                    } else {
                        m_mouseInfo.rightclicked = 0;
                        m_mouseInfo.leftclicked = 0;
                    }

                } else {
                    m_mouseInfo.picked = 0;
                } 
            }

        }


        // 높이 보정
        if (m_keyPressed[VK_SHIFT] && m_terrain.m_constantData.EditFlag != 1)
        {
            m_terrain.m_constantData.EditFlag = 2;
        }
         

        switch (m_terrain.m_constantData.EditFlag) { 
        case 0: // Terrain 높이 , brush
            if (m_mouseInfo.leftclicked == 1 ||
                m_mouseInfo.rightclicked == 1) // 최소 최대 축 업데이트
            {
                 
                minX = min(minX, m_mouseInfo.mousePos.x - m_mouseInfo.radius);
                maxX = max(maxX, m_mouseInfo.mousePos.x + m_mouseInfo.radius);
                minZ = min(minZ, m_mouseInfo.mousePos.z - m_mouseInfo.radius);
                maxZ = max(maxZ, m_mouseInfo.mousePos.z + m_mouseInfo.radius);

                canCreateCollisionBox = true;
            }     
            break;
        case 1: // Bill board 설치
            if (m_mouseInfo.leftclicked == 1)
            {
                shared_ptr<Billboard> billboard = make_shared<Billboard>(m_device, m_context, Vector4(m_mouseInfo.mousePos.x, 1.2f, m_mouseInfo.mousePos.z, 1.0f));

                m_BillboardList.push_back(billboard);
            }
            m_mouseInfo.leftclicked = 0;
            m_mouseInfo.rightclicked = 0; // 클릭했을때 높이변경도 되는 것 방지
            break;
        case 2: // 높이 보정
            if (m_mouseInfo.leftclicked == 1)
            {
                if (m_terrain.m_constantData.CorrectionAxis.x == 100.0f)
                {
                    m_terrain.m_constantData.CorrectionAxis.x = m_mouseInfo.mousePos.x;
                    m_terrain.m_constantData.CorrectionAxis.w = m_mouseInfo.mousePos.z;
                }
                else 
                { 
                    m_terrain.m_constantData.CorrectionAxis.y =
                        m_mouseInfo.mousePos.x;
                    m_terrain.m_constantData.CorrectionAxis.z =
                        m_mouseInfo.mousePos.z;
                }

                CorrectionFlag = true;

            }
            else if(m_mouseInfo.leftclicked == 0 && CorrectionFlag)// 클릭을 뗐을때
            {

                m_terrain.heightCorrection(m_device, m_context);

                m_terrain.m_constantData.CorrectionAxis =
                    Vector4(100.0f, 100.0f, 100.0f, 100.0f);

                CorrectionFlag = false;

                
                m_terrain.m_constantData.EditFlag = 0;

            }
            break;
        case 3: // 터닝 포인트
            if (m_mouseInfo.rightclicked == 1 && b_turnFlag)
            {
                m_turn->AddPoint(m_device, m_context, m_mouseInfo.mousePos);
                m_turn->m_pickedPoint = m_turn->m_points[m_turn->m_points.size() - 1];
                b_turnFlag = false;

                cout<<m_turn->m_points.size()<<endl;
            } else if (m_mouseInfo.rightclicked == 0)
            {
                b_turnFlag = true;
            }
            m_mouseInfo.leftclicked = 0;
            m_mouseInfo.rightclicked = 0; // 클릭했을때 높이변경도 되는 것 방지
            break;
        }


        if (m_mouseInfo.leftclicked == 1 || m_mouseInfo.rightclicked == 1) // 최소 최대 축 업데이트
        {
           

            minX = min(minX, m_mouseInfo.mousePos.x - m_mouseInfo.radius);
            maxX = max(maxX, m_mouseInfo.mousePos.x + m_mouseInfo.radius);
            minZ = min(minZ, m_mouseInfo.mousePos.z - m_mouseInfo.radius);
            maxZ = max(maxZ, m_mouseInfo.mousePos.z + m_mouseInfo.radius);

            canCreateCollisionBox = true;
        }

        if (canCreateCollisionBox && AutoTerrainCollisionBox)
        {
            float height = m_terrain.GetMaxheight(m_context, minX, maxX, minZ, maxZ );

            Vector3 center = Vector3((maxX + minX) * 0.5f, height * 0.5f,
                                     (minZ + maxZ) * 0.5f);


            
            Vector3 extent = Vector3(abs(maxX - minX) * 0.5f + 0.5f, height * 0.5f, abs(maxZ - minZ) * 0.5f + 0.5f);

            shared_ptr<CollisionBox> box =
                make_shared<CollisionBox>(m_device, m_context, center, extent);



            m_collisionBox.push_back(box);

            minX = 100.f;
            maxX = -100.f;
            minZ = 100.f;
            maxZ = -100.f;

            AutoTerrainCollisionBox = false;
            canCreateCollisionBox = false;
        }

        if (DirectTerrainCollisionBox)
        {
            m_mouseInfo.picked = 0;
            m_terrainQuad->m_isVisible = true;
            Vector3 pos = m_mouseInfo.mousePos;


            m_terrainQuad->UpdateWorldRow(
                Matrix::CreateScale(Vector3(quadX, quadZ, 1.0f)) *
                Matrix::CreateRotationX(XM_PI * 0.5f) *
                Matrix::CreateTranslation(Vector3(pos.x, 0.0f, pos.z)));

            if (m_mouseInfo.leftclicked == 1 && directFlag == 0.0f) {



                float height = m_terrain.GetMaxheight(
                    m_context, pos.x - quadX , pos.x + quadX, pos.z - quadZ,
                    pos.z + quadZ);

                Vector3 center = Vector3(pos.x, height * 0.5f, pos.z);




                Vector3 extent =
                    Vector3(quadX * 0.5f, height * 0.5f, quadZ * 0.5f);

                            shared_ptr<CollisionBox> box = make_shared<CollisionBox>(
                    m_device, m_context, center, extent);

                    m_collisionBox.push_back(box);
                    directFlag++;

                     

                m_mouseInfo.leftclicked = 0;
                m_mouseInfo.rightclicked = 0;
            }
              
            m_mouseInfo.leftclicked = 0;
            m_mouseInfo.rightclicked = 0;





        } else
            m_terrainQuad->m_isVisible = false;

    }

     
      
    m_globalConstsCPU.globalTime += dt;


 

    // 조명의 위치 반영
    for (int i = 0; i < MAX_LIGHTS; i++)
        m_lightSphere[i]->UpdateWorldRow(
            Matrix::CreateScale(
                std::max(0.01f, m_globalConstsCPU.Sun.radius)) *
            Matrix::CreateTranslation(m_globalConstsCPU.Sun.position));

    if (objEdit)
        MousePickingUpdate(viewRow, projRow); 

    //for (auto& i : m_BillboardList)
    //{
    //    i->Update(m_device, m_context);
    //}
     //billboard.Update(m_device, m_context, viewRow, projRow, eyeWorld);

    if (m_snowOn) {
        m_terrain.m_constantData.Texindex = 1; 
        m_surplusConstsCPU.SnowOn = true;
    } else { 
        m_terrain.m_constantData.Texindex = 0;
        m_surplusConstsCPU.SnowOn = false;
    } 
        
    if (brush)
    {
        m_terrain.m_constantData.brush = true;
    }
    else
    {
        m_terrain.m_constantData.brush = false;
    }

    if (EditScene)
    {
        m_character->UpdateConstantBuffers(m_device, m_context);
        UpdateCharacterAnimation(dt); 
    }


    AppBase::UpdateGlobalConstants(eyeWorld, viewRow, projRow, reflectRow,
                                   OrthoView, OrthoProj);

    m_terrain.Update(m_device, m_context, viewRow, projRow, eyeWorld);



    for (auto &i : m_EditList) {

        i->UpdateConstantBuffers(m_device, m_context);
    }
    m_terrainQuad->UpdateConstantBuffers(m_device, m_context);


    if (m_snowOn) {
        m_snow.Update(m_device, m_context);
        m_lake.m_constantData.snowOn = true;
        m_lake.m_constantData.waveStrength = 0.0f;
    } else
        m_lake.m_constantData.snowOn = false;


    m_rain.Update(m_context);


    m_lake.Update(m_device, m_context, reflectProj);

    
    m_map->Update(m_device, m_context);

    m_lightSphere[0]->UpdateConstantBuffers(m_device, m_context);

    //m_test->UpdateConstantBuffers(m_device, m_context);
    //m_test1->UpdateConstantBuffers(m_device, m_context);
    //m_test2->UpdateConstantBuffers(m_device, m_context);

    if (terrainEdit)
        m_turn->Update(m_device, m_context);

    m_ssao.Update(m_device, m_context);

}

void SceneManager::UpdateBasicScene(float dt) {

     
    if (!EditScene) { 
        // 카메라의 이동
        m_Basiccamera.UpdateBasicKeyboard(dt, m_keyPressed, m_character, cutAway);
    }
    else
    {
        m_Basiccamera.UpdateKeyboard(dt, m_keyPressed);
    }

    // 반사 행렬 추가
    const Vector3 eyeWorld = m_Basiccamera.GetEyePos();
    const Matrix reflectRow = Matrix::CreateReflection(m_mirrorPlane);
    const Matrix viewRow = m_Basiccamera.GetViewRow();
    const Matrix projRow = m_Basiccamera.GetProjRow();
    Matrix viewproj = viewRow.Transpose() * projRow.Transpose();


    if (!EditScene) {
        MousePickingUpdateBasic(viewRow, projRow);
        UpdateCharacterAnimation(dt);
        m_character->UpdateConstantBuffers(m_device, m_context);
    }

    m_basicConstsCPU.strengthIBL = 1.0f;

    m_basicConstsCPU.globalTime += dt;
    AppBase::UpdateBasicConstants(eyeWorld, viewRow, projRow);
     
    for (auto& i : m_BasicList)
    {
        i->UpdateConstantBuffers(m_device, m_context); 
    }

    cutAway->UpdateConstantBuffers(m_device, m_context);

}

void SceneManager::RenderEditScene() {
    // 모든 샘플러들을 공통으로 사용 (뒤에서 더 추가됩니다.)
    m_context->VSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());
    m_context->PSSetSamplers(0, UINT(Graphics::sampleStates.size()),
                             Graphics::sampleStates.data());

    // 공용 텍스춰들: "Common.hlsli"에서 register(t10)부터 시작
    vector<ID3D11ShaderResourceView *> commonSRVs = {
        m_envSRV.Get(), m_specularSRV.Get(), m_irradianceSRV.Get(),
        m_brdfSRV.Get(), m_TRmapSRV.Get()};
    m_context->PSSetShaderResources(10, UINT(commonSRVs.size()),
                                    commonSRVs.data());

     

    vector<ID3D11ShaderResourceView *> RTT = {m_EditresolvedSRV.Get()}; 
    m_context->PSSetShaderResources(19, UINT(RTT.size()), RTT.data());


    AppBase::SetMainViewport();

    if (b_ssao || b_onlySSAO)
        DepthPass();
 
  
    // 일단 햇빛 그림자맵만
    AppBase::SetShadowViewport();

    SunShadowPass();

    // 다시 렌더링 해상도로 되돌리기
    AppBase::SetMainViewport();  

    // 반사맵
    if (realLake)
        RenderReflect();

    TerrainMapPass();
         
    RenderGameObject();

    m_context->ResolveSubresource(m_EditresolvedBuffer.Get(), 0,
                                  m_EditBuffer.Get(), 0,
                                  DXGI_FORMAT_R16G16B16A16_FLOAT);

}   

void SceneManager::RenderBasicScene() {


       const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    vector<ID3D11RenderTargetView *> rtvs2 = {m_BasicRTV.Get()}; 


        for (size_t i = 0; i < rtvs2.size(); i++) {
        m_context->ClearRenderTargetView(rtvs2[i], clearColor);
    }
    m_context->OMSetRenderTargets(UINT(rtvs2.size()), rtvs2.data(),
                                  m_depthStencilView.Get());

        m_context->ClearDepthStencilView(m_depthStencilView.Get(),
                                     D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                     1.0f, 0);

        AppBase::SetGlobalConsts(m_basicConstsGPU);

        AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                               : Graphics::defaultSolidPSO);

        for (auto& i : m_BasicList)
        {
            i->Render(m_context);
        }
          

        cutAway->Render(m_context);

        if (!EditScene)
        {
            AppBase::SetPipelineState(Graphics::skinnedSolidPSO);
            m_character->Render(m_context, m_terrain.m_terrainPassSRV);
        }

        AppBase::SetPipelineState(Graphics::skyboxSolidPSO);
        m_skybox->Render(m_context);


} 

void SceneManager::DepthPass() 
{   
        // SSAO

        m_context->OMSetRenderTargets(1, m_ssao.GetRTV().GetAddressOf(),
                                      m_depthOnlyDSV2.Get());
        m_context->ClearDepthStencilView(
            m_depthOnlyDSV2.Get(), D3D11_CLEAR_DEPTH,    
            1.0f,
            0);  
         
        AppBase::SetGlobalConsts(m_globalConstsGPU);   
         
        AppBase::SetPipelineState(Graphics::defaultSolidPSO);
        m_context->PSSetShader(Graphics::ssaoNormalPS.Get(), 0, 0);
        for (auto& i : m_EditList)
        {   
            i->Render(m_context);  
        }     
           
        AppBase::SetPipelineState(Graphics::skinnedSolidPSO);
        m_context->PSSetShader(Graphics::ssaoNormalPS.Get(), 0 ,0 );
        m_character->Render(m_context, m_terrain.m_terrainPassSRV);

        //AppBase::SetPipelineState(Graphics::terrainSolidPSO);
        //m_context->PSSetShader(Graphics::ssaoNormalPS.Get(), 0, 0);
        //m_terrain.Render(m_context);

        


}

void SceneManager::SunShadowPass()
{
        AppBase::SetPipelineState(Graphics::depthOnlyPSO);
        m_context->OMSetRenderTargets(0, NULL, m_SunShadowDSV.Get());
        m_context->ClearDepthStencilView(m_SunShadowDSV.Get(),
                                         D3D11_CLEAR_DEPTH, 1.0f, 0);
        AppBase::SetGlobalConsts(m_SunShadowGlobalConstGPU);

        for (auto &i : m_EditList) {
            if (i->m_castShadow && i->m_isVisible)
            i->Render(m_context);
        }   

        m_skybox->Render(m_context); 

        AppBase::SetPipelineState(Graphics::terrainShadowPSO);
        m_terrain.Render(m_context);
         
        AppBase::SetPipelineState(Graphics::lakeShadowPSO);
        m_lake.Render(m_context, m_reflectSRV, m_terrain.m_terrainPassSRV);
          
        AppBase::SetPipelineState(Graphics::skinnedShadowPSO);
        m_character->Render(m_context, m_terrain.m_terrainPassSRV);

        if(b_ssao || b_onlySSAO)
            m_ssao.CreateSSAOMap(m_context, m_depthOnlySRV2);
}

void SceneManager::TerrainMapPass()
{
        const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

        vector<ID3D11RenderTargetView *> rtvs2 = {m_terrainMapRTV.Get()};

        // terrain Map
        m_context->OMSetRenderTargets(UINT(rtvs2.size()), rtvs2.data(),
                                      m_depthOnlyDSV.Get());
        m_context->ClearDepthStencilView(
            m_depthOnlyDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f,
            0);
        AppBase::SetGlobalConsts(m_terrainMapConstGPU);
        AppBase::SetPipelineState(Graphics::terrainSolidPSO);
        m_terrain.Render(m_context);

        AppBase::SetPipelineState(Graphics::lakePSO);
        m_lake.Render(m_context, m_reflectSRV, m_terrain.m_terrainPassSRV);
}

void SceneManager::RenderGameObject() 
{
        const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};  
        vector<ID3D11RenderTargetView *> rtvs = {m_EditRTV.Get()};

        for (size_t i = 0; i < rtvs.size(); i++) {
            m_context->ClearRenderTargetView(rtvs[i], clearColor);
        }
        m_context->OMSetRenderTargets(UINT(rtvs.size()), rtvs.data(),
                                      m_depthStencilView.Get());

        vector<ID3D11ShaderResourceView *> depthSRVs = {
            m_SunShadowSRV.Get(), m_depthOnlySRV.Get(),
            m_terrainDepthOnlySRV.Get(), m_ssao.GetSSAOMapSRV().Get()};
        m_context->PSSetShaderResources(15, UINT(depthSRVs.size()),
                                        depthSRVs.data());

        m_context->ClearDepthStencilView(
            m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f, 0);


#pragma region 일반 물체
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                               : Graphics::defaultSolidPSO);
        AppBase::SetGlobalConsts(m_globalConstsGPU);

        // 투명도 x
        //nowObjCount = 0;
        //for (auto &i : m_EditList) {

        //    if (m_frustum.FrustumCulling(i)) {
        //    nowObjCount++;
        //    i->Render(m_context);         
        //    }    
        //}
        //if (m_terrainQuad->m_isVisible) {
        //    m_terrainQuad->Render(m_context);
        //}

        if (m_terrain.m_constantData.EditFlag == 3)
            m_turn->Render(m_device, m_context);

        if (m_turn->b_drawRange)
        {
            AppBase::SetPipelineState(Graphics::normalsPSO);
            m_turn->RenderRange(m_device, m_context);
        }





        if (m_drawOBB) {
            AppBase::SetPipelineState(Graphics::BoundingBoxPSO);

            for (auto &i : m_EditList) {

            if (i->m_boxDraw)
                i->RenderWireBBox(m_context);
            }
            m_character->RenderWireBBox(m_context);

            if (EditScene)
            {
            AppBase::SetPipelineState(Graphics::collisionPSO);
            if (m_collisionBox.size() > 0) {
                for (auto &i : m_collisionBox)
                    i->Render(m_context);
            }
            }

        }

#pragma endregion


#pragma region 날씨
        if (m_rainOn) {
            AppBase::SetPipelineState(Graphics::rainPSO);
            m_rain.RenderCS(m_context);
            m_rain.Render(m_context);
        }


        if (m_snowOn) {
            AppBase::SetPipelineState(Graphics::snowPSO);
            m_snow.Render(m_context);
        }
#pragma endregion


#pragma region Skybox
        AppBase::SetPipelineState(Graphics::skyboxSolidPSO);
        m_skybox->m_materialConstsCPU.TexOnly = 1;
        m_skybox->UpdateConstantBuffers(m_device, m_context);
        m_skybox->Render(m_context);
#pragma endregion

          
            

              
#pragma region 지형 , Lake , 빌보드
        // 터레인
        m_context->OMSetDepthStencilState(m_defaultDepthState.Get(), 0);
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::terrainWirePSO
                                               : Graphics::terrainSolidPSO); 
        m_context->PSSetShaderResources(8, 1, m_depthOnlySRV2.GetAddressOf());

        if (terrainEdit && m_terrain.m_constantData.EditFlag != 2) {
            if (brush) {
            m_terrain.PixelRender(m_context);
            } else {
            m_terrain.PassRender(m_context); 
            m_terrain.SmoothRender(m_context);
            }  
        }     
         
        m_terrain.HeightRender(m_context);
        //m_terrain.Render(m_context); 

        // Lake
        AppBase::SetPipelineState(Graphics::lakePSO);
        m_lake.Render(m_context, m_reflectSRV, m_terrain.m_terrainPassSRV);



        if (m_terrainQuad->m_isVisible) {
            AppBase::SetPipelineState(Graphics::stencilMaskPSO);
            m_terrainQuad->Render(m_context);
        }
          
        // 지형들
        AppBase::SetPipelineState(Graphics::BillboardPSO);

        for (auto &i : m_BillboardList) {
            i->Render(m_context, m_terrain.m_terrainPassSRV);
        }
        // billboard.Render(m_context);



#pragma endregion


#pragma region 실루엣
         
        // 실루엣
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::terrainWirePSO
                                               : Graphics::terrainSolidPSO); 
        AppBase::SetGlobalConsts(m_globalConstsGPU);
        m_context->OMSetDepthStencilState(Graphics::maskDepth.Get(), 1.0f);
        m_context->PSSetConstantBuffers(
            2, 1, m_turn->m_constantBuffer.GetAddressOf());
        m_terrain.Render(m_context); 
           
        if (EditScene) {
            AppBase::SetPipelineState(Graphics::skinnedSolidPSO);
            m_context->OMSetDepthStencilState(Graphics::maskDepth.Get(), 2.0f);
            m_character->Render(m_context, m_terrain.m_terrainPassSRV);
        }    
           
        if (silhouette)
        {
            if (EditScene) {
            AppBase::SetPipelineState(Graphics::skinnedSilhouettePSO); // Rim 이용
            //AppBase::SetPipelineState(Graphics::silhouettePSO); // 크기 키워서 외곽선 -> PS 수정해야함
            m_context->OMSetDepthStencilState(Graphics::drawMasked.Get(), 1.0f);
            m_character->Render(m_context, m_terrain.m_terrainPassSRV);
            } 
        }  
#pragma endregion

         
         

#pragma region UI 
        // UI
          
        m_context->OMSetDepthStencilState(m_uiDepthState.Get(), 0);
        AppBase::SetPipelineState(Graphics::uiPSO);
        m_map->Render(m_context, m_terrainMapSRV);
        m_key->Render(m_context);

        if (keyViewerOn)
            m_keyViewer.Render(m_context);



        // 투명도
        m_context->OMSetDepthStencilState(m_defaultDepthState.Get(), 0);
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::defaultWirePSO
                                               : Graphics::defaultSolidPSO);
        AppBase::SetGlobalConsts(m_globalConstsGPU);
        nowObjCount = 0;
        for (auto &i : m_EditList) {  

            if (m_frustum.FrustumCulling(i)) {
            nowObjCount++;
            i->Render(m_context); 
            }
        }
#pragma endregion

 

}

void SceneManager::RenderPostEffect() {
        if (EditScene) {
            m_floatRTV = m_EditRTV;
            m_floatBuffer = m_EditBuffer;
        } else {
            m_floatRTV = m_BasicRTV;
            m_floatBuffer = m_BasicBuffer;
        }


        m_context->ResolveSubresource(m_resolvedBuffer.Get(), 0,
                                      m_floatBuffer.Get(), 0,
                                      DXGI_FORMAT_R16G16B16A16_FLOAT);

        // PostEffects
        AppBase::SetPipelineState(Graphics::postEffectsPSO);

        vector<ID3D11ShaderResourceView *> postEffectsSRVs = {
            m_resolvedSRV.Get(), m_depthOnlySRV.Get(), m_ssao.GetSSAOMapSRV().Get()};

        // 그림자맵 확인용 임시 
        // AppBase::SetGlobalConsts(m_shadowGlobalConstsGPU[0]);
        AppBase::SetGlobalConsts(m_globalConstsGPU);

        // 20번에 넣어줌  
        m_context->PSSetShaderResources(20, UINT(postEffectsSRVs.size()),
                                        postEffectsSRVs.data());
        m_context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), NULL);


        m_context->PSSetConstantBuffers(3, 1,
                                        m_postEffectsConstsGPU.GetAddressOf());
        m_screenSquare->Render(m_context);

        // 단순 이미지 처리와 블룸
        AppBase::SetPipelineState(Graphics::postProcessingPSO);
        m_postProcess.Render(m_context);
}

void SceneManager::RenderReflect() {
        vector<ID3D11RenderTargetView *> lakeRTV = {m_reflectRTV.Get()};
        const float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};  

        for (int i = 0; i < lakeRTV.size(); i++) {
            m_context->ClearRenderTargetView(lakeRTV[i], clearColor);
        }

        m_context->OMSetRenderTargets(UINT(lakeRTV.size()), lakeRTV.data(),
                                      m_depthStencilView.Get());
        m_context->ClearDepthStencilView(
            m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f, 0);
        AppBase::SetGlobalConsts(m_reflectGlobalConstsGPU);
        AppBase::SetPipelineState(m_drawAsWire ? Graphics::reflectWirePSO
                                               : Graphics::reflectSolidPSO);
        for (auto &i : m_EditList) {
            i->Render(m_context);
        }

        AppBase::SetPipelineState(Graphics::reflectSkinnedSolidPSO);
        m_character->Render(m_context, m_terrain.m_terrainPassSRV);

        AppBase::SetPipelineState(m_drawAsWire
                                      ? Graphics::reflectSkyboxWirePSO
                                      : Graphics::reflectSkyboxSolidPSO);
        m_skybox->Render(m_context);

        AppBase::SetPipelineState(m_drawAsWire
                                      ? Graphics::reflectTerrainWirePSO
                                      : Graphics::reflectTerrainSolidPSO);
        m_terrain.Render(m_context);
            
        //if (m_snowOn)
        //{
        //    AppBase::SetPipelineState(Graphics::reflectSnowPSO);
        //    m_snow.Render(m_context);
        //}
        //else if (m_rainOn)
        //{
        //    AppBase::SetPipelineState(Graphics::reflectRainPSO);
        //    m_rain.Render(m_context);
        //}
}


void SceneManager::UpdateCharacterAnimation(float dt) {
        static int frameCount = 0;



    static int state = 0;
        static bool stateFlag = false;
    static bool jump = false;
        static bool move = false;
    static bool onGround = true;
    static float speed = 2.0f;
    static bool key_X = false;
    static float wait = 0.0f;
        Vector3 front = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 subfront = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 RayDir = Vector3(0.0f, 0.0f, 1.0f);
    Vector3 subDir = Vector3(0.0f, 0.0f, 1.0f);
        wait += dt;

    if (m_keyPressed['K'] && wait >= 0.3f)
    {
        keyViewerOn = !keyViewerOn;
        wait = 0.0f;
    }

    if (keyViewerOn)
    {
        m_key->m_constantData.push = 1;
    }
    else
    {
        m_key->m_constantData.push = 0;
    }

    // key type 초기화
    InitKeyViewer();

        Vector3 pos = m_character->m_worldRow.Translation();
        m_character->m_worldRow.Translation() = Vector3(0.0f);
        static float r; // 바라보는 방향
        move = false;

        if (m_keyPressed['X']) // 키뷰어 땜에 따로 분리
        {
            m_keyViewer.m_key[KEY_X]->m_constantData.push = 1;
            key_X = true;
        }
        else
        {
            key_X = false;
        }

        if (key_X && jump == false && onGround) {



            state = 2;
            frameCount = 30;
            jump = true;
            stateFlag = true;

        }
    

#pragma region 이동
             

        // turning point 검사해서 영역 안에 있으면 카메라의 viewDir 변경
        if (!m_turn->m_points.empty())
        {
            if (m_turn->InArea(pos))
            {
                m_turn->Turning(m_Editcamera, pos);
            }
        }
        

            if (AppBase::m_keyPressed[VK_UP]) {
            m_keyViewer.m_key[KEY_UP]->m_constantData.push = 1;

            if (AppBase::m_keyPressed[VK_LEFT]) {
            
            m_keyViewer.m_key[KEY_LEFT]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(-45.0f)));
              

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f)); 

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } 
            else if (AppBase::m_keyPressed[VK_RIGHT]) {
            
            m_keyViewer.m_key[KEY_RIGHT]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(45.0f)));

            // 회전

            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } 
            else {
            
            // 이동
            front = mainCamera.GetViewDir();

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            }

            // pos += front * dt * 2;
            RayDir = front;
            move = true;
        } 
            else if (AppBase::m_keyPressed[VK_DOWN]) {

            m_keyViewer.m_key[KEY_DOWN]->m_constantData.push = 1;

            if (AppBase::m_keyPressed[VK_LEFT]) {
            
            m_keyViewer.m_key[KEY_LEFT]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(-135.0f)));

            // 회전

            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } 
            else if (AppBase::m_keyPressed[VK_RIGHT]) {
            
            m_keyViewer.m_key[KEY_RIGHT]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(135.0f)));

            // 회전

            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            }
            else {
            
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(180.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            }

            RayDir = front;
            move = true;
        } else if (AppBase::m_keyPressed[VK_RIGHT]) {

            m_keyViewer.m_key[KEY_RIGHT]->m_constantData.push = 1;

            if (AppBase::m_keyPressed[VK_UP]) {
            
            m_keyViewer.m_key[KEY_UP]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(45.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } else if (AppBase::m_keyPressed[VK_DOWN]) {
 
            m_keyViewer.m_key[KEY_DOWN]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(135.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } else {
    
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(90.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            }

            RayDir = front;
            // pos += front * dt * 2;

            move = true;
        } else if (AppBase::m_keyPressed[VK_LEFT]) {

        m_keyViewer.m_key[KEY_LEFT]->m_constantData.push = 1;

            if (AppBase::m_keyPressed[VK_UP]) {
            
            m_keyViewer.m_key[KEY_UP]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(-45.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } else if (AppBase::m_keyPressed[VK_DOWN]) {

            m_keyViewer.m_key[KEY_DOWN]->m_constantData.push = 1;
            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(-135.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            } else {

            // 이동
            front = Vector3::Transform(
                mainCamera.GetViewDir(),
                Matrix::CreateRotationY(XMConvertToRadians(-90.0f)));

            // 회전
            Vector3 ro;
            ro = front;
            ro.Normalize();
            float dotProduct = ro.Dot(Vector3(0.0f, 0.0f, 1.0f));
            float angle = acos(std::clamp(dotProduct, -1.0f, 1.0f));

            if (ro.x < 0)
                angle = -angle;

            r = XM_PI + angle;

            }

            RayDir = front;
            move = true;
        }

#pragma endregion

    


    if (move && jump == false)
    {
            state = 1;

            if (stateFlag == false)
                frameCount = 0;

            stateFlag = true; // 상태 진행중
    } else if (move == false && jump == false)
    {
        state = 0;

        if (stateFlag == true)
            frameCount = 0;

        stateFlag = false;
        move = false;

    }






    if (jump && frameCount == m_character->m_aniData.clips[2].keys[0].size() - 60)
    {
        jump = false;
    }


    m_character->UpdateAnimation(m_context, state, frameCount);

    if (jump) {
        if (frameCount >= 30 &&
            frameCount <
                (30 + m_character->m_aniData.clips[2].keys[0].size() - 60) *
                    0.5)
            pos.y += 0.03f;
        else
            pos.y -= 0.03;
        //frameCount += 1;

    }
    //else
    //    frameCount += 1;

#pragma region 대시
    if (m_keyPressed['Z']) {

        m_keyViewer.m_key[KEY_Z]->m_constantData.push = 1;

        speed = 3.0f;

        if (jump)
            frameCount += 1;
        else
            frameCount += 2;
    } else {
        frameCount += 1;
        speed = 2.0f;
    }


#pragma endregion 


#pragma region 충돌

    if (EditScene)
    {
        if (abs(front.x) + abs(front.y) + abs(front.z) <= 1.0f) {
            RayDir.Normalize();
            SimpleMath::Ray cRay = SimpleMath::Ray(pos, RayDir);

            float dist = 100.0f;

            for (auto &i : m_collisionBox) {
                if (cRay.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        front = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                }
            }

             for (auto &i : m_collisionObj) {
                if (cRay.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        front = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                }
            }           
        } else {

            RayDir = Vector3(0.0f, 0.0f, front.z);
            subDir = Vector3(front.x, 0.0f, 0.0f);
            Vector3 frontUp = RayDir;
            Vector3 frontRight = subDir;
            RayDir.Normalize();
            subDir.Normalize();
            SimpleMath::Ray cRay1 = SimpleMath::Ray(pos, RayDir);
            SimpleMath::Ray cRay2 = SimpleMath::Ray(pos, subDir);

            float dist = 100.0f;

            for (auto &i : m_collisionBox) {
                if (cRay1.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        frontUp = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                } else if (cRay2.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        frontRight = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                }
            }

            for (auto &i : m_collisionObj) {
                if (cRay1.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        frontUp = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                } else if (cRay2.Intersects(i->m_box, dist)) {
                    if (dist <= m_character->extent.z) {
                        frontRight = Vector3(0.0f, 0.0f, 0.0f);
                        break;
                    }
                }
            }

            front = frontUp + frontRight;
        }
    }

#pragma endregion




     pos += front * dt * speed;

          //// 캐릭터 높이 확인
     //m_context->ResolveSubresource(m_terrain.m_resolveTexture.Get(), 0,
     //                              m_terrain.m_terrainPassBuffer.Get(), 0,
     //                              DXGI_FORMAT_R32G32B32A32_FLOAT); 

     //float pixel[4] = {-1.0f, -1.0f, -1.0f, -1.0f};

     //D3D11_BOX box;
     //box.left = 640;
     //box.right = 640+ 1;
     //box.top = 360;
     //box.bottom = 360 + 1;
     //box.front = 0;
     //box.back = 1;
     //m_context->CopySubresourceRegion(m_terrain.m_stagingTexture.Get(), 0, 0, 0, 0, m_terrain.m_terrainPassBuffer.Get(), 0,
     //                                 &box);

     // D3D11_MAPPED_SUBRESOURCE mappedResource;
     // m_context->Map(m_terrain.m_stagingTexture.Get(), 0, D3D11_MAP_READ, 0,
     //              &mappedResource); 

     // memcpy(pixel, mappedResource.pData, sizeof(float) * 4);

     // 
     // m_context->Unmap(m_terrain.m_stagingTexture.Get(), 0);

     float height = m_terrain.GetheightGPUtoCPU(m_context);

     //if (pos.y - m_character->extent.y <= height) {
     //   pos.y = height + m_character->extent.y;
     //   onGround = true;
     //} else {
     //   pos.y -= 0.05f;
     //   onGround = false;
     //}
       
     // cpu
     // 중력
     if (!jump && pos.y - m_character->extent.y > height)
     {
        pos.y -= 0.1f;
        onGround = false;
     } else
        onGround = true;

     if (pos.y - m_character->extent.y <= height) {
        pos.y = height + m_character->extent.y;
     } 

     // gpu
     //if (!jump && pos.y - m_character->extent.y > m_pos.x)
     //{
     //   pos.y -= 0.05f;
     //   onGround = false;
     //} else
     //   onGround = true;

        
     m_globalConstsCPU.characterWorldPos = pos;

    // cout << r << endl;


        m_character->UpdateWorldRow(Matrix::CreateRotationY(r) *
                                Matrix::CreateTranslation(pos));
}

void SceneManager::GetPosFromGPU(Vector2 texcoord)
{
        //// 일부만 복사할 때 사용
        //D3D11_BOX box;
        //box.left = texcoord.x;
        //box.right = texcoord.x + 1;
        //box.top = texcoord.y;
        //box.bottom = texcoord.y + 1;
        //box.front = 0;
        //box.back = 1;
        //m_context->CopySubresourceRegion(m_posStagingTexture.Get(), 0, 0, 0, 0,
        //                                 m_terrain.m_terrainPassBuffer.Get(), 0,
        //                                 &box);

        //D3D11_MAPPED_SUBRESOURCE
        //ms; // m_indexStagingTexture에 있는 색깔값을 cpu로 복사
        //m_context->Map(m_posStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
        //               &ms); // D3D11_MAP_READ 주의

        //memcpy(m_pos, ms.pData, sizeof(uint16_t) * 4);
        //m_context->Unmap(m_posStagingTexture.Get(), NULL);
}

void SceneManager::InitKeyViewer()
{
    for (auto& i : m_keyViewer.m_key)
    {
        i->m_constantData.push = 0;
    }
}