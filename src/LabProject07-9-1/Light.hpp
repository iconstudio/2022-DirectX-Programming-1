#pragma once

class CLight
{
public:
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;

	// 위치
	XMFLOAT3 m_xmf3Position;

	XMFLOAT3 m_xmf3Direction;

	// 점 조명의 거리 감쇄량 지수
	// 빛의 양을 (지수[0]) + (지수[1] * 거리) + (지수[2] 제곱 * 거리 제곱)으로 나눈다.
	XMFLOAT3 m_xmf3Attenuation;
	// 빛의 최대 영향 거리
	float m_fRange;

	// 비추는(Spot) 조명에서 비추는 각도
	float m_fTheta; //cos(m_fTheta)
	// 비추는(Spot) 조명에서 최대로 비추는 각도 (m_fRange에 의해 결정)
	float m_fPhi; //cos(m_fPhi)
	// 비추는(Spot) 조명에서 파이 각도에서 세타 각도 사이의 거리에 따른 감쇄량
	float m_fFalloff;

	bool m_bEnable;
	int m_nType;
	float padding;
};

struct StaticLights
{
	CLight everyLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
	int m_nLights;
};
