#pragma once
#include "Scene.h"

class SceneIntro : public CScene
{
public:
	SceneIntro(GameFramework& framework);

	void Build() override;
};

class SceneMain : public CScene
{
public:
	SceneMain(GameFramework& framework);

	void Build() override;
};

class SceneGame : public CScene
{
public:
	SceneGame(GameFramework& framework);

	void Build() override;
};

class SceneGameEnd : public CScene
{
public:
	SceneGameEnd(GameFramework& framework);

	void Build() override;
};

class SceneCredit : public CScene
{
public:
	SceneCredit(GameFramework& framework);

	void Build() override;
};
