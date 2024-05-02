# DirectX11
### 각 셰이더 역할 간단 설명



__Basic__             : 캐릭터 및 일반 게임오브젝트



__Billboard__         : 빌보드



__Bloom__             : Bloom 효과 -> DOF 에 필요한 Blur 효과



__Combine__           : 기존 렌더링 장면 + 후처리 장면 합치는 용도



__Common__            : 공통 CBV , SRV , 함수 모음



__DepthOnly__         : 깊이값만 저장



__Normal__            : 전환전 범위 표시용



__Posteffect__        : 후처리 ( 안개 , SSAO 맵 )



__Rain__              : 날씨 - 비



__Snow__              : 날씨 - 눈



__SSAOBlurCS__        : SSAO 맵 Blur



__SSAOCS__            : SSAO 맵 계산



__SSAONormal__        : SSAO 에 필요한 법선맵 계산



__SSRCS__             : SSR 맵 계산



__Sampling__          : 텍스처 샘플링만 해주는 용도



__Silhouette__        : 실루엣



__Simple__            : 기본 색상값 ( 충돌 상자 )



__Skybox__            : 하늘



__Lake__              : 호수 관련


__Terrain__           : 지형 관련



__GetHeightCS__       : 현재 캐릭터 좌표에 해당하는 높이값 추출



__InitPixelCS__       : Texture Buffer 픽셀값 초기화



__TerrainCS__         : 정점단위 지형 높이맵 저장



__ChangeTextureCS__   : 지형 텍스처 한번에 바꾸는 용도



__CorrectionCS__      : 지형 높이 보정



__TerrainPassCS__     : 지형 높이 변경



__TerrainPixelCS__    : 지형 텍스처 스플래팅



__TerrainSmoothCS__   : 지형 높이 부드럽게 



__UI__                : UI 관련




__shadow 가 붙은 셰이더는 그림자 관련__

