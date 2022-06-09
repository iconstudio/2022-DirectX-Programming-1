#include "pch.hpp"
#include "Model.hpp"

Model* Model::Load(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmdlist
	, Pipeline* pipeline
	, const char* pstrFileName)
{
	FILE* pInFile = NULL;
	fopen_s(&pInFile, pstrFileName, "rb");
	if (!pInFile)
	{
		throw "모델 파일을 불러올 수 없습니다!";
	}

	rewind(pInFile);

	Model* root = NULL;
	char token[64] = { '\0' };

	while (true)
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Hierarchy>:"))
		{
			root = Model::LoadFrameHierarchyFromFile(device, cmdlist, pipeline, pInFile);
		}
		else if (!strcmp(token, "</Hierarchy>"))
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, _T("Frame Hierarchy\n"));
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

	pMeshInfo->m_nVertices = ::ReadIntegerFromFile(pInFile);
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
				pMeshInfo->m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(token, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(pInFile);
			if (nColors > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_COLOR;
				pMeshInfo->m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(token, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(pInFile);
			if (nNormals > 0)
			{
				pMeshInfo->m_nType |= VERTEXT_NORMAL;
				pMeshInfo->m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(pMeshInfo->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
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
			pMeshInfo->countPolygons = ::ReadIntegerFromFile(pInFile);
			if (pMeshInfo->countPolygons > 0)
			{
				pMeshInfo->countPolygonIndices = new int[pMeshInfo->countPolygons];
				pMeshInfo->m_ppnSubSetIndices = new UINT * [pMeshInfo->countPolygons];
				for (int i = 0; i < pMeshInfo->countPolygons; i++)
				{
					::ReadStringFromFile(pInFile, token);
					if (!strcmp(token, "<SubMesh>:"))
					{
						int nIndex = ::ReadIntegerFromFile(pInFile);
						pMeshInfo->countPolygonIndices[i] = ::ReadIntegerFromFile(pInFile);
						if (pMeshInfo->countPolygonIndices[i] > 0)
						{
							pMeshInfo->m_ppnSubSetIndices[i] = new UINT[pMeshInfo->countPolygonIndices[i]];
							nReads = (UINT)::fread(pMeshInfo->m_ppnSubSetIndices[i], sizeof(int), pMeshInfo->countPolygonIndices[i], pInFile);
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

RawMaterialsBox* Model::LoadRawMaterials(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlist, FILE* pInFile)
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
	, ID3D12GraphicsCommandList* cmdlist
	, Pipeline* pipeline
	, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0;

	Model* root = nullptr;
	CMaterialMesh* mesh = nullptr;

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
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(token, "<TransformMatrix>:"))
		{
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			nReads = (UINT)::fread(&root->localTransform, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(token, "<Mesh>:"))
		{
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			auto raw_mesh = root->LoadRawMesh(pInFile);

			if (raw_mesh)
			{
				if (raw_mesh->m_nType & VERTEXT_NORMAL)
				{
					mesh = new CLightenMesh(device, cmdlist, raw_mesh);
				}

				if (mesh)
				{
					root->SetMesh(mesh);
				}

				delete raw_mesh;
			}
		}
		else if (!strcmp(token, "<Materials>:"))
		{
			auto raw_materials = root->LoadRawMaterials(device, cmdlist, pInFile);
			if (raw_materials && (raw_materials->m_nMaterials > 0))
			{
				if (!root)
				{
					throw "메쉬를 불러오는 중에 객체가 생성되지 않는 문제 발생!";
				}

				if (!mesh)
				{
					throw "재질을 저장할 메쉬가 생성되지 않음!";
				}

				mesh->m_nMaterials = raw_materials->m_nMaterials;
				mesh->m_ppMaterials = new CMaterial * [raw_materials->m_nMaterials];

				for (int i = 0; i < raw_materials->m_nMaterials; i++)
				{
					mesh->m_ppMaterials[i] = NULL;

					CMaterial* pMaterial = new CMaterial();

					CMaterialColors* pMaterialColors = new CMaterialColors(&raw_materials->m_pMaterials[i]);
					pMaterial->SetMaterialColors(pMaterialColors);

					if (root->GetMeshType() & VERTEXT_NORMAL)
					{
						pMaterial->SetShader(pipeline);
					}

					mesh->SetMaterial(i, pMaterial);
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
					auto pChild = Model::LoadFrameHierarchyFromFile(device, cmdlist, pipeline, pInFile);

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
