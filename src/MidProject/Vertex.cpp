#include "pch.hpp"
#include "Vertex.hpp"

CVertex::CVertex()
{}

CVertex::CVertex(float x, float y, float z)
{}

CVertex::CVertex(float position[3])
{}

CVertex::CVertex(const XMFLOAT3 & position)
{}

CVertex::CVertex(XMFLOAT3 && position)
{}

CVertex::~CVertex()
{}

CDiffusedVertex::CDiffusedVertex(const CVertex& position)
{}

CDiffusedVertex::CDiffusedVertex(CVertex && position)
{}

CDiffusedVertex::CDiffusedVertex(const XMFLOAT4 colour)
{}

CDiffusedVertex::CDiffusedVertex(const CVertex & position, const XMFLOAT4 colour)
{}

CDiffusedVertex::CDiffusedVertex(CVertex && position, const XMFLOAT4 colour)
{}

CDiffusedVertex::~CDiffusedVertex()
{}

CLightenVertex::CLightenVertex(const CVertex& position)
{}

CLightenVertex::CLightenVertex(CVertex && position)
{}

CLightenVertex::CLightenVertex(const CVertex & position, const XMFLOAT3 normal)
{}

CLightenVertex::CLightenVertex(CVertex && position, const XMFLOAT3 normal)
{}

CLightenVertex::~CLightenVertex()
{}
