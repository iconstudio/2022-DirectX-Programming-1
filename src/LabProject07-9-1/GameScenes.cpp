#include "stdafx.h"
#include "GameScenes.hpp"

StageIntro::StageIntro(GameFramework& framework)
	: CScene(framework, "Intro")
{}

void StageIntro::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{

}

void StageIntro::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{

}

StageMain::StageMain(GameFramework& framework)
	: CScene(framework, "Main")
{}

void StageMain::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{

}

void StageMain::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{

}

StageGame::StageGame(GameFramework& framework)
	: CScene(framework, "Game")
{}

void StageGame::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{

}

void StageGame::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{

}

StageGameEnd::StageGameEnd(GameFramework& framework)
	: CScene(framework, "Complete")
{}

void StageGameEnd::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{

}

void StageGameEnd::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{

}

StageCredit::StageCredit(GameFramework& framework)
	: CScene(framework, "Credit")
{}

void StageCredit::OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{

}

void StageCredit::OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{

}
