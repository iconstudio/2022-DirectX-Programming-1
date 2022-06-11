#include "pch.hpp"
#include "RawMesh.hpp"
#include "Model.hpp"
#include "Mesh.h"

Model* Model::Load(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmdlist
	, Pipeline* pipeline
	, const char* filepath)
{
	if (!std::filesystem::exists(filepath))
	{
		throw "모델 파일을 찾을 수 없음!";
	}

	FILE* pfile = nullptr;
	fopen_s(&pfile, filepath, "rb");
	if (!pfile)
	{
		throw "모델 파일을 불러올 수 없습니다!";
	}
	rewind(pfile);

	char token[64] = { '\0' };

	Model* root = nullptr;
	while (true)
	{
		ReadStringFromFile(pfile, token);

		if (!strcmp(token, "<Hierarchy>:"))
		{
			root = Model::LoadFrameHierarchyFromFile(device, cmdlist, pipeline, pfile);
		}
		else if (!strcmp(token, "</Hierarchy>"))
		{
			break;
		}
	}

	if (!root)
	{
		throw "모델 객체가 생성되지 않음!";
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	root->PrintFrameInfo();
#endif

	return root;
}

RawMesh* Model::LoadRawMesh(FILE* mfile)
{
	char token[64] = { '\0' };

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
			auto count = ReadIntegerFromFile(mfile);
			if (0 < count)
			{
				raw_mesh->myVertexType |= VERTEXT_POSITION;

				auto& containter = raw_mesh->myPositions;
				containter.reserve(count);

				XMFLOAT3 result{};
				for (int k = 0; k < count; ++k)
				{
					fread(&result, sizeof(XMFLOAT3), 1, mfile);

					containter.push_back(result);
				}
			}
		}
		else if (!strcmp(token, "<Colors>:"))
		{
			auto count = ReadIntegerFromFile(mfile);
			if (0 < count)
			{
				raw_mesh->myVertexType |= VERTEXT_COLOR;

				auto& containter = raw_mesh->myColours;
				containter.reserve(count);

				XMFLOAT4 result{};
				for (int k = 0; k < count; ++k)
				{
					fread(&result, sizeof(XMFLOAT4), 1, mfile);

					containter.push_back(result);
				}
			}
		}
		else if (!strcmp(token, "<Normals>:"))
		{
			auto count = ReadIntegerFromFile(mfile);
			if (0 < count)
			{
				raw_mesh->myVertexType |= VERTEXT_NORMAL;

				auto& containter = raw_mesh->myNormals;
				containter.reserve(count);

				XMFLOAT3 result{};
				for (int k = 0; k < count; ++k)
				{
					fread(&result, sizeof(XMFLOAT3), 1, mfile);

					containter.push_back(result);
				}
			}
		}
		else if (!strcmp(token, "<Indices>:"))
		{
			auto count = ReadIntegerFromFile(mfile);
			if (0 < count)
			{
				raw_mesh->m_pnIndices = new UINT[count];
				fread(raw_mesh->m_pnIndices, sizeof(int), count, mfile);
			}
		}
		else if (!strcmp(token, "<SubMeshes>:"))
		{
			const auto polygons_count = static_cast<size_t>(ReadIntegerFromFile(mfile));

			raw_mesh->countPolygons = polygons_count;
			if (0 < polygons_count)
			{
				raw_mesh->ReservePolygons(polygons_count);
				for (size_t i = 0; i < polygons_count; i++)
				{
					ReadStringFromFile(mfile, token);
					if (!strcmp(token, "<SubMesh>:"))
					{
						const int nIndex = ReadIntegerFromFile(mfile);
						auto& polygon = raw_mesh->PolygonAt(i);

						const size_t index_count = ReadIntegerFromFile(mfile);
						
						if (0 < index_count)
						{
							polygon.Reserve(index_count);

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
	, FILE* pfile)
{
	char token[64]{};
	UINT nReads = 0;

	int nFrame = 0;

	Model* root = nullptr;
	CLightenMesh* mesh = nullptr;

	while (true)
	{
		ReadStringFromFile(pfile, token);
		if (!strcmp(token, "<Frame>:"))
		{
			root = new Model();

			nFrame = ReadIntegerFromFile(pfile);

			char name[64]{};
			ReadStringFromFile(pfile, name);

			root->myName = name;
		}
		else if (!strcmp(token, "<Transform>:"))
		{
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pfile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pfile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pfile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pfile); //Quaternion
		}
		else if (!strcmp(token, "<TransformMatrix>:"))
		{
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			nReads = (UINT)::fread(&root->localMatrix, sizeof(float), 16, pfile);
		}
		else if (!strcmp(token, "<Mesh>:"))
		{
			if (!root)
			{
				throw "모델을 불러오는 중에 모델 객체가 생성되지 않음!";
			}

			auto raw_mesh = root->LoadRawMesh(pfile);

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
			auto raw_materials = root->LoadRawMaterials(device, cmdlist, pfile);
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

				raw_materials.erase(raw_materials.begin(), raw_materials.end());
			}
		}
		else if (!strcmp(token, "<Children>:"))
		{
			int children_count = ReadIntegerFromFile(pfile);
			if (0 < children_count)
			{
				for (int i = 0; i < children_count; i++)
				{
					if (auto child = Model::LoadFrameHierarchyFromFile(device, cmdlist, pipeline, pfile); child)
					{
						root->Attach(child);
					}

#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR debug_frame_info[256] = { 0 };
					_stprintf_s(debug_frame_info, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), child, root);
					OutputDebugString(debug_frame_info);
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
