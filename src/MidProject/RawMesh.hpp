#pragma once
#include "Vertex.hpp"
#include "Polygon.hpp"

class RawMesh
{
public:
	RawMesh();
	~RawMesh();

	void ReservePolygons(const size_t count);

	template<typename VertexType>
		requires(std::is_base_of_v<CVertex, VertexType>)
	constexpr void AddVertex(const VertexType& vbo);
	template<typename VertexType>
		requires(std::is_base_of_v<CVertex, VertexType>)
	constexpr void AddVertex(VertexType&& vbo);
	void AddPolygon(const CPolygon& polygon);
	void AddPolygon(CPolygon&& polygon);

	const CPolygon& PolygonAt(const size_t index) const;
	CPolygon& PolygonAt(const size_t index);

	char m_pstrMeshName[256] = { 0 };

	UINT myVertexType = 0x00;

	XMFLOAT3 m_xmf3AABBCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_xmf3AABBExtents = XMFLOAT3(0.0f, 0.0f, 0.0f);

	int countVertices = 0;
	std::vector<XMFLOAT3> myPositions;
	std::vector<XMFLOAT4> myColours;
	std::vector<XMFLOAT3> myNormals;

	int m_nIndices = 0;
	UINT* m_pnIndices = NULL;

	size_t countPolygons = 0;
	std::vector<CPolygon> myPolygons;
};

template<typename VertexType>
	requires(std::is_base_of_v<CVertex, VertexType>)
constexpr void RawMesh::AddVertex(const VertexType& vbo)
{
	if constexpr (std::is_same_v<VertexType, CVertex>)
	{
		countVertices++;

		myPositions.push_back(vbo.myPosition);
	}
	else if constexpr (std::is_same_v<VertexType, CDiffusedVertex>)
	{
		countVertices++;

		myPositions.push_back(vbo.myPosition);
		myColours.push_back(vbo.myColour);
	}
	else if constexpr (std::is_same_v<VertexType, CLightenVertex>)
	{
		countVertices++;

		myPositions.push_back(vbo.myPosition);
		myColours.push_back(vbo.myColour);
	}
	else
	{
		countVertices++;

		myPositions.push_back(vbo.myPosition);
		myColours.push_back(vbo.myColour);
		myNormals.push_back(vbo.myNormal);
	}
}

template<typename VertexType>
	requires(std::is_base_of_v<CVertex, VertexType>)
constexpr void RawMesh::AddVertex(VertexType&& vbo)
{
	if constexpr (std::is_same_v<VertexType, CVertex>)
	{
		countVertices++;

		myPositions.push_back(std::forward<XMFLOAT3>(vbo.myPosition));
	}
	else if constexpr (std::is_same_v<VertexType, CDiffusedVertex>)
	{
		countVertices++;

		myPositions.push_back(std::forward<XMFLOAT3>(vbo.myPosition));
		myColours.push_back(std::forward<XMFLOAT4>(vbo.myColour));
	}
	else if constexpr (std::is_same_v<VertexType, CLightenVertex>)
	{
		countVertices++;

		myPositions.push_back(std::forward<XMFLOAT3>(vbo.myPosition));
		myColours.push_back(std::forward<XMFLOAT3>(vbo.myNormal));
	}
	else
	{
		countVertices++;

		myPositions.push_back(std::forward<XMFLOAT3>(vbo.myPosition));
		myColours.push_back(std::forward<XMFLOAT4>(vbo.myColour));
		myNormals.push_back(std::forward<XMFLOAT3>(vbo.myNormal));
	}
}
