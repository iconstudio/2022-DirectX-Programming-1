#pragma once
#include "stdafx.hpp"

class GameViewport
{
public:
	GameViewport() {}
	virtual ~GameViewport() {}

	void Set(int nLeft, int nTop, int nWidth, int nHeight);

	int m_nLeft = 0;
	int m_nTop = 0;
	int m_nWidth = 0;
	int m_nHeight = 0;
};
