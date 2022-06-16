#pragma once
#include "Mesh.h"
#include <array>

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
	std::array<XMFLOAT3, 4> MakeRect(const XMFLOAT3 offset) const;
};
