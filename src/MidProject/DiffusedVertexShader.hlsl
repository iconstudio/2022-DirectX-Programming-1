#include "Basis.hlsli"

VS_DIFFUSED_OUTPUT main(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT result;
    result.colour = input.colour;

    float4 prop_position = mul(float4(input.position, 1.0f), gmtxGameObject);
    result.position_world = prop_position.xyz;
    result.position = mul(mul(prop_position, gmtxView), gmtxProjection);

    return result;
}
