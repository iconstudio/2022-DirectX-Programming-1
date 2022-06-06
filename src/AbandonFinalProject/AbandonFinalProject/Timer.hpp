#pragma once

constexpr ULONG MAX_SAMPLE_COUNT = 50UL;

class Timer
{
public:
	Timer(float frame_max);
	virtual ~Timer();

	void Tick();
	void Start();
	void Stop();
	void Reset();

	unsigned long GetFrameRate();
	float GetDeltaTime();
	float GetTotalTime();

private:
	const float myFrameLimit;
	float myDeltaTime;
	double timeScale;

	__int64 performDefaultCounter;
	__int64 performPausedCounter;
	__int64 m_nStopPerformanceCounter;
	__int64 performCurrentCounter;
	__int64 m_nLastPerformanceCounter;

	__int64 m_nPerformanceFrequencyPerSec;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	float m_fFPSTimeElapsed;

	bool isPaused;
};

