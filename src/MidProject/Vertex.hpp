#pragma once

class CVertex
{
public:
	XMFLOAT3 myPosition;
};

class CDiffusedVertex : public CVertex
{
public:
	XMFLOAT4 myColour;
};

class CLightenVertex : public CVertex
{
public:
	XMFLOAT3 myNormal;
};

