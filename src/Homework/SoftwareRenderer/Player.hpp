#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "PlayerBullet.hpp"

class Player : public GameObject
{
public:
	Player();
	virtual ~Player();

	void SetHwnd(HWND hwnd);
	void SetCamera(shared_ptr<GameCamera>& cam);

	void AddBullet(PlayerBullet* bullet);
	void ReturnBullet(PlayerBullet* bullet);

	void Crawl(DWORD dwdir, float accel);

	virtual void Move(const XMFLOAT3& dit, float distance);
	virtual void Rotate(float pitch, float yaw, float roll);

	virtual void Update(float elapsed_time) override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCamera;

	const int myBulletMax;

private:
	PlayerBullet* FindLastBullet();

	HWND Window;
	POINT Cursor;
	DWORD Orientation;

	shared_ptr<GameCamera> Camera;

	float shootDelay;
	const float shootCooldown;
	bool shootLocking;

	int myBulletShooted;
	std::vector<PlayerBullet*> myBulletPool;
};
