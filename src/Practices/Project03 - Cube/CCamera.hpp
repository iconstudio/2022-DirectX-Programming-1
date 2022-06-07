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
	

	// 카메라 변환 행렬
	XMFLOAT4X4 m_xmf4x4View;

	// 투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;

	// 뷰포트와 씨저 사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;
};

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
}
