#pragma once
#include "Scene.hpp"
#include "Light.hpp"
#include "Vertex.hpp"

class IlluminatedScene : public Scene
{
public:
	IlluminatedScene(Framework& framework, const char* name, P3DDevice device, P3DGrpCommandList cmdlist);

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
	virtual	void PrepareRendering(P3DGrpCommandList cmdlist) const;
	virtual void Render(P3DGrpCommandList cmdlist) const;
	virtual void Release();

	P3DDevice dxDevice;
	P3DGrpCommandList dxTaskList;

	//DiffusedVertex myTestVertices[24];

	CLight* myLights = NULL;
	int numberLights = 0;

	XMFLOAT4 m_xmf4GlobalAmbient;

	ID3D12Resource* m_pd3dcbLights = NULL;
	StaticLights* myStaticLights = NULL;
};

