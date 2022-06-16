#include "pch.hpp"
#include "Terrains.hpp"
#include "Pipeline.hpp"
#include "Arithmetics.hpp"
#include <limits>

TerrainData::TerrainData(size_t w, size_t h)
	: myHeightMap()
	, myMapWidth(w), myMapHeight(h), myScale()
{}

TerrainData::~TerrainData()
{
	myHeightMap.clear();
}

void TerrainData::Awake(const Filepath& image)
{
	if (!std::filesystem::exists(image))
	{
		throw "지형 파일을 찾을 수 없음!";
	}

	std::ifstream reader{ image, std::ios::in | std::ios::binary };
	if (!reader)
	{
		throw "지형 파일 읽는 데에 실패!";
	}

	myHeightMap.resize(myMapHeight);

	BYTE byte_read{};
	constexpr auto byte_max = std::numeric_limits<BYTE>::max();

	for (size_t k = 0; k < myMapHeight; ++k)
	{
		auto& row = myHeightMap.at(k);
		row.reserve(myMapWidth);

		for (size_t t = 0; t < myMapWidth; ++t)
		{
			reader >> byte_read;

			const auto height_normalized = float(byte_read) / float(byte_max);
			row.push_back(height_normalized);
		}
	}
	reader.close();
}

void TerrainData::Start(const XMFLOAT3& scale)
{
	myScale = scale;

	const auto& w = myMapWidth;
	const auto& h = myMapHeight;
	const auto& heights = myHeightMap;

	myHeights.resize(h);

	float cx{}, cy{}, cz{};
	CVertex pos;

	size_t height_index_x = 0, height_index_z = 0;
	for (const auto& row : heights)
	{
		auto& my_heights_row = myHeights.at(height_index_z);
		my_heights_row.resize(w);

		for (const auto& height : row)
		{
			cy = height * scale.y;

			auto& my_height_item = my_heights_row.at(height_index_x);
			my_height_item = cy; // 높이 저장

			pos = CVertex(cx, cy, cz);
			//CDiffusedVertex result{ pos, XMFLOAT4(Colors::Aquamarine) };

			const auto norm = GetNormal(int(height_index_x), int(height_index_z));

			CLightenVertex result{ pos, norm };
			myRawMesh.AddVertex(result);

			cx += scale.x;
			height_index_x++;
		}

		cx = 0.0f;
		cz += scale.z;

		height_index_x = 0;
		height_index_z++;
	}
}

float TerrainData::GetHeight(int x, int z) const
{
	return myHeights.at(z).at(x);
}

float TerrainData::GetRawHeight(int x, int z) const
{
	return myHeightMap.at(z).at(x);
}

float TerrainData::GetActualHeight(float fx, float fz, bool reverse) const
{
	fx = fx / myScale.x;
	fz = fz / myScale.z;

	auto x = static_cast<size_t>(fx);
	auto z = static_cast<size_t>(fz);

	const auto& w = myMapWidth;
	const auto& h = myMapHeight;
	const auto& heights = myHeightMap;

	if (x < 0 || z < 0 || w <= x || h <= z)
	{
		return 0.0f;
	}

	const float x_ratio = fx - int(fx);
	const float z_ratio = fz - int(fz);

	float height_bl = GetRawHeight(x, z);
	float height_br = GetRawHeight(x + 1, z);
	float height_tl = GetRawHeight(x, z + 1);
	float height_tr = GetRawHeight(x + 1, z + 1);
	if (reverse)
	{
		if (x_ratio <= z_ratio)
			height_br = height_bl + (height_tr - height_tl);
		else
			height_tl = height_tr + (height_bl - height_br);
	}
	else
	{
		if (z_ratio < 1.0f - x_ratio)
			height_tr = height_tl + (height_br - height_bl);
		else
			height_bl = height_tl + (height_br - height_tr);
	}

	float height_top = height_tl * (1 - x_ratio) + height_tr * x_ratio;
	float height_bot = height_bl * (1 - x_ratio) + height_br * x_ratio;
	float result = height_bot * (1 - z_ratio) + height_top * z_ratio;

	return result * myScale.y;
}

XMFLOAT3 TerrainData::GetNormal(int x, int z) const
{
	if (x < 0.0f || z < 0.0f || myMapWidth <= x || myMapHeight <= z)
	{
		return Transformer::Up;
	}

	int x_add = x < myMapWidth - 1 ? 1 : -1;
	int z_add = z < myMapHeight - 1 ? 1 : -1;

	const auto& yscale = myScale.y;
	const auto y1 = GetRawHeight(x, z) * yscale;
	const auto y2 = GetRawHeight(x + x_add, z) * yscale;
	const auto y3 = GetRawHeight(x, z + z_add) * yscale;

	XMFLOAT3 edge1 = { 0.0f, y3 - y1, myScale.z };
	XMFLOAT3 edge2 = { myScale.x, y2 - y1, 0.0f };

	return Vector3::CrossProduct(edge1, edge2);
}

