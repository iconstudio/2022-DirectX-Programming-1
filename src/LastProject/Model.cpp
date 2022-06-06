#include "pch.hpp"
#include "Model.hpp"

Model* Model::Load(P3DDevice device
	, P3DGrpCommandList cmdlist
	, Pipeline* pipeline
	, const char* filepath)
{
	FILE* pInFile = NULL;
	fopen_s(&pInFile, filepath, "rb");
	if (!pInFile)
	{
		throw "모델 파일을 불러올 수 없습니다!";
	}

	rewind(pInFile);

	Model* root = nullptr;
	char token[64] = { '\0' };

	while (true)
	{
		ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Hierarchy>:"))
		{
			root = LoadFrameHierarchyFromFile(device, cmdlist, pipeline, pInFile);
		}
		else if (!strcmp(token, "</Hierarchy>"))
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	OutputDebugString(L"Frame Hierarchy\n");

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

std::vector<RawMaterial*> Model::LoadRawMaterials(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlist, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int mat_index = 0;
	RawMaterial* raw_mat = nullptr;

	std::vector<RawMaterial*> result{};
	result.reserve(ReadIntegerFromFile(pInFile));

	for (; ; )
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Material>:"))
		{
			raw_mat = new RawMaterial;
			result.push_back(raw_mat);

			mat_index = ReadIntegerFromFile(pInFile);
		}
		else if (!strcmp(token, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(token, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(raw_mat->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(token, "</Materials>"))
		{
			break;
		}
	}
	return result;
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
			if (!root)
			{
				throw "게임 객체가 생성되지 않음!";
			}

			auto raw_mesh = root->LoadRawMesh(pInFile);
			if (raw_mesh)
			{
				Mesh* mesh = nullptr;
				if (raw_mesh->m_nType & VERTEXT_NORMAL)
				{
					mesh = new CIlluminatedMesh(device, cmdlist, raw_mesh);
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
			const auto mat_count = raw_materials.size();

			if (0 < mat_count)
			{
				if (!root)
				{
					throw "메쉬를 불러오는 중에 객체가 생성되지 않는 문제 발생!";
				}

				auto& target = root->myMaterials;
				target.reserve(mat_count);
				
				auto inserter = std::back_inserter(target);
				std::transform(raw_materials.begin(), raw_materials.end()
					, inserter
					, [&](RawMaterial* raw) {
					auto mat = new CMaterial;

					auto mat_color = new CMaterialColors(raw);
					mat->SetMaterialColors(mat_color);

					if (root->GetMeshType() & VERTEXT_NORMAL)
					{
						mat->SetShader(pipeline);
					}

					return mat;
				});

				// 원본 재질 삭제
				raw_materials.erase(raw_materials.begin(), raw_materials.end());
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
