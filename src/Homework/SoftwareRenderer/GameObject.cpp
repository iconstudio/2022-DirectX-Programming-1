#include "stdafx.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "GameTransform.hpp"
#include "Mesh.hpp"

GameObject::GameObject(GameScene& scene)
	: Scene(scene), Camera(nullptr)
	, MeshPtr(nullptr), m_dwColor(RGB(255, 0, 0)), m_Pen(CreatePen(PS_SOLID, 0, m_dwColor))
	, Collider()
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
	m_bActive = bActive;
}

void GameObject::SetMesh(std::shared_ptr<CMesh> pMesh)
{
	MeshPtr = pMesh;
}

void GameObject::SetColor(DWORD dwColor)
{
	if (m_dwColor != dwColor)
	{
		DeleteObject(m_Pen);
		m_Pen = CreatePen(PS_SOLID, 0, dwColor);
		m_dwColor = dwColor;
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
	return XMFLOAT3(Transform.GetPosition());
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

void GameObject::Move(const XMFLOAT3& vDirection, float fSpeed)
{
	Transform.Move(vDirection, fSpeed);
}

void GameObject::MoveStrafe(float fDistance)
{
	Transform.MoveStrafe(fDistance);
}

void GameObject::MoveUp(float fDistance)
{
	Transform.MoveUp(fDistance);
}

void GameObject::MoveForward(float fDistance)
{
	Transform.MoveForward(fDistance);
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

void GameObject::SetDirection(const XMFLOAT3& xmf3MovingDirection)
{
	SetDirection(std::move(XMFLOAT3(xmf3MovingDirection)));
}

void GameObject::SetDirection(XMFLOAT3&& xmf3MovingDirection)
{
	m_xmf3MovingDirection = Vector3::Normalize(std::forward<XMFLOAT3>(xmf3MovingDirection));
}

void GameObject::SetSpeed(float fSpeed)
{
	m_fMovingSpeed = fSpeed;
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

void GameObject::Update(float fElapsedTime)
{
	if (m_fRotationSpeed != 0.0f)
	{
		Rotate(m_xmf3RotationAxis, m_fRotationSpeed * fElapsedTime);
	}

	if (m_fMovingSpeed != 0.0f)
	{
		Move(m_xmf3MovingDirection, m_fMovingSpeed * fElapsedTime);
	}

	UpdateBoundingBox();
}

void GameObject::Render(HDC surface, const XMFLOAT4X4& world, CMesh* mesh)
{
	if (mesh)
	{
		GamePipeline::SetWorldTransform(world);

		HPEN hOldPen = (HPEN)::SelectObject(surface, m_Pen);
		mesh->Render(surface);
		::SelectObject(surface, hOldPen);
	}
}

void GameObject::Render(HDC surface)
{
	if (Camera)
	{
		if (Camera->IsInFrustum(Collider))
		{
			GameObject::Render(surface, Transform.GetWorldMatrix(), MeshPtr.get());
		}
	}
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
