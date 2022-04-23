#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GameTransform.hpp"
#include "GameObject.hpp"
#include "Mesh.hpp"

void GameViewport::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_nLeft = nLeft;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

GameCamera::GameCamera()
	: Transform(), Follower(nullptr)
{
	m_xmf3Up = GameTransform::Up;
}

GameCamera::~GameCamera()
{}

void GameCamera::GenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	//m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));

	// 카메라를 위한 카메라 변환 행렬
	// 1행
	m_xmf4x4View._11 = m_xmf3Right.x;
	m_xmf4x4View._12 = m_xmf3Up.x;
	m_xmf4x4View._13 = m_xmf3Look.x;

	// 2행
	m_xmf4x4View._21 = m_xmf3Right.y;
	m_xmf4x4View._22 = m_xmf3Up.y;
	m_xmf4x4View._23 = m_xmf3Look.y;

	// 3행
	m_xmf4x4View._31 = m_xmf3Right.z;
	m_xmf4x4View._32 = m_xmf3Up.z;
	m_xmf4x4View._33 = m_xmf3Look.z;

	// 4행
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);

	// 원근 투영 행렬
	m_xmf4x4ViewPerspectiveProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4PerspectiveProject);

	// 직교 투영 행렬
	m_xmf4x4OrthographicProject = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4OrthographicProject);

	// 카메라를 위한 월드 변환 행렬
	// 카메라를 월드 위치로 옮긴다.
	// 1행
	m_xmf4x4InverseView._11 = m_xmf3Right.x;
	m_xmf4x4InverseView._12 = m_xmf3Right.y;
	m_xmf4x4InverseView._13 = m_xmf3Right.z;

	m_xmf4x4InverseView._21 = m_xmf3Up.x;
	m_xmf4x4InverseView._22 = m_xmf3Up.y;
	m_xmf4x4InverseView._23 = m_xmf3Up.z;

	m_xmf4x4InverseView._31 = m_xmf3Look.x;
	m_xmf4x4InverseView._32 = m_xmf3Look.y;
	m_xmf4x4InverseView._33 = m_xmf3Look.z;

	m_xmf4x4InverseView._41 = m_xmf3Position.x;
	m_xmf4x4InverseView._42 = m_xmf3Position.y;
	m_xmf4x4InverseView._43 = m_xmf3Position.z;

	m_xmFrustumView.Transform(m_xmFrustumWorld, XMLoadFloat4x4(&m_xmf4x4InverseView));
}

void GameCamera::SetLookAt(const XMFLOAT3 xmf3Position, const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);

	m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31));

	m_xmf3Up = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32));

	m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33));
}

void GameCamera::SetLookAt(const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up)
{
	SetLookAt(m_xmf3Position, xmf3LookAt, xmf3Up);
	//XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	//m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	//m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	//m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void GameCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight);
}

void GameCamera::SetFollower(GameObject* target)
{
	Follower = target;
}

void GameCamera::SetFOVAngle(float fFOVAngle)
{
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(fFOVAngle * 0.5f)));
}

void GameCamera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFOVAngle)
{
	float fAspectRatio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, xmmtxProjection);

	BoundingFrustum::CreateFromMatrix(m_xmFrustumView, xmmtxProjection);
}

void GameCamera::GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(fWidth, hHeight, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4OrthographicProject, xmmtxProjection);
}

bool GameCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
	return(m_xmFrustumWorld.Intersects(xmBoundingBox));
}

void GameCamera::Move(const XMFLOAT3& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
}

void GameCamera::Move(XMFLOAT3&& xmf3Shift)
{
	m_xmf3Position = Vector3::Add(std::move(m_xmf3Position), xmf3Shift);
}

void GameCamera::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z));
}

void GameCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
}

void GameCamera::Update(const XMFLOAT3& offset, float fTimeElapsed)
{
	if (Follower)
	{
		auto& transform = Follower->Transform;
		const auto& position = transform.GetPosition();
		const auto& right = transform.GetRight();
		const auto& up = transform.GetUp();
		const auto& look = transform.GetLook();

		XMFLOAT4X4 mtxRotate = Matrix4x4::Identity();
		mtxRotate._11 = right.X;
		mtxRotate._21 = up.X;
		mtxRotate._31 = look.X;

		mtxRotate._12 = right.Y;
		mtxRotate._22 = up.Y;
		mtxRotate._32 = look.Y;

		mtxRotate._13 = right.Z;
		mtxRotate._23 = up.Z;
		mtxRotate._33 = look.Z;

		// 팔로워 기준으로 변환된 유격 좌표
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(offset, mtxRotate);

		// 팔로워 기준으로 세워진 고정 좌표
		XMFLOAT3 pos = Vector3::Add(XMFLOAT3(position), xmf3Offset);
		
		// 현재 카메라 위치에서 고정 좌표로 향하는 벡터
		XMFLOAT3 xmf3Direction = Vector3::Subtract(pos, m_xmf3Position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		float fTimeLagScale = fTimeElapsed * (1.0f / 0.25f);
		float fDistance = fLength * fTimeLagScale;

		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;

		// 천천히 카메라 이동
		if (0.0f < fDistance)
		{
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);

			SetLookAt(XMFLOAT3(position), XMFLOAT3(up));
		}
	}
}
