#pragma once
#include "PlayableObjet.hpp"

class GameObject : public PlayableObjet
{
public:

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Reset() override;
	virtual void Update(float delta_time) override;
};

