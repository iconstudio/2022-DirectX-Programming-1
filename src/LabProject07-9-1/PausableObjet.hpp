#pragma once
#include "PlayableObjet.hpp"

class PausableObjet : public PlayableObjet
{
public:
	PausableObjet();
	PausableObjet(bool paused);

	virtual void Pause();
	virtual void Resume();
	virtual bool TryPause();

	bool IsPaused() const;

private:
	bool isPaused;
};

