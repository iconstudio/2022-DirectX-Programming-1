#pragma once
#include "GameObject.hpp"

class Model : public GameObject
{
public:
	static Model* Load(P3DDevice device
		, P3DGrpCommandList cmd_list
		, CShader* pipeline
		, const char* pstrFileName);

	static Model* LoadFrameHierarchyFromFile(P3DDevice device
		, P3DGrpCommandList cmd_list
		, CShader* pipeline
		, FILE* pInFile);

	static CMeshLoadInfo* LoadRawMesh(FILE* pInFile);
	static RawMaterialsBox* LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmd_list, FILE* pInFile);

};
