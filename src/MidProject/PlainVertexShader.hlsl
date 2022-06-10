#include "Basis.hlsli"

VS_DIFFUSED_OUTPUT main(VS_DIFFUSED_INPUT input)
{
    VS_DIFFUSED_OUTPUT result;
    result.position = float4(input.position, 1.0f);
    result.colour = input.colour;
    result.position_world.x = input.position.x * screen_w;
    result.position_world.y = input.position.y * screen_h;
    
	return result;
}