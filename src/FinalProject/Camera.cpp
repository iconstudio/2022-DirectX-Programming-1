#include "pch.hpp"
#include "Player.h"
#include "Camera.h"
#include "Arithmetics.hpp"

GameCamera::GameCamera()
	: localTransform(), worldTransform()
	, localMatrix(localTransform.GetMatrix())
	, worldMatrix(worldTransform.GetMatrix())
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_nMode = 0x00;
	myPlayer = NULL;
}

GameCamera::~GameCamera()
{}

void GameCamera::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	UINT ncbElementBytes = ((sizeof(CBufferCamera) + 255) & ~255);

	m_pd3dcbCamera = CreateBufferResource(device, cmdlist
		, NULL, ncbElementBytes
		, D3D12_HEAP_TYPE_UPLOAD
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, NULL);

	m_pd3dcbCamera->Map(0, NULL, (void**)&m_pcbMappedCamera);
}

void GameCamera::UpdateOffset(const XMFLOAT3 addition)
{
	addtionalPositon = addition;
}

void GameCamera::Update(const XMFLOAT3& xmf3LookAt, float delta_time)
{}

void GameCamera::PrepareRendering(P3DGrpCommandList cmdlist)
{
	cmdlist->RSSetViewports(1, &m_d3dViewport);
	cmdlist->RSSetScissorRects(1, &m_d3dScissorRect);

	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4View, &xmf4x4View, sizeof(XMFLOAT4X4));

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	::memcpy(&m_pcbMappedCamera->m_xmf4x4Projection, &xmf4x4Projection, sizeof(XMFLOAT4X4));

	::memcpy(&m_pcbMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbCamera->GetGPUVirtualAddress();
	cmdlist->SetGraphicsRootConstantBufferView(0, d3dGpuVirtualAddress);
}


void GameCamera::Release()
{
	if (m_pd3dcbCamera)
	{
		m_pd3dcbCamera->Unmap(0, NULL);
		m_pd3dcbCamera->Release();
	}
}

void GameCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}

void GameCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void GameCamera::Rotate(float pitch, float yaw, float roll)
{}

void GameCamera::SetLookAt(const XMFLOAT3& look)
{}

void GameCamera::BuildProjectionMatrix(float znear, float zfar, float aspect, float degfov)
{
	const auto fov = XMConvertToRadians(degfov);
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(fov, aspect, znear, zfar);
}

void GameCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void GameCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void GameCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSpaceShipCamera

CSpaceShipCamera::CSpaceShipCamera()
	: GameCamera()
{}

CSpaceShipCamera::CSpaceShipCamera(const GameCamera& pCamera)
	: GameCamera(pCamera)
{
	m_nMode = SPACESHIP_CAMERA;
}

CSpaceShipCamera::~CSpaceShipCamera() {}

void CSpaceShipCamera::Rotate(float x, float y, float z)
{
	if (myPlayer && (x != 0.0f))
	{
		XMFLOAT3 xmf3Right = myPlayer->GetRight();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right), XMConvertToRadians(x));

		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, myPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, myPlayer->GetPosition());
	}

	if (myPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = myPlayer->GetUp();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(y));

		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, myPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, myPlayer->GetPosition());
	}

	if (myPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = myPlayer->GetLook();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(z));
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, myPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, myPlayer->GetPosition());
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFirstPersonCamera

CFirstPersonCamera::CFirstPersonCamera()
	: GameCamera()
{}

CFirstPersonCamera::CFirstPersonCamera(const GameCamera& pCamera)
	: GameCamera(pCamera)
{
	m_nMode = FIRST_PERSON_CAMERA;
	
		if (pCamera.GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y = 0.0f;
			m_xmf3Look.y = 0.0f;
			m_xmf3Right = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		}
}

CFirstPersonCamera::~CFirstPersonCamera() {}

void CFirstPersonCamera::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (myPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = myPlayer->GetUp();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up), XMConvertToRadians(y));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	if (myPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = myPlayer->GetLook();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look), XMConvertToRadians(z));
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, myPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, myPlayer->GetPosition());
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CThirdPersonCamera

CThirdPersonCamera::CThirdPersonCamera()
	: GameCamera()
{}

CThirdPersonCamera::CThirdPersonCamera(const GameCamera& pCamera)
	: GameCamera(pCamera)
{
	m_nMode = THIRD_PERSON_CAMERA;

	if (pCamera.GetMode() == SPACESHIP_CAMERA)
	{
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3Right.y = 0.0f;
		m_xmf3Look.y = 0.0f;
		m_xmf3Right = Vector3::Normalize(m_xmf3Right);
		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	}

}

CThirdPersonCamera::~CThirdPersonCamera() {}

void CThirdPersonCamera::Update(const XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	if (myPlayer)
	{
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		XMFLOAT3 xmf3Right = myPlayer->GetRight();
		XMFLOAT3 xmf3Up = myPlayer->GetUp();
		XMFLOAT3 xmf3Look = myPlayer->GetLook();

		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 = xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 = xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 = xmf3Look.z;

		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);
		XMFLOAT3 xmf3Position = Vector3::Add(myPlayer->GetPosition(), xmf3Offset);
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;

		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			SetLookAt(xmf3LookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(const XMFLOAT3& xmf3LookAt)
{
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, myPlayer->GetUp());

	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
