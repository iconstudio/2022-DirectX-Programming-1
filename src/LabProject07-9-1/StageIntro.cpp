#include "pch.hpp"
#include "StageIntro.hpp"

StageIntro::StageIntro(GameFramework& framework, HWND hwnd)
	: Scene(framework, hwnd, "Intro")
{}

void StageIntro::Awake(P3DDevice device, P3DGrpCommandList cmd_list)
{
	Scene::Awake(device, cmd_list);
}

void StageIntro::Start()
{
	Scene::Start();
}

void StageIntro::Reset()
{
	Scene::Reset();
}

void StageIntro::Update(float elapsed_time)
{
	Scene::Update(elapsed_time);
}

void StageIntro::Render()
{
	Scene::Render();
}

void StageIntro::RenderUI(HDC surface)
{
	Scene::RenderUI(surface);
}

void StageIntro::OnAwake()
{}

void StageIntro::OnInialized()
{}

void StageIntro::OnUpdate()
{}

void StageIntro::OnRender()
{}

void StageIntro::OnWindows(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}

void StageIntro::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
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

void StageIntro::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}
