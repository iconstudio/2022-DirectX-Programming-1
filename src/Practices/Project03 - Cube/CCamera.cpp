#include "pch.h"
#include "CCamera.hpp"
#include "Arithmetics.hpp"

CCamera::CCamera()
	: m_xmf4x4View(Matrix4x4::Identity()), m_xmf4x4Projection(Matrix4x4::Identity())
{
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
}

void CCamera::SetViewport(int left, int top, int w, int h, float
	zmin, float zmax)
{
	m_d3dViewport.TopLeftX = float(left);
	m_d3dViewport.TopLeftY = float(top);
	m_d3dViewport.Width = float(w);
	m_d3dViewport.Height = float(h);
	m_d3dViewport.MinDepth = zmin;
	m_d3dViewport.MaxDepth = zmax;
}

void CCamera::SetScissorRect(LONG left, LONG top, LONG right, LONG bottom)
{
	m_d3dScissorRect.left = left;
	m_d3dScissorRect.top = top;
	m_d3dScissorRect.right = right;
	m_d3dScissorRect.bottom = bottom;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookat, XMFLOAT3
	xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(pos, lookat, xmf3Up);
}

void CCamera::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList)
{}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* cmdlist)
{
	XMFLOAT4X4 xmf4x4View{};

	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));

	//루트 파라메터 인덱스 1의
	cmdlist->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection{};
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));

	cmdlist->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* cmdlist)
{
	cmdlist->RSSetViewports(1, &m_d3dViewport);
	cmdlist->RSSetScissorRects(1, &m_d3dScissorRect);
}

void CCamera::ReleaseShaderVariables()
{}
