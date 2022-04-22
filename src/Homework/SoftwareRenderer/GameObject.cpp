#include "stdafx.hpp"
#include "GamePipeline.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "Mesh.hpp"

GameObject::GameObject(GameScene& scene)
	: Scene(scene), Camera(nullptr)
	, m_pMesh(nullptr), m_dwColor(RGB(255, 0, 0)), m_Pen(CreatePen(PS_SOLID, 0, m_dwColor))
	, m_xmf4x4World(Matrix4x4::Identity())
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
	m_pMesh = pMesh;
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
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void GameObject::SetPosition(const XMFLOAT3& xmf3Position)
{
	SetPosition(std::move(XMFLOAT3(xmf3Position)));
}

void GameObject::SetPosition(XMFLOAT3&& xmf3Position)
{
	m_xmf4x4World._41 = xmf3Position.x;
	m_xmf4x4World._42 = xmf3Position.y;
	m_xmf4x4World._43 = xmf3Position.z;
}

void GameObject::SetRotationTransform(XMFLOAT4X4* pmxf4x4Transform)
{
	m_xmf4x4World._11 = pmxf4x4Transform->_11;
	m_xmf4x4World._12 = pmxf4x4Transform->_12;
	m_xmf4x4World._13 = pmxf4x4Transform->_13;

	m_xmf4x4World._21 = pmxf4x4Transform->_21;
	m_xmf4x4World._22 = pmxf4x4Transform->_22;
	m_xmf4x4World._23 = pmxf4x4Transform->_23;

	m_xmf4x4World._31 = pmxf4x4Transform->_31;
	m_xmf4x4World._32 = pmxf4x4Transform->_32;
	m_xmf4x4World._33 = pmxf4x4Transform->_33;
}

void GameObject::AddPosition(XMFLOAT3&& xmf3Position)
{
	m_xmf4x4World._41 += xmf3Position.x;
	m_xmf4x4World._42 += xmf3Position.y;
	m_xmf4x4World._43 += xmf3Position.z;
}

