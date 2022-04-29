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
	: Transform()
	, Follower(nullptr)
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

void GameCamera::SetLookAt(const XMFLOAT3 pos, const XMFLOAT3 look, const XMFLOAT3 up)
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

void GameCamera::SetLookAt(const XMFLOAT3 look, const XMFLOAT3 xmf3Up)
{
	Transform.LookAt(look, xmf3Up);
	projectionView = Transform.GetWorldMatrix();
	//SetLookAt(myPosition, look, xmf3Up);
	//XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(myPosition, look, xmf3Up);
	//myRight = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	//myUp = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	//myLook = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void GameCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight)
{
	m_Viewport.Set(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight);

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

void GameCamera::GeneratePerspectiveProjectionMatrix(float znear, float zfar, float fov)
{
	float fAspectRatio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));

	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), fAspectRatio, znear, zfar);

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

void GameCamera::Update(const XMFLOAT3& offset, float fTimeElapsed)
{
	if (Follower)
	{
		auto& transform = Follower->Transform;
		auto& fwlWorld = transform.GetWorldMatrix();
		auto& fwlRight = transform.myRight;
		auto& fwlUp = transform.myUp;
		auto& fwlLook = transform.myLook;
		auto& fwlPosition = transform.myPosition;

		auto xright = XMFLOAT3(fwlRight);
		auto xup = XMFLOAT3(fwlUp);
		auto xlook = XMFLOAT3(fwlLook);
		auto xpos = XMFLOAT3(fwlPosition);

		// 팔로워 기준으로 회전만 시키는 행렬
		auto&& mtxRotate = Matrix4x4::Identity();
		mtxRotate._11 = fwlRight.X; // 1열
		mtxRotate._12 = fwlRight.Y;
		mtxRotate._13 = fwlRight.Z;

		mtxRotate._21 = fwlUp.X; // 2열
		mtxRotate._22 = fwlUp.Y;
		mtxRotate._23 = fwlUp.Z;

		mtxRotate._31 = fwlLook.X; // 3열
		mtxRotate._32 = fwlLook.Y;
		mtxRotate._33 = fwlLook.Z;

		// 팔로워 기준으로 변환된 유격 좌표
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(offset, mtxRotate);

		// 팔로워 기준으로 세워진 고정 좌표
		XMFLOAT3 follow_pos = Vector3::Add(xpos, xmf3Offset);
		
		auto myPosition = XMFLOAT3(Transform.GetPosition());

		// 현재 카메라 위치에서 고정 좌표로 향하는 벡터
		auto&& move_vector = Vector3::Subtract(follow_pos, myPosition);
		float move_far = Vector3::Length(move_vector);

		float fTimeLagScale = fTimeElapsed * (1.0f / 0.1f);
		float move_distane = move_far * fTimeLagScale;

		if (move_far < move_distane) move_distane = move_far;
		if (move_far < 0.01f) move_distane = move_far;

		// 천천히 카메라 이동
		if (0.0f < move_distane)
		{
			Transform.Translate(Vector3::ScalarProduct(Vector3::Normalize(move_vector), move_distane));

			SetLookAt(xpos, xup);
		}
	}
}
