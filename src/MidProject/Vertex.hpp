#pragma once

class CVertex
{
public:
	CVertex();
	CVertex(float x, float y, float z);
	CVertex(float position[3]);
	CVertex(const XMFLOAT3& position);
	CVertex(XMFLOAT3&& position);
	virtual ~CVertex();

	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex(const CVertex& position);
	CDiffusedVertex(CVertex&& position);
	CDiffusedVertex(const XMFLOAT4 colour);
	CDiffusedVertex(const CVertex& position, const XMFLOAT4 colour);
	CDiffusedVertex(CVertex&& position, const XMFLOAT4 colour);
	virtual ~CDiffusedVertex();

	XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	CLightenVertex(const CVertex& position);
	CLightenVertex(CVertex&& position);
	CLightenVertex(const CVertex& position, const XMFLOAT3 normal);
	CLightenVertex(CVertex&& position, const XMFLOAT3 normal);
	virtual ~CLightenVertex();

	XMFLOAT3 myNormal;
};

