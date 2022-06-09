#include "pch.hpp"
#include "Model.hpp"

Model* Model::Load(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmd_list
	, Pipeline* pipeline
	, const char* pstrFileName)
{
	FILE* pInFile = NULL;
	fopen_s(&pInFile, pstrFileName, "rb");
	if (!pInFile)
	{
		throw "�� ������ �ҷ��� �� �����ϴ�!";
	}

	rewind(pInFile);

	Model* root = NULL;
	char token[64] = { '\0' };

	while (true)
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Hierarchy>:"))
		{
			root = Model::LoadFrameHierarchyFromFile(device, cmd_list, pipeline, pInFile);
		}
		else if (!strcmp(token, "</Hierarchy>"))
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	wsprintf(pstrDebug, L"Frame Hierarchy\n");
	OutputDebugString(pstrDebug);

	GameObject::PrintFrameInfo(root, NULL);
#endif

	return root;
}

RawMesh* Model::LoadRawMesh(FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	RawMesh* pMeshInfo = new RawMesh;

	pMeshInfo->numberMyVertex = ::ReadIntegerFromFile(pInFile);
	::ReadStringFromFile(pInFile, pMeshInfo->m_pstrMeshName);

	for (; ; )
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(pMeshInfo->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(token, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(pInFile);
			if (nPositions > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_POSITION;
				pMeshInfo->myPositions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pMeshInfo->myPositions, sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(token, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			if (nColors > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_COLOR;
				pMeshInfo->myColours = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pMeshInfo->myColours, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(token, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			if (nNormals > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_NORMAL;
				pMeshInfo->myNormals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pMeshInfo->myNormals, sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(token, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(pInFile);
			if (nIndices > 0)
			{
				pMeshInfo->m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(pMeshInfo->m_pnIndices, sizeof(int), nIndices, pInFile);
			}
		}
		else if (!strcmp(token, "<SubMeshes>:"))
		{
			pMeshInfo->numberMyMesh = ::ReadIntegerFromFile(pInFile);
			if (pMeshInfo->numberMyMesh > 0)
			{
				pMeshInfo->m_pnSubSetIndices = new int[pMeshInfo->numberMyMesh];
				pMeshInfo->m_ppnSubSetIndices = new UINT * [pMeshInfo->numberMyMesh];
				for (int i = 0; i < pMeshInfo->numberMyMesh; i++)
				{
					::ReadStringFromFile(pInFile, token);
					if (!strcmp(token, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(pInFile);
						pMeshInfo->m_pnSubSetIndices[i] = ::ReadIntegerFromFile(pInFile);
						if (pMeshInfo->m_pnSubSetIndices[i] > 0)
						{
							pMeshInfo->m_ppnSubSetIndices[i] = new UINT[pMeshInfo->m_pnSubSetIndices[i]];
							nReads = (UINT)::fread(pMeshInfo->m_ppnSubSetIndices[i], sizeof(int), pMeshInfo->m_pnSubSetIndices[i], pInFile);
						}

					}
				}
			}
		}
		else if (!strcmp(token, "</Mesh>"))
		{
			break;
		}
	}
	return(pMeshInfo);
}

RawMaterialsBox* Model::LoadRawMaterials(ID3D12Device* device, ID3D12GraphicsCommandList* cmd_list, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nMaterial = 0;

	RawMaterialsBox* pMaterialsInfo = new RawMaterialsBox;

	pMaterialsInfo->m_nMaterials = ::ReadIntegerFromFile(pInFile);
	pMaterialsInfo->m_pMaterials = new RawMaterial[pMaterialsInfo->m_nMaterials];

	for (; ; )
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Material>:"))
		{
			nMaterial = ::ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(token, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterialsInfo->m_pMaterials[nMaterial].m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "</Materials>"))
		{
			break;
		}
	}
	return(pMaterialsInfo);
}

Model* Model::LoadFrameHierarchyFromFile(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmd_list
	, Pipeline* pipeline
	, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0;

	Model* root = nullptr;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, token);
		if (!strcmp(token, "<Frame>:"))
		{
			root = new Model();

			nFrame = ::ReadIntegerFromFile(pInFile);
			::ReadStringFromFile(pInFile, root->m_pstrFrameName);
		}
		else if (!strcmp(token, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(token, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&root->localTransform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(token, "<Mesh>:"))
		{
			RawMesh* pMeshInfo = root->LoadRawMesh(pInFile);
			if (pMeshInfo)
			{
				Mesh* pMesh = NULL;
				if (pMeshInfo->m_nType & VERTEXT_NORMAL)
				{
					pMesh = new CIlluminatedMesh(device, cmd_list, pMeshInfo);
				}
				if (pMesh) root->SetMesh(pMesh);
				delete pMeshInfo;
			}
		}
		else if (!strcmp(token, "<Materials>:"))
		{
			RawMaterialsBox* pMaterialsInfo = root->LoadRawMaterials(device, cmd_list, pInFile);
			if (pMaterialsInfo && (pMaterialsInfo->m_nMaterials > 0))
			{
				if (!root)
				{
					throw "�޽��� �ҷ����� �߿� ��ü�� �������� �ʴ� ���� �߻�!";
				}

				root->m_nMaterials = pMaterialsInfo->m_nMaterials;
				root->m_ppMaterials = new CMaterial*[pMaterialsInfo->m_nMaterials];

				for (int i = 0; i < pMaterialsInfo->m_nMaterials; i++)
				{
					root->m_ppMaterials[i] = NULL;

					CMaterial* pMaterial = new CMaterial();

					CMaterialColors* pMaterialColors = new CMaterialColors(&pMaterialsInfo->m_pMaterials[i]);
					pMaterial->SetMaterialColors(pMaterialColors);

					if (root->GetMeshType() & VERTEXT_NORMAL)
					{
						pMaterial->SetShader(pipeline);
					}

					root->SetMaterial(i, pMaterial);
				}
			}
		}
		else if (!strcmp(token, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (0 < nChilds)
			{
				for (int i = 0; i < nChilds; i++)
				{
					auto pChild = Model::LoadFrameHierarchyFromFile(device, cmd_list, pipeline, pInFile);

					if (pChild)
					{
						root->Attach(pChild);
					}

#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), pChild, root);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(token, "</Frame>"))
		{
			break;
		}
	}

	return root;
}