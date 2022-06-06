#include "pch.hpp"
#include "CubeMesh.hpp"
#include "Vertex.hpp"

CubeMesh::CubeMesh(float w, float h, float d)
	: CMesh(6)
{
	float fHalfWidth = w * 0.5f;
	float fHalfHeight = h * 0.5f;
	float fHalfDepth = d * 0.5f;

	CPolygon pFrontFace(4);
	pFrontFace.Push(CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace.Push(CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace.Push(CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace.Push(CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	Push(std::move(pFrontFace));
	//Set(0, std::move(pFrontFace));

	CPolygon pTopFace(4);
	pTopFace.Push(CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace.Push(CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace.Push(CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace.Push(CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	Push(std::move(pTopFace));
	//Set(0, std::move(pTopFace));

	CPolygon pBackFace(4);
	pBackFace.Push(CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace.Push(CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace.Push(CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace.Push(CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	Push(std::move(pBackFace));
	//Set(2, std::move(pBackFace));

	CPolygon pBottomFace(4);
	pBottomFace.Push(CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace.Push(CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace.Push(CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace.Push(CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	Push(std::move(pBottomFace));
	//Set(3, std::move(pBottomFace));

	CPolygon pLeftFace(4);
	pLeftFace.Push(CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace.Push(CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace.Push(CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace.Push(CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	Push(std::move(pLeftFace));
	//Set(4, std::move(pLeftFace));

	CPolygon pRightFace(4);
	pRightFace.Push(CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace.Push(CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace.Push(CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace.Push(CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	Push(std::move(pRightFace));
	//Set(5, std::move(pRightFace));

	BuildFragments();

	staticCollider = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}
