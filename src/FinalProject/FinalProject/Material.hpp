#pragma once

class Material
{
public:
	Material(RawMaterial* info);
	Material(const int id);
	virtual ~Material();

	void SetShader(const int id);

	virtual void PrepareRendering(P3DGrpCommandList cmdlist);

	int myShaderID;

	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4 m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); // (r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

class RawMaterial
{
public:
	RawMaterial(const UINT type = 0);
	~RawMaterial();

	const UINT myType;

	XMFLOAT4 m_xmf4AlbedoColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4 m_xmf4EmissiveColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4SpecularColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	float m_fGlossiness = 0.0f;
	float m_fSmoothness = 0.0f;
	float m_fSpecularHighlight = 0.0f;
	float m_fMetallic = 0.0f;
	float m_fGlossyReflection = 0.0f;
};
