#pragma once
#include "pch.h"

const ULONG MAX_SAMPLE_COUNT = 50; // 50ȸ�� ������ ó���ð��� �����Ͽ� ����Ѵ�.

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Start() {}
	void Tick(float fLockFPS = 0.0f);
	void Stop() {}
	void Reset();

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);
	float GetTimeElapsed();

private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;

	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT]; // ������ �ð��� �����ϱ� ���� �迭
	ULONG m_nSampleCount; // ������ ������ Ƚ��

	unsigned long m_nCurrentFrameRate; // ������ ������ ����Ʈ
	unsigned long m_nFramesPerSecond; // �ʴ� ������ ��
	float m_fFPSTimeElapsed; // ������ ����Ʈ ��� �ҿ� �ð�
	bool m_bStopped;
};

