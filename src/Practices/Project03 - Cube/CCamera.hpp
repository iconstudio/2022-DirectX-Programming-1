#pragma once
#include "GameObject.hpp"

class CCamera
{
public:
	CCamera();
	virtual ~CCamera();

	void SetViewport(int left, int top, int w, int h, float zmin, float zmax);
	void SetScissorRect(LONG left, LONG top, LONG right, LONG bottom);

	void GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookat, XMFLOAT3 up);
	void GenerateProjectionMatrix(float znear, float zfar, float
		aspect, float fov);

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList
		* cmdlist);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* cmdlist);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdlist);
	virtual void ReleaseShaderVariables();

protected:
	

	// ī�޶� ��ȯ ���
	XMFLOAT4X4 m_xmf4x4View;

	// ���� ��ȯ ���
	XMFLOAT4X4 m_xmf4x4Projection;

	// ����Ʈ�� ���� �簢��
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
};

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
}
