#pragma once
#include "Object.h"

class Model : public GameObject
{
public:
	static Model* Load(P3DDevice device
		, P3DGrpCommandList cmdlist
		, Pipeline* pipeline
		, const char* filepath);

	static Model* LoadFrameHierarchyFromFile(P3DDevice device
		, P3DGrpCommandList cmdlist
		, Pipeline* pipeline
		, FILE* pfile);

	static RawMesh* LoadRawMesh(FILE* pfile);
	static std::vector<RawMaterial*> LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmdlist, FILE* pfile);

};
