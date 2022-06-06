#include "PlainShaders.hlsli"

float4 main(VS_PLAIN_OUTPUT input) : SV_TARGET
{
    // input.colour;
   // return float(1.0f);
    return input.colour;
}
