#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

// 쉐이더에서 include할 내용들은 .hlsli 파일에 작성
// Properties -> Item Type: Does not participate in build으로 설정

#define MAX_LAKEEFFECT 10

#define MAX_LIGHTS 1 // 쉐이더에서도 #define 사용 가능
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

// 샘플러들을 모든 쉐이더에서 공통으로 사용
SamplerState linearWrapSampler : register(s0);
SamplerState linearClampSampler : register(s1);
SamplerState shadowPointSampler : register(s2);
SamplerComparisonState shadowCompareSampler : register(s3);

// 공용 텍스춰들 t10 부터 시작
TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfTex : register(t13);
TextureCube TRmapTex : register(t14);


Texture2D SunShadowMap : register(t15);
Texture2D PhantomObjDepthMap : register(t16);
Texture2D PhantomTerrainDepthMap : register(t17);
Texture2D ssaoMap : register(t18);

Texture2D EditRTV : register(t19);


struct Light
{
    float3 radiance; // Strength
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
    
    uint type;
    float radius;
    float haloRadius;
    float haloStrength;

    matrix viewProj;
    matrix invProj;
};

// 공용 Constants
cbuffer GlobalConstants : register(b10)
{
    matrix view;
    matrix proj;
    matrix invProj; // 역프로젝션행렬
    matrix viewProj;
    matrix invViewProj; // Proj -> World
    matrix OrthoView;
    matrix OrthoProj;

    float3 characterWorldPos;
    bool graphicLower;

    float3 eyeWorld;
    float strengthIBL;

    int textureToDraw = 0; // 0: Env, 1: Specular, 2: Irradiance, 그외: 검은색
    float envLodBias = 0.0f; // 환경맵 LodBias
    float lodBias = 0.0f; // 다른 물체들 LodBias
    float globalTime = 0.0f;

    float thickness = 1.0f;
    int ssao = 0;
    float2 globalpadding;
    
    Light lights[MAX_LIGHTS];
    Light Sun;
};

cbuffer MouseInfo : register(b11) {
    float3 mousePos;
    float radius;
    float3 WorldNearPos;
    float heightlength;
    float3 WorldFarPos;
    int leftclicked;
    int rightclicked;
    int picked; // terrain 만
    float2 mouseTexcoord;
};

cbuffer SurplusConstants : register(b12) {
    float cutAwayRadius;
    bool EditScene;
    bool SnowOn;
    int surplusPadding;
    matrix TerrainInvViewProj;
    matrix TerrainViewProj;
};

#ifdef SKINNED

// 관절 개수 제약을 없애게 위해 StructuredBuffer 사용
StructuredBuffer<float4x4> boneTransforms : register(t9);

// cbuffer SkinnedConstants : register(b3)
//{
//     float4x4 boneTransforms[52]; // 관절 개수에 따라 조절
// };

#endif

struct VertexShaderInput
{
    float3 posModel : POSITION; //모델 좌표계의 위치 position
    float3 normalModel : NORMAL0; // 모델 좌표계의 normal    
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;

    #ifdef SKINNED
    float4 boneWeights0 : BLENDWEIGHT0;
    float4 boneWeights1 : BLENDWEIGHT1;
    uint4 boneIndices0 : BLENDINDICES0;
    uint4 boneIndices1 : BLENDINDICES1;
#endif
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (조명 계산에 사용)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
};

