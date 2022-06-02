#include "pch.hpp"
#include "Framework.hpp"
#include "Scene.hpp"

Framework::Framework(GraphicsCore& renderer, long width, long height)
	: WeakSingleton(renderer)
	, myWindow(NULL), myClientInstance(NULL)
	, frameWidth(width), frameHeight(height)
{}

Framework& Framework::SetHWND(HWND window)
{
	myWindow = window;
	return *this;
}

Framework& Framework::SetHInstance(HINSTANCE instance)
{
	myClientInstance = instance;
	return *this;
}

void Framework::Awake()
{

}

void Framework::Start()
{}

void Framework::Reset()
{}

void Framework::Update(float delta_time)
{}

void Framework::OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info)
{}

void Framework::OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state)
{}

void Framework::OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{}
