#pragma once
#include "Scene.h"

class SceneIntro : public CScene
{
public:
	SceneIntro(GameFramework& framework);
};

class SceneMain : public CScene
{
public:
	SceneMain(GameFramework& framework);
};

class SceneGame : public CScene
{
public:
	SceneGame(GameFramework& framework);
};

class SceneGameEnd : public CScene
{
public:
	SceneGameEnd(GameFramework& framework);
};

class SceneCredit : public CScene
{
public:
	SceneCredit(GameFramework& framework);
};
