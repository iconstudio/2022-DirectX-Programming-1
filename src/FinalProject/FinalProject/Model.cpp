#include "pch.hpp"
#include "Model.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"

Model* Load(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmdlist
	, const char* pstrFileName)
{
	FILE* pInFile = NULL;
	fopen_s(&pInFile, pstrFileName, "rb");
	if (!pInFile)
	{
		throw "모델 파일을 불러올 수 없습니다!";
	}

	rewind(pInFile);

	Model* root = nullptr;
	char token[64] = { '\0' };

	while (true)
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Hierarchy>:"))
		{
			root = LoadFrameHierarchyFromFile(device, cmdlist, pInFile);
		}
		else if (!strcmp(token, "</Hierarchy>"))
		{
			break;
		}
	}

	OutputDebugString(L"Frame Hierarchy\n");

	//GameObject::PrintFrameInfo(root, NULL);

	return root;
}

RawMesh* LoadRawMesh(FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;

	RawMesh* raw = new RawMesh;

	raw->numberVertices = ReadIntegerFromFile(pInFile);
	ReadStringFromFile(pInFile, raw->myName);

	while (true)
	{
		::ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Bounds>:"))
		{
			nReads = (UINT)::fread(&(raw->m_xmf3AABBCenter), sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&(raw->m_xmf3AABBExtents), sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(token, "<Positions>:"))
		{
			nPositions = ReadIntegerFromFile(pInFile);

			if (nPositions > 0)
			{
				raw->m_nType |= VERTEXT_POSITION;
				raw->m_pxmf3Positions = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(raw->m_pxmf3Positions, sizeof(XMFLOAT3), nPositions, pInFile);
			}
		}
		else if (!strcmp(token, "<Colors>:"))
		{
			nColors = ReadIntegerFromFile(pInFile);

			if (nColors > 0)
			{
				raw->m_nType |= VERTEXT_COLOR;
				raw->m_pxmf4Colors = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(raw->m_pxmf4Colors, sizeof(XMFLOAT4), nColors, pInFile);
			}
		}
		else if (!strcmp(token, "<Normals>:"))
		{
			nNormals = ReadIntegerFromFile(pInFile);

			if (nNormals > 0)
			{
				raw->m_nType |= VERTEXT_NORMAL;
				raw->m_pxmf3Normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(raw->m_pxmf3Normals, sizeof(XMFLOAT3), nNormals, pInFile);
			}
		}
		else if (!strcmp(token, "<Indices>:"))
		{
			nIndices = ReadIntegerFromFile(pInFile);

			// 모든 메쉬에서 색인된 정점 불러오기
			if (nIndices > 0)
			{
				raw->m_pnIndices = new UINT[nIndices];
				nReads = (UINT)::fread(raw->m_pnIndices, sizeof(int), nIndices, pInFile);
			}
		}
		else if (!strcmp(token, "<SubMeshes>:"))
		{
			const auto sub_count = ReadIntegerFromFile(pInFile);
			raw->numberSubMeshes = sub_count;

			if (0 < sub_count)
			{
				raw->numberSubMeshesIndexes = new int[sub_count] {};
				raw->indexSubMeshes = new UINT * [sub_count] {};

				for (int i = 0; i < sub_count; i++)
				{
					ReadStringFromFile(pInFile, token);

					if (!strcmp(token, "<SubMesh>:"))
					{
						int nIndex = ReadIntegerFromFile(pInFile);
						const auto sub_index_count = ReadIntegerFromFile(pInFile);

						raw->numberSubMeshesIndexes[i] = sub_index_count;

						if (0 < sub_index_count)
						{
							raw->indexSubMeshes[i] = new UINT[sub_index_count];

							// 정점의 인덱스 불러오기
							nReads = (UINT)fread(raw->indexSubMeshes[i], sizeof(int), sub_index_count, pInFile);
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

	return (raw);
}

std::vector<RawMaterial*> LoadRawMaterials(ID3D12Device* device, ID3D12GraphicsCommandList* cmdlist, FILE* pInFile)
{
	char token[64] = { '\0' };
	UINT nReads = 0;

	std::vector<RawMaterial*> result{};

	const auto materials_count = ReadIntegerFromFile(pInFile);
	result.reserve(materials_count);

	int process_index = 0;
	RawMaterial* process_mat = nullptr;

	while (true)
	{
		ReadStringFromFile(pInFile, token);

		if (!strcmp(token, "<Material>:"))
		{
			process_index = ReadIntegerFromFile(pInFile);
			process_mat = new RawMaterial(0);

			result[process_index] = process_mat;
		}
		else
		{
			if (!process_mat)
			{
				throw "원본 재질이 생성되지 않음!";
			}

			if (!strcmp(token, "<AlbedoColor>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<EmissiveColor>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<SpecularColor>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
			}
			else if (!strcmp(token, "<Glossiness>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_fGlossiness), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<Smoothness>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_fSmoothness), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<Metallic>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_fSpecularHighlight), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<SpecularHighlight>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_fMetallic), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "<GlossyReflection>:"))
			{
				nReads = (UINT)fread(&(process_mat->m_fGlossyReflection), sizeof(float), 1, pInFile);
			}
			else if (!strcmp(token, "</Materials>"))
			{
				break;
			}
		}
	}

	return result;
}

Model* LoadFrameHierarchyFromFile(ID3D12Device* device
	, ID3D12GraphicsCommandList* cmdlist
	, FILE* pInFile)
{
	UINT count_reads = 0;
	int nFrame = 0;

	Model* root = nullptr;
	Mesh* skeleton = nullptr;

	std::string token{};
	token.reserve(20);

	for (; ; )
	{
		ReadStringFromFile(pInFile, token);
		if ("<Frame>:" == token)
		{
			// 모델 객체 생성 및 이름 할당
			root = new Model();

			nFrame = ReadIntegerFromFile(pInFile);
			ReadStringFromFile(pInFile, root->myName);
		}
		else if ("<Transform>:" == token)
		{
			if (!root)
			{
				throw "모델 객체가 생성되지 않음!";
			}

			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;

			constexpr auto stride = sizeof(float);
			count_reads = (UINT)::fread(&xmf3Position, stride, 3, pInFile);
			count_reads = (UINT)::fread(&xmf3Rotation, stride, 3, pInFile); //Euler Angle
			count_reads = (UINT)::fread(&xmf3Scale, stride, 3, pInFile);
			count_reads = (UINT)::fread(&xmf4Rotation, stride, 4, pInFile); //Quaternion
		}
		else if ("<TransformMatrix>:" == token)
		{
			if (!root)
			{
				throw "모델 객체가 생성되지 않음!";
			}

			auto& root_transform = root->localTransform;
			auto& root_mat = root_transform.GetWorldMatrix();
			count_reads = (UINT)::fread(&root_mat, sizeof(float), 16, pInFile);
		}
		else if ("<Mesh>:" == token)
		{
			if (!root)
			{
				throw "모델 객체가 생성되지 않음!";
			}

			// 원본 모델 불러오기
			auto raw = LoadRawMesh(pInFile);

			if (raw)
			{
				if (raw->m_nType & VERTEXT_NORMAL)
				{
					// 메쉬 생성
					skeleton = new Mesh(device, cmdlist, raw);
				}

				if (skeleton)
				{
					// 모델에 메쉬 할당
					root->SetMesh(skeleton);
				}

				// 원본 모델 삭제
				delete raw;
			}
			else
			{
				throw "모델 내용이 없음!";
			}
		}
		else if ("<Materials>:" == token)
		{
			if (!root)
			{
				throw "메쉬를 불러오는 중에 객체가 생성되지 않는 문제 발생!";
			}

			if (!skeleton)
			{
				throw "원본 메쉬가 존재하지 않음!";
			}

			// 원본 재질 불러오기
			auto raw_materials = LoadRawMaterials(device, cmdlist, pInFile);
			const auto raw_count = raw_materials.size();

			if (0 < raw_count)
			{
				auto& target = skeleton->myMaterials;
				target.reserve(raw_count);

				auto inserter = std::back_inserter(target);
				std::transform(raw_materials.begin(), raw_materials.end()
					, inserter
					, [&](RawMaterial* raw) {
					return new Material(raw);
				});

				// 원본 재질 삭제
				raw_materials.erase(raw_materials.begin(), raw_materials.end());
			}
		}
		else if ("<Children>:" == token)
		{
			if (!root)
			{
				throw "메쉬를 불러오는 중에 객체가 생성되지 않는 문제 발생!";
			}

			int children_count = ReadIntegerFromFile(pInFile);

			if (0 < children_count)
			{
				for (int i = 0; i < children_count; i++)
				{
					auto child = LoadFrameHierarchyFromFile(device, cmdlist, pInFile);

					if (child)
					{
						root->Attach(child);
					}

#ifdef _WITH_DEBUG_RUNTIME_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, _T("(Child Frame: %p) (Parent Frame: %p)\n"), child, root);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if ("</Frame>" == token)
		{
			break;
		}
	}

	return root;
}
