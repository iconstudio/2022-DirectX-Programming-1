#include "Basis.hlsli"

VS_PLAIN_OUTPUT main(VS_PLAIN_INPUT input)
{
    VS_PLAIN_OUTPUT result;
    result.colour = input.colour;

    float dx = input.position.x;
    float dy = input.position.y;

    result.position_world = input.position;
    result.position = float4(input.position, 1.0f);

    return result;
}