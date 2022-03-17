#include "stdafx.h"
#include "Player.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	XMFLOAT3 xmf3CameraPosition;
	XMStoreFloat3(&xmf3CameraPosition, XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&m_xmf3CameraOffset)));
	m_pCamera->SetLookAt(xmf3CameraPosition, m_xmf3Position, m_xmf3Up);

	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Look), fDistance)));
		if (dwDirection & DIR_BACKWARD) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Look), -fDistance)));
		if (dwDirection & DIR_RIGHT) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Right), fDistance)));
		if (dwDirection & DIR_LEFT) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Right), -fDistance)));
		if (dwDirection & DIR_UP) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Up), fDistance)));
		if (dwDirection & DIR_DOWN) XMStoreFloat3(&xmf3Shift, XMVectorAdd(XMLoadFloat3(&xmf3Shift), XMVectorScale(XMLoadFloat3(&m_xmf3Up), -fDistance)));

		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		XMStoreFloat3(&m_xmf3Velocity, XMVectorAdd(XMLoadFloat3(&m_xmf3Velocity), XMLoadFloat3(&xmf3Shift)));
	}
	else
	{
		XMStoreFloat3(&m_xmf3Position, XMVectorAdd(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3Shift)));
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate));
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		XMStoreFloat3(&m_xmf3Look, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Look), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate));
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		XMStoreFloat3(&m_xmf3Up, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Up), xmmtxRotate));
		XMStoreFloat3(&m_xmf3Right, XMVector3TransformNormal(XMLoadFloat3(&m_xmf3Right), xmmtxRotate));
	}

	XMVECTOR xmvLook = XMVector3Normalize(XMLoadFloat3(&m_xmf3Look));
	XMVECTOR xmvUp = XMVector3Normalize(XMLoadFloat3(&m_xmf3Up));
	XMVECTOR xmvRight = XMVector3Normalize(XMVector3Cross(xmvUp, xmvLook));
	xmvUp = XMVector3Normalize(XMVector3Cross(xmvLook, xmvRight));

	XMStoreFloat3(&m_xmf3Right, xmvRight);
	XMStoreFloat3(&m_xmf3Up, xmvUp);
	XMStoreFloat3(&m_xmf3Look, xmvLook);
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(&xmf4x4View, XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Position), XMLoadFloat3(&xmf3LookAt), XMLoadFloat3(&xmf3Up)));

	XMVECTORF32 xmf32vRight = { xmf4x4View._11, xmf4x4View._21, xmf4x4View._31, 0.0f };
	XMVECTORF32 xmf32vUp = { xmf4x4View._12, xmf4x4View._22, xmf4x4View._32, 0.0f };
	XMVECTORF32 xmf32vLook = { xmf4x4View._13, xmf4x4View._23, xmf4x4View._33, 0.0f };

	XMStoreFloat3(&m_xmf3Right, XMVector3Normalize(xmf32vRight));
	XMStoreFloat3(&m_xmf3Up, XMVector3Normalize(xmf32vUp));
	XMStoreFloat3(&m_xmf3Look, XMVector3Normalize(xmf32vLook));
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMVECTOR xmvVelocity = XMLoadFloat3(&m_xmf3Velocity);
	XMVECTOR xmvDeceleration = XMVector3Normalize(XMVectorScale(xmvVelocity, -1.0f));
	float fLength = XMVectorGetX(XMVector3Length(xmvVelocity));
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	XMStoreFloat3(&m_xmf3Velocity, XMVectorAdd(xmvVelocity, XMVectorScale(xmvDeceleration, fDeceleration)));
}

void CPlayer::Animate(float fElapsedTime)
{
	OnUpdateTransform();

	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

//void CPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
//{
//	CGameObject::Render(hDCFrameBuffer, pCamera);
//}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplanePlayer::CAirplanePlayer()
{
}

CAirplanePlayer::~CAirplanePlayer()
{
}

//void CAirplanePlayer::Animate(float fElapsedTime)
//{
//	CPlayer::Animate(fElapsedTime);
//}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMLoadFloat4x4(&m_xmf4x4World)));
}

//void CAirplanePlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
//{
//	CPlayer::Render(hDCFrameBuffer, pCamera);
//}

