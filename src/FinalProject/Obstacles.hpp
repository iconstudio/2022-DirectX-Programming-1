#pragma once
#include "Object.h"

class Rock : public GameObject
{
public:
	virtual constexpr COLLISION_TAGS GetTag() const noexcept
	{
		return COLLISION_TAGS::ROCK;
	}
};

class Tree : public GameObject
{
public:
	virtual constexpr COLLISION_TAGS GetTag() const noexcept
	{
		return COLLISION_TAGS::TREE;
	}
};