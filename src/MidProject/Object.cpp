#include "pch.hpp"
#include "Object.h"
#include "Pipeline.hpp"
#include "Arithmetics.hpp"
#include "Mesh.h"

WCHAR debug_frame_info[256]{};

GameObject::GameObject()
	: myName()
	, localTransform(), localMatrix(localTransform.GetMatrix())
	, worldTransform(), worldMatrix(worldTransform.GetMatrix())
	, isTransformModified(true)
	, m_pMesh(nullptr)
	, staticCollider(nullptr), myCollider(nullptr)
{}

GameObject::~GameObject()
{
	Release();
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::SetMesh(CMesh* pMesh)
{
	m_pMesh = pMesh;
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

void GameObject::Release()
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

void GameObject::SetMatrix(const XMFLOAT4X4& mat)
{
	localTransform.SetMatrix(mat);

	OnTransformUpdate();
}

void GameObject::SetMatrix(XMFLOAT4X4&& mat)
{
	localTransform.SetMatrix(std::forward<XMFLOAT4X4>(mat));

	OnTransformUpdate();
}

void GameObject::SetScale(float x, float y, float z)
{
	localTransform.SetScale(x, y, z);

	OnTransformUpdate();
}

void GameObject::SetPosition(float x, float y, float z)
{
	localTransform.SetPosition(x, y, z);

	OnTransformUpdate();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	localTransform.SetPosition(pos);

	OnTransformUpdate();
}

void GameObject::SetPosition(XMFLOAT3&& pos)
{
	localTransform.SetPosition(std::forward<XMFLOAT3>(pos));

	OnTransformUpdate();
}

void GameObject::SetRotation(const XMFLOAT4X4& tfrm)
{
	localTransform.SetRotation(tfrm);

	OnTransformUpdate();
}

void GameObject::SetRotation(XMFLOAT4X4&& tfrm)
{
	localTransform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));

	OnTransformUpdate();
}

void GameObject::Translate(float x, float y, float z)
{
	localTransform.Translate(x, y, z);

	OnTransformUpdate();
}

void GameObject::Translate(const XMFLOAT3& shift)
{
	localTransform.Translate(shift);

	OnTransformUpdate();
}

void GameObject::Translate(XMFLOAT3&& shift)
{
	localTransform.Translate(std::forward<XMFLOAT3>(shift));

	OnTransformUpdate();
}

void GameObject::Move(const XMFLOAT3& dir, float distance)
{
	localTransform.Move(dir, distance);

	OnTransformUpdate();
}

void GameObject::Move(XMFLOAT3&& dir, float distance)
{
	localTransform.Move(std::forward<XMFLOAT3>(dir), distance);

	OnTransformUpdate();
}

void GameObject::MoveStrafe(float distance)
{
	localTransform.MoveStrafe(distance);

	OnTransformUpdate();
}

void GameObject::MoveForward(float distance)
{
	localTransform.MoveForward(distance);

	OnTransformUpdate();
}

void GameObject::MoveUp(float distance)
{
	localTransform.MoveUp(distance);

	OnTransformUpdate();
}

void GameObject::Rotate(const XMFLOAT4X4& tfrm)
{
	localTransform.Rotate(tfrm);

	OnTransformUpdate();
}

void GameObject::Rotate(XMFLOAT4X4 && tfrm)
{
	localTransform.Rotate(std::forward<XMFLOAT4X4>(tfrm));

	OnTransformUpdate();
}

void GameObject::Rotate(float pitch, float yaw, float roll)
{
	localTransform.Rotate(pitch, yaw, roll);

	OnTransformUpdate();
}

void GameObject::Rotate(const XMFLOAT3& axis, float angle)
{
	localTransform.Rotate(axis, angle);

	OnTransformUpdate();
}

void GameObject::Rotate(const XMFLOAT4& quaternion)
{
	localTransform.Rotate(quaternion);

	OnTransformUpdate();
}

void GameObject::Rotate(XMFLOAT4&& quaternion)
{
	localTransform.Rotate(std::forward<XMFLOAT4>(quaternion));

	OnTransformUpdate();
}

void GameObject::LookTo(const XMFLOAT3& look, const XMFLOAT3& up)
{
	localTransform.LookTo(look, up);

	OnTransformUpdate();
}

void GameObject::LookAt(const XMFLOAT3 & look, const XMFLOAT3 & up)
{
	localTransform.LookAt(look, up);

	OnTransformUpdate();
}

constexpr COLLISION_TAGS GameObject::GetTag() const noexcept
{
	return COLLISION_TAGS::NONE;
}

XMFLOAT3 GameObject::GetPosition()
{
	return XMFLOAT3(worldTransform.GetPosition());
}

XMFLOAT3 GameObject::GetLook()
{
	return XMFLOAT3(worldTransform.GetLook());
}

XMFLOAT3 GameObject::GetUp()
{
	return XMFLOAT3(worldTransform.GetUp());
}

XMFLOAT3 GameObject::GetRight()
{
	return XMFLOAT3(worldTransform.GetRight());
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

