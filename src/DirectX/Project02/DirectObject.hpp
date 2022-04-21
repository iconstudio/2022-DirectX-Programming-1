#pragma once
#include "pch.h"

class DirectObject
{
public:
	DirectObject();

	void SetDevice(ID3D12Device* device);

	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;

protected:
	ID3D12Device* Device;
};

