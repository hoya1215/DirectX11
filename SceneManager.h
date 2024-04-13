#pragma once



#include "AppBase.h"
#include "GeometryGenerator.h"
#include "ImageFilter.h"
#include "Model.h"
#include "Billboard.h"
#include "Particle.h"
#include "Terrain.h"
#include "Snow.h"
#include "Rain.h"
#include "Lake.h"
#include "Frustum.h"
#include "SkinnedMeshModel.h"
#include "ConstantBuffers.h"
#include "CollisionBox.h"
#include "KeyViewer.h"
#include "TurningPoint.h"
#include "SSAO.h"
 

  
struct LakeEffect
{
    Vector3 pos;
    float maxRadius;
    float nowRadius;
};

struct TerrainPixel
{
    Vector3 pos; // 픽셀위치의 정점 위치
    float texIndex; 
};

enum class GraphicState
{
    UPPER,
    MIDDLE, 
    LOWER,

    END
};


struct BoundingSquare {

    Vector3 right;
    Vector3 left;
    Vector3 up;
    Vector3 down;

    float intersect(Vector3 ray) {
        if (ray.x <= right.x && ray.x >= left.x && ray.z <= up.z &&
            ray.z >= down.z) {
            return ray.y;
        } else
            return 0;
    }
};  

class SceneManager : public AppBase {
  public:
    SceneManager();  

    virtual bool Initialize() override;  
    virtual void UpdateGUI() override;
    virtual void Update(float dt) override;
    virtual void Render() override;
     
    void UpdateLights(float dt);
    void MousePickingUpdate(const Matrix View, const Matrix Proj);
    void MousePickingUpdateBasic(const Matrix View, const Matrix Proj);

    void UpdateEditScene(float dt);
    void UpdateBasicScene(float dt);
    void UpdateCharacterAnimation(float dt);
    void GetPosFromGPU(Vector2 texcoord);
    void InitKeyViewer();

    void RenderEditScene();
    void RenderBasicScene(); 
    void DepthPass();
    void SunShadowPass();
    void TerrainMapPass();
    void RenderGameObject();
    void RenderReflect();
    void RenderPostEffect();

  protected:
    GraphicState m_graphicState = GraphicState::MIDDLE;

    shared_ptr<Model> m_ground;
    shared_ptr<Model> m_background;
    shared_ptr<Model> m_mainObj;
    shared_ptr<Model> m_lightSphere[MAX_LIGHTS];

    shared_ptr<Model> m_cursorSphere;
    shared_ptr<Model> m_screenSquare;
    shared_ptr<Model> m_door;
    shared_ptr<SkinnedMeshModel> m_character;
    Vector4 m_pos = Vector4{0.0f};
    shared_ptr<Model> m_ui;

      
    // 나무
    shared_ptr<Model> m_tree01;
    shared_ptr<Model> m_tree02;

    // Grass
    shared_ptr<Model> m_grass01;

    // Rock
    shared_ptr<Model> m_rock01;

    // Fence
    shared_ptr<Model> m_fence;

    // House
    shared_ptr<Model> m_house01;

    // 터레인
    Terrain m_terrain;
    shared_ptr<Model> m_terrainQuad;
    BoundingBox m_terrainBox;
    Lake m_lake;
    DirectX::SimpleMath::Plane m_lakePlane;
    bool realLake = true; // Lake 최적화
    bool AutoTerrainCollisionBox = false;
    bool DirectTerrainCollisionBox = false;
    StructuredBuffer<TerrainPixelInfo> m_terrainPixel;
    float quadX = 2.0f;
    float quadZ = 2.0f;

    //int EditFlag = 0;
    list<shared_ptr<CollisionBox>> m_collisionBox;
    list<shared_ptr<Model>> m_collisionObj;

    // Bill board
    vector<shared_ptr<Billboard>> m_BillboardList;
    bool b_billboard;
    Billboard billboard;   


    // weather
    int m_weather = 1;

    Snow m_snow;
    Rain m_rain;
    bool m_snowOn = false;
    bool m_rainOn = false;

    BoundingSphere m_mainBoundingSphere;
    BoundingSphere m_doorBoundingSphere;

    bool m_usePerspectiveProjection = true;

    // 거울
    shared_ptr<Model> m_mirror;
    DirectX::SimpleMath::Plane m_mirrorPlane;
    float m_mirrorAlpha = 1.0f; // Opacity

    // 거울이 아닌 물체들의 리스트 (for문으로 그리기 위함)
    list<shared_ptr<Model>> m_EditList;
    vector<shared_ptr<Model>> m_BasicList;
           
    // Picking
    bool m_picked = false; // edit 중인 물체가 있으면 true 로 다른 물체 pick 못하게 
    bool m_clicked = false; // 클릭중인지
    bool m_scale = false;
    shared_ptr<Model> pickedModel; 
    Vector3 prevPos;
    Vector3 newPos;
    bool newMove = false; // 처음 이동할때
     
     

    // 물체 생성 삭제
    bool m_createNewObj = false;
    bool m_deletePickedObj = false;
    bool m_deleteCollisionBox = false; 

    // 장면전환
    shared_ptr<Model> cutAway;
    bool StartcutAway = false; // 장면 전환 시작    

    // Frustum
    Frustum m_frustum;
    int nowObjCount = 0;

    // KeyViewer
    KeyViewer m_keyViewer;
    shared_ptr<UI> m_map;
    shared_ptr<UI> m_key;  
    bool keyViewerOn = false;

    // 조명 초기 위치
    Vector3 SunStartPos;
    float sunTheta = 0.0f;  
      
    // 스텐실 테스트 
    shared_ptr<Model> m_test;
    bool test = false;

    shared_ptr<Model> m_test1;
    shared_ptr<Model> m_test2;

    bool silhouette = false;
        
    // 터닝 포인트
    shared_ptr<TurningPoint> m_turn;
    bool b_turn = false;
    bool b_createTurn = false;
    bool b_turnFlag = false;

    // SSAO
    SSAO m_ssao;
    bool b_onlySSAO = false;
    bool b_ssao = false;
};

