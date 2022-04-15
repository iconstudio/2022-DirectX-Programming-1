#include "pch.h"
#include "DirectObject.hpp"

DirectObject::DirectObject()
	: Device(nullptr)
{}

void DirectObject::SetDevice(ID3D12Device* device)
{
	Device = device;
}
