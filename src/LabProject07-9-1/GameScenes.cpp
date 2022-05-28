#include "stdafx.h"
#include "GameScenes.hpp"

SceneIntro::SceneIntro(GameFramework& framework)
	: CScene(framework, "Intro")
{}

void SceneIntro::Build()
{
	CScene::Build();
}

SceneMain::SceneMain(GameFramework& framework)
	: CScene(framework, "Main")
{}

void SceneMain::Build()
{
	CScene::Build();
}

SceneGame::SceneGame(GameFramework& framework)
	: CScene(framework, "Game")
{}

void SceneGame::Build()
{
	CScene::Build();
}

SceneGameEnd::SceneGameEnd(GameFramework& framework)
	: CScene(framework, "Complete")
{}

void SceneGameEnd::Build()
{
	CScene::Build();
}

SceneCredit::SceneCredit(GameFramework& framework)
	: CScene(framework, "Credit")
{}

void SceneCredit::Build()
{
	CScene::Build();
}