float3 BasicLightRadiance(Light Sun, float3 normalWorld, float3 posWorld, float ambientCoefficient, float diffuseCoefficient, float specularCoefficient, Texture2D SunShadowMap)
{
    float3 sunDir = -Sun.direction;
    float3 r = reflect(-sunDir, normalWorld);
    float3 viewDir = eyeWorld - posWorld;

    float ambient = ambientCoefficient;
    float diffuse = dot(normalWorld, sunDir) * diffuseCoefficient;
    float specular = pow(max(dot(r, viewDir), 0.0), 32) * specularCoefficient;

    float shadowFactor = 1.0;

    // depth 비교
    float4 posDepth = mul(float4(posWorld, 1.0), Sun.viewProj); // 해 기준 depth
    posDepth.xyz /= posDepth.w;
    float2 Suntexcoord = posDepth.xy;
    Suntexcoord.x = Suntexcoord.x * 0.5 + 0.5;
    Suntexcoord.y = -Suntexcoord.y * 0.5 + 0.5;

    int width, height, numMips;
    SunShadowMap.GetDimensions(0, width, height, numMips);
    float dx = 1.0 / (float)width;
    float percent = 0.0;
    const float2 offsets[9] = {float2(-1, -1), float2(0, -1), float2(1, -1),
                               float2(-1, 0),  float2(0, 0),  float2(1, 0),
                               float2(-1, 1),  float2(0, 1),  float2(1, 1)};

    [unroll] for (int i = 0; i < 9; i++) {
        percent += SunShadowMap
                       .SampleCmpLevelZero(shadowCompareSampler,
                                           Suntexcoord.xy + offsets[i] * dx,
                                           posDepth.z - 0.00003)
                       .r;
    }

    shadowFactor = percent / 9.0;

    // SSAO
    float SSAO = 1.0;
    if (ssao == 1) {
        float4 posProj = mul(float4(posWorld, 1.0), viewProj);
        float2 ssaoTex = posProj.xy;
        ssaoTex.xy /= posProj.w;
        ssaoTex.x = ssaoTex.x * 0.5 + 0.5;
        ssaoTex.y = -ssaoTex.y * 0.5 + 0.5;

        SSAO = ssaoMap.SampleLevel(linearWrapSampler, ssaoTex, 0).r;

    }

        // 그림자 PCF
     //shadowFactor = SunShadowMap
     //                   .SampleCmpLevelZero(shadowCompareSampler,
     //                   Suntexcoord.xy,
     //                                       posDepth.z - 0.0001)
     //                   .r;

    return ((ambient + specular) * Sun.radiance + diffuse * Sun.radiance * shadowFactor) * SSAO;



            // 일반 그림자
     //float SunDepth = SunShadowMap.Sample(shadowPointSampler, Suntexcoord).r;

     //if (SunDepth + 0.0002 < posDepth.z)
     //    shadowFactor = 0.0;







}

float AlphaCheck(float3 CPos, float3 pos) {

    float dist = length(pos.xy - CPos.xy);
    //if (pos.x >= CPos.x - 0.3 && pos.x <= CPos.x + 0.3 &&
    //    pos.y >= CPos.y - 0.5 && pos.y <= CPos.y + 0.5)
    //    return 1;
    //else
    //    return 0;

    if (dist <= 0.6)
        return 1;
    else
        return 0;
}

static const float2 poissonDisk[16] =
{
    float2(-0.94201624, -0.39906216), float2(0.94558609, -0.76890725),
            float2(-0.094184101, -0.92938870), float2(0.34495938, 0.29387760),
            float2(-0.91588581, 0.45771432), float2(-0.81544232, -0.87912464),
            float2(-0.38277543, 0.27676845), float2(0.97484398, 0.75648379),
            float2(0.44323325, -0.97511554), float2(0.53742981, -0.47373420),
            float2(-0.26496911, -0.41893023), float2(0.79197514, 0.19090188),
            float2(-0.24188840, 0.99706507), float2(-0.81409955, 0.91437590),
            float2(0.19984126, 0.78641367), float2(0.14383161, -0.14100790)
};

