#include "Basis.hlsli"

VS_PLAIN_OUTPUT main(VS_PLAIN_INPUT input)
{
    VS_PLAIN_OUTPUT result;
    result.colour = input.colour;

    result.position_world = float2(input.position.x * screen_w, input.position.y * screen_h);
    result.position = float4(input.position, 0.0f, 1.0f);

    return result;
}