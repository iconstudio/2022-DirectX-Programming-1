#include "pch.hpp"
#include "HelicopterPlayer.hpp"
#include "Arithmetics.hpp"

HellicopterPlayer::HellicopterPlayer()
	: CPlayer()
{}

void HellicopterPlayer::Awake(P3DDevice device, P3DGrpCommandList cmdlist)
{
	CPlayer::Awake(device, cmdlist);

	m_pMainRotorFrame = FindFrame("rotor");
	m_pTailRotorFrame = FindFrame("black_m_7");
}

HellicopterPlayer::~HellicopterPlayer()
{}

void HellicopterPlayer::Update(float delta_time)
{
	if (m_pMainRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationY(XMConvertToRadians(360.0f * 2.0f) * delta_time);
		m_pMainRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pMainRotorFrame->localMatrix);
	}

	if (m_pTailRotorFrame)
	{
		XMMATRIX xmmtxRotate = XMMatrixRotationX(XMConvertToRadians(360.0f * 4.0f) * delta_time);
		m_pTailRotorFrame->localMatrix = Matrix4x4::Multiply(xmmtxRotate, m_pTailRotorFrame->localMatrix);
	}

	CPlayer::Update(delta_time);
}
