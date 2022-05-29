#include "stdafx.h"
#include "Object.h"
#include "Material.hpp"
#include "Shader.h"

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

void GameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void GameObject::SetShader(Pipeline* pipeline)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial();
	m_ppMaterials[0]->SetShader(pipeline);
}

void GameObject::SetShader(int index, Pipeline* pipeline)
{
	if (m_ppMaterials[index])
	{
		m_ppMaterials[index]->SetShader(pipeline);
	}
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
	if (mySibling) mySibling->Animate(time_elapsed, parent);
	if (myChild) myChild->Animate(time_elapsed, &worldTransform);
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

	UpdateUniforms(cmd_list, &worldTransform);

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

void GameObject::BuildMaterials(P3DDevice device, P3DGrpCommandList cmd_list)
{}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	worldTransform = (pxmf4x4Parent) ? Matrix4x4::Multiply(localTransform, *pxmf4x4Parent) : localTransform;

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

void GameObject::PrintFrameInfo(GameObject* root, GameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), root, pParent);
	OutputDebugString(pstrDebug);

	if (root->mySibling) GameObject::PrintFrameInfo(root->mySibling, pParent);
	if (root->myChild) GameObject::PrintFrameInfo(root->myChild, root);
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

void CRotatingObject::Render(ID3D12GraphicsCommandList* cmd_list, GameCamera* pCamera)
{
	GameObject::Render(cmd_list, pCamera);
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

