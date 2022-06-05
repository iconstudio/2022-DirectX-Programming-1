#include "PlainShaders.hlsli"

float4 main(VS_DIFFUSED_OUTPUT diffuse) : SV_TARGET
{
	return diffuse.colour;
}
