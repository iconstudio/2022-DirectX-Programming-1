#include "pch.hpp"
#include "CubeMesh.hpp"

CubeMesh::CubeMesh(P3DDevice device, P3DGrpCommandList cmdlist)
	: Mesh()
{
	auto cube_origin = new RawMesh;

	CIlluminatedMesh::CIlluminatedMesh(device, cmdlist, cube_origin);
}

CubeMesh::CubeMesh(P3DDevice device, P3DGrpCommandList cmdlist, float w, float h, float d)
	: Mesh()
{}
