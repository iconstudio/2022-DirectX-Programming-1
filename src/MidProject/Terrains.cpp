#include "pch.hpp"
#include "Terrains.hpp"

TerrainData::TerrainData(size_t w, size_t h, XMFLOAT3 scale)
	: myHeightMap(), myColourMap()
	, myWidth(w), myHeight(h), m_xmf3Scale(scale)
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
		throw "지형 파일을 찾을 수 없음!";
	}

	std::ifstream reader{ image, std::ios::in | std::ios::binary };
	if (!reader)
	{
		throw "지형 파일 읽는 데에 실패!";
	}

	myHeightMap.resize(myHeight);
	myColourMap.resize(myHeight);

	BYTE byte_read{};
	for (size_t k = 0; k < myHeight; ++k)
	{
		auto& row = myHeightMap.at(k);
		row.reserve(myWidth);

		auto& color_row = myColourMap.at(k);
		color_row.reserve(myWidth);

		for (size_t t = 0; t < myWidth; ++t)
		{
			reader >> byte_read;
			row.push_back(byte_read);
		}
	}
	reader.close();
}

BYTE TerrainData::GetHeight(int x, int z) const
{
	return myHeightMap.at(z).at(x);
}

XMFLOAT4 TerrainData::GetColor(int x, int z) const
{
	return myColourMap.at(z).at(x);
}

TerrainMesh::TerrainMesh()
	: myRawMesh(), myMesh(nullptr)
{}

TerrainMesh::~TerrainMesh()
{}

void TerrainMesh::Awake(const TerrainData& data)
{
	const auto& w = data.myWidth;
	const auto& h = data.myHeight;
	const auto& heights = data.myHeightMap;
	const auto& scale = data.m_xmf3Scale;
	
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

			height_index_x++;

			pos = CVertex(cx, cy, cz);

			CLightenVertex result{ pos, Transformer::Up };

			myRawMesh.AddVertex(result);

			cx += scale.x;
		}

		cx = 0.0f;
		cz += scale.z;

		height_index_x = 0;
		height_index_z++;
	}
}

float TerrainMesh::GetPeekHeight(int x, int z) const
{
	return myHeights.at(z).at(x);
}

float TerrainMesh::GetHeight(float x, float z) const
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= m_nWidth) || (z >= m_nLength)) return(XMFLOAT3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * m_nWidth);
	int xHeightMapAdd = (x < (m_nWidth - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (m_nLength - 1)) ? m_nWidth : -m_nWidth;
	float y1 = (float)myHeightMap[nHeightMapIndex] * m_xmf3Scale.y;
	float y2 = (float)myHeightMap[nHeightMapIndex + xHeightMapAdd] * m_xmf3Scale.y;
	float y3 = (float)myHeightMap[nHeightMapIndex + zHeightMapAdd] * m_xmf3Scale.y;
	XMFLOAT3 xmf3Edge1 = XMFLOAT3(0.0f, y3 - y1, m_xmf3Scale.z);
	XMFLOAT3 xmf3Edge2 = XMFLOAT3(m_xmf3Scale.x, y2 - y1, 0.0f);
	XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge1, xmf3Edge2, true);

	return(xmf3Normal);

}

Terrain::Terrain()
{}

Terrain::~Terrain()
{}

void Terrain::Awake(const TerrainMesh & data)
{}

void Terrain::PrepareRendering(P3DGrpCommandList cmdlist) const
{}

void Terrain::Render(P3DGrpCommandList cmdlist) const
{}
