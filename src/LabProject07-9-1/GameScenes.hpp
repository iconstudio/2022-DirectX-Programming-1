#pragma once
#include "Scene.h"

class StageIntro : public CScene
{
public:
	StageIntro(GameFramework& framework);
};

class StageMain : public CScene
{
public:
	StageMain(GameFramework& framework);
};

class StageGame : public CScene
{
public:
	StageGame(GameFramework& framework);
};

class StageGameEnd : public CScene
{
public:
	StageGameEnd(GameFramework& framework);
};

class StageCredit : public CScene
{
public:
	StageCredit(GameFramework& framework);
};