static const float2 diskSamples[64] =
{
    float2(0.08588739595227426, -0.030025691920664423),
            float2(0.00023643037646479872, 0.20841231102873747), float2(-0.021014193513360258, -0.041295564670943447),
            float2(0.17222130793060222, 0.04208273481757199), float2(-0.10188002229260999, 0.09867635981047702),
            float2(0.14228303321566757, -0.2580966083703571), float2(0.1981280219522779, 0.23339378133878957),
            float2(-0.3507565783285025, -0.08514244901034494), float2(0.4109031442251335, -0.1385572016757659),
            float2(-0.206881894838767, 0.4355205101923277), float2(-0.07853732533126384, -0.40242593228540835),
            float2(0.3160903818840148, 0.21239359469104005), float2(-0.38171373492394534, 0.07981848579134865),
            float2(0.20730755700310005, -0.41694988161154317), float2(0.02116212593410295, 0.452217832379357),
            float2(-0.4796904605571695, -0.28651041940741073), float2(0.5297239385935364, 0.021582930653477564),
            float2(-0.37273632966671305, 0.37495166129580154), float2(-0.09981792946437304, -0.5906748143084893),
            float2(0.2916051250135627, 0.4431329722846758), float2(-0.48289435190476276, 0.07681350373204565),
            float2(0.5187852940910572, -0.3945141647783003), float2(-0.13963649020950908, 0.49027792217786015),
            float2(-0.38061248065802, -0.40308214832312955), float2(0.6158104812277763, 0.0983407178807064),
            float2(-0.6196449607402823, 0.23431768221667326), float2(0.2364557733641609, -0.6229671072840033),
            float2(0.3005763530869763, 0.6111111637098147), float2(-0.6384314720901211, -0.1738869117858379),
            float2(0.573665374558835, -0.15349124960603555), float2(-0.3196477554226972, 0.7061658310018843),
            float2(-0.1831977675319068, -0.5792765986262596), float2(0.6030483926071213, 0.4205516346121553),
            float2(-0.7539032462944255, 0.21931618991687488), float2(0.404646236896082, -0.6316149350590375),
            float2(0.08658582362354603, 0.7874143627737417), float2(-0.5005622663103718, -0.4234605043899057),
            float2(0.7390320541648133, -0.08070989534160762), float2(-0.6159218824886621, 0.4341605161757036),
            float2(-0.03309358947453572, -0.7000026169501067), float2(0.48803776980647995, 0.6519132695663368),
            float2(-0.77524869240331, -0.20524910902740462), float2(0.7399362385810723, -0.44195229094163613),
            float2(-0.23965632319752905, 0.7230660336872499), float2(-0.29944141213045217, -0.7621092069740195),
            float2(0.7010758517114678, 0.3812171234085407), float2(-0.7827431721781429, 0.3506678790434763),
            float2(0.3590538137372583, -0.6914906946153192), float2(0.18705594586863877, 0.8722461169359508),
            float2(-0.8103576005204458, -0.4865832198499747), float2(0.9524568869822917, -0.2807021815875268),
            float2(-0.5434861760406469, 0.8102944285691839), float2(-0.09371333179679404, -0.9210740863451133),
            float2(0.7331372463675839, 0.5308557721617637), float2(-0.8357615934512088, 0.09499243670075716),
            float2(0.6528317322451158, -0.7138708943030685), float2(-0.10113238393363477, 0.9474249151791483),
            float2(-0.616352398484564, -0.763336531568522), float2(0.9208229492320843, 0.08175463425604013),
            float2(-0.7960583335354177, 0.5466929855893787), float2(0.1380914715442238, -0.9144556469794014),
            float2(0.40238820357725436, 0.9331942124203743), float2(-0.9055908846548567, -0.39092628874429447),
            float2(0.9515415333809746, -0.3779463117824522),
};

