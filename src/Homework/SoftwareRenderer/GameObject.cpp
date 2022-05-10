#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameScene.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"
#include "GameTransform.hpp"
#include "GameMesh.hpp"
#include "Fragment.hpp"
#include "Mesh.hpp"

GameObject::GameObject(GameScene& scene)
	: Scene(scene), Camera(nullptr)
	, isActivated(true), isStatic(false)
	, myMesh(), Transform(), Collider()
	, Direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), Speed(0.0f), Friction(0.0f)
{}

GameObject::GameObject(GameScene& scene, float x, float y, float z)
	: GameObject(scene)
{
	SetPosition(x, y, z);
	UpdateBoundingBox();
}

GameObject::GameObject(GameScene& scene, const XMFLOAT3& position)
	: GameObject(scene)
{
	SetPosition(position);
	UpdateBoundingBox();
}

GameObject::GameObject(GameScene& scene, XMFLOAT3&& position)
	: GameObject(scene)
{
	SetPosition(position);
	UpdateBoundingBox();
}

GameObject::~GameObject()
{}

void GameObject::SetActive(bool flag)
{
	isActivated = flag;
}

void GameObject::SetStatic(bool flag)
{
	isStatic = flag;
}

void GameObject::SetMesh(const shared_ptr<CMesh>& mesh)
{
	myMesh.SetMesh(mesh);
}

void GameObject::SetColor(COLORREF color)
{
	myMesh.SetColor(color);
}

void GameObject::SetCamera(shared_ptr<GameCamera>& cam)
{
	Camera = cam;
}

void GameObject::SetPosition(float x, float y, float z)
{
	Transform.SetPosition(x, y, z);
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	SetPosition(std::move(XMFLOAT3(pos)));
}

void GameObject::SetPosition(XMFLOAT3&& pos)
{
	Transform.SetPosition(std::forward<XMFLOAT3>(pos));
}

void GameObject::SetRotation(const XMFLOAT4X4& tfrm)
{
	Transform.SetRotation(tfrm);
}

void GameObject::AddPosition(XMFLOAT3&& vector)
{
	Transform.Translate(vector);
}

bool GameObject::IsActivated() const
{
	return isActivated;
}

bool GameObject::IsStatic() const
{
	return isStatic;
}

XMFLOAT3&& GameObject::GetPosition() const
{
	return std::move(XMFLOAT3(Transform.GetPosition()));
}

XMFLOAT3&& GameObject::GetRight() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetRight()));
}

void GameObject::Destroy()
{}

void GameObject::Activate()
{
	SetActive(true);
}

void GameObject::Diactivate()
{
	SetActive(false);
}

XMFLOAT3&& GameObject::GetUp() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetUp()));
}

XMFLOAT3&& GameObject::GetLook() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetLook()));
}

void GameObject::Move(const XMFLOAT3& dir, float distance)
{
	Transform.Move(dir, distance);
}

void GameObject::MoveStrafe(float distance)
{
	Transform.MoveStrafe(distance);
}

void GameObject::MoveUp(float distance)
{
	Transform.MoveUp(distance);
}

void GameObject::MoveForward(float distance)
{
	Transform.MoveForward(distance);
}

void GameObject::Rotate(float pitch, float yaw, float roll)
{
	Transform.Rotate(pitch, yaw, roll);
}

void GameObject::Rotate(const XMFLOAT3& axis, float angle)
{
	Transform.Rotate(axis, angle);
}

void GameObject::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	Transform.LookTo(to, up);
}

void GameObject::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	Transform.LookAt(from, up);
}

void GameObject::SetVelocity(const XMFLOAT3& vector)
{
	SetVelocity(std::move(XMFLOAT3(vector)));
}

void GameObject::SetVelocity(XMFLOAT3&& vector)
{
	auto&& vel = std::forward<XMFLOAT3>(vector);
	Direction = Vector3::Normalize(vel);
	Speed = Vector3::Length(vel);
}

void GameObject::SetDirection(const XMFLOAT3& direction)
{
	SetDirection(std::move(XMFLOAT3(direction)));
}

void GameObject::SetDirection(XMFLOAT3&& direction)
{
	Direction = Vector3::Normalize(std::forward<XMFLOAT3>(direction));
}

void GameObject::SetSpeed(const float value)
{
	Speed = value;
}

void GameObject::AddSpeed(const float value, const float max)
{
	auto speed = GetSpeed() + value;
	if (max < speed) speed = max;

	SetSpeed(speed);
}

void GameObject::AddSpeed(const float value)
{
	SetSpeed(GetSpeed() + value);
}

float GameObject::GetSpeed() const
{
	return Speed;
}

void GameObject::SetRotationAxis(const XMFLOAT3& axis)
{
	SetRotationAxis(std::move(XMFLOAT3(axis)));
}

void GameObject::SetRotationAxis(XMFLOAT3&& axis)
{
	m_xmf3RotationAxis = Vector3::Normalize(std::forward<XMFLOAT3>(axis));
}

void GameObject::SetRotationSpeed(float speed)
{
	m_fRotationSpeed = speed;
}

void GameObject::Update(float elapsed_time)
{
	if (m_fRotationSpeed != 0.0f)
	{
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * elapsed_time);
	}

	Move(Direction, Speed);

	if (0.0f != Friction)
	{
		float deceleration = Friction * elapsed_time;
		if (Speed < deceleration)
		{
			Speed = 0.0f;
		}
		else
		{
			Speed -= deceleration;
		}
	}

	if (0.0f != elapsed_time)
	{
		UpdateBoundingBox();
	}
}

void GameObject::PrepareRendering(GameScene& scene)
{
	if (myMesh.IsAvailable())
	{
		GamePipeline::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.PrepareRendering(scene);
	}
}

void GameObject::Render(HDC surface) const
{
	if (myMesh.IsAvailable())
	{
		GamePipeline::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.RenderByFragments(surface);
	}
}

bool GameObject::CheckCameraBounds() const
{
	return Camera && Camera->IsInFrustum(Collider);
}

void GameObject::UpdateBoundingBox()
{
	if (myMesh.IsAvailable())
	{
		const auto& mat = Transform.GetWorldMatrix();
		const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);

		const auto& original_collider = myMesh.GetCollider();
		original_collider.Transform(Collider, float4x4);

		const auto orientation = DirectX::XMLoadFloat4(&Collider.Orientation);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&Collider.Orientation, quaternion);
	}
}

void GameObject::CreateRay(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir)
{
	const auto& mat = Transform.GetWorldMatrix();
	const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, float4x4 * view);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	const auto origin = DirectX::XMLoadFloat3(&xmf3CameraOrigin);
	ray_pos = XMVector3TransformCoord(origin, xmmtxToModel);
	ray_dir = XMVector3TransformCoord(pick_pos, xmmtxToModel);
	ray_dir = XMVector3Normalize(ray_dir - ray_pos);
}

int GameObject::Raycast(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance)
{
	int nIntersected = 0;
	if (myMesh.IsAvailable())
	{
		XMVECTOR ray_pos, ray_dir;
		CreateRay(pick_pos, view, ray_pos, ray_dir);

		nIntersected = myMesh.GetMesh()->Raycast(ray_pos, ray_dir, max_distance);
	}

	return (nIntersected);
}
