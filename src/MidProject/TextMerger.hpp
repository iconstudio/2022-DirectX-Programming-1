#pragma once
#include "Mesh.h"

class TextMerger
{
public:
	TextMerger(const char* text, float size, XMVECTORF32 color);

	std::vector<std::string> Awake();
	void Start(P3DDevice device, P3DGrpCommandList cmdlist);

	const std::string myText;
	const float lineWidth;
	const XMFLOAT4 myColour;
	
	std::vector<std::string> myStructure;
	const float textWidth, textHeight;

	CPlainMesh* myMerger;

private:
	const char* Build(const char one) const;
	const XMFLOAT3* MakeRect(const XMFLOAT3 offset) const;
};
