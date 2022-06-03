#include "pch.hpp"
#include "PausableObjet.hpp"

PausableObjet::PausableObjet()
	: isPaused(false)
{}

PausableObjet::PausableObjet(bool paused)
	: isPaused(paused)
{
	if (isPaused)
	{
		Pause();
	}
}

void PausableObjet::Pause()
{
	isPaused = true;
}

void PausableObjet::Resume()
{
	isPaused = false;
}

bool PausableObjet::TryPause()
{
	if (isPaused)
	{
		return false;
	}
	else
	{
		Pause();
		return true;
	}
}

bool PausableObjet::IsPaused() const
{
	return isPaused;
}
