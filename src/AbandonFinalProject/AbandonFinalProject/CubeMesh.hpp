#pragma once
#include "Mesh.hpp"

class CubeMesh : public Mesh
{
public:
	CubeMesh();
	virtual ~CubeMesh();

	virtual void Awake(P3DDevice device, P3DGrpCommandList cmdlist) override;
};