static const float2 diskSamples128 [ 128 ] =
    {
    float2( -0.03425155690403161 ,  -0.05308116694763974 ),
    float2( 0.1701658708041952 ,  0.01668543570344616 ),     float2( -0.0035564179558435216 ,  0.01012922303657976 ),
    float2( 0.042032023050289025 ,  -0.10103893678993567 ),     float2( 0.08876267316065523 ,  0.22014816021957 ),
    float2( -0.1619893781077028 ,  -0.002005884644887393 ),     float2( 0.14701679815346985 ,  -0.05189721436187111 ),
    float2( -0.02655626058355382 ,  0.32148258777006367 ),     float2( -0.11709832500985635 ,  -0.29071076965603954 ),
    float2( 0.2403032010960865 ,  0.11497559921774712 ),     float2( -0.24378737713395401 ,  0.014135440109880784 ),
    float2( 0.07909642214670401 ,  -0.22388357386286012 ),     float2( 0.14612440006824978 ,  0.28587418619250693 ),
    float2( -0.3659644193389189 ,  -0.09020077746674479 ),     float2( 0.3170654737569668 ,  -0.1441435027431372 ),
    float2( -0.10824819920158484 ,  0.2776263564673448 ),     float2( -0.06290442040746787 ,  -0.3381361172907859 ),
    float2( 0.2405387753050873 ,  0.2512758215538293 ),     float2( -0.3043762431930471 ,  0.06120600309244391 ),
    float2( 0.2489326138441718 ,  -0.349387577765235 ),     float2( 0.015162085258529232 ,  0.3320626420934838 ),
    float2( -0.28521233318232864 ,  -0.37017063686994645 ),     float2( 0.48637060081580835 ,  0.11399821465270887 ),
    float2( -0.394774226460478 ,  0.1451167676799726 ),     float2( 0.12032869522330024 ,  -0.4210316978178582 ),
    float2( 0.2583459561308781 ,  0.3156600609327172 ),     float2( -0.45336481751054686 ,  -0.08923081698071722 ),
    float2( 0.3660487583736684 ,  -0.21884524734116903 ),     float2( -0.22346084781991707 ,  0.5161608404479042 ),
    float2( -0.0696797864894372 ,  -0.36602462298813776 ),     float2( 0.4835315541299822 ,  0.26623511495199864 ),
    float2( -0.4720846497924996 ,  0.15335451684181983 ),     float2( 0.3179805064946509 ,  -0.5044882107264742 ),
    float2( 0.0468243052516784 ,  0.49538941241146456 ),     float2( -0.35646530916554264 ,  -0.2739820958458027 ),
    float2( 0.4449560489507415 ,  -0.002446735154789822 ),     float2( -0.32330537135508486 ,  0.402956229337284 ),
    float2( -0.01750329706286821 ,  -0.5108000047018931 ),     float2( 0.3918340867289236 ,  0.42052529852136766 ),
    float2( -0.5081079756499899 ,  -0.0390615215609137 ),     float2( 0.45740441720545194 ,  -0.3450995921621081 ),
    float2( -0.15676677880129802 ,  0.536720937308919 ),     float2( -0.28833676825830556 ,  -0.41151374698524723 ),
    float2( 0.543452776123166 ,  0.18321283928691023 ),     float2( -0.6175247329878804 ,  0.22030743421079402 ),
    float2( 0.16091261739081153 ,  -0.6077679032188688 ),     float2( 0.18990857697650332 ,  0.4922564549045882 ),
    float2( -0.6346020184228193 ,  -0.31611100952698196 ),     float2( 0.5967207918468932 ,  -0.1472730333750505 ),
    float2( -0.3533613276596112 ,  0.4755015087569663 ),     float2( -0.07229022383617588 ,  -0.561126426694602 ),
    float2( 0.49213295587618056 ,  0.3726190616145742 ),     float2( -0.5485506678483738 ,  -0.01915260335058353 ),
    float2( 0.36095006506290145 ,  -0.4824161429174605 ),     float2( -0.0975649968481083 ,  0.5886663176907788 ),
    float2( -0.47129714700855185 ,  -0.3966303493569307 ),     float2( 0.5816639501537203 ,  0.07608649957215763 ),
    float2( -0.511327971551968 ,  0.31747635077432923 ),     float2( 0.07369153215774078 ,  -0.6032833786295734 ),
    float2( 0.27028186042790214 ,  0.58633412759484 ),     float2( -0.6951167618983334 ,  -0.163297714874072 ),
    float2( 0.5128128973137175 ,  -0.3248982403219308 ),     float2( -0.33622424946798996 ,  0.7370709541697704 ),
    float2( -0.3167090724313575 ,  -0.6636244386541041 ),     float2( 0.5877903697834362 ,  0.2145636791671983 ),
    float2( -0.6668043361982273 ,  0.12748420573942199 ),     float2( 0.3616322501053157 ,  -0.6128791443337742 ),
    float2( 0.2303501436629964 ,  0.7570238855370316 ),     float2( -0.5713729051877507 ,  -0.3590175830066332 ),
    float2( 0.8154315626693184 ,  -0.07147596549184625 ),     float2( -0.5481271962396196 ,  0.4823134794923314 ),
    float2( -0.1204039415027747 ,  -0.8438843682587722 ),     float2( 0.6196520669884248 ,  0.433601013897339 ),
    float2( -0.7206578054698676 ,  -0.12313564725824332 ),     float2( 0.49787576349805396 ,  -0.47959360515941807 ),
    float2( -0.12345525831731086 ,  0.820142633790211 ),     float2( -0.3933182278968285 ,  -0.687777505619843 ),
    float2( 0.8518309872748233 ,  0.23711148588668174 ),     float2( -0.7619279361428896 ,  0.4691460929962891 ),
    float2( 0.2904961935632503 ,  -0.6881697998507909 ),     float2( 0.23968374158714884 ,  0.7947592692706416 ),
    float2( -0.8116969091378421 ,  -0.21148066661237613 ),     float2( 0.7120030881771048 ,  -0.2980271616759561 ),
    float2( -0.4667532944404384 ,  0.6987608495898804 ),     float2( -0.22330002353162032 ,  -0.8593445320313498 ),
    float2( 0.6274453387112535 ,  0.40473530455824064 ),     float2( -0.8172551108077651 ,  0.2344758979801263 ),
    float2( 0.5619928217820943 ,  -0.6315016679098718 ),     float2( 0.033309114558007794 ,  0.8523487067988677 ),
    float2( -0.6743343851596627 ,  -0.48302831238801924 ),     float2( 0.8013036726071068 ,  -0.08064052284878126 ),
    float2( -0.5267210288774481 ,  0.5628035412696127 ),     float2( 0.08201380932736946 ,  -0.8075342054040922 ),
    float2( 0.5409380336484304 ,  0.7411923022125474 ),     float2( -0.8835125499641053 ,  -0.2144566487558743 ),
    float2( 0.7389816341586297 ,  -0.3744736122912272 ),     float2( -0.3027649759136447 ,  0.8595495827121186 ),
    float2( -0.43869822484344256 ,  -0.8200584710061951 ),     float2( 0.7261211657699493 ,  0.22958265750604837 ),
    float2( -0.8309384028884655 ,  0.31823712524664227 ),     float2( 0.37009445367157295 ,  -0.7268828619588146 ),
    float2( 0.3051119427488622 ,  0.9337626299594328 ),     float2( -0.8588618831074927 ,  -0.45947872378965904 ),
    float2( 0.8104514418042801 ,  -0.19927753858032918 ),     float2( -0.5508455963205285 ,  0.7064062659598491 ),
    float2( -0.03649530431827998 ,  -0.9267323398225243 ),     float2( 0.6649783667840259 ,  0.4923449795845305 ),
    float2( -1.0021896375385928 ,  0.006468872733901763 ),     float2( 0.5687172728634211 ,  -0.5624263731723224 ),
    float2( -0.0029509697214619504 ,  0.9456292762835624 ),     float2( -0.5624077897321023 ,  -0.6943809542101097 ),
    float2( 0.9472331521273569 ,  0.2149957968332787 ),     float2( -0.8369176694596512 ,  0.5346919282243854 ),
    float2( 0.22262486487056726 ,  -0.9445735109395622 ),     float2( 0.47167072810393657 ,  0.7622441113645463 ),
    float2( -0.9384800809679902 ,  -0.22159218701053768 ),     float2( 0.7926243911893078 ,  -0.4656014094626464 ),
    float2( -0.408912027679533 ,  0.8596210109798386 ),     float2( -0.4167858780276352 ,  -0.9468277804011632 ),
    float2( 0.8786368172080327 ,  0.516243916119742 ),     float2( -0.8992391922641874 ,  0.22872055857502202 ),
    float2( 0.6139297675354396 ,  -0.894627043184961 ),     float2( 0.07200082109232493 ,  1.046422529023392 ),
    float2( -0.8425997551039348 ,  -0.6590594841076259 ),     float2( 1.0758380211107275 ,  -0.05170775035487607 ),
    float2( -0.7573948243093395 ,  0.6439344775533422 ),     float2( -0.05454741645396839 ,  -0.9307164889333771 ),
    float2( 0.7628938334406858 ,  0.7124073240762442 ),     };
#endif // __COMMON_HLSLI__