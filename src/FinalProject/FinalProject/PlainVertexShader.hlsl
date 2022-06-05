#include "PlainShaders.hlsli"

VS_DIFFUSED_OUTPUT main(VS_DIFFUSED_INPUT input) : SV_POSITION
{
    VS_DIFFUSED_OUTPUT result;
    result.colour = input.colour;
    result.world = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

    return result;
}
