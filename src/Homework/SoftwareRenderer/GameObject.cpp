#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameScene.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"
#include "GameTransform.hpp"
#include "Fragment.hpp"
#include "Mesh.hpp"

GameObject::GameObject(GameScene& scene)
	: Scene(scene), Camera(nullptr)
	, MeshPtr(nullptr), myColour(RGB(255, 0, 0)), myPen(CreatePen(PS_SOLID, 0, myColour))
	, Collider()
	, Direction(XMFLOAT3(0.0f, 0.0f, 0.0f)), Speed(0.0f), Friction(0.0f)
{}

GameObject::GameObject(GameScene & scene, float x, float y, float z)
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

void GameObject::SetActive(bool bActive)
{
	isActivated = bActive;
}

void GameObject::SetMesh(std::shared_ptr<CMesh>& pMesh)
{
	MeshPtr = pMesh;
}

void GameObject::SetColor(DWORD dwColor)
{
	if (myColour != dwColor)
	{
		DeleteObject(myPen);
		myPen = CreatePen(PS_SOLID, 0, dwColor);
		myColour = dwColor;
	}
}

void GameObject::SetCamera(std::shared_ptr<GameCamera>& cam)
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

XMFLOAT3&& GameObject::GetPosition()
{
	return std::move(XMFLOAT3(Transform.GetPosition()));
}

XMFLOAT3&& GameObject::GetRight()
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetRight()));
}

XMFLOAT3&& GameObject::GetUp()
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetUp()));
}

XMFLOAT3&& GameObject::GetLook()
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetLook()));
}

void GameObject::Move(const XMFLOAT3& vDirection, float distance)
{
	Transform.Move(vDirection, distance);
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

void GameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	Transform.Rotate(fPitch, fYaw, fRoll);
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

void GameObject::SetRotationAxis(const XMFLOAT3& xmf3RotationAxis)
{
	SetRotationAxis(std::move(XMFLOAT3(xmf3RotationAxis)));
}

void GameObject::SetRotationAxis(XMFLOAT3&& xmf3RotationAxis)
{
	m_xmf3RotationAxis = Vector3::Normalize(std::forward<XMFLOAT3>(xmf3RotationAxis));
}

void GameObject::SetRotationSpeed(float fSpeed)
{
	m_fRotationSpeed = fSpeed;
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

void GameObject::PrepareRendering(GameScene& group)
{
	if (MeshPtr)
	{
		GamePipeline::SetWorldTransform(Transform.GetWorldMatrix());

		//auto hOldPen = HPEN(SelectObject(surface, myPen));
		//MeshPtr->Render(surface);
		//SelectObject(surface, hOldPen);
	}
}

void GameObject::Render(HDC surface) const
{
	if (MeshPtr)
	{
		const auto& world = Transform.GetWorldMatrix();
		GamePipeline::SetWorldTransform(world);

		auto hOldPen = HPEN(SelectObject(surface, myPen));
		MeshPtr->Render(surface);
		SelectObject(surface, hOldPen);
	}
}

bool GameObject::CheckCameraBounds() const
{
	if (Camera)
	{
		if (Camera->IsInFrustum(Collider))
		{
			return true;
		}
	}
	return false;
}

void GameObject::UpdateBoundingBox()
{
	if (MeshPtr)
	{
		const auto& mat = Transform.GetWorldMatrix();
		const auto float4x4 = XMLoadFloat4x4(&mat);
		MeshPtr->Collider.Transform(Collider, float4x4);

		auto quaternion = XMQuaternionNormalize(XMLoadFloat4(&Collider.Orientation));
		XMStoreFloat4(&Collider.Orientation, quaternion);
	}
}

void GameObject::GenerateRayForPicking(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir)
{
	const auto& mat = Transform.GetWorldMatrix();
	const auto float4x4 = XMLoadFloat4x4(&mat);
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, float4x4 * view);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	ray_pos = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	ray_dir = XMVector3TransformCoord(pick_pos, xmmtxToModel);
	ray_dir = XMVector3Normalize(ray_dir - ray_pos);
}

int GameObject::PickObjectByRayIntersection(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance)
{
	int nIntersected = 0;
	if (MeshPtr)
	{
		XMVECTOR ray_pos, ray_dir;
		GenerateRayForPicking(pick_pos, view, ray_pos, ray_dir);

		nIntersected = MeshPtr->CheckRayIntersection(ray_pos, ray_dir, max_distance);
	}

	return (nIntersected);
}
