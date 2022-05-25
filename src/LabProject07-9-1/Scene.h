#pragma once
#include "Shader.h"
#include "Player.h"
#include "Light.hpp"

class CScene
{
public:
    CScene();
    ~CScene();

	bool OnMouseEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnKeyboardEvent(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void BuildDefaultLightsAndMaterials();
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, GameCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	CPlayer						*m_pPlayer = NULL;

public:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	GameObject					**m_ppGameObjects = NULL;
	int							m_nGameObjects = 0;
	

	CLight						*m_pLights = NULL;
	int							m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	float						m_fElapsedTime = 0.0f;
};
