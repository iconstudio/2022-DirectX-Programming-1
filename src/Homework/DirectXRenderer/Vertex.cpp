#include "pch.hpp"
#include "Vertex.hpp"

CVertex::CVertex()
	: myPosition()
{}

CVertex::CVertex(const float x, const float y, const float z)
	: myPosition(x, y, z)
{}

CVertex::CVertex(const XMFLOAT3& position)
	: myPosition(position)
{}

CVertex::CVertex(XMFLOAT3&& position)
	: myPosition(std::forward<XMFLOAT3>(position))
{}

CVertex::~CVertex()
{}

CDiffusedVertex::CDiffusedVertex(XMFLOAT3 color)
	: CVertex()
	, myColour(color)
{}

CDiffusedVertex::CDiffusedVertex(float x, float y, float z, XMFLOAT3 color)
	: CVertex(x, y, z)
	, myColour(color)
{}

CDiffusedVertex::CDiffusedVertex(const XMFLOAT3& position, XMFLOAT3 color)
	: CVertex(position)
	, myColour(color)
{}

CDiffusedVertex::CDiffusedVertex(XMFLOAT3&& position, XMFLOAT3 color)
	: CVertex(std::forward<XMFLOAT3>(position))
	, myColour(color)
{}

CDiffusedVertex::~CDiffusedVertex()
{}

CLightenVertex::CLightenVertex(XMFLOAT3 norm)
	: CVertex()
	, myNormal(norm)
{}

CLightenVertex::CLightenVertex(float x, float y, float z, XMFLOAT3 norm)
	: CVertex(x, y, z)
	, myNormal(norm)
{}

CLightenVertex::CLightenVertex(const XMFLOAT3& position, XMFLOAT3 norm)
	: CVertex(position)
	, myNormal(norm)
{}

CLightenVertex::CLightenVertex(XMFLOAT3&& position, XMFLOAT3 norm)
	: CVertex(std::forward<XMFLOAT3>(position))
	, myNormal(norm)
{}

CLightenVertex::~CLightenVertex()
{}
