#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GamePipeline.hpp"
#include "GameTransform.hpp"
#include "GameObject.hpp"
#include "Mesh.hpp"

void GameViewport::Set(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_nLeft = nLeft;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

GameCamera::GameCamera()
	: Transform(), localPosition(), lookOffset()
	, Follower(nullptr)
	, m_Viewport()
{
	Transform.myRight = GameTransform::Right;
	Transform.myUp = GameTransform::Up;
	Transform.myLook = GameTransform::Forward;
}

GameCamera::~GameCamera()
{}

void GameCamera::GenerateViewMatrix()
{
	auto&& myRight = XMFLOAT3(Transform.GetRight());
	auto&& myUp = XMFLOAT3(Transform.GetUp());
	auto&& myLook = XMFLOAT3(Transform.GetLook());

	//myLook = Vector3::Normalize(myLook);
	//myRight = Vector3::Normalize(Vector3::CrossProduct(myUp, myLook));
	//myUp = Vector3::Normalize(Vector3::CrossProduct(myLook, myRight));
	Transform.myRight = Vector3::Normalize(myRight);
	Transform.myUp = Vector3::Normalize(myUp);
	Transform.myLook = Vector3::Normalize(myLook);

	auto myPosition = XMFLOAT3(Transform.GetPosition());

	// 카메라를 위한 카메라 변환 행렬
	// 1행
	projectionView._11 = myRight.x;
	projectionView._12 = myUp.x;
	projectionView._13 = myLook.x;

	// 2행
	projectionView._21 = myRight.y;
	projectionView._22 = myUp.y;
	projectionView._23 = myLook.y;

	// 3행
	projectionView._31 = myRight.z;
	projectionView._32 = myUp.z;
	projectionView._33 = myLook.z;

	// 4행
	projectionView._41 = -Vector3::DotProduct(myPosition, myRight);
	projectionView._42 = -Vector3::DotProduct(myPosition, myUp);
	projectionView._43 = -Vector3::DotProduct(myPosition, myLook);

	// 원근 투영 행렬
	projectionPerspective = Matrix4x4::Multiply(projectionView, m_xmf4x4PerspectiveProject);

	// 직교 투영 행렬
	projectionOrthographic = Matrix4x4::Multiply(projectionView, m_xmf4x4ViewOrthographicProject);

	// 카메라를 위한 월드 변환 행렬
	// 카메라를 월드 위치로 옮긴다.
	// 1행
	m_xmf4x4InverseView._11 = myRight.x;
	m_xmf4x4InverseView._12 = myRight.y;
	m_xmf4x4InverseView._13 = myRight.z;

	m_xmf4x4InverseView._21 = myUp.x;
	m_xmf4x4InverseView._22 = myUp.y;
	m_xmf4x4InverseView._23 = myUp.z;

	m_xmf4x4InverseView._31 = myLook.x;
	m_xmf4x4InverseView._32 = myLook.y;
	m_xmf4x4InverseView._33 = myLook.z;

	m_xmf4x4InverseView._41 = myPosition.x;
	m_xmf4x4InverseView._42 = myPosition.y;
	m_xmf4x4InverseView._43 = myPosition.z;

	StaticCollider.Transform(Collider, XMLoadFloat4x4(&m_xmf4x4InverseView));
}

void GameCamera::LookAt(const XMFLOAT3 pos, const XMFLOAT3 look, const XMFLOAT3 up)
{
	Transform.SetPosition(pos);
	Transform.LookAt(look, up);
	projectionView = Transform.GetWorldMatrix();

	//myPosition = pos;
	//projectionView = Matrix4x4::LookAtLH(myPosition, look, up);

	//myRight = Vector3::Normalize(XMFLOAT3(projectionView._11, projectionView._21, projectionView._31));

	//myUp = Vector3::Normalize(XMFLOAT3(projectionView._12, projectionView._22, projectionView._32));

	//myLook = Vector3::Normalize(XMFLOAT3(projectionView._13, projectionView._23, projectionView._33));
}

void GameCamera::LookAt(const XMFLOAT3 look, const XMFLOAT3 up)
{
	Transform.LookAt(look, up);
	projectionView = Transform.GetWorldMatrix();
	//LookAt(myPosition, look, xmf3Up);
	//XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(myPosition, look, xmf3Up);
	//myRight = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	//myUp = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	//myLook = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void GameCamera::SetViewport(int left, int top, int w, int h)
{
	m_Viewport.Set(left, top, w, h);
	m_fAspectRatio = float(w) / float(h);

	GamePipeline::SetViewport(m_Viewport);
}

void GameCamera::SetFollower(GameObject* target)
{
	Follower = target;
}

void GameCamera::SetFOVAngle(float angle)
{
	m_fFOVAngle = angle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(angle * 0.5f)));
}

