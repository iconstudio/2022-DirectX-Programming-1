#include "pch.hpp"
#include "GameObject.hpp"
#include "Transformer.hpp"
#include "GameTransform.hpp"
#include "GameMesh.hpp"
#include "Arithmetics.hpp"

GameObject::GameObject()
	: isActivated(true)
	, myMesh(), Transform(), myCollider()
{}

GameObject::GameObject(float x, float y, float z)
	: GameObject()
{
	SetPosition(x, y, z);
}

GameObject::GameObject(float list[3])
	: GameObject()
{
	SetPosition(list[0], list[1], list[2]);
}

GameObject::GameObject(const XMFLOAT3& position)
	: GameObject()
{
	SetPosition(position);
}

GameObject::GameObject(XMFLOAT3&& position)
	: GameObject()
{
	SetPosition(position);
}

GameObject::~GameObject()
{}

void GameObject::SetActive(bool flag)
{
	isActivated = flag;
	if (flag)
	{
		UpdateBoundingBox();
	}
}

void GameObject::SetMesh(const shared_ptr<CMesh>& mesh)
{
	myMesh.SetMesh(mesh);
}

void GameObject::SetColor(COLORREF color)
{
	myMesh.SetColor(color);
}

void GameObject::Activate()
{
	SetActive(true);
}

void GameObject::Deactivate()
{
	SetActive(false);
}

bool GameObject::IsActivated() const
{
	return isActivated;
}

void GameObject::SetWorldMatrix(XMFLOAT4X4&& tfrm)
{
	Transform.SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4& tfrm)
{
	Transform.SetWorldMatrix(tfrm);
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
	SetRotation(std::move(XMFLOAT4X4(tfrm)));
}

void GameObject::SetRotation(XMFLOAT4X4&& tfrm)
{
	Transform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));
}

void GameObject::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	Transform.LookTo(to, up);
}

void GameObject::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	Transform.LookAt(from, up);
}

void GameObject::AddPosition(XMFLOAT3&& vector)
{
	Transform.Translate(vector);
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

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3(Transform.GetPosition());
}

XMFLOAT3 GameObject::GetRight() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetRight()));
}

XMFLOAT3 GameObject::GetUp() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetUp()));
}

XMFLOAT3 GameObject::GetLook() const
{
	return Vector3::Normalize(XMFLOAT3(Transform.GetLook()));
}

void GameObject::PrepareRendering(GameScene& scene) const
{
	if (myMesh.IsAvailable())
	{
		Transformer::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.PrepareRendering(scene);
	}
}

void GameObject::Render(HDC surface) const
{
	if (myMesh.IsAvailable())
	{
		Transformer::SetWorldTransform(Transform.GetWorldMatrix());

		myMesh.RenderByFragments(surface);
	}
}

void GameObject::UpdateBoundingBox()
{
	if (myMesh.IsAvailable())
	{
		const auto& mat = Transform.GetWorldMatrix();
		const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);

		const auto& original_collider = myMesh.GetCollider();
		original_collider.Transform(myCollider, float4x4);

		const auto orientation = DirectX::XMLoadFloat4(&myCollider.Orientation);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&myCollider.Orientation, quaternion);
	}
}
