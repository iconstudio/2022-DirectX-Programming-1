#pragma once
#include "Mesh.h"

class CubeMesh : public Mesh
{
	CubeMesh(P3DDevice device, P3DGrpCommandList cmdlist, float w, float h, float d);

	virtual void ReleaseUploadBuffers();

};
