#include "pch.hpp"
#include "Object.h"
#include "Material.hpp"
#include "Pipeline.hpp"
#include "Arithmetics.hpp"

WCHAR debug_frame_info[256]{};

GameObject::GameObject()
	: m_pstrFrameName()
{
	ZeroMemory(m_pstrFrameName, sizeof(m_pstrFrameName));

	localTransform = Matrix4x4::Identity();
	worldTransform = Matrix4x4::Identity();
}

GameObject::~GameObject()
{
	if (m_pMesh)
	{
		m_pMesh->Release();
	}
}

void GameObject::Attach(GameObject* pChild)
{
	if (pChild)
	{
		pChild->m_pParent = this;
	}
	if (myChild)
	{
		if (pChild) pChild->mySibling = myChild->mySibling;
		myChild->mySibling = pChild;
	}
	else
	{
		myChild = pChild;
	}
}

constexpr COLLISION_TAGS GameObject::GetTag() const noexcept
{
	return COLLISION_TAGS::NONE;
}

void GameObject::SetMesh(CMaterialMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::Animate(float time_elapsed, XMFLOAT4X4* parent)
{
	UpdateCollider(&worldTransform);

	if (mySibling)
	{
		mySibling->Animate(time_elapsed, parent);
	}

	if (myChild)
	{
		myChild->Animate(time_elapsed, &worldTransform);
	}
}

void GameObject::Render(P3DGrpCommandList cmdlist, GameCamera* pCamera)
{
	OnPrepareRender();

	UpdateUniforms(cmdlist); 

	if (m_pMesh)
	{
		m_pMesh->Render(cmdlist);
	}

	if (mySibling) mySibling->Render(cmdlist, pCamera);
	if (myChild) myChild->Render(cmdlist, pCamera);
}

void GameObject::InitializeUniforms(P3DDevice device, P3DGrpCommandList cmdlist)
{}

void GameObject::UpdateUniforms(P3DGrpCommandList cmdlist)
{
	XMFLOAT4X4 xmf4x4World{};

	const auto my_mat = XMLoadFloat4x4(&worldTransform);
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(my_mat));

	// 두번째 루트 매개인자에서 0번째 메모리에 float 16개 전달
	cmdlist->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void GameObject::UpdateUniforms(P3DGrpCommandList cmdlist, CMaterial* pMaterial)
{}

void GameObject::ReleaseUniforms()
{}

void GameObject::SetOriginalCollider(const shared_ptr<BoundingOrientedBox>& box)
{
	staticCollider = box;
}

void GameObject::BuildCollider()
{
	auto handle = new BoundingOrientedBox(*staticCollider);
	myCollider = unique_ptr<BoundingOrientedBox>(handle);
}

void GameObject::UpdateCollider(const XMFLOAT4X4* mat)
{
	if (myCollider)
	{
		const auto float4x4 = DirectX::XMLoadFloat4x4(mat);

		const auto& original_collider = staticCollider;
		original_collider->Transform(*myCollider, float4x4);

		auto& angle = myCollider->Orientation;

		const auto orientation = DirectX::XMLoadFloat4(&angle);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&angle, quaternion);
	}
}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	worldTransform = (pxmf4x4Parent) ? Matrix4x4::Multiply(localTransform, *pxmf4x4Parent) : localTransform;

	UpdateCollider(&worldTransform);

	if (mySibling) mySibling->UpdateTransform(pxmf4x4Parent);
	if (myChild) myChild->UpdateTransform(&worldTransform);
}

void GameObject::SetPosition(float x, float y, float z)
{
	localTransform._41 = x;
	localTransform._42 = y;
	localTransform._43 = z;

	UpdateTransform(NULL);
}

void GameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	localTransform = Matrix4x4::Multiply(mtxScale, localTransform);

	UpdateTransform(NULL);
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
	localTransform = Matrix4x4::Multiply(mtxRotate, localTransform);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	localTransform = Matrix4x4::Multiply(mtxRotate, localTransform);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	localTransform = Matrix4x4::Multiply(mtxRotate, localTransform);

	UpdateTransform(NULL);
}

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

XMFLOAT3 GameObject::GetPosition()
{
	return(XMFLOAT3(worldTransform._41, worldTransform._42, worldTransform._43));
}

XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(worldTransform._31, worldTransform._32, worldTransform._33)));
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(worldTransform._21, worldTransform._22, worldTransform._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(worldTransform._11, worldTransform._12, worldTransform._13)));
}

const GameObject* GameObject::FindFrame(const char* name) const
{
	if (0 == strcmp(m_pstrFrameName, name))
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
	if (0 == strcmp(m_pstrFrameName, name))
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

void CRotatingObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	GameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

void CRotatingObject::Render(ID3D12GraphicsCommandList* cmdlist, GameCamera* pCamera)
{
	GameObject::Render(cmdlist, pCamera);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRevolvingObject::CRevolvingObject()
{
	m_xmf3RevolutionAxis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_fRevolutionSpeed = 0.0f;
}

CRevolvingObject::~CRevolvingObject()
{}

void CRevolvingObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3RevolutionAxis), XMConvertToRadians(m_fRevolutionSpeed * fTimeElapsed));
	localTransform = Matrix4x4::Multiply(localTransform, mtxRotate);

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHellicopterObject::CHellicopterObject()
{}

CHellicopterObject::~CHellicopterObject()
{}

void CHellicopterObject::Awake()
{}

void CHellicopterObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localTransform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localTransform);
	}

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CApacheObject::CApacheObject()
{}

CApacheObject::~CApacheObject()
{}

void CApacheObject::Awake()
{
	m_pMainRotorFrame = FindFrame("rotor");
	m_pTailRotorFrame = FindFrame("black_m_7");
}

void CApacheObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localTransform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->localTransform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localTransform);
	}

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSuperCobraObject::CSuperCobraObject()
{}

CSuperCobraObject::~CSuperCobraObject()
{}

void CSuperCobraObject::Awake()
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

void CGunshipObject::Awake()
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

void CMi24Object::Awake()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

