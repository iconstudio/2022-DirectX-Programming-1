#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "PlayerBullet.hpp"

class Player : public GameObject
{
public:
	Player(GameScene& scene);
	virtual ~Player();

	void SetHwnd(HWND hwnd);

	void AddBullet(PlayerBullet* bullet);
	void ReturnBullet(PlayerBullet* bullet);

	void Crawl(DWORD dwdir, float accel);

	virtual void Move(const XMFLOAT3& dit, float distance);
	virtual void Rotate(float pitch, float yaw, float roll);

	virtual void Update(float elapsed_time);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnUpdateTransform();

	friend class GameCamera;

	const int myBulletMax;

private:
	PlayerBullet* FindLastBullet();

	HWND Window;
	POINT Cursor;
	DWORD Orientation;
	bool focused;

	float shootDelay;
	const float shootCooldown;

	int myBulletShooted;
	std::vector<PlayerBullet*> myBulletPool;
};
