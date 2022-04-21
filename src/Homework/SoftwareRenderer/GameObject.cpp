#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Mesh.hpp"

GameObject::GameObject()
{}

GameObject::~GameObject()
{}

void GameObject::SetActive(bool bActive)
{
	m_bActive = bActive;
}

void GameObject::SetMesh(CMesh* pMesh)
{
	m_pMesh = pMesh;
	if (pMesh)
	{
		pMesh->AddRef();
	}
}
