#pragma once
#include "Object.h"

class Model : public GameObject
{
public:
	static Model* Load(P3DDevice device
		, P3DGrpCommandList cmdlist
		, Pipeline* pipeline
		, const char* pstrFileName);

	static Model* LoadFrameHierarchyFromFile(P3DDevice device
		, P3DGrpCommandList cmdlist
		, Pipeline* pipeline
		, FILE* pInFile);

	static RawMesh* LoadRawMesh(FILE* pInFile);
	static RawMaterialsBox* LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmdlist, FILE* pInFile);

};
