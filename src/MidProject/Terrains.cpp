#include "pch.hpp"
#include "Terrains.hpp"
#include "Arithmetics.hpp"
#include <limits>

TerrainData::TerrainData(size_t w, size_t h)
	: myHeightMap(), myColourMap()
	, myMapWidth(w), myMapHeight(h), m_xmf3Scale()
{}

TerrainData::~TerrainData()
{
	myHeightMap.clear();
	myColourMap.clear();
}

void TerrainData::Awake(const Filepath& image)
{
	if (!std::filesystem::exists(image))
	{
		throw "���� ������ ã�� �� ����!";
	}

	std::ifstream reader{ image, std::ios::in | std::ios::binary };
	if (!reader)
	{
		throw "���� ���� �д� ���� ����!";
	}

	myHeightMap.resize(myMapHeight);
	myColourMap.resize(myMapHeight);

	BYTE byte_read{};
	constexpr auto byte_max = std::numeric_limits<BYTE>::max();

	for (size_t k = 0; k < myMapHeight; ++k)
	{
		auto& row = myHeightMap.at(k);
		row.reserve(myMapWidth);

		auto& color_row = myColourMap.at(k);
		color_row.reserve(myMapWidth);

		for (size_t t = 0; t < myMapWidth; ++t)
		{
			reader >> byte_read;
			row.push_back(float(byte_read) / float(byte_max));
		}
	}
	reader.close();
}

void TerrainData::Start(const XMFLOAT3& scale)
{
	m_xmf3Scale = scale;

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
			my_height_item = cy; // ���� ����
			height_index_x++;

			pos = CVertex(cx, cy, cz);
			CDiffusedVertex result{ pos, XMFLOAT4(Colors::Aquamarine) };
			//CLightenVertex result{ pos, Transformer::Up };
			myRawMesh.AddVertex(result);

			cx += scale.x;
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

BYTE TerrainData::GetRawHeight(int x, int z) const
{
	return myHeightMap.at(z).at(x);
}

float TerrainData::GetActualHeight(float x, float z, bool reverse) const
{
	return 0.0f;
}

XMFLOAT4 TerrainData::GetColor(int x, int z) const
{
	return myColourMap.at(z).at(x);
}

XMFLOAT3 TerrainData::GetNormal(int x, int z) const
{
	if (x < 0.0f
		|| z < 0.0f
		|| x >= myMapWidth
		|| z >= myMapHeight)
	{
		return Transformer::Up;
	}

	int xHeightMapAdd = x < myMapWidth - 1 ? 1 : -1;
	int zHeightMapAdd = z < myMapHeight - 1 ? 1 : -1;

	const auto y1 = GetHeight(x, z);
	const auto y2 = GetHeight(x + xHeightMapAdd, z);
	const auto y3 = GetHeight(x, z + zHeightMapAdd);

	XMFLOAT3 edge1 = { 0.0f, y3 - y1, m_xmf3Scale.z };
	XMFLOAT3 edge2 = { m_xmf3Scale.x, y2 - y1, 0.0f };

	return Vector3::CrossProduct(edge1, edge2);
}

Terrain::Terrain(size_t w, size_t h)
	: myData(w, h), countVertices(0), myMesh(nullptr)
{}

Terrain::~Terrain()
{}

void Terrain::Awake(const Filepath& image)
{
	myData.Awake(image);
}

void Terrain::Start(P3DDevice device, P3DGrpCommandList cmdlist, const XMFLOAT3& scale)
{
	myData.Start(scale);

	const auto& raw_terrain = myData.myRawMesh;
	myMesh = new CDiffusedMesh(device, cmdlist, raw_terrain);
	if (!myMesh)
	{
		throw "������ �޽� ���� ����!";
	}
	myMesh->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	const auto& w = myData.myMapWidth;
	const auto& h = myData.myMapHeight;
	const auto& heights = myData.myHeightMap;

	countVertices = ((w * 2) * (h - 1)) + ((h - 1) - 1);
	UINT* pnIndices = new UINT[countVertices];
	UINT temp;

	// ���� ����
	//CPolygon terrain_poly{};
	for (int j = 0, z = 0; z < int(h) - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (size_t x = 0; x < w; x++)
			{
				if (x == 0 && 0 < z) // �ι�° ��
				{
					temp = (UINT)(x + (z * w));
					//terrain_poly.Add(temp);
					pnIndices[j++] = temp;
				}
				pnIndices[j++] = (UINT)(x + (z * w));
				pnIndices[j++] = (UINT)((x + (z * w)) + w);
			}
		}
		else
		{
			for (int x = int(w) - 1; 0 <= x; x--)
			{
				if (x == w - 1) // ������ ��
				{
					pnIndices[j++] = UINT(x + (z * w));
				}

				pnIndices[j++] = UINT(x + (z * w));
				pnIndices[j++] = UINT((x + (z * w)) + w);
			}
		}
	}

	myMesh->countPolygons = 1;
	myMesh->myIndexBuffers = new ID3D12Resource * [1];
	myMesh->myUploadingIndexBuffer = new ID3D12Resource * [1];
	myMesh->myIndexBufferViews = new D3D12_INDEX_BUFFER_VIEW[1];

	const auto indice_size = sizeof(UINT) * countVertices;

	auto& index_buffer = myMesh->myIndexBuffers[0];
	auto& buffer_view = myMesh->myIndexBufferViews[0];
	auto& upload_buffer = myMesh->myUploadingIndexBuffer[0];

	index_buffer = CreateBufferResource(device, cmdlist
		, pnIndices, indice_size
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, &upload_buffer);

	buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
	buffer_view.Format = DXGI_FORMAT_R32_UINT;
	buffer_view.SizeInBytes = static_cast<UINT>(indice_size);
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
		myMesh->RenderIndexed(cmdlist, 0, countVertices);
	}
}

float Terrain::GetHeight(int x, int z) const
{
	return 0.0f;
}

float Terrain::GetHeight(float x, float z) const
{
	return 0.0f;
}

XMFLOAT4 Terrain::GetColor(int x, int z) const
{
	return XMFLOAT4();
}

XMFLOAT3 Terrain::GetNormal(int x, int z) const
{
	return XMFLOAT3();
}