void GameCamera::SetLocalPosition(const XMFLOAT3& pos)
{
	SetLocalPosition(std::move(XMFLOAT3(pos)));
}

void GameCamera::SetLocalPosition(XMFLOAT3 && pos)
{
	localPosition = std::forward<XMFLOAT3>(pos);
}

void GameCamera::SetLookOffset(const XMFLOAT3& vector)
{
	SetLookOffset(std::move(XMFLOAT3(vector)));
}

void GameCamera::SetLookOffset(XMFLOAT3&& vector)
{
	lookOffset = std::forward<XMFLOAT3>(vector);
}

void GameCamera::GeneratePerspectiveProjectionMatrix(float znear, float zfar, float fov)
{
	float ratio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));

	const auto rad = XMConvertToRadians(fov);
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(rad, ratio, znear, zfar);

	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, xmmtxProjection);

	BoundingFrustum::CreateFromMatrix(StaticCollider, xmmtxProjection);
}

void GameCamera::GenerateOrthographicProjectionMatrix(float znear, float zfar, float w, float h)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(w, h, znear, zfar);
	XMStoreFloat4x4(&m_xmf4x4ViewOrthographicProject, xmmtxProjection);
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

void GameCamera::Translate(float x, float y, float z)
{
	Transform.Translate(x, y, z);
}

void GameCamera::Translate(const XMFLOAT3& shift)
{
	Transform.Translate(shift);
}

void GameCamera::Translate(XMFLOAT3&& shift)
{
	Transform.Translate(std::forward<XMFLOAT3>(shift));
}

void GameCamera::Move(const XMFLOAT3& dir, float distance)
{
	Transform.Move(dir, distance);
}

void GameCamera::Move(XMFLOAT3&& dir, float distance)
{
	Transform.Move(std::forward<XMFLOAT3>(dir), distance);
}

void GameCamera::Rotate(float pitch, float yaw, float roll)
{
	Transform.Rotate(pitch, yaw, roll);
	/*
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
	}*/
}

void GameCamera::Update(float fTimeElapsed)
{
	if (Follower)
	{
		auto& transform = Follower->Transform;

		const auto& fwlWorld = transform.GetWorldMatrix();
		const auto fwlRight = XMFLOAT3(transform.myRight);
		const auto fwlUp = XMFLOAT3(transform.myUp);
		const auto fwlLook = XMFLOAT3(transform.myLook);
		const auto fwlPosition = XMFLOAT3(transform.myPosition);

		// 팔로워 기준으로 회전만 시키는 행렬
		auto fwlRot = Matrix4x4::Identity();
		// 1열
		fwlRot._11 = fwlRight.x; fwlRot._12 = fwlRight.y; fwlRot._13 = fwlRight.z;
		// 2열
		fwlRot._21 = fwlUp.x; fwlRot._22 = fwlUp.y; fwlRot._23 = fwlUp.z;
		// 3열
		fwlRot._31 = fwlLook.x; fwlRot._32 = fwlLook.y; fwlRot._33 = fwlLook.z;

		const auto myPosition = XMFLOAT3(Transform.GetPosition());

		// 팔로워 기준으로 변환된 유격 좌표
		const auto&& offset_from = Vector3::TransformCoord(localPosition, fwlRot);

		// 팔로워 기준으로 세워진 고정 좌표
		const auto look_from = Vector3::Add(fwlPosition, offset_from);

		// 팔로워 기준으로 변환된 바라보는 유격 좌표
		const auto&& offset_at = Vector3::TransformCoord(lookOffset, fwlRot);

		// 팔로워 기준으로 변환된 바라보는 좌표
		const auto look_at = Vector3::Add(fwlPosition, offset_at);

		// 현재 카메라 위치에서 고정 좌표로 향하는 벡터
		const auto move_vector = Vector3::Subtract(look_from, myPosition);
		float move_far = Vector3::Length(move_vector);

		float fTimeLagScale = fTimeElapsed * (1.0f / 0.1f);
		float move_distane = move_far * fTimeLagScale;

		if (move_far < move_distane) move_distane = move_far;
		if (move_far < 0.01f) move_distane = move_far;

		// 천천히 카메라 이동
		if (0.0f < move_distane)
		{
			Transform.Translate(Vector3::ScalarProduct(Vector3::Normalize(move_vector), move_distane));

			LookAt(look_at, fwlUp);
		}
	}
}
