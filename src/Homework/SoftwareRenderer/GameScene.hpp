#pragma once
#include "stdafx.hpp"
#include "GameObject.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"

using ObjectPtr = std::shared_ptr<GameObject>;
using CGroupPtr = std::shared_ptr<GameCollsionGroup>;

class GameScene
{
public:
	GameScene(UINT sz_horizontal, UINT sz_vertical, UINT sz_up);
	~GameScene();

	void SetCamera(std::shared_ptr<GameCamera> cam);

	void Start();
	void Update(float elapsed_time);
	void Render(HDC surface);

	CGroupPtr CreateCollisionGroup();
	CGroupPtr FindProperGroup(const XMFLOAT3& position);

	template<class Type>
	ObjectPtr CreateInstance(float x, float y, float z);

	template<class Type>
	ObjectPtr CreateInstance(const XMFLOAT3& position);

	template<class Type>
	ObjectPtr CreateInstance(XMFLOAT3&& position);

	friend class GameCollsionGroup;

	// ���� ������ ũ��
	const INT worldSizeH, worldSizeV, worldSizeU;

	// ���� �浹 ����
	UINT collisionAreaIndex;

private:
	// �� ���� ��� �ν��Ͻ�
	std::vector<ObjectPtr> Instances;

	// �� ���� �浹 ����
	std::vector<CGroupPtr> collisionAreas;

	// �÷��̾� ��ü (Instances �ȿ��� ����)
	std::shared_ptr<Player> myPlayer;

	// �����ӿ�ũ���� ���� ī�޶�
	std::shared_ptr<GameCamera> myCamera;

	// �÷��̾� ���� ��ġ
	XMFLOAT3 playerSpawnPoint = XMFLOAT3{ 50.0f, 50.0f, 0.0f };
};

class GameCollsionGroup
{
public:
	GameCollsionGroup(const UINT index, UINT sz_horizontal, UINT sz_vertical, UINT sz_up);

	void SetPosition(XMFLOAT3&& position);

	void AddInstance(ObjectPtr& ptr);

	bool Contains(const XMFLOAT3& point);

private:
	const UINT Index;
	BoundingBox Collider;

	std::vector<ObjectPtr> Instances;
};
