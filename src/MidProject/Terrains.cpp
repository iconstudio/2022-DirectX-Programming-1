#include "pch.hpp"
#include "Terrains.hpp"

TerrainData::TerrainData(int w, int h, XMFLOAT3 scale)
	: m_pHeightMapPixels()
	, m_nWidth(w), m_nLength(h), m_xmf3Scale(scale)
{}

TerrainData::~TerrainData()
{
	m_pHeightMapPixels.clear();
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

	m_pHeightMapPixels.resize(m_nLength);

	BYTE byte_read{};
	for (int k = 0; k < m_nLength; ++k)
	{
		auto& row = m_pHeightMapPixels.at(k);
		row.reserve(m_nWidth);

		for (int t = 0; t < m_nWidth; ++t)
		{
			reader >> byte_read;
			row.push_back(byte_read);
		}
	}
	reader.close();
}

BYTE TerrainData::GetHeight(int x, int z) const
{
	return m_pHeightMapPixels.at(z).at(x);
}

TerrainMesh::TerrainMesh()
	: myRawMesh(nullptr)
{}

TerrainMesh::~TerrainMesh()
{}

Terrain::Terrain()
	: myMesh()
{}

Terrain::~Terrain()
{}
