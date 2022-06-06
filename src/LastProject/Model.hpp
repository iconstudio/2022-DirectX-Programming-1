#pragma once
#include "GameObject.hpp"

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
		, FILE* pInFile);

	static RawMesh* LoadRawMesh(FILE* pInFile);
	static std::vector<RawMaterial*> LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmdlist, FILE* pInFile);
};
