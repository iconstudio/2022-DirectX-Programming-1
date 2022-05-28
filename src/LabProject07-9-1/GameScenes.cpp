#include "stdafx.h"
#include "GameScenes.hpp"

StageIntro::StageIntro(GameFramework& framework)
	: CScene(framework, "Intro")
{}

StageMain::StageMain(GameFramework& framework)
	: CScene(framework, "Main")
{}

StageGame::StageGame(GameFramework& framework)
	: CScene(framework, "Game")
{}

StageGameEnd::StageGameEnd(GameFramework& framework)
	: CScene(framework, "Complete")
{}

StageCredit::StageCredit(GameFramework& framework)
	: CScene(framework, "Credit")
{}
