#pragma once

class PlayableObjet
{
public:
	virtual ~PlayableObjet();

	virtual void Awake() = 0;
	virtual void Start() = 0;
	virtual void Reset() = 0;
	virtual void Update(float delta_time) = 0;
};
