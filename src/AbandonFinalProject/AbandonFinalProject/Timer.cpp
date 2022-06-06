#include "pch.hpp"
#include "Timer.hpp"

Timer::Timer(float frame_max)
	: myFrameLimit(frame_max)
	, performDefaultCounter(0)
	, performPausedCounter(0), m_nStopPerformanceCounter(0)
	, m_nCurrentFrameRate(0), m_nFramesPerSecond(0), m_fFPSTimeElapsed(0.0f)
	, isPaused(false)
{
	auto place = reinterpret_cast<LARGE_INTEGER*>(&m_nPerformanceFrequencyPerSec);
	if (FALSE == QueryPerformanceFrequency(place))
	{
		throw "초당 CPU 실행 수 얻기 실패!";
	}

	place = reinterpret_cast<LARGE_INTEGER*>(&m_nLastPerformanceCounter);
	if (FALSE == QueryPerformanceCounter(place))
	{
		throw "시스템의 시간 간격 얻기 실패!";
	}

	timeScale = 1.0 / (double)m_nPerformanceFrequencyPerSec;

	performDefaultCounter = m_nLastPerformanceCounter;

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_fFPSTimeElapsed = 0.0f;
}

Timer::~Timer()
{}

void Timer::Tick()
{
	if (isPaused)
	{
		myDeltaTime = 0.0f;
		return;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&performCurrentCounter);

	auto current_tick = performCurrentCounter - m_nLastPerformanceCounter;
	float time_elapsed = static_cast<float>(current_tick * timeScale);

	if (myFrameLimit > 0.0f)
	{
		while (time_elapsed < (1.0f / myFrameLimit))
		{
			::QueryPerformanceCounter((LARGE_INTEGER*)&performCurrentCounter);
			time_elapsed = float((performCurrentCounter - m_nLastPerformanceCounter) * timeScale);
		}
	}

	m_nLastPerformanceCounter = performCurrentCounter;

	if (fabsf(time_elapsed - myDeltaTime) < 1.0f)
	{
		::memmove(&m_fFrameTime[1], m_fFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(float));
		m_fFrameTime[0] = time_elapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	m_nFramesPerSecond++;
	m_fFPSTimeElapsed += time_elapsed;
	if (m_fFPSTimeElapsed > 1.0f)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_fFPSTimeElapsed = 0.0f;
	}

	myDeltaTime = 0.0f;

	for (ULONG i = 0; i < m_nSampleCount; i++) myDeltaTime += m_fFrameTime[i];
	if (m_nSampleCount > 0) myDeltaTime /= m_nSampleCount;
}

unsigned long Timer::GetFrameRate()
{
	return m_nCurrentFrameRate;
}

float Timer::GetDeltaTime()
{
	return myDeltaTime;
}

float Timer::GetTotalTime()
{
	if (isPaused)
	{
		const auto current_tick = (m_nStopPerformanceCounter - performPausedCounter);
		const auto delta = current_tick - performDefaultCounter;
		return float(delta * timeScale);
	}
	else
	{
		const auto current_tick = (performCurrentCounter - performPausedCounter);
		const auto delta = current_tick - performDefaultCounter;
		return float(delta * timeScale);
	}
}

void Timer::Reset()
{
	__int64 nPerformanceCounter = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	performDefaultCounter = nPerformanceCounter;
	m_nLastPerformanceCounter = nPerformanceCounter;
	m_nStopPerformanceCounter = 0;
	isPaused = false;
}

void Timer::Start()
{
	__int64 nPerformanceCounter = 0;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	if (isPaused)
	{
		performPausedCounter += (nPerformanceCounter - m_nStopPerformanceCounter);
		m_nLastPerformanceCounter = nPerformanceCounter;
		m_nStopPerformanceCounter = 0;
		isPaused = false;
	}
}

void Timer::Stop()
{
	if (!isPaused)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nStopPerformanceCounter);
		isPaused = true;
	}
}
