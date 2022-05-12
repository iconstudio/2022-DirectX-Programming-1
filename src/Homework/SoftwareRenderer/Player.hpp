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
	void SetLookOffset(const XMFLOAT3& vector);
	void SetLookOffset(XMFLOAT3&& vector);
	void Start();

	void AddBullet(PlayerBullet* bullet);
	void ReturnBullet(PlayerBullet* bullet);
	void RideOn(RailBorder* entrance);
	void TakeOff(RailBorder* exit);
	void Crawl(DWORD dwdir, float accel);

	void SetPosition(const XMFLOAT3& pos) override;

	void Update(float elapsed_time) override;
	void UpdateCamera(float elapsed_time = 0.0f);

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
	XMFLOAT3 lookOffset;
	float headAngle;

	float shootDelay;
	const float shootCooldown;
	bool shootLocking;

	int myBulletShooted;
	std::vector<PlayerBullet*> myBulletPool;
};
