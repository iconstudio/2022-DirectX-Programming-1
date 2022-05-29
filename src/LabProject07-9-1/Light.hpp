#pragma once

class CLight
{
public:
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;

	// ��ġ
	XMFLOAT3 m_xmf3Position;

	XMFLOAT3 m_xmf3Direction;

	// �� ������ �Ÿ� ���ⷮ ����
	// ���� ���� (����[0]) + (����[1] * �Ÿ�) + (����[2] ���� * �Ÿ� ����)���� ������.
	XMFLOAT3 m_xmf3Attenuation;
	// ���� �ִ� ���� �Ÿ�
	float m_fRange;

	// ���ߴ�(Spot) ������ ���ߴ� ����
	float m_fTheta; //cos(m_fTheta)
	// ���ߴ�(Spot) ������ �ִ�� ���ߴ� ���� (m_fRange�� ���� ����)
	float m_fPhi; //cos(m_fPhi)
	// ���ߴ�(Spot) ������ ���� �������� ��Ÿ ���� ������ �Ÿ��� ���� ���ⷮ
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
