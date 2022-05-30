#include "stdafx.h"
#include "TaggedObject.hpp"
#include "GameFramework.h"

TaggedObject::TaggedObject(GameFramework& framework, const char* name)
	: myFramework(framework), myName(name)
{}

TaggedObject::~TaggedObject()
{}

const std::string& TaggedObject::GetName() const noexcept
{
	return myName;
}