Terrain::Terrain(size_t w, size_t h)
	: myData(w, h), countVertices(0), myMesh(nullptr)
{}

Terrain::~Terrain()
{}

void Terrain::ReleaseUploadBuffer()
{
	if (myMesh)
	{
		if (myMesh->myUploadingIndexBuffer)
		{
			if (myMesh->myUploadingIndexBuffer[0])
			{
				myMesh->myUploadingIndexBuffer[0]->Release();
			}

			delete[] myMesh->myUploadingIndexBuffer;
			myMesh->myUploadingIndexBuffer = nullptr;
		}
	}
}

void Terrain::Awake(const Filepath& image)
{
	myData.Awake(image);
}

void Terrain::Start(P3DDevice device, P3DGrpCommandList cmdlist, const XMFLOAT3& scale)
{
	myData.Start(scale);

	const auto& w = myData.myMapWidth;
	const auto& h = myData.myMapHeight;
	const auto& heights = myData.myHeightMap;

	countVertices = ((w * 2) * (h - 1)) + ((h - 1) - 1);
	auto indices = new UINT[countVertices];
	UINT temp;

	// 정점 색인
	CPolygon terrain_poly{};
	for (int j = 0, z = 0; z < int(h) - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (size_t x = 0; x < w; x++)
			{
				if (x == 0 && 0 < z) // 두번째 줄
				{
					temp = UINT(x + z * w);
					indices[j++] = temp;
				}

				temp = UINT(x + z * w);
				indices[j++] = temp;

				temp = UINT(x + z * w + w);
				indices[j++] = temp;
			}
		}
		else
		{
			for (int x = int(w) - 1; 0 <= x; x--)
			{
				if (x == w - 1) // 마지막 열
				{
					temp = UINT(x + z * w);
					indices[j++] = temp;
				}

				temp = UINT(x + z * w);
				indices[j++] = temp;

				temp = UINT(x + z * w + w);
				indices[j++] = temp;
			}
		}
	}

	auto& raw_terrain = myData.myRawMesh;
	//raw_terrain.AddPolygon(terrain_poly);

	myMesh = new CLightenMesh(device, cmdlist, raw_terrain);
	if (!myMesh)
	{
		throw "지형의 메쉬 생성 실패!";
	}
	myMesh->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	auto terrain_mat = make_shared<CMaterial>();
	terrain_mat->m_xmf4Diffuse = XMFLOAT4(Colors::Chocolate);
	terrain_mat->m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	terrain_mat->m_xmf4Emissive = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	terrain_mat->m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	terrain_mat->SetShader(Pipeline::illuminatedShader);

	myMesh->AddMaterial(terrain_mat);
	myMesh->myDefaultMaterial = terrain_mat;

	myMesh->countPolygons = 1;
	myMesh->myIndexBuffers = new ID3D12Resource * [1];
	myMesh->myUploadingIndexBuffer = new ID3D12Resource * [1];
	myMesh->myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[1];

	const auto indice_size = sizeof(UINT) * countVertices;

	auto& index_buffer = myMesh->myIndexBuffers[0];
	auto& buffer_view = myMesh->myIndexBufferViews[0];
	auto& upload_buffer = myMesh->myUploadingIndexBuffer[0];

	index_buffer = CreateBufferResource(device, cmdlist
		, indices, indice_size
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, &upload_buffer);

	buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	buffer_view.Format = DXGI_FORMAT_R32_UINT;
	buffer_view.SizeInBytes = static_cast<UINT>(indice_size);

	delete[] indices;
}

void Terrain::PrepareRendering(P3DGrpCommandList cmdlist) const
{
	if (myMesh)
	{
		myMesh->PrepareRendering(cmdlist);
	}
}

void Terrain::Render(P3DGrpCommandList cmdlist) const
{
	PrepareRendering(cmdlist);

	if (myMesh)
	{
		auto& proceed_mat = myMesh->myDefaultMaterial;
		auto& pipeline = proceed_mat->m_pShader;

		if (pipeline)
		{
			pipeline->PrepareRendering(cmdlist);
			proceed_mat->PrepareRendering(cmdlist);

			myMesh->RenderIndexed(cmdlist, 0, countVertices);
		}
		else
		{
			throw "파이프라인과 쉐이더가 존재하지 않음!";
		}
	}
}

float Terrain::GetHeight(int x, int z) const
{
	return myData.GetHeight(x, z);
}

float Terrain::GetHeight(float x, float z, bool reverse) const
{
	return myData.GetActualHeight(x, z, reverse);
}

float Terrain::GetRawHeight(int x, int z) const
{
	return myData.GetRawHeight(x, z);
}

XMFLOAT3 Terrain::GetNormal(int x, int z) const
{
	return XMFLOAT3();
}
