#include "pch.hpp"
#include "Object.h"
#include "Material.hpp"
#include "Shader.h"
#include "Camera.h"
#include "Arithmetics.hpp"

GameObject::GameObject()
	: m_pstrFrameName()
	, localTransform(), worldTransform()
{
	ZeroMemory(m_pstrFrameName, sizeof(m_pstrFrameName));
}

GameObject::GameObject(float x, float y, float z)
	: GameObject()
{
	SetPosition(x, y, z);
}

GameObject::GameObject(float posiion[3])
	: GameObject(posiion[0], posiion[1], posiion[2])
{}

GameObject::GameObject(const XMFLOAT3& position)
	: GameObject(position.x, position.y, position.z)
{}

GameObject::GameObject(XMFLOAT3&& position)
	: GameObject(std::forward<float>(position.x)
		, std::forward<float>(position.y)
		, std::forward<float>(position.z))
{}

GameObject::~GameObject()
{
	if (m_pMesh)
	{
		m_pMesh->Release();
	}

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}

	if (m_ppMaterials)
	{
		delete[] m_ppMaterials;
	}
}

void GameObject::Attach(GameObject* pChild, bool bReferenceUpdate)
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

void GameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::SetMaterial(int index, CMaterial* mat)
{
	if (m_ppMaterials[index])
		m_ppMaterials[index]->Release();

	m_ppMaterials[index] = mat;

	if (m_ppMaterials[index])
		m_ppMaterials[index]->AddRef();
}

void GameObject::Animate(float time_elapsed, XMFLOAT4X4* parent)
{
	if (mySibling)
	{
		mySibling->Animate(time_elapsed, parent);
	}

	if (myChild)
	{
		myChild->Animate(time_elapsed, nullptr);
	}
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

void GameObject::Render(P3DGrpCommandList cmd_list, GameCamera* pCamera)
{
	OnPrepareRender();

	auto& my_mat = worldTransform.GetWorldMatrix();
	UpdateUniforms(cmd_list, &my_mat);

	if (0 < m_nMaterials)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				auto& pipeline = m_ppMaterials[i]->m_pShader;
				if (pipeline)
				{
					pipeline->Render(cmd_list, pCamera);
				}
				else
				{
					throw "파이프라인과 쉐이더가 존재하지 않음!";
				}

				m_ppMaterials[i]->UpdateUniforms(cmd_list);
			}

			if (m_pMesh)
			{
				m_pMesh->Render(cmd_list, i);
			}
		}
	}

	if (mySibling) mySibling->Render(cmd_list, pCamera);
	if (myChild) myChild->Render(cmd_list, pCamera);
}

void GameObject::InitializeUniforms(P3DDevice device, P3DGrpCommandList cmd_list)
{}

void GameObject::UpdateUniforms(P3DGrpCommandList cmd_list)
{}

void GameObject::UpdateUniforms(P3DGrpCommandList cmd_list, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World{};

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	cmd_list->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void GameObject::UpdateUniforms(P3DGrpCommandList cmd_list, CMaterial* pMaterial)
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

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4& tfrm)
{
	localTransform.SetWorldMatrix(tfrm);

	OnUpdateTransform();
}

void GameObject::SetWorldMatrix(XMFLOAT4X4&& tfrm)
{
	localTransform.SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));

	OnUpdateTransform();
}

void GameObject::SetPosition(float x, float y, float z)
{
	localTransform.SetPosition(x, y, z);

	OnUpdateTransform();
}

void GameObject::SetPosition(const XMFLOAT3& pos)
{
	localTransform.SetPosition(pos);

	OnUpdateTransform();
}

void GameObject::SetPosition(XMFLOAT3&& pos)
{
	localTransform.SetPosition(std::forward<XMFLOAT3>(pos));

	OnUpdateTransform();
}

void GameObject::SetScale(float x, float y, float z)
{
	localTransform.SetScale(x, y, z);

	OnUpdateTransform();
}

void GameObject::SetRotation(const XMFLOAT4X4& tfrm)
{
	localTransform.SetRotation(tfrm);

	OnUpdateTransform();
}

void GameObject::SetRotation(XMFLOAT4X4&& tfrm)
{
	localTransform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));

	OnUpdateTransform();
}

void GameObject::LookTo(XMFLOAT3& to, XMFLOAT3& up)
{
	localTransform.LookTo(to, up);

	OnUpdateTransform();
}

void GameObject::LookAt(XMFLOAT3& from, XMFLOAT3& up)
{
	localTransform.LookAt(from, up);

	OnUpdateTransform();
}

void GameObject::AddPosition(const XMFLOAT3& vector)
{
	localTransform.Translate(vector);

	OnUpdateTransform();
}

