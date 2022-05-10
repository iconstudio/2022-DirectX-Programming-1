#pragma once
#include "stdafx.hpp"
#include "GameCamera.hpp"
#include "GameObject.hpp"
#include "GameMesh.hpp"
#include "Mesh.hpp"
#include "Fragment.hpp"
#include "Player.hpp"
#include "PlayerBullet.hpp"

using ObjectPtr = shared_ptr<GameObject>;

class GameScene
{
public:
	GameScene(GameFramework& framework, size_t sz_x, size_t height, size_t sz_y);
	~GameScene();

	void SetHwnd(HWND hwnd);
	void SetCamera(shared_ptr<GameCamera>& cam);

	void Start();
	void Update(float elapsed_time);
	void PrepareRenderingWorld();
	void PrepareRendering();
	void Render(HDC surface);

	void AddFragment(const CFragment& fragment);
	HPEN ReadyPen(COLORREF color);
	void PrepareRenderingCollider(const BoundingBox& collider);
	void PrepareRenderingCollider(const BoundingOrientedBox& collider);
	void PrepareRenderingCollider(const BoundingFrustum& collider);

	void Kill(GameObject* obj);
	void Kill(ObjectPtr& obj);
	Enemy* SpawnEnemy(ENEMY_TYPES type, const XMFLOAT3& pos);

	void OnMouse(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnKeyboard(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
	void OnHWND(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

	friend class GameCollsionGroup;
	friend class Player;
	friend class GameObject;

	// �޽�
	shared_ptr<CMesh> meshPlayer;
	shared_ptr<CMesh> meshPlayerBullet;
	shared_ptr<CMesh> meshEnemyCube;
	shared_ptr<CMesh> meshEnemyManta;
	shared_ptr<CMesh> meshEnemyBullet;

	// ���� 1���� 15������ ���� ���
	shared_ptr<CMesh> meshPillars[15];

	// ���� �޽�
	shared_ptr<CMesh> meshRail;

private:
	void BuildMeshes();
	void BuildWorld();
	void BuildObjects();
	void CompleteBuilds();

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
	std::vector<ObjectPtr>::difference_type staticBound;

	// ���� �浹 ����
	size_t collisionAreaIndex;

	// �÷��̾ ��ġ�� ������ ��ȣ
	size_t worldPlayerPositionIndex;
	// �÷��̾��� ����� ��ġ
	float worldPlayerPosition;

	// ������ �� ����
	std::vector<CFragment> Fragments;

	// ������ �� �� ����
	std::unordered_map<COLORREF, HPEN> Pens;

	// �÷��̾� ��ü
	shared_ptr<Player> myPlayer;

	// �����ӿ�ũ���� ���� ī�޶�
	shared_ptr<GameCamera> myCamera;

	// �÷��̾� ���� ��ġ
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 5.0f, 0.0f, 1.0f };

	//std::vector<Pillar> Pillars;
};
