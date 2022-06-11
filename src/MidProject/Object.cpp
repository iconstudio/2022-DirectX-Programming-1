#include "pch.hpp"
#include "Object.h"
#include "Pipeline.hpp"
#include "Arithmetics.hpp"
#include "Mesh.h"

WCHAR debug_frame_info[256]{};

GameObject::GameObject()
	: myName()
	, myTransform()
	, localMatrix(Matrix4x4::Identity()), worldMatrix(Matrix4x4::Identity())
	, isTransformModified(true)
	, staticCollider(nullptr), myCollider(nullptr)
{}

GameObject::~GameObject()
{
	if (m_pMesh)
	{
		m_pMesh->Release();
	}
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::SetMesh(CMaterialMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box)
{
	staticCollider = box;
}

void GameObject::SetOriginalCollider(shared_ptr<BoundingOrientedBox>&& box)
{
	staticCollider = std::forward<shared_ptr<BoundingOrientedBox>>(box);
}

void GameObject::Attach(GameObject* pChild)
{
	if (pChild)
	{
		pChild->m_pParent = this;
	}

	if (myChild)
	{
		if (pChild)
		{
			pChild->mySibling = myChild->mySibling;
		}

		myChild->mySibling = pChild;
	}
	else
	{
		myChild = pChild;
	}
}

void GameObject::BuildCollider()
{
	auto handle = new BoundingOrientedBox(*staticCollider);
	myCollider = unique_ptr<BoundingOrientedBox>(handle);
}

void GameObject::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{}

void GameObject::Animate(float delta_time)
{
	Update(delta_time);

	if (isTransformModified)
	{
		EnumerateTransforms();
		isTransformModified = false;
	}

	if (mySibling)
	{
		mySibling->Animate(delta_time);
	}

	if (myChild)
	{
		myChild->Animate(delta_time, worldMatrix);
	}
}

void GameObject::Animate(float delta_time, const XMFLOAT4X4& parent)
{
	Update(delta_time);

	if (isTransformModified)
	{
		EnumerateTransforms(parent);
		isTransformModified = false;
	}

	if (mySibling)
	{
		mySibling->Animate(delta_time, parent);
	}

	if (myChild)
	{
		myChild->Animate(delta_time, worldMatrix);
	}
}

void GameObject::Update(float delta_time)
{
	// 
}

void GameObject::UpdateTransform()
{
	worldMatrix = localMatrix;
}

void GameObject::UpdateTransform(const XMFLOAT4X4& parent)
{
	worldMatrix = Matrix4x4::Multiply(localMatrix, parent);
}

void GameObject::UpdateTransform(XMFLOAT4X4&& parent)
{
	worldMatrix = Matrix4x4::Multiply(localMatrix, std::forward<XMFLOAT4X4>(parent));
}

void GameObject::EnumerateTransforms()
{
	UpdateTransform();
	UpdateCollider();

	if (mySibling)
	{
		mySibling->EnumerateTransforms();
	}
	if (myChild)
	{
		myChild->EnumerateTransforms(worldMatrix);
	}
}

void GameObject::EnumerateTransforms(const XMFLOAT4X4& parent)
{
	UpdateTransform(parent);
	UpdateCollider();

	if (mySibling)
	{
		mySibling->EnumerateTransforms(parent);
	}
	if (myChild)
	{
		myChild->EnumerateTransforms(worldMatrix);
	}
}

void GameObject::EnumerateTransforms(XMFLOAT4X4&& parent)
{
	const auto& mat = std::forward<XMFLOAT4X4>(parent);

	UpdateTransform(mat);
	UpdateCollider();

	if (mySibling)
	{
		mySibling->EnumerateTransforms(mat);
	}
	if (myChild)
	{
		myChild->EnumerateTransforms(worldMatrix);
	}
}

void GameObject::UpdateCollider()
{
	if (myCollider)
	{
		const auto float4x4 = DirectX::XMLoadFloat4x4(&worldMatrix);

		const auto& original_collider = staticCollider;
		original_collider->Transform(*myCollider, float4x4);

		auto& angle = myCollider->Orientation;

		const auto orientation = DirectX::XMLoadFloat4(&angle);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&angle, quaternion);
	}
}

void GameObject::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	XMFLOAT4X4 xmf4x4World{};

	const auto my_mat = XMLoadFloat4x4(&worldMatrix);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(my_mat));

	// 두번째 루트 매개인자에서 0번째 메모리에 float 16개 전달
	cmdlist->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void GameObject::Render(P3DGrpCommandList cmdlist, GameCamera* camera) const
{
	PrepareRendering(cmdlist);

	if (m_pMesh)
	{
		m_pMesh->Render(cmdlist);
	}

	if (mySibling)
	{
		mySibling->Render(cmdlist, camera);
	}

	if (myChild)
	{
		myChild->Render(cmdlist, camera);
	}
}

void GameObject::ReleaseUniforms()
{}

bool GameObject::CheckCollisionWith(GameObject* other) const
{
	const auto& other_collision = other->myCollider;
	if (myCollider && other_collision)
	{
		return other_collision->Intersects(*myCollider);
	}

	return false;
}

void GameObject::CollideWith(GameObject* other)
{
	const auto& tag = other->GetTag();
	switch (tag)
	{
		case COLLISION_TAGS::NONE:
		{}
		break;
	}
}

void GameObject::OnTransformUpdate()
{
	isTransformModified = true;
}

