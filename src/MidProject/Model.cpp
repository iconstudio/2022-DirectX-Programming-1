#include "pch.hpp"
#include "RawMesh.hpp"
#include "Model.hpp"
#include "Pipeline.hpp"

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

RawMesh* Model::LoadRawMesh(FILE* mfile)
{
	char token[64] = { '\0' };

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	auto raw_mesh = new RawMesh;
	raw_mesh->countVertices = ReadIntegerFromFile(mfile);
	ReadStringFromFile(mfile, raw_mesh->m_pstrMeshName);

	while (true)
	{
		ReadStringFromFile(mfile, token);

		if (!strcmp(token, "<Bounds>:"))
		{
			fread(&(raw_mesh->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, mfile);
			fread(&(raw_mesh->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, mfile);
		}
		else if (!strcmp(token, "<Positions>:"))
		{
			nPositions = ::ReadIntegerFromFile(mfile);
			if (nPositions > 0)
			{
				raw_mesh->myVertexType |= VERTEXT_POSITION;
				raw_mesh->m_pxmf3Positions = new XMFLOAT3[nPositions];
				fread(raw_mesh->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, mfile);
			}
		}
		else if (!strcmp(token, "<Colors>:"))
		{
			nColors = ::ReadIntegerFromFile(mfile);
			if (nColors > 0)
			{
				raw_mesh->myVertexType |= VERTEXT_COLOR;
				raw_mesh->m_pxmf4Colors = new XMFLOAT4[nColors];
				fread(raw_mesh->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, mfile);
			}
		}
		else if (!strcmp(token, "<Normals>:"))
		{
			nNormals = ::ReadIntegerFromFile(mfile);
			if (nNormals > 0)
			{
				raw_mesh->myVertexType |= VERTEXT_NORMAL;
				raw_mesh->m_pxmf3Normals = new XMFLOAT3[nNormals];
				fread(raw_mesh->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, mfile);
			}
		}
		else if (!strcmp(token, "<Indices>:"))
		{
			nIndices = ::ReadIntegerFromFile(mfile);
			if (nIndices > 0)
			{
				raw_mesh->m_pnIndices = new UINT[nIndices];
				fread(raw_mesh->m_pnIndices, sizeof(int), nIndices, mfile);
			}
		}
		else if (!strcmp(token, "<SubMeshes>:"))
		{
			const auto polygons_count = static_cast<size_t>(ReadIntegerFromFile(mfile));

			raw_mesh->countPolygons = polygons_count;
			if (0 < polygons_count)
			{
				raw_mesh->ReservePolygons(polygons_count);

				//raw_mesh->countPolygonIndices = new int[raw_mesh->countPolygons];
				//raw_mesh->indexByPolygons = new UINT * [raw_mesh->countPolygons];
				for (size_t i = 0; i < polygons_count; i++)
				{
					ReadStringFromFile(mfile, token);
					if (!strcmp(token, "<SubMesh>:"))
					{
						const int nIndex = ReadIntegerFromFile(mfile);
						auto& polygon = raw_mesh->PolygonAt(i);

						const size_t index_count = ReadIntegerFromFile(mfile);
						
						//raw_mesh->countPolygonIndices[i] = index_count;
						if (0 < index_count)
						{
							polygon.Reserve(index_count);

							//raw_mesh->indexByPolygons[i] = new UINT[raw_mesh->countPolygonIndices[i]];

							UINT read_index{};
							for (size_t k = 0; k < index_count; ++k)
							{
								fread(&read_index, sizeof(int), 1, mfile);
								polygon.Add(read_index);
							}
						}
					} // a sub mesh
				} // for polygons_count
			}
		}
		else if (!strcmp(token, "</Mesh>"))
		{
			break;
		}
	}
	return raw_mesh;
}

std::vector<RawMaterial*> Model::LoadRawMaterials(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlist, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nMaterial = 0;

	const auto mat_count = ReadIntegerFromFile(pInFile);

	std::vector<RawMaterial*> result{};
	result.reserve(mat_count);

	RawMaterial* process = nullptr;

	while (0 < mat_count)
	{
		ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(pInFile);

			process = new RawMaterial;
			result.push_back(process);
		}
		else if (nullptr != process)
		{
			if (!strcmp(token, "<AlbedoColor>:"))
			{
				nReads = (UINT)::fread(&(process->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<EmissiveColor>:"))
			{
				nReads = (UINT)::fread(&(process->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<SpecularColor>:"))
			{
				nReads = (UINT)::fread(&(process->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<Glossiness>:"))
			{
				nReads = (UINT)::fread(&(process->m_fGlossiness), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<Smoothness>:"))
			{
				nReads = (UINT)::fread(&(process->m_fSmoothness), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<Metallic>:"))
			{
				nReads = (UINT)::fread(&(process->m_fSpecularHighlight), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<SpecularHighlight>:"))
			{
				nReads = (UINT)::fread(&(process->m_fMetallic), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<GlossyReflection>:"))
			{
				nReads = (UINT)::fread(&(process->m_fGlossyReflection), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "</Materials>"))
			{
				break;
			}
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
	CLightenMesh* mesh = nullptr;

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
				if (raw_mesh->myVertexType & VERTEXT_NORMAL)
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
			const auto raw_count = raw_materials.size();

			if (0 < raw_count)
			{
				if (!root)
				{
					throw "메쉬를 불러오는 중에 객체가 생성되지 않는 문제 발생!";
				}

				if (!mesh)
				{
					throw "재질을 저장할 메쉬가 생성되지 않음!";
				}

				mesh->AssignMaterial(raw_materials, pipeline);
				/*
				for (int i = 0; i < raw_count; i++)
				{
					mesh->m_ppMaterials[i] = NULL;

					auto pMaterial = new CMaterial;

					auto matcol = new CMaterialColors(raw_materials.at(i));
					pMaterial->SetMaterialColors(matcol);

					if (root->GetMeshType() & VERTEXT_NORMAL)
					{
						pMaterial->SetShader(pipeline);
					}

					mesh->SetMaterial(i, pMaterial);
				}
				*/
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