void GameObject::AddPosition(XMFLOAT3&& vector)
{
	localTransform.Translate(std::forward<XMFLOAT3>(vector));

	OnUpdateTransform();
}

void GameObject::Move(const XMFLOAT3& dir, float distance)
{
	localTransform.Move(dir, distance);

	OnUpdateTransform();
}

void GameObject::Move(XMFLOAT3&& direction, float distance)
{
	localTransform.Move(std::forward<XMFLOAT3>(direction), distance);

	OnUpdateTransform();
}

void GameObject::MoveStrafe(float distance)
{
	localTransform.MoveStrafe(distance);

	OnUpdateTransform();
}

void GameObject::MoveUp(float distance)
{
	localTransform.MoveUp(distance);

	OnUpdateTransform();
}

void GameObject::MoveForward(float distance)
{
	localTransform.MoveForward(distance);

	OnUpdateTransform();
}

void GameObject::Rotate(float pitch, float yaw, float roll)
{
	localTransform.Rotate(pitch, yaw, roll);

	OnUpdateTransform();
}

void GameObject::Rotate(const XMFLOAT3& axis, float angle)
{
	localTransform.Rotate(axis, angle);

	OnUpdateTransform();
}

void GameObject::Rotate(const XMFLOAT4& quaternion)
{
	localTransform.Rotate(quaternion);

	OnUpdateTransform();
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3(localTransform.GetPosition());
}

XMFLOAT3 GameObject::GetRight() const
{
	return Vector3::Normalize(XMFLOAT3(localTransform.GetRight()));
}

XMFLOAT3 GameObject::GetUp() const
{
	return Vector3::Normalize(XMFLOAT3(localTransform.GetUp()));
}

XMFLOAT3 GameObject::GetLook() const
{
	return Vector3::Normalize(XMFLOAT3(localTransform.GetLook()));
}

void GameObject::BeginUpdateTransform()
{
	const auto& my_mat = localTransform.GetWorldMatrix();

	worldTransform.SetWorldMatrix(my_mat);
	UpdateCollider(my_mat);

	if (mySibling)
	{
		mySibling->BeginUpdateTransform();
	}

	if (myChild)
	{
		myChild->UpdateTransform(worldTransform);
	}
}

void GameObject::UpdateTransform(const Transformer& parent)
{
	auto& my_mat = localTransform.GetWorldMatrix();
	const auto& parent_mat = parent.GetWorldMatrix();
	const auto result = Matrix4x4::Multiply(my_mat, parent_mat);

	worldTransform.SetWorldMatrix(result);
	UpdateCollider(result);

	if (mySibling)
	{
		mySibling->UpdateTransform(parent);
	}

	if (myChild)
	{
		myChild->UpdateTransform(worldTransform);
	}
}

void GameObject::UpdateCollider(const XMFLOAT4X4& mat)
{
	if (myCollider)
	{
		const auto float4x4 = DirectX::XMLoadFloat4x4(&mat);

		const auto& original_collider = staticCollider;
		original_collider->Transform(*myCollider, float4x4);

		auto& angle = myCollider->Orientation;

		const auto orientation = DirectX::XMLoadFloat4(&angle);
		const auto quaternion = DirectX::XMQuaternionNormalize(orientation);
		DirectX::XMStoreFloat4(&angle, quaternion);
	}
}

void GameObject::OnUpdateTransform()
{
	BeginUpdateTransform();
}

void GameObject::UpdateBoundingBox()
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

void GameObject::PrintFrameInfo(GameObject* root, GameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), root, pParent);
	OutputDebugString(pstrDebug);

	if (root->mySibling) GameObject::PrintFrameInfo(root->mySibling, pParent);
	if (root->myChild) GameObject::PrintFrameInfo(root->myChild, root);
}

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
		auto xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);

		auto& rot = m_pMainRotorFrame->localTransform;
		auto& rot_mat = rot.GetWorldMatrix();

		rot_mat = Matrix4x4::Multiply(xmmtxRotate, rot_mat);
	}

	if (m_pTailRotorFrame)
	{
		auto xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);

		auto& rot = m_pMainRotorFrame->localTransform;
		auto& rot_mat = rot.GetWorldMatrix();

		rot_mat = Matrix4x4::Multiply(xmmtxRotate, rot_mat);
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
		auto xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);

		auto& rot = m_pMainRotorFrame->localTransform;
		auto& rot_mat = rot.GetWorldMatrix();

		rot_mat = Matrix4x4::Multiply(xmmtxRotate, rot_mat);
	}

	if (m_pTailRotorFrame)
	{
		auto xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);

		auto& rot = m_pMainRotorFrame->localTransform;
		auto& rot_mat = rot.GetWorldMatrix();

		rot_mat = Matrix4x4::Multiply(xmmtxRotate, rot_mat);
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

