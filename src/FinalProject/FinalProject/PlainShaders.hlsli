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

cbuffer Camera : register(b0)
{
    matrix myCameraMatrix : packoffset(c0);
    matrix myCameraProjection : packoffset(c4);
    float3 myCameraPosition : packoffset(c8);
};

cbuffer GameInstance : register(b1)
{
    matrix myWorldMatrix : packoffset(c0);
};

cbuffer TestInstance : register(b2)
{
    float myTestPos : packoffset(c0);
    float myTestColour : packoffset(c2);
};
