#pragma once
#include "GameObject.hpp"

class Model : public GameObject
{
public:
};

Model* Load(P3DDevice device
	, P3DGrpCommandList cmdlist
	, const char* filepath);

Model* LoadFrameHierarchyFromFile(P3DDevice device
	, P3DGrpCommandList cmdlist
	, FILE* pInFile);

RawMesh* LoadRawMesh(FILE* pInFile);
std::vector<RawMaterial*> LoadRawMaterials(P3DDevice device, P3DGrpCommandList cmdlist, FILE* pInFile);

int ReadIntegerFromFile(FILE* file);
float ReadFloatFromFile(FILE* file);
BYTE ReadStringFromFile(FILE* file, char* token);
BYTE ReadStringFromFile(FILE* file, std::string& out);
