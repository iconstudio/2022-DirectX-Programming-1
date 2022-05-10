#include "stdafx.hpp"
#include "PlaneMesh.hpp"
#include "GameTransform.hpp"

PlaneMesh::PlaneMesh(const float width, const float height)
	: CMesh(1)
{
	float fHalfWidth = width * 0.5f;
	float fHalfDepth = height * 0.5f;

	CPolygon face(4);
	face.Push(CVertex(-fHalfWidth, 0, -fHalfDepth));
	face.Push(CVertex(+fHalfWidth, 0, -fHalfDepth));
	face.Push(CVertex(+fHalfWidth, 0, +fHalfDepth));
	face.Push(CVertex(-fHalfWidth, 0, +fHalfDepth));
	Push(std::move(face));

	BuildFragments();
}
