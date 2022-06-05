#include "Light.hlsli"

float4 main(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
    input.normalW = normalize(input.normalW);
    
    return gMaterial.m_cDiffuse;
    //float4 color = Lighting(input.positionW, input.normalW);
    //return (color);
#endif
}

