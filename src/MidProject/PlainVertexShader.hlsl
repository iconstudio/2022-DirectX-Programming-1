#include "Basis.hlsli"

VS_DIFFUSED_OUTPUT main(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT result;
    result.position = float4(input.position, 1.0f);
    result.colour = input.colour;
    result.position_world = float3(input.position.y * screen_h
    , input.position.x * screen_w
    , 1.0f);
    
	return result;
}