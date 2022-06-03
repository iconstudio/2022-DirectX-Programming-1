struct VS_DIFFUSED_INPUT
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
    float3 position : SV_POSITION;
    float3 position_world : POSITION;
    float4 colour : COLOR;
};

cbuffer Camera : register(b1, space0)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};
