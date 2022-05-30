#pragma once
#include "Object.h"

class Vehicles : public GameObject
{
	virtual constexpr COLLISION_TAGS GetTag() const noexcept;
};
