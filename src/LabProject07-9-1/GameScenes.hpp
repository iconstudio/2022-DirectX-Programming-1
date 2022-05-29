#pragma once
#include "Scene.h"

class StageIntro : public CScene
{
public:
	StageIntro(GameFramework& framework);

	// CScene��(��) ���� ��ӵ�
	void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
};

class StageMain : public CScene
{
public:
	StageMain(GameFramework& framework);

	// CScene��(��) ���� ��ӵ�
	void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
};

class StageGame : public CScene
{
public:
	StageGame(GameFramework& framework);

	// CScene��(��) ���� ��ӵ�
	void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
};

class StageGameEnd : public CScene
{
public:
	StageGameEnd(GameFramework& framework);

	// CScene��(��) ���� ��ӵ�
	void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
};

class StageCredit : public CScene
{
public:
	StageCredit(GameFramework& framework);

	// CScene��(��) ���� ��ӵ�
	void OnMouseEvent(HWND hwnd, UINT msg, WPARAM btn, LPARAM info) override;
	void OnKeyboardEvent(HWND hwnd, UINT msg, WPARAM key, LPARAM state) override;
};
