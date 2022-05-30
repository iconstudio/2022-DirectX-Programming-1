#pragma once

class CLight
{
public:
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;

	// ��ġ
	XMFLOAT3 m_xmf3Position;

	// ���ߴ�(Spot) ������ ���� �������� ��Ÿ ���� ������ �Ÿ��� ���� ���ⷮ
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;

	// ���ߴ�(Spot) ������ ���ߴ� ����
	float 					m_fTheta; //cos(m_fTheta)

	// �� ������ �Ÿ� ���ⷮ ����
	// ���� ���� (����[0]) + (����[1] * �Ÿ�) + (����[2] ���� * �Ÿ� ����)���� ������.
	XMFLOAT3				m_xmf3Attenuation;

	// ���ߴ�(Spot) ������ �ִ�� ���ߴ� ���� (m_fRange�� ���� ����)
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;

	int						m_nType;

	// ���� �ִ� ���� �Ÿ�
	float					m_fRange;

	float					padding;
};

struct StaticLights
{
	CLight everyLights[MAX_LIGHTS];
	XMFLOAT4 m_xmf4GlobalAmbient;
	int m_nLights;
};
