#pragma once

class DelegateObjet
{
public:
	virtual ~DelegateObjet();

	virtual void OnMouse(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) = 0;
	virtual void OnKeyboard(HWND hwnd, UINT msg, WPARAM key, LPARAM state) = 0;
	virtual void OnWindow(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) = 0;
};
