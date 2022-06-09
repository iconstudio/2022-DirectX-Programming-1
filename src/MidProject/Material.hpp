#pragma once

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

class CMaterial
{
public:
	CMaterial();
	CMaterial(RawMaterial* raw_material);
	CMaterial& operator=(RawMaterial* raw_material);
	virtual ~CMaterial();

	void SetShader(Pipeline* pipeline);

	void PrepareRendering(P3DGrpCommandList cmdlist) const;

	Pipeline* m_pShader;
	XMFLOAT4 m_xmf4Ambient;
	XMFLOAT4 m_xmf4Diffuse;
	XMFLOAT4 m_xmf4Specular;
	XMFLOAT4 m_xmf4Emissive;

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
};
