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

class RawMaterialsBox
{
public:
	int m_nMaterials = 0;
	RawMaterial* m_pMaterials = NULL;
};

class CMaterialColors
{
public:
	CMaterialColors() {}
	CMaterialColors(RawMaterial* pMaterialInfo);
	virtual ~CMaterialColors() {}

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4 m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	XMFLOAT4 m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); //(r,g,b,a=power)
	XMFLOAT4 m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	Pipeline* m_pShader = NULL;

	CMaterialColors* m_pMaterialColors = NULL;

	void SetMaterialColors(CMaterialColors* pMaterialColors);
	void SetShader(Pipeline* pShader);

	void UpdateUniforms(P3DGrpCommandList cmdlist);
};
