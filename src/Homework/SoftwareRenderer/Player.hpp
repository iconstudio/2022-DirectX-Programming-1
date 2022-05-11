#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "PlayerBullet.hpp"

enum class PLAYER_STATES
{
	IDLE = 0, NORMAL, RIDING, DEAD
};

class Player : public GameObject
{
public:
	Player();
	~Player();

	constexpr GOBJECT_TAGS GetTag() const override { return GOBJECT_TAGS::PLAYER; }

	void SetHwnd(HWND hwnd);
	void SetCamera(shared_ptr<GameCamera>& cam);

	void AddBullet(PlayerBullet* bullet);
	void ReturnBullet(PlayerBullet* bullet);

	void Start();
	void RideOn(RailBorder* entrance);
	void TakeOff(RailBorder* exit);
	void Crawl(DWORD dwdir, float accel);

	void Move(const XMFLOAT3& dit, float distance) override;
	void Rotate(float pitch, float yaw, float roll) override;

	void Update(float elapsed_time) override;

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	PLAYER_STATES myStatus;

	const int myBulletMax;

private:
	PlayerBullet* FindLastBullet();

	HWND Window;
	POINT Cursor;
	DWORD Orientation;

	shared_ptr<GameCamera> Camera;
	GameTransform mySight;

	float shootDelay;
	const float shootCooldown;
	bool shootLocking;

	int myBulletShooted;
	std::vector<PlayerBullet*> myBulletPool;
};
