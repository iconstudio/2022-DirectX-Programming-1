#pragma once

class TaggedObject
{
public:
	TaggedObject(GameFramework& framework, const char* name);
	virtual ~TaggedObject();

	virtual const std::string& GetName() const noexcept;

	GameFramework& myFramework;
	const std::string myName;
};
