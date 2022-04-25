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
	Transform.myUp = GameTransform::Up;
}

GameCamera::~GameCamera()
{}

void GameCamera::GenerateViewMatrix()
{
	auto& myRight = Transform.myRight;
	auto& myUp = Transform.myUp;
	auto& myLook = Transform.myLook;
	auto& myPosition = Transform.myPosition;

	auto xright = XMFLOAT3(myPosition);
	auto xup = XMFLOAT3(myPosition);
	auto xlook = XMFLOAT3(myPosition);
	auto xpos = XMFLOAT3(myPosition);

	myLook = Vector3::Normalize(xlook);
	myRight = Vector3::Normalize(Vector3::CrossProduct(xup, xlook));
	//myUp = Vector3::Normalize(Vector3::CrossProduct(myLook, myRight));

	// 카메라를 위한 카메라 변환 행렬
	// 1행
	projectionView._11 = myRight.X;
	projectionView._12 = myUp.X;
	projectionView._13 = myLook.X;

	// 2행
	projectionView._21 = myRight.Y;
	projectionView._22 = myUp.Y;
	projectionView._23 = myLook.Y;

	// 3행
	projectionView._31 = myRight.Z;
	projectionView._32 = myUp.Z;
	projectionView._33 = myLook.Z;

	// 4행
	projectionView._41 = -Vector3::DotProduct(xpos, xright);
	projectionView._42 = -Vector3::DotProduct(xpos, xup);
	projectionView._43 = -Vector3::DotProduct(xpos, xlook);

	// 원근 투영 행렬
	projectionPerspective = Matrix4x4::Multiply(projectionView, m_xmf4x4PerspectiveProject);

	// 직교 투영 행렬
	projectionOrthographic = Matrix4x4::Multiply(projectionView, projectionOrthographic);

	// 카메라를 위한 월드 변환 행렬
	// 카메라를 월드 위치로 옮긴다.
	// 1행
	m_xmf4x4InverseView._11 = myRight.X;
	m_xmf4x4InverseView._12 = myRight.Y;
	m_xmf4x4InverseView._13 = myRight.Z;

	m_xmf4x4InverseView._21 = myUp.X;
	m_xmf4x4InverseView._22 = myUp.Y;
	m_xmf4x4InverseView._23 = myUp.Z;

	m_xmf4x4InverseView._31 = myLook.X;
	m_xmf4x4InverseView._32 = myLook.Y;
	m_xmf4x4InverseView._33 = myLook.Z;

	m_xmf4x4InverseView._41 = myPosition.X;
	m_xmf4x4InverseView._42 = myPosition.Y;
	m_xmf4x4InverseView._43 = myPosition.Z;

	StaticCollider.Transform(Collider, XMLoadFloat4x4(&m_xmf4x4InverseView));
}

void GameCamera::SetLookAt(const XMFLOAT3 xmf3Position, const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up)
{
	myPosition = xmf3Position;
	projectionView = Matrix4x4::LookAtLH(myPosition, xmf3LookAt, xmf3Up);

	myRight = Vector3::Normalize(XMFLOAT3(projectionView._11, projectionView._21, projectionView._31));

	myUp = Vector3::Normalize(XMFLOAT3(projectionView._12, projectionView._22, projectionView._32));

	myLook = Vector3::Normalize(XMFLOAT3(projectionView._13, projectionView._23, projectionView._33));
}

void GameCamera::SetLookAt(const XMFLOAT3 xmf3LookAt, const XMFLOAT3 xmf3Up)
{
	SetLookAt(myPosition, xmf3LookAt, xmf3Up);
	//XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(myPosition, xmf3LookAt, xmf3Up);
	//myRight = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	//myUp = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	//myLook = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
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

	BoundingFrustum::CreateFromMatrix(StaticCollider, xmmtxProjection);
}

void GameCamera::GenerateOrthographicProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fWidth, float hHeight)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(fWidth, hHeight, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&projectionOrthographic, xmmtxProjection);
}

bool GameCamera::IsInFrustum(const BoundingBox& collider) const
{
	return (Collider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingOrientedBox& collider) const
{
	return (Collider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingSphere& collider) const
{
	return (Collider.Intersects(collider));
}

void GameCamera::Translate(const XMFLOAT3& shift)
{
	myPosition = Vector3::Add(myPosition, shift);
}

void GameCamera::Translate(XMFLOAT3&& shift)
{
	myPosition = Vector3::Add(std::move(myPosition), shift);
}

void GameCamera::Move(const XMFLOAT3& dir, float distance)
{
	Translate(Vector3::ScalarProduct(Vector3::Normalize(dir), distance));
}

void GameCamera::Move(XMFLOAT3&& dir, float distance)
{
	Translate(Vector3::ScalarProduct(Vector3::Normalize(std::forward<XMFLOAT3>(dir)), distance));
}

void GameCamera::Move(float x, float y, float z)
{
	Translate(XMFLOAT3(x, y, z));
}

void GameCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&myRight), XMConvertToRadians(fPitch));
		myLook = Vector3::TransformNormal(myLook, mtxRotate);
		myUp = Vector3::TransformNormal(myUp, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&myUp), XMConvertToRadians(fYaw));
		myLook = Vector3::TransformNormal(myLook, mtxRotate);
		myRight = Vector3::TransformNormal(myRight, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&myLook), XMConvertToRadians(fRoll));
		myUp = Vector3::TransformNormal(myUp, mtxRotate);
		myRight = Vector3::TransformNormal(myRight, mtxRotate);
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
		XMFLOAT3 xmf3Direction = Vector3::Subtract(pos, myPosition);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		float fTimeLagScale = fTimeElapsed * (1.0f / 0.1f);
		float fDistance = fLength * fTimeLagScale;

		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;

		// 천천히 카메라 이동
		if (0.0f < fDistance)
		{
			myPosition = Vector3::Add(myPosition, xmf3Direction, fDistance);

			SetLookAt(XMFLOAT3(position), XMFLOAT3(up));
		}
	}
}
