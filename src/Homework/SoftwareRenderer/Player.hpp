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
	void AddBullet(shared_ptr<PlayerBullet> bullet);

	void Crawl(DWORD dwdir, float accel);

	virtual void Move(const XMFLOAT3& dit, float distance);
	virtual void Rotate(float pitch, float yaw, float roll);

	virtual void Update(float elapsed_time);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnUpdateTransform();

	friend class GameCamera;

private:
	shared_ptr<PlayerBullet> FindLastBullet() const;

	HWND Window;
	POINT Cursor;
	DWORD Orientation;
	bool focused;

	float shootDelay;
	const float shootCooldown;

	int myBulletShooted;
	const int myBulletMax;
	std::vector<shared_ptr<PlayerBullet>> myBulletPool;
};