XMFLOAT3&& GameObject::GetPosition()
{
	return std::move(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3&& GameObject::GetLook()
{
	XMFLOAT3 xmf3LookAt(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33); 

	return Vector3::Normalize(xmf3LookAt);
}

XMFLOAT3&& GameObject::GetUp()
{
	XMFLOAT3 xmf3Up(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);

	return Vector3::Normalize(xmf3Up);
}

XMFLOAT3&& GameObject::GetRight()
{
	XMFLOAT3 xmf3Right(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);

	return Vector3::Normalize(xmf3Right);
}

void GameObject::SetMovingDirection(const XMFLOAT3& xmf3MovingDirection)
{
	SetMovingDirection(std::move(XMFLOAT3(xmf3MovingDirection)));
}

void GameObject::SetMovingDirection(XMFLOAT3&& xmf3MovingDirection)
{
	m_xmf3MovingDirection = Vector3::Normalize(xmf3MovingDirection);
}

void GameObject::SetMovingSpeed(float fSpeed)
{
	m_fMovingSpeed = fSpeed;
}

void GameObject::SetMovingRange(float fRange)
{
	m_fMovingRange = fRange;
}

void GameObject::SetRotationAxis(const XMFLOAT3& xmf3RotationAxis)
{
	SetRotationAxis(std::move(XMFLOAT3(xmf3RotationAxis)));
}

void GameObject::SetRotationAxis(XMFLOAT3&& xmf3RotationAxis)
{
	m_xmf3RotationAxis = Vector3::Normalize(xmf3RotationAxis);
}

void GameObject::SetRotationSpeed(float fSpeed)
{
	m_fRotationSpeed = fSpeed;
}

void GameObject::Move(XMFLOAT3& vDirection, float fSpeed)
{
	AddPosition(Vector3::ScalarProduct(vDirection, fSpeed, false));
}

void GameObject::MoveForward(float fDistance)
{
	//auto&& distance = Vector3::ScalarProduct(GetLook(), fDistance);

	//GameObject::SetPosition(Vector3::Add(GetPosition(), distance));
	AddPosition(Vector3::ScalarProduct(GetLook(), fDistance));
}

void GameObject::MoveUp(float fDistance)
{
	//auto&& distance = Vector3::ScalarProduct(GetUp(), fDistance);

	//GameObject::SetPosition(Vector3::Add(GetPosition(), distance));
	AddPosition(Vector3::ScalarProduct(GetUp(), fDistance));
}

void GameObject::MoveStrafe(float fDistance)
{
	//auto&& distance = Vector3::ScalarProduct(GetRight(), fDistance);

	//SetPosition(Vector3::Add(GetPosition(), distance));
	AddPosition(Vector3::ScalarProduct(GetRight(), fDistance));
}

void GameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationYawPitchRoll(fPitch, fYaw, fRoll);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void GameObject::Rotate(XMFLOAT3& xmf3RotationAxis, float fAngle)
{
	XMFLOAT4X4 mtxRotate = Matrix4x4::RotationAxis(xmf3RotationAxis, fAngle);

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void GameObject::LookTo(XMFLOAT3& xmf3LookTo, XMFLOAT3& xmf3Up)
{
	auto&& xmf4x4View = Matrix4x4::LookToLH(GetPosition(), xmf3LookTo, xmf3Up);

	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void GameObject::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	auto&& xmf4x4View = Matrix4x4::LookAtLH(GetPosition(), xmf3LookAt, xmf3Up);

	m_xmf4x4World._11 = xmf4x4View._11; m_xmf4x4World._12 = xmf4x4View._21; m_xmf4x4World._13 = xmf4x4View._31;
	m_xmf4x4World._21 = xmf4x4View._12; m_xmf4x4World._22 = xmf4x4View._22; m_xmf4x4World._23 = xmf4x4View._32;
	m_xmf4x4World._31 = xmf4x4View._13; m_xmf4x4World._32 = xmf4x4View._23; m_xmf4x4World._33 = xmf4x4View._33;
}

void GameObject::Animate(float fElapsedTime)
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

void GameObject::Render(HDC hDCFrameBuffer, XMFLOAT4X4* pxmf4x4World, CMesh* pMesh)
{
	if (pMesh)
	{
		GamePipeline::SetWorldTransform(pxmf4x4World);

		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, m_Pen);
		pMesh->Render(hDCFrameBuffer);
		::SelectObject(hDCFrameBuffer, hOldPen);
	}
}

void GameObject::Render(HDC hDCFrameBuffer)
{
	if (Camera->IsInFrustum(Collider))
	{
		GameObject::Render(hDCFrameBuffer, &m_xmf4x4World, m_pMesh.get());
	}
}

void GameObject::UpdateBoundingBox()
{
	if (m_pMesh)
	{
		m_pMesh->Collider.Transform(Collider, XMLoadFloat4x4(&m_xmf4x4World));

		auto quaternion = XMQuaternionNormalize(XMLoadFloat4(&Collider.Orientation));
		XMStoreFloat4(&Collider.Orientation, quaternion);
	}
}

void GameObject::GenerateRayForPicking(XMVECTOR& pick_pos, XMMATRIX& view, XMVECTOR& ray_pos, XMVECTOR& ray_dir)
{
	XMMATRIX xmmtxToModel = XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4World) * view);

	XMFLOAT3 xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	ray_pos = XMVector3TransformCoord(XMLoadFloat3(&xmf3CameraOrigin), xmmtxToModel);
	ray_dir = XMVector3TransformCoord(pick_pos, xmmtxToModel);
	ray_dir = XMVector3Normalize(ray_dir - ray_pos);
}

int GameObject::PickObjectByRayIntersection(XMVECTOR& pick_pos, XMMATRIX& view, float* max_distance)
{
	int nIntersected = 0;
	if (m_pMesh)
	{
		XMVECTOR ray_pos, ray_dir;
		GenerateRayForPicking(pick_pos, view, ray_pos, ray_dir);

		nIntersected = m_pMesh->CheckRayIntersection(ray_pos, ray_dir, max_distance);
	}
	return (nIntersected);
}
