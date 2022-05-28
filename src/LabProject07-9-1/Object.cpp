#include "pch.hpp"
#include "stdafx.h"
#include "Object.h"
#include "Material.hpp"
#include "Shader.h"

GameObject::GameObject()
{
	m_xmf4x4Transform = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

GameObject::~GameObject()
{
	if (m_ppMaterials) delete[] m_ppMaterials;
}

void GameObject::SetChild(GameObject* pChild, bool bReferenceUpdate)
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

void GameObject::SetMesh(CMesh* pMesh)
{
	m_pMesh = pMesh;
}

void GameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	m_ppMaterials[nMaterial] = pMaterial;
}

void GameObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (mySibling) mySibling->Animate(fTimeElapsed, pxmf4x4Parent);
	if (myChild) myChild->Animate(fTimeElapsed, &m_xmf4x4World);
}

GameObject* GameObject::FindFrame(const char* pstrFrameName)
{
	if (0 == strcmp(m_pstrFrameName, pstrFrameName))
	{
		return this;
	}

	if (mySibling)
	{
		GameObject* pFrameObject = NULL;
		if (pFrameObject = mySibling->FindFrame(pstrFrameName))
			return(pFrameObject);
	}

	if (myChild)
	{
		GameObject* pFrameObject = NULL;
		if (pFrameObject = myChild->FindFrame(pstrFrameName))
			return(pFrameObject);
	}

	return(NULL);
}

void GameObject::Render(PtrGrpCommandList pd3dCommandList, GameCamera* pCamera)
{
	OnPrepareRender();

	UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

	if (m_nMaterials > 0)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i])
			{
				auto& pipeline = m_ppMaterials[i]->myPipeline;
				pipeline.PrepareRendering(pd3dCommandList, 0);

				m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
			}

			if (m_pMesh)
			{
				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (mySibling) mySibling->Render(pd3dCommandList, pCamera);
	if (myChild) myChild->Render(pd3dCommandList, pCamera);
}

void GameObject::CreateShaderVariables(PtrDevice pd3dDevice, PtrGrpCommandList pd3dCommandList)
{}

void GameObject::UpdateShaderVariables(PtrGrpCommandList pd3dCommandList)
{}

void GameObject::UpdateShaderVariable(PtrGrpCommandList pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void GameObject::UpdateShaderVariable(PtrGrpCommandList pd3dCommandList, CMaterial* pMaterial)
{}

void GameObject::ReleaseShaderVariables()
{}

void GameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();

	if (mySibling) mySibling->ReleaseUploadBuffers();
	if (myChild) myChild->ReleaseUploadBuffers();
}

void GameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4Transform, *pxmf4x4Parent) : m_xmf4x4Transform;

	if (mySibling) mySibling->UpdateTransform(pxmf4x4Parent);
	if (myChild) myChild->UpdateTransform(&m_xmf4x4World);
}

void GameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4Transform._41 = x;
	m_xmf4x4Transform._42 = y;
	m_xmf4x4Transform._43 = z;

	UpdateTransform(NULL);
}

void GameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void GameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxScale, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

XMFLOAT3 GameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 GameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 GameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 GameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
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
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void GameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4Transform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4Transform);

	UpdateTransform(NULL);
}

void GameObject::PrintFrameInfo(GameObject* parent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), this, parent);
	OutputDebugString(pstrDebug);

	if (mySibling)
	{
		mySibling->PrintFrameInfo(parent);
	}

	if (myChild)
	{
		myChild->PrintFrameInfo(this);
	}
}

void GameObject::PrintFrameInfo(GameObject* pParent)
{}

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

void CRotatingObject::Render(PtrGrpCommandList pd3dCommandList, GameCamera* pCamera)
{
	GameObject::Render(pd3dCommandList, pCamera);
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
	m_xmf4x4Transform = Matrix4x4::Multiply(m_xmf4x4Transform, mtxRotate);

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHellicopterObject::CHellicopterObject()
{}

CHellicopterObject::~CHellicopterObject()
{}

void CHellicopterObject::OnInitialize()
{}

void CHellicopterObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CApacheObject::CApacheObject()
{}

CApacheObject::~CApacheObject()
{}

void CApacheObject::OnInitialize()
{
	m_pMainRotorFrame = FindFrame("rotor");
	m_pTailRotorFrame = FindFrame("black_m_7");
}

void CApacheObject::Animate(float fTimeElapsed, XMFLOAT4X4* pxmf4x4Parent)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * fTimeElapsed);
		m_pMainRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->m_xmf4x4Transform);
	}
	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 4.0f) * fTimeElapsed);
		m_pTailRotorFrame->m_xmf4x4Transform = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->m_xmf4x4Transform);
	}

	GameObject::Animate(fTimeElapsed, pxmf4x4Parent);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSuperCobraObject::CSuperCobraObject()
{}

CSuperCobraObject::~CSuperCobraObject()
{}

void CSuperCobraObject::OnInitialize()
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

void CGunshipObject::OnInitialize()
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

void CMi24Object::OnInitialize()
{
	m_pMainRotorFrame = FindFrame("Top_Rotor");
	m_pTailRotorFrame = FindFrame("Tail_Rotor");
}

