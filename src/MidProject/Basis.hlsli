//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			16 
#define MAX_MATERIALS		512 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

struct LIGHT
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular;
    float3 m_vPosition;
    float m_fFalloff;
    float3 m_vDirection;
    float m_fTheta; //cos(m_fTheta)
    float3 m_vAttenuation;
    float m_fPhi; //cos(m_fPhi)
    bool m_bEnable;
    int m_nType;
    float m_fRange;
    float padding;
};

struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; //a = power
    float4 m_cEmissive;
};

//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
    // 화면의 절대 좌표 
    float4 position : SV_POSITION;
    // 렌더링 좌표
    float3 position_world : POSITION;
    // 색상
    float4 colour : COLOR;
};

struct VS_PLAIN_INPUT
{
    float2 position : POSITION;
    float4 colour : COLOR;
};

struct VS_PLAIN_OUTPUT
{
    // 화면의 절대 좌표
    float4 position : SV_POSITION;
    // 렌더링 좌표
    float2 position_world : POSITION;
    // 색상
    float4 colour : COLOR;
};

cbuffer cbCameraInfo : register(b0, space0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

// GPU 인스턴스
cbuffer cbGameObjectInfo : register(b1, space0)
{
    matrix gmtxGameObject : packoffset(c0);
    MATERIAL gMaterial : packoffset(c4);
};

cbuffer cbLights : register(b0, space1)
{
    LIGHT gLights[MAX_LIGHTS];
    float4 gcGlobalAmbientLight;
    int gnLights;
};

// 화면 해상도
cbuffer cbResolutions : register(b0, space2)
{
    float screen_w;
    float screen_h;
};
