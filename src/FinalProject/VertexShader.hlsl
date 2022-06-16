#include "Basis.hlsli"

VS_LIGHTING_OUTPUT main(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	
    float4 prop_position = mul(float4(input.position, 1.0f), gmtxGameObject);
    output.positionW = prop_position.xyz;
    output.position = mul(mul(prop_position, gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	
	return output;
}
