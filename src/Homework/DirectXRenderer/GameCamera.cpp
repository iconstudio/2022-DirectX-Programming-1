#include "pch.hpp"
#include "GameCamera.hpp"
#include "Transformer.hpp"
#include "GameViewport.hpp"
#include "GameTransform.hpp"
#include "GameEntity.hpp"
#include "Mesh.hpp"
#include "Arithmetics.hpp"

const auto identity = Matrix4x4::Identity();

GameCamera::GameCamera()
	: Transform(), localPosition()
	, myRoll(0.0f), myYaw(0.0f), myPitch(0.0f)
	, m_Viewport(), myFarPlane()
	, projectionView(identity), m_xmf4x4InverseView(identity)
	, m_xmf4x4PerspectiveProject(identity), projectionPerspective(identity)
	, m_xmf4x4ViewOrthographicProject(identity), projectionOrthographic(identity)
	, staticCollider(), myCollider()
{
	Transform.myRight = GameTransform::Right;
	Transform.myUp = GameTransform::Up;
	Transform.myLook = GameTransform::Forward;
}

GameCamera::~GameCamera()
{}

void GameCamera::SetViewport(int left, int top, int w, int h)
{
	m_Viewport.Set(left, top, w, h);
	m_fAspectRatio = float(w) / float(h);

	Transformer::SetViewport(m_Viewport);
}

void GameCamera::SetFOVAngle(float angle)
{
	m_fFOVAngle = angle;
	m_fProjectRectDistance = float(1.0f / tan(DegreeToRadian(angle * 0.5f)));
}

void GameCamera::GeneratePerspectiveProjectionMatrix(float znear, float zfar, float fov)
{
	float ratio = (float(m_Viewport.m_nWidth) / float(m_Viewport.m_nHeight));

	myFarPlane = zfar;

	const auto rad = XMConvertToRadians(fov);
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(rad, ratio, znear, zfar);

	XMStoreFloat4x4(&m_xmf4x4PerspectiveProject, xmmtxProjection);

	BoundingFrustum::CreateFromMatrix(staticCollider, xmmtxProjection);
}

void GameCamera::GenerateOrthographicProjectionMatrix(float znear, float zfar, float w, float h)
{
	XMMATRIX xmmtxProjection = XMMatrixOrthographicLH(w, h, znear, zfar);
	XMStoreFloat4x4(&m_xmf4x4ViewOrthographicProject, xmmtxProjection);
}

void GameCamera::SetLocalPosition(const XMFLOAT3& pos)
{
	SetLocalPosition(std::move(XMFLOAT3(pos)));
}

void GameCamera::SetLocalPosition(XMFLOAT3&& pos)
{
	localPosition = std::forward<XMFLOAT3>(pos);
}

void GameCamera::SetRotation(const XMFLOAT4X4& tfrm)
{
	Transform.SetRotation(tfrm);
}

void GameCamera::SetRotation(XMFLOAT4X4&& tfrm)
{
	Transform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));
}

void GameCamera::GenerateViewMatrix()
{
	auto& myRight = Transform.GetRight();
	auto& myUp = Transform.GetUp();
	auto& myLook = Transform.GetLook();
	auto myPosition = XMFLOAT3(Transform.GetPosition());

	Transform.myRight = Vector3::Normalize(XMFLOAT3(myRight));
	Transform.myUp = Vector3::Normalize(XMFLOAT3(myUp));
	Transform.myLook = Vector3::Normalize(XMFLOAT3(myLook));

	// ī�޶� ���� ī�޶� ��ȯ ���
	// 1��
	projectionView._11 = myRight.x;
	projectionView._12 = myUp.x;
	projectionView._13 = myLook.x;

	// 2��
	projectionView._21 = myRight.y;
	projectionView._22 = myUp.y;
	projectionView._23 = myLook.y;

	// 3��
	projectionView._31 = myRight.z;
	projectionView._32 = myUp.z;
	projectionView._33 = myLook.z;

	// 4��
	projectionView._41 = -Vector3::DotProduct(myPosition, XMFLOAT3(myRight));
	projectionView._42 = -Vector3::DotProduct(myPosition, XMFLOAT3(myUp));
	projectionView._43 = -Vector3::DotProduct(myPosition, XMFLOAT3(myLook));

	// ���� ���� ���
	projectionPerspective = Matrix4x4::Multiply(projectionView, m_xmf4x4PerspectiveProject);

	// ���� ���� ���
	projectionOrthographic = Matrix4x4::Multiply(projectionView, m_xmf4x4ViewOrthographicProject);

	// ī�޶� ���� ���� ��ȯ ���
	// �浹ü�� ���� ��ġ�� �ű��.
	const auto& world_mat = Transform.GetWorldMatrix();

	staticCollider.Transform(myCollider, XMLoadFloat4x4(&world_mat));
}

void GameCamera::Update(const XMFLOAT3& look_at, const GameTransform& follower, const XMFLOAT4X4& rotation, float time_elapsed)
{
	const auto& fwlWorld = follower.GetWorldMatrix();
	const auto fwlRight = XMFLOAT3(follower.myRight);
	const auto fwlUp = XMFLOAT3(follower.myUp);
	const auto fwlLook = XMFLOAT3(follower.myLook);
	const auto fwlPosition = XMFLOAT3(follower.myPosition);

	// �ȷο� �������� ȸ���� ��Ű�� ���
	XMFLOAT4X4 fwlRot = fwlWorld;
	fwlRot._41 = 0;
	fwlRot._42 = 0;
	fwlRot._43 = 0;

	const auto&& myPosition = XMFLOAT3(Transform.GetPosition());

	// �ȷο� �������� ��ȯ�� ���� ��ǥ
	const auto&& offset_from = Vector3::TransformCoord(localPosition, fwlRot);

	// �ȷο� �������� ������ ���� ��ǥ
	const XMFLOAT3 look_from = Vector3::Add(fwlPosition, offset_from);

	// ���� ī�޶� ��ġ���� ���� ��ǥ�� ���ϴ� ����
	const XMFLOAT3 move_vector = Vector3::Subtract(look_from, myPosition);

		float move_far = Vector3::Length(move_vector);
	if (0 < move_far)
	{
		float time_lag_scale = time_elapsed * (1.0f / 0.1f);
		float move_distane = move_far * time_lag_scale;

		if (move_distane < EPSILON)
			move_distane = 0.0f;
		else if (move_far < move_distane)
			move_distane = move_far;

		// õõ�� ī�޶� �̵�
		if (0.0f < move_distane)
		{
			Transform.Translate(Vector3::ScalarProduct(Vector3::Normalize(move_vector), move_distane));
		}
	}
		
	//LookAt(look_at, fwlUp);
	SetRotation(rotation);

}

void GameCamera::LookAt(const XMFLOAT3 pos, const XMFLOAT3 look, const XMFLOAT3 up)
{
	Transform.SetPosition(pos);
	Transform.LookAt(look, up);
}

void GameCamera::LookAt(const XMFLOAT3 look, const XMFLOAT3 up)
{
	Transform.LookAt(look, up);
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

void GameCamera::Rotate(const XMFLOAT3& axis, float angle)
{
	Transform.Rotate(axis, angle);
}

void GameCamera::Rotate(float pitch, float yaw, float roll)
{
	Transform.Rotate(pitch, yaw, roll);
}

bool GameCamera::IsInFrustum(const BoundingBox& collider) const
{
	return (myCollider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingOrientedBox& collider) const
{
	return (myCollider.Intersects(collider));
}

bool GameCamera::IsInFrustum(const BoundingSphere& collider) const
{
	return (myCollider.Intersects(collider));
}
