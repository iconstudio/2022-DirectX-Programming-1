#pragma once
#include "pch.h"

const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균한다.

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

	float m_fFrameTime[MAX_SAMPLE_COUNT]; // 프레임 시간을 누적하기 위한 배열
	ULONG m_nSampleCount; // 누적된 프레임 횟수

	unsigned long m_nCurrentFrameRate; // 현재의 프레임 레이트
	unsigned long m_nFramesPerSecond; // 초당 프레임 수
	float m_fFPSTimeElapsed; // 프레임 레이트 계산 소요 시간
	bool m_bStopped;
};

