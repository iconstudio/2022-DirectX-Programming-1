float4 main(float4 input : SV_POSITION) : SV_TARGET
{
	//프리미티브의 모든 픽셀의 색상을 노란색으로 반환한다.
	
	return (float4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow
}