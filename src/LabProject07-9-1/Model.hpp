#pragma once
#include "Object.h"

class Model : public GameObject
{
public:
	static Model* Load(P3DDevice device, P3DGrpCommandList cmd_list, const char* pstrFileName);

	static Model* LoadFrameHierarchyFromFile(P3DDevice device, P3DGrpCommandList cmd_list, FILE* pInFile);

	static CMeshLoadInfo* LoadRawMesh(FILE* pInFile);
	static RawMaterialsBox* LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmd_list, FILE* pInFile);

};
