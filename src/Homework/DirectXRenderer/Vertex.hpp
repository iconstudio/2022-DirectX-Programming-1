#pragma once

class CVertex
{
public:
	CVertex();
	CVertex(float x, float y, float z);
	CVertex(const XMFLOAT3& position);
	CVertex(XMFLOAT3&& position);
	virtual ~CVertex();

	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	CDiffusedVertex(XMFLOAT3 color);
	CDiffusedVertex(float x, float y, float z, XMFLOAT3 color);
	CDiffusedVertex(const XMFLOAT3& position, XMFLOAT3 color);
	CDiffusedVertex(XMFLOAT3&& position, XMFLOAT3 color);
	virtual ~CDiffusedVertex();

	XMFLOAT3 myColour;
};

class CLightenVertex : public CVertex
{
public:
	CLightenVertex(XMFLOAT3 norm);
	CLightenVertex(float x, float y, float z, XMFLOAT3 norm);
	CLightenVertex(const XMFLOAT3& position, XMFLOAT3 norm);
	CLightenVertex(XMFLOAT3&& position, XMFLOAT3 norm);
	virtual ~CLightenVertex();

	XMFLOAT3 myNormal;
};
