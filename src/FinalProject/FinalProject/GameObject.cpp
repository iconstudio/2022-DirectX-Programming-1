#include "pch.hpp"
#include "GameObject.hpp"
#include "Arithmetics.hpp"
#include "Material.hpp"
#include "Mesh.hpp"

GameObject::GameObject()
	: m_pstrFrameName()
	, localTransform(), worldTransform()
	, myParent(nullptr), mySibling(nullptr), myChild(nullptr)
	, m_nMaterials(0), m_ppMaterials(nullptr), m_pMesh(nullptr)
	, isActivated(true)
	, transformModified(false)
{}

GameObject::GameObject(float x, float y, float z)
	: GameObject()
{
	SetPosition(x, y, z, true);
}

GameObject::GameObject(float list[3])
	: GameObject()
{
	SetPosition(list[0], list[1], list[2], true);
}

GameObject::GameObject(const XMFLOAT3& position)
	: GameObject()
{
	SetPosition(position, true);
}

GameObject::GameObject(XMFLOAT3&& position)
	: GameObject()
{
	SetPosition(position, true);
}

GameObject::~GameObject()
{}

void GameObject::Awake()
{}

void GameObject::Start()
{}

void GameObject::Reset()
{}

void GameObject::Update(float delta_time)
{
	if (0.0f != delta_time)
	{
		if (transformModified)
		{
			UpdateTransform();
			transformModified = false;
		}
	}
}

void GameObject::PrepareRendering(P3DGrpCommandList cmdlist)
{
	XMFLOAT4X4 xmf4x4World{};

	const auto& my_mat = worldTransform.GetWorldMatrix();

	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&my_mat)));

	cmdlist->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void GameObject::Render(P3DGrpCommandList cmdlist)
{
	PrepareRendering(cmdlist);

	if (0 < m_nMaterials)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				m_ppMaterials[i]->PrepareRendering(cmdlist);
			}

			if (m_pMesh)
			{
				m_pMesh->Render(cmdlist, i);
			}
		}
	}

	if (mySibling)
	{
		mySibling->Render(cmdlist);
	}

	if (myChild) 
	{
		myChild->Render(cmdlist);
	}
}

void GameObject::SetActive(bool flag)
{
	isActivated = flag;

	if (flag)
	{
		OnUpdateTransform();
	}
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

GameObject* GameObject::FindFrame(const char* frame_name)
{
	if (frame_name == m_pstrFrameName)
	{
		return this;
	}

	if (mySibling)
	{
		if (auto root = mySibling->FindFrame(frame_name); root)
			return root;
	}

	if (myChild)
	{
		if (auto root = myChild->FindFrame(frame_name); root)
			return root;
	}

	return nullptr;
}

void GameObject::OnUpdateTransform()
{
	UpdateTransform();
}

void GameObject::UpdateTransform()
{
	const auto& my_mat = localTransform.GetWorldMatrix();
	worldTransform.SetWorldMatrix(my_mat);

	if (mySibling)
	{
		mySibling->UpdateTransform();
	}

	if (myChild)
	{
		myChild->UpdateTransform(my_mat);
	}
}

void GameObject::UpdateTransform(const XMFLOAT4X4& parent)
{
	const auto& my_mat = localTransform.GetWorldMatrix();
	const auto world_mat = Matrix4x4::Multiply(my_mat, parent);
	worldTransform.SetWorldMatrix(world_mat);

	if (mySibling)
	{
		mySibling->UpdateTransform(parent);
	}

	if (myChild)
	{
		myChild->UpdateTransform(world_mat);
	}
}

void GameObject::SetWorldMatrix(XMFLOAT4X4&& tfrm, bool begin)
{
	localTransform.SetWorldMatrix(std::forward<XMFLOAT4X4>(tfrm));

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::SetWorldMatrix(const XMFLOAT4X4& tfrm, bool begin)
{
	localTransform.SetWorldMatrix(tfrm);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::SetPosition(float x, float y, float z, bool begin)
{
	localTransform.SetPosition(x, y, z);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::SetPosition(const XMFLOAT3& pos, bool begin)
{
	SetPosition(std::move(XMFLOAT3(pos)), begin);
}

void GameObject::SetPosition(XMFLOAT3&& pos, bool begin)
{
	localTransform.SetPosition(std::forward<XMFLOAT3>(pos));

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::SetRotation(const XMFLOAT4X4& tfrm, bool begin)
{
	SetRotation(std::move(XMFLOAT4X4(tfrm)), begin);
}

void GameObject::SetRotation(XMFLOAT4X4&& tfrm, bool begin)
{
	localTransform.SetRotation(std::forward<XMFLOAT4X4>(tfrm));

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::LookTo(XMFLOAT3& to, XMFLOAT3& up, bool begin)
{
	localTransform.LookTo(to, up);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::LookAt(XMFLOAT3& from, XMFLOAT3& up, bool begin)
{
	localTransform.LookAt(from, up);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::AddPosition(XMFLOAT3&& vector, bool begin)
{
	localTransform.Translate(vector);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::Move(const XMFLOAT3& dir, float distance, bool begin)
{
	localTransform.Move(dir, distance);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::MoveStrafe(float distance, bool begin)
{
	localTransform.MoveStrafe(distance);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::MoveUp(float distance, bool begin)
{
	localTransform.MoveUp(distance);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::MoveForward(float distance, bool begin)
{
	localTransform.MoveForward(distance);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::Rotate(float pitch, float yaw, float roll, bool begin)
{
	localTransform.Rotate(pitch, yaw, roll);

	if (begin)
	{
		OnUpdateTransform();
	}
}

void GameObject::Rotate(const XMFLOAT3& axis, float angle, bool begin)
{
	localTransform.Rotate(axis, angle);

	if (begin)
	{
		OnUpdateTransform();
	}
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