void GameObject::SetPosition(float x, float y, float z)
{
	localMatrix._41 = x;
	localMatrix._42 = y;
	localMatrix._43 = z;

	OnTransformUpdate();
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	SetPosition(position.x, position.y, position.z);
}

void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	localMatrix = Matrix4x4::Multiply(mtxScale, localMatrix);

	OnTransformUpdate();
}

void GameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);

	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);

	GameObject::SetPosition(xmf3Position);
}

void GameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);

	GameObject::SetPosition(xmf3Position);
}

void GameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	localMatrix = Matrix4x4::Multiply(mtxRotate, localMatrix);

	OnTransformUpdate();
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	localMatrix = Matrix4x4::Multiply(mtxRotate, localMatrix);

	OnTransformUpdate();
}

void GameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	localMatrix = Matrix4x4::Multiply(mtxRotate, localMatrix);

	OnTransformUpdate();
}

constexpr COLLISION_TAGS GameObject::GetTag() const noexcept
{
	return COLLISION_TAGS::NONE;
}

XMFLOAT3 GameObject::GetPosition()
{
	return(XMFLOAT3(worldMatrix._41, worldMatrix._42, worldMatrix._43));
}

XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(worldMatrix._31, worldMatrix._32, worldMatrix._33)));
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(worldMatrix._21, worldMatrix._22, worldMatrix._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(worldMatrix._11, worldMatrix._12, worldMatrix._13)));
}

const GameObject* GameObject::FindFrame(const char* name) const
{
	if (name == myName)
	{
		return this;
	}

	if (mySibling)
	{
		if (auto root = mySibling->FindFrame(name); root)
			return root;
	}

	if (myChild)
	{
		if (auto root = myChild->FindFrame(name); root)
			return root;
	}

	return nullptr;
}

GameObject* GameObject::FindFrame(const char* name)
{
	if (name == myName)
	{
		return this;
	}

	if (mySibling)
	{
		if (auto root = mySibling->FindFrame(name); root)
			return root;
	}

	if (myChild)
	{
		if (auto root = myChild->FindFrame(name); root)
			return root;
	}

	return nullptr;
}

const GameObject* GameObject::GetParent() const
{
	return m_pParent;
}

GameObject* GameObject::GetParent()
{
	return m_pParent;
}

UINT GameObject::GetMeshType() const
{
	return (m_pMesh) ? m_pMesh->GetType() : 0;
}

void GameObject::PrintFrameInfo() const
{
	swprintf_s(debug_frame_info, 256, L"프레임 계층 열거: (Frame: %p)\n", this);
	OutputDebugString(debug_frame_info);
	ZeroMemory(debug_frame_info, sizeof(debug_frame_info));

	if (mySibling)
	{
		mySibling->PrintFrameInfo();
	}

	if (myChild)
	{
		myChild->PrintFrameInfo(this);
	}
}

void GameObject::PrintFrameInfo(const GameObject* parent) const
{
	swprintf_s(debug_frame_info, 256, L"(Frame: %p) (Parent: %p)\n", this, parent);
	OutputDebugString(debug_frame_info);
	ZeroMemory(debug_frame_info, sizeof(debug_frame_info));

	if (mySibling)
	{
		mySibling->PrintFrameInfo(parent);
	}

	if (myChild)
	{
		myChild->PrintFrameInfo(this);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 15.0f;
}

CRotatingObject::~CRotatingObject()
{}

void CRotatingObject::Update(float delta_time)
{
	GameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * delta_time);

	GameObject::Update(delta_time);
}

CRevolvingObject::CRevolvingObject()
{
	m_xmf3RevolutionAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{}

void CRevolvingObject::Update(float delta_time)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RevolutionAxis), XMConvertToRadians(m_fRevolutionSpeed * delta_time));
	localMatrix = Matrix4x4::Multiply(localMatrix, mtxRotate);

	GameObject::Update(delta_time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHellicopterObject::CHellicopterObject()
{}

CHellicopterObject::~CHellicopterObject()
{}

void CHellicopterObject::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{}

void CHellicopterObject::Update(float delta_time)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * delta_time);
		m_pMainRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localMatrix);
	}

	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * delta_time);
		m_pTailRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localMatrix);
	}

	GameObject::Update(delta_time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CApacheObject::CApacheObject()
{}

CApacheObject::~CApacheObject()
{}

void CApacheObject::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	m_pMainRotorFrame = FindFrame("rotor");
	m_pTailRotorFrame = FindFrame("black_m_7");
}

void CApacheObject::Update(float delta_time)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * delta_time);
		m_pMainRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localMatrix);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * delta_time);
		m_pTailRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localMatrix);
	}

	GameObject::Update(delta_time);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSuperCobraObject::CSuperCobraObject()
{}

CSuperCobraObject::~CSuperCobraObject()
{}

void CSuperCobraObject::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	m_pMainRotorFrame = FindFrame("MainRotor_LOD0");
	m_pTailRotorFrame = FindFrame("TailRotor_LOD0");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGunshipObject::CGunshipObject()
{}

CGunshipObject::~CGunshipObject()
{}

void CGunshipObject::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	m_pMainRotorFrame = FindFrame("Rotor");
	m_pTailRotorFrame = FindFrame("Back_Rotor");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMi24Object::CMi24Object()
{}

CMi24Object::~CMi24Object()
{}

void CMi24Object::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

