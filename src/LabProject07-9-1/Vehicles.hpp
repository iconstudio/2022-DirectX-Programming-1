#pragma once
#include "Object.h"

class Vehicles : public GameObject
{
public:
	virtual constexpr COLLISION_TAGS GetTag() const noexcept
	{
		return COLLISION_TAGS::CAR;
	}
};
