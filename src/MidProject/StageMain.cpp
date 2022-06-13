#include "pch.hpp"
#include "StageMain.hpp"
#include "GameFramework.h"

StageMain::StageMain(GameFramework& framework)
	: Scene(framework, "Main")
	, textTitle("HelliAttack", 24, Colors::CadetBlue)
	, textAuthor("JinYoonSeong", 24, Colors::CadetBlue)
	, textDate("2022-06-13", 24, Colors::CadetBlue)
{
	constexpr float colors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetBackgroundColor(colors);
}

void StageMain::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	Scene::Awake(device, cmdlist);

	textTitle.Start(device, cmdlist);
	textAuthor.Start(device, cmdlist);
	textDate.Start(device, cmdlist);
}

void StageMain::Start()
{
	Scene::Start();
}

void StageMain::Reset()
{
	Scene::Reset();
}

void StageMain::Update(float delta_time)
{
	Scene::Update(delta_time);
}

void StageMain::PrepareRendering()
{}

void StageMain::Render() const
{}

void StageMain::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageMain::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		{
		}
		break;

		case WM_RBUTTONDOWN:
		{
		}
		break;

		case WM_LBUTTONUP:
		{
		}
		break;

		case WM_RBUTTONUP:
		{
		}
		break;

		case WM_MOUSEMOVE:
		{
		}
		break;

		default:
		{}
		break;
	}
}

void StageMain::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (key)
			{
				case VK_SPACE:
				case VK_RETURN:
				{
					myFramework.JumpToNextStage();
				}
				break;
			}
		}
		break;
	}
}
