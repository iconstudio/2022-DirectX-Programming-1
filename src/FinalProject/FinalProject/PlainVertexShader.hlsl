#include "PlainShaders.hlsli"

VS_PLAIN_OUTPUT main(VS_PLAIN_INPUT input)
{
    VS_PLAIN_OUTPUT result;
    
    result.position = float4(input.position, 1.0f);
    result.position_world = input.position;
    //result.position_world = mul(float4(input.position, 1.0f), myWorldMatrix).xyz;
    //result.position = mul(mul(float4(result.position_world, 1.0f), myCameraProjection), myWorldMatrix);
    result.colour = input.colour;
    return result;
}
