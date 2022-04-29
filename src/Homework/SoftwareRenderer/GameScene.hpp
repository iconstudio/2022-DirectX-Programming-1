#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Fragment.hpp"
#include "Player.hpp"

using ObjectPtr = std::shared_ptr<GameObject>;
using CGroupPtr = std::shared_ptr<GameCollsionGroup>;

class GameScene
{
public:
	GameScene(GameFramework& framework, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(std::shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRendering();
	void Render(HDC surface);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCollsionGroup;

	// ���� ������ ũ��
	const INT worldSizeH, worldSizeV, worldSizeU;

	// ���� �浹 ����
	UINT collisionAreaIndex;

private:
	void BuildCollisionGroups();
	void BuildWorld();
	void BuildObjects();
	void BuildEnemies();

	CGroupPtr CreateCollisionGroup();
	CGroupPtr FindProperGroup(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(float x, float y, float z);

	template<class Type>
	Type* CreateInstance(const XMFLOAT3& position);

	template<class Type>
	Type* CreateInstance(XMFLOAT3&& position);

	// ������ �ڵ�
	HWND Window;

	// �����ӿ�ũ
	GameFramework& Framework;

	// �� ���� ��� �ν��Ͻ�
	std::vector<ObjectPtr> Instances;

	// �� ���� �浹 ����
	std::vector<CGroupPtr> collisionAreas;

	// ������ �� �浹 ����
	std::vector<CGroupPtr> preparedCollisionAreas;

	// �÷��̾� ��ü (Instances �ȿ��� ����)
	std::shared_ptr<Player> myPlayer;

	// �����ӿ�ũ���� ���� ī�޶�
	std::shared_ptr<GameCamera> myCamera;

	// �÷��̾� ���� ��ġ
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 5.0f, 0.0f, 1.0f };
};

class GameCollsionGroup
{
public:
	GameCollsionGroup(GameScene& scene, const size_t index, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);

	void SetPosition(XMFLOAT3&& position);

	void AddInstance(ObjectPtr& ptr);

	void PrepareRendering();
	void AddFragment(const CFragment& fragment);
	void Render(HDC surface) const;

	bool Contains(const XMFLOAT3& point);

	friend class GameScene;

private:
	GameScene& Scene;
	const size_t Index;

	BoundingBox Collider;

	std::vector<ObjectPtr> Instances;
	std::vector<CFragment> Fragments;
};
